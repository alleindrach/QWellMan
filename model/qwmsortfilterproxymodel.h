#ifndef QWMSORTFILTERPROXYMODEL_H
#define QWMSORTFILTERPROXYMODEL_H

#include <QObject>
#include "qexsortfilterproxymodel.h"
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QStringList>
class QWMSortFilterProxyModel : public QExSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit QWMSortFilterProxyModel(QObject *parent = nullptr);
    Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const ;
    Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const ;
    bool insertRecord(int row, const QSqlRecord &record);
    bool insertRecordDirect(int row, const QSqlRecord &record);
    bool removeRecord(int row);
    QSqlRecord record() const;
    QSqlRecord record(int ) const;
    QSqlRecord record(QModelIndex) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setFilterFunction( std::function<bool (int , const QModelIndex &)>  acceptor);
    bool submitAll() ;
    bool submit() override;
    void revert() override;
    int  visibleFieldsCount();
    bool isFieldVisible(const QString & field);
    const QString  groupTitle(const int col) const;
    QString fieldName(QModelIndex  index);
    bool showGroup();
    void setShowGroup(bool );
    const int realColumn(const int col) const ;
    const int groupedColumn(const int col ) const;
    virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
    QSqlError  lastError() ;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;

signals:
public  slots:
    void  on_source_model_data_changed(QModelIndex,QModelIndex,QVector<int>);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    //    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    std::function<bool (int , const QModelIndex &)>  _filterFunction{nullptr};
    bool _showGroup{false};
    QHash<QString,QStringList> _fieldGroup;
    QHash<int,QString > _groupIndex;

friend class QWMMain;
};

#endif // QWMSORTFILTERPROXYMODEL_H
