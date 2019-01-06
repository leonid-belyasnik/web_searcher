/**
* \file	UsePool.h
* \brief Implement concept of thread pool
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	03/01/2019
*/

#ifndef USEPOOL_H
#define USEPOOL_H

#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <future>
#include <functional>

namespace dx_test {

/**
* \brief Task command
*/
struct TaskCommand
{
    uint32_t id;
    std::function<void()> fn;
};

/**
 * @brief The UsePool class thread pool
 */
class UsePool
{
    UsePool(const UsePool&) = delete;
    UsePool& operator&(const UsePool&) = delete;
public:
    UsePool();
    virtual ~UsePool();
    void start(uint32_t thread_num = 0, uint32_t max_used = 0);
    void terminate();
    void stop();
    void pause();
    void resume();
    void abort();
    bool isTerminated() const;
    bool isRunning() const;
    bool isPaused() const;
protected:
    void waitDone();
    void enqueueCmd(TaskCommand cmd);
    void startPool(uint32_t threads_num);
    void resetState();
    bool isFull();
    bool newID(uint32_t &value);
    size_t queueSize() const;
    size_t usedSize() const;
private:
    std::vector<std::thread> m_thread_pool;
    std::queue<TaskCommand> m_tasks;
    std::vector<uint32_t> m_used;
    mutable std::mutex m_mutex_tasks;
    mutable std::mutex m_mutex_used;
    std::condition_variable m_cv_tasks;
    std::condition_variable m_cv_done;
    std::atomic_bool m_terminate{ false };
    std::atomic_bool m_stopped{ true };
    std::atomic_bool m_paused{ false };
    std::atomic_uint m_num{0};
    uint32_t m_max_used;
};

} // dx_test
#endif // USEPOOL_H
