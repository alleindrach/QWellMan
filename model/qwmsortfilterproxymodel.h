#ifndef QWMSORTFILTERPROXYMODEL_H
#define QWMSORTFILTERPROXYMODEL_H

#include <QObject>
//#include "qexsortfilterproxymodel.h"
#include <QSqlRecord>
#include <QSqlError>
#include <QList>
#include <QStringList>
#include <QSortFilterProxyModel>
class QWMSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit QWMSortFilterProxyModel(QString idWell,QObject *parent = nullptr);
    Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    int mapColumnToSource(const int ) const;
    int mapRowToSource(const int row) const;
    bool insertRecord(int row, const QSqlRecord &record);
    bool insertRecordDirect(int row, const QSqlRecord &record);
    bool removeRecord(int row);
    QSqlRecord record() const;
    QSqlRecord record(int ) const;
    QSqlRecord record(QModelIndex) const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setFilterFunction( std::function<bool (int , const QModelIndex &)>  acceptor);
    void setSortFunction( std::function<bool (const QModelIndex &, const QModelIndex &)>  acceptor);
    bool submitAll() ;
    bool submit() override;
    void revert() override;
    void reset();
    int  visibleFieldsCount();
    bool isFieldVisible(const QString & field);
    const QString  groupTitle(const int col) const;
    QString fieldName(QModelIndex  index);
    bool showGroup();
    void setShowGroup(bool );
    int realColumn(const int col) const ;
    int groupedColumn(const int col ) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
    QSqlError  lastError() ;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;
    inline void  setIDWell(QString idWell){
        _idWell=idWell;
    }
    inline QString idWell(){
        return _idWell;
    }
    void calc(int row);
    void calcAll();
    void setParentId(QString);
    QString parentId();
signals:
    void rowsChanged();
    void submitted(QString tablename);
    void dataModified(QString tablename,QString parentId);
public  slots:
    void  on_source_model_data_changed(QModelIndex,QModelIndex,QVector<int>);
    void on_rows_changed();
    void on_source_model_data_submitted(QString table);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    std::function<bool (int , const QModelIndex &)>  _filterFunction{nullptr};
    std::function<bool (const QModelIndex & , const QModelIndex &)>  _sortFunction{nullptr};
    bool _showGroup{false};
    QHash<QString,QStringList> _fieldGroup;
    QHash<int,QString > _groupIndex;
    QString _idWell;
    QString _parentId;
    friend class QWMMain;
};

#endif // QWMSORTFILTERPROXYMODEL_H
