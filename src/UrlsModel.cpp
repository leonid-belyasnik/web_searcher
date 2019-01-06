#include "UrlsModel.h"
#include "UrlsList.h"

using namespace dx_test;

namespace {

QString stateToStr(UrlState state)
{
    switch (state)
    {
        case UrlState::Load:
            return QStringLiteral("load");
        case UrlState::Search:
            return QStringLiteral("search");
        case UrlState::Found:
            return QStringLiteral("found");
        case UrlState::Disfound:
            return QStringLiteral("not found");
        case UrlState::Error:
            return QStringLiteral("error");
        default:
            return QStringLiteral("undefined");
    }
}

}

UrlsModel::UrlsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_list(nullptr)
{
}

int UrlsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_list)
        return 0;

    return m_list->items().size();
}

QVariant UrlsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_list)
        return QVariant();

    auto id = static_cast<uint32_t>(index.row() + 1);
    if (!m_list->items().contains(id))
        return QVariant();

    const UrlItem item = m_list->items()[id];

    switch (role)
    {
    case IdRole:
        return QVariant(item.id);
    case ParentIdRole:
        return QVariant(item.parent_id);
    case UrlRole:
        return QVariant(item.url);
    case StateRole:
    {
        if (item.state == UrlState::Error)
        {
            QVariant(stateToStr(item.state) + QStringLiteral(": ") + item.err_msg);
        }

        return QVariant(stateToStr(item.state));
    }
    }

    return QVariant();
}

QHash<int, QByteArray> UrlsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole] = "id";
    names[ParentIdRole] = "parent_id";
    names[UrlRole] = "url";
    names[StateRole] = "state";
    return names;
}

UrlsList *UrlsModel::list() const
{
    return m_list;
}

QModelIndex UrlsModel::getIndex(uint32_t id) const
{
    if (!m_list->items().contains(id))
    {
        return QModelIndex();
    }

    return createIndex(static_cast<int>(id - 1), 0);
}

void UrlsModel::setList(UrlsList *list)
{
    beginResetModel();

    if (m_list)
    {
        m_list->disconnect(this);
    }

    m_list = list;

    if (m_list) {
        connect(m_list, &UrlsList::preItemAppended, this, [=](uint32_t id) {
            const int index = static_cast<const int>(id - 1);
            beginInsertRows(QModelIndex(), index, index);
        });
        connect(m_list, &UrlsList::postItemAppended, this, [=]() {
            endInsertRows();
        });

        connect(m_list, &UrlsList::preItemRemoved, this, [=](uint32_t id) {
            const int index = static_cast<const int>(id - 1);
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(m_list, &UrlsList::postItemRemoved, this, [=]() {
            endRemoveRows();
        });

        connect(m_list, &UrlsList::dataChanged, this, [=](uint32_t id) {
            auto index = getIndex(id);
            emit dataChanged(index, index);
        });

        connect(m_list, &UrlsList::resetList, this, [=]() {
            emit listChanged();
        });
    }

    endResetModel();
}
