#include "UsePool.h"

namespace dx_test {

UsePool::UsePool()
{

}

UsePool::~UsePool()
{
    stop();
}

void UsePool::start(uint32_t thread_num/* = 0*/, uint32_t max_used/* = 0*/)
{
    resetState();
    startPool(thread_num);
    m_stopped = false;
    m_max_used = max_used;
}

void UsePool::startPool(uint32_t threads_num)
{
    if (threads_num == 0)
    {
        uint32_t numCPU = std::thread::hardware_concurrency();
        if (numCPU == 0)
        {
            numCPU = 1;
        }
        else
        {
            threads_num = numCPU * 2; // use medium optimal
        }
    }

    for (uint32_t i = 0; i < threads_num; i++)
    {
        m_thread_pool.emplace_back([/*i, */this]
        {
            for (;;)
            {
                uint32_t task_id;
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_mutex_tasks);
                    this->m_cv_tasks.wait(
                        lock,
                        [this]
                        {
                            return (this->m_terminate || !this->m_tasks.empty());
                        }
                    );

                    if (this->m_paused)
                    {
                        continue;
                    }

                    if (this->m_terminate)
                    {
                        return;
                    }

                    if (this->m_tasks.empty())
                    {
                        continue;
                    }

                    auto command = this->m_tasks.front();
                    task_id = command.id;
                    task = command.fn;
                    this->m_tasks.pop();
                }

                try
                {
                    // execute the task
                    task();
                    {
                        std::lock_guard<std::mutex> lock(m_mutex_used);
                        m_used.push_back(task_id);
                    }
                    m_cv_done.notify_one();
                }
                catch (const std::exception&/* e*/)
                {
                    //qCritical() << e.what() << endl;
                }
            }
        }
        );
    }
}

void UsePool::terminate()
{
    if (!m_terminate)
    {
        m_terminate = true;
        m_cv_tasks.notify_all();
    }

    m_paused = false;
}

void UsePool::resetState()
{
    m_terminate = false;
    m_stopped = true;
    m_paused = false;
    std::lock_guard<std::mutex> lock(m_mutex_used);
    m_used.clear();
    m_num = 0;
}

bool UsePool::isTerminated() const
{
    return m_terminate;
}

bool UsePool::isRunning() const
{
    return !m_stopped;
}

bool UsePool::isPaused() const
{
    return m_paused;
}

void UsePool::enqueueCmd(TaskCommand cmd)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex_tasks);
        m_tasks.push(cmd);
    }

    m_cv_tasks.notify_one();
}

void UsePool::stop()
{
    if (m_stopped || m_terminate)
    {
        return;
    }

    terminate();

    for (std::thread &thread : m_thread_pool)
    {
        if (thread.joinable())
            thread.join();
    }

    m_thread_pool.clear();

    m_stopped = true;
    m_cv_done.notify_all();
}

void UsePool::pause()
{
    if (isRunning())
    {
        m_paused = true;
        m_cv_tasks.notify_all();
    }
}

void UsePool::resume()
{
    if (isRunning())
    {
        m_paused = false;
        m_cv_tasks.notify_all();
    }
}

void UsePool::abort()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex_tasks);
        while (!m_tasks.empty())
        {
            m_tasks.pop();
        }
    }
    stop();
}

bool UsePool::isFull()
{
    return (m_max_used == m_num.load());
}

bool UsePool::newID(uint32_t &value)
{
    if (m_max_used && m_max_used == m_num.load())
    {
        value = m_num;
        return false;
    }

    value = ++m_num;
    return true;
}

void UsePool::waitDone()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(m_mutex_used);
        this->m_cv_done.wait(lock);

        if (m_terminate || m_used.size() == m_num.load())
        {
            break;
        }
    }
}

size_t UsePool::queueSize() const
{
    std::lock_guard<std::mutex> lock(m_mutex_tasks);
    return this->m_tasks.size();
}

size_t UsePool::usedSize() const
{
    std::lock_guard<std::mutex> lock(m_mutex_used);
    return this->m_used.size();
}

} // dx_test
