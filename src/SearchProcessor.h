/**
* \file	SearchProcessor.h
* \brief Implementation of URLs graph formation and search in it
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	03/01/2019
*/

#ifndef SEARCHPROCESSOR_H
#define SEARCHPROCESSOR_H

#include <QObject>
#include <QtCore>

#include "UsePool.h"
#include "IProcessDelegate.h"

class QNetworkReply;

namespace dx_test {

struct SearcherConf
{
    uint32_t threadsNum;
    uint32_t maxDeep;
    QString rootUrl;
    QString keyword;
};

class SearchProcessor : public QObject, public UsePool
{
    Q_OBJECT
public:
    explicit SearchProcessor(
            IProcessDelegate *processDelegate,
            QObject *parent = nullptr);

    ~SearchProcessor(){}

public slots:
    void run(const SearcherConf &conf);
    void stop();
    void pause();
private:
    void start(const QString &root_url);
    bool addTask(uint32_t parent_id, const QString &url);
    void handle(uint32_t id, uint32_t parent_id, const QString &url);
    bool findKeyword(const QString &content);
    void findURLs(uint32_t parent_id, const QString &content);
    bool checkUsingUrl(const QString &url);
private:
    SearcherConf m_conf;
    IProcessDelegate *m_process;
    QStringList m_listURLs;
    std::mutex m_mtxUrls;
};

} // dx_test
#endif // SEARCHPROCESSOR_H
