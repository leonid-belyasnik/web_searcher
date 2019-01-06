#include "UrlsList.h"

using namespace dx_test;

UrlsList::UrlsList(QObject *parent) : QObject(parent)
{
}

UrlItemsType UrlsList::items() const
{
    return m_urls;
}

bool UrlsList::isExists(uint32_t id)
{
    return (m_urls.contains(id));
}

bool UrlsList::setItem(const UrlItem &item)
{
    auto id = item.id;
    if (!isExists(id))
        return false;

    const UrlItem &oldItem = m_urls[id];
    if (item.id == oldItem.id
     && item.parent_id == oldItem.parent_id
     && item.url == oldItem.url
     && item.state == oldItem.state)
    {
        return false;
    }

    m_urls[id] = item;
    return true;
}

void UrlsList::appendItem(UrlItem item)
{
    emit preItemAppended(item.id);
    m_urls.insert(item.id, item);
    emit postItemAppended();

}

void UrlsList::removeItems()
{
    for (auto it = m_urls.begin(); it != m_urls.end();)
    {
        auto id = (*it).id;
        emit preItemRemoved(id);
        m_urls.erase(it++);
        emit postItemRemoved();
    }

    emit resetList();
}

void UrlsList::editItem(uint32_t id, UrlState state, const QString &msg)
{
    if (!isExists(id))
        return;

    UrlItem item = m_urls[id];
    item.state = state;
    item.err_msg = msg;
    setItem(item);
    emit dataChanged(item.id);
}
