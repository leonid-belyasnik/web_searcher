#include "SearchProcessor.h"
#include <QtNetwork>

namespace dx_test {

static const QString sg_patternUrl("https?:\\/\\/[^\\s\\Q!\\^*:@&|=+$?#()\\\\[\\]{}`'\"\\E<,>]*");

SearchProcessor::SearchProcessor(
        IProcessDelegate *processDelegate,
        QObject *parent/* = nullptr*/)
    : QObject(parent)
    , m_process(processDelegate)
{
    Q_ASSERT(m_process);
}

void SearchProcessor::run(const SearcherConf &conf)
{
    if (isRunning())
    {
        if (isPaused())
        {
            resume();
            m_process->onResume();
        }

        return;
    }
    m_conf = conf;
    m_listURLs.clear();
    UsePool::start(m_conf.threadsNum, conf.maxDeep);
    m_process->onStarted();
    addTask(0, m_conf.rootUrl);
    auto thDone = std::thread([this](){
        this->waitDone();
        this->stop();
        this->m_process->onDone();
    });
    thDone.detach();
}

void SearchProcessor::stop()
{
    UsePool::stop();
    m_process->onStopped();
}

void SearchProcessor::pause()
{
    UsePool::pause();
    m_process->onPaused();
}

bool SearchProcessor::addTask(uint32_t parent_id, const QString &url)
{
    uint32_t new_id;
    if (!newID(new_id) && isFull())
    {
        return false;
    }

    enqueueCmd(
                {
                    new_id,
                    std::bind(
                        std::mem_fn(&SearchProcessor::handle),
                        this,
                        new_id, parent_id, url
                    )
                }
    );

    return true;
}

bool SearchProcessor::findKeyword(const QString &content)
{
    return content.contains(m_conf.keyword);
}

void SearchProcessor::findURLs(uint32_t parent_id, const QString &content)
{
    int pos{0};
    QRegExp urlExp(sg_patternUrl);
    while ((pos = urlExp.indexIn(content, pos)) != -1)
    {
        while (isPaused())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (isTerminated())
        {
            break;
        }
        QString matchUrl(urlExp.cap(0));
        if (!checkUsingUrl(matchUrl)) // avoid loops in the cycle
        {
            bool added = addTask(parent_id, matchUrl);
            if (!added)
            {
                break;
            }
        }
        pos += urlExp.matchedLength();
    }
}

void SearchProcessor::handle(uint32_t id, uint32_t parent_id, const QString &url)
{
    m_process->addNew(id, parent_id, url);
    QNetworkAccessManager net_manager;
    QNetworkRequest request;
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setUrl(QUrl(url));
    QEventLoop event;
    m_process->onLoad(id);
    QNetworkReply *response = net_manager.get(request);
    connect(response, SIGNAL(sslErrors(QList<QSslError>)), response, SLOT(ignoreSslErrors()));
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    if (response->error() == QNetworkReply::NoError)
    {
        QString content = response->readAll();
        m_process->onSearch(id);
        findURLs(id, content);
        m_process->onFound(id, findKeyword(content));
    }
    else
    {
        m_process->onError(id, response->errorString());
    }

    response->deleteLater();
    m_process->onStatus(QString::number(id) + QStringLiteral(": ") + url);
    m_process->onProgress(static_cast<uint32_t>(usedSize()));
}

bool SearchProcessor::checkUsingUrl(const QString &url)
{
    QString checkUrl = url;
    auto pos = checkUrl.lastIndexOf('/');
    if (pos == checkUrl.size() - 1)
    {
        checkUrl = checkUrl.left(pos);
    }

    std::lock_guard<std::mutex> lock(m_mtxUrls);
    if (m_listURLs.contains(checkUrl))
    {
        return true;
    }

    m_listURLs << checkUrl;

    return false;
}

} // dx_test
