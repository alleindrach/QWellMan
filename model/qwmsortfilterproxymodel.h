#ifndef QWMSORTFILTERPROXYMODEL_H
#define QWMSORTFILTERPROXYMODEL_H

#include <QObject>
#include "qexsortfilterproxymodel.h"
#include <QSqlRecord>
#include <QSqlError>
class QWMSortFilterProxyModel : public QExSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit QWMSortFilterProxyModel(QObject *parent = nullptr);
    Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const ;
    Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const ;
    bool insertRecord(int row, const QSqlRecord &record);
    QSqlRecord record() const;
    QSqlRecord record(int ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setFilterFunction( std::function<bool (int , const QModelIndex &)>  acceptor);
    bool submitAll() ;
    bool submit() override;
    void revert() override;
    int  visibleFieldsCount();
    bool isFieldVisible(const QString & field);
    QSqlError  lastError() ;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
signals:


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    //    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    std::function<bool (int , const QModelIndex &)>  _filterFunction{nullptr};
};

#endif // QWMSORTFILTERPROXYMODEL_H
