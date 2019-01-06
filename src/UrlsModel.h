/**
* \file	UrlsModel.h
* \brief Abstract model to prepare data for QML list-view
*
* \author Leonid Belyasnik (leonid.belyasnik@gmail.com)
* \date	04/01/2019
*/

#ifndef URLSMODEL_H
#define URLSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <memory>

namespace dx_test {

class UrlsList;

/**
 * @brief The UrlsModel class model to prepare data for QML ListView
 */
class UrlsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(dx_test::UrlsList *list READ list WRITE setList NOTIFY listChanged)

public:
    explicit UrlsModel(QObject *parent = nullptr);

    enum {
        IdRole = Qt::UserRole,
        ParentIdRole,
        UrlRole,
        StateRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    UrlsList *list() const;
    void setList(UrlsList *list);
private:
    QModelIndex getIndex(uint32_t id) const;

signals:
    void listChanged();
private:
    UrlsList *m_list;
};

} // dx_test
#endif // URLSMODEL_H
