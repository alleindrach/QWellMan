#ifndef QWMSORTFILTERPROXYMODEL_H
#define QWMSORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
class QWMSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit QWMSortFilterProxyModel(int type,QObject *parent = nullptr);

signals:


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    int _type{0};
};

#endif // QWMSORTFILTERPROXYMODEL_H
