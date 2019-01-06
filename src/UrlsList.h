/**
* \file	UrlsList.h
* \brief Storage of result
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	04/01/2019
*/

#ifndef URLSLIST_H
#define URLSLIST_H

#include <QObject>
#include <QMap>

namespace dx_test {

/**
 * @brief The UrlState enum - describing the state handling url
 */
enum class UrlState : uint32_t
{
    Undefined = 0,
    Load,
    Search,
    Found,
    Disfound,
    Error
};

/**
* \brief Url info
*/
struct UrlItem
{
    uint32_t id;
    uint32_t parent_id;
    QString url;
    UrlState state;
    QString err_msg;
};

using UrlItemsType = QMap<uint32_t, UrlItem>;

/**
 * @brief The UrlsList class to store of result and preparing container for model
 */
class UrlsList : public QObject
{
    Q_OBJECT
public:
    explicit UrlsList(QObject *parent = nullptr);

    UrlItemsType items() const;
    bool isExists(uint32_t id);
    bool setItem(const UrlItem &item);

signals:
    void preItemAppended(uint32_t id);
    void postItemAppended();

    void preItemRemoved(uint32_t id);
    void postItemRemoved();

    void dataChanged(uint32_t id);
    void resetList();

public slots:
    void appendItem(UrlItem item);
    void removeItems();
    void editItem(uint32_t id, UrlState state, const QString &msg);

private:
    UrlItemsType m_urls;
};

} // dx_test
#endif // URLSLIST_H
