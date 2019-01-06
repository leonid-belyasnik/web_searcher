#include "Handler.h"

using namespace dx_test;

Handler::Handler(std::shared_ptr<UrlsList> urlsList, QObject *parent)
    : QObject(parent)
    , m_urlsList(urlsList)
    , m_processor(std::make_unique<dx_test::SearchProcessor>(this, this))
{
    qRegisterMetaType<UrlItem>("UrlItem");
    qRegisterMetaType<SearcherConf>("SearcherConf");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<UrlState>("UrlState");
    connect(this, SIGNAL(sig_appendItem(UrlItem)), m_urlsList.get(), SLOT(appendItem(UrlItem)));
    connect(this, SIGNAL(sig_clearItems()), m_urlsList.get(), SLOT(removeItems()));
    connect(this, SIGNAL(sig_editItem(uint32_t, UrlState, const QString)), m_urlsList.get(), SLOT(editItem(uint32_t, UrlState, const QString&)));
    connect(this, SIGNAL(sig_run(const SearcherConf &)), m_processor.get(), SLOT(run(const SearcherConf &)));
    connect(this, SIGNAL(sig_stop()), m_processor.get(), SLOT(stop()));
    connect(this, SIGNAL(sig_pause()), m_processor.get(), SLOT(pause()));
}

void Handler::sltRun(
        int threadsNum,
        int maxDeep,
        const QString &keyword,
        const QString &rootUrl
        )
{
    emit sig_run({
          static_cast<uint32_t>(threadsNum),
          static_cast<uint32_t>(maxDeep),
          rootUrl, keyword});
}

void Handler::sltStop()
{
    emit sig_stop();
}

void Handler::sltPause()
{
    emit sig_pause();
}

// IProcessDelegate

void Handler::addNew(uint32_t id, uint32_t parent_id, const QString &url)
{
    emit sig_appendItem({ id, parent_id, url, UrlState::Undefined, "" });
}

void Handler::onLoad(uint32_t id)
{
    emit sig_editItem(id, UrlState::Load, "");
}

void Handler::onSearch(uint32_t id)
{
    emit sig_editItem(id, UrlState::Search, "");
}
void Handler::onFound(uint32_t id, bool value)
{
    emit sig_editItem(id, ((value) ? UrlState::Found : UrlState::Disfound), "");
}

void Handler::onError(uint32_t id, const QString &msg)
{
    emit sig_editItem(id, UrlState::Error, msg);
}

void Handler::onStarted()
{
    emit sig_clearItems();
    emit sig_onRunning();
}

void Handler::onPaused()
{
    emit sig_onPaused();
}

void Handler::onStopped()
{
    emit sig_onStopped();
}

void Handler::onResume()
{
    emit sig_onResume();
}

void Handler::onDone()
{
    emit sig_onDone();
}

void Handler::onStatus(const QString &msg)
{
    emit sig_setStatus(msg);
}

void Handler::onProgress(uint32_t used_num)
{
    emit sig_onProgress(used_num);
}
