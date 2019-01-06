/**
* \file	Handler.h
* \brief Handler of signals and slots between QML and C++
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	04/01/2019
*/

#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QVariant>
#include <memory>
#include "UrlsList.h"
#include "IProcessDelegate.h"
#include "SearchProcessor.h"

namespace dx_test {

/**
 * @brief The Handler class
 */
class Handler : public QObject, public IProcessDelegate
{
    Q_OBJECT
public:
    explicit Handler(std::shared_ptr<UrlsList> urlsList, QObject *parent = nullptr);

signals:
    void sig_setStatus(QVariant text);
    void sig_onRunning();
    void sig_onStopped();
    void sig_onResume();
    void sig_onPaused();
    void sig_onDone();
    void sig_onProgress(QVariant num);
    void sig_appendItem(UrlItem item);
    void sig_editItem(uint32_t id, UrlState state, const QString &msg);
    void sig_clearItems();
    void sig_run(const SearcherConf &conf);
    void sig_stop();
    void sig_pause();
public slots:
    void sltRun(
            int threadsNum,
            int maxDeep,
            const QString &keyword,
            const QString &rootUrl
            );

    void sltStop();
    void sltPause();
    // IProcessDelegate
    void addNew(uint32_t id, uint32_t parent_id, const QString &url) override;
    void onLoad(uint32_t id) override;
    void onSearch(uint32_t id) override;
    void onFound(uint32_t id, bool value) override;
    void onError(uint32_t id, const QString &msg) override;
    void onStarted() override;
    void onPaused() override;
    void onStopped() override;
    void onResume() override;
    void onDone() override;
    void onStatus(const QString &msg) override;
    void onProgress(uint32_t used_num) override;
private:
    std::shared_ptr<UrlsList> m_urlsList;
    std::unique_ptr<SearchProcessor> m_processor {nullptr};
};

} // dx_test
#endif // HANDLER_H
