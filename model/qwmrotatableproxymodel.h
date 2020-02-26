#ifndef QWMROTATEPROXYMODEL_H
#define QWMROTATEPROXYMODEL_H
#include "qexsortfilterproxymodel.h"
#include <QSqlRecord>
#include <QSqlError>
#include <QItemSelection>
class QWMRotatableProxyModel : public QExSortFilterProxyModel
{
    Q_OBJECT

public:
    enum Mode{H,V};
    Q_ENUM(Mode);
    explicit QWMRotatableProxyModel(Mode mode=H,QObject *parent = nullptr);

    bool insertRecord(int row, const QSqlRecord &record);
    QSqlRecord record() const;
    QSqlRecord record(QModelIndex) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const ;
    Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const ;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Q_INVOKABLE virtual QItemSelection mapSelectionToSource(const QItemSelection &selection) const;
    Q_INVOKABLE virtual QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QString fieldName(QModelIndex  index);
    QString tableName();
    bool showGroup();
    void setShowGroup(bool show);
    bool submitAll() ;
    bool submit() override;
    void revert() override;
    int  visibleFieldsCount();
    bool isFieldVisible(const QString & field);
    virtual void setSourceModel(QAbstractItemModel *sourceModel);
    Mode mode();
    void setMode(Mode m);
    QSqlError  lastError() ;
public  slots:
    void  on_source_model_data_changed(QModelIndex,QModelIndex,QVector<int>);
signals:
    void  modeChange(Mode m);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    //    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    Mode _mode{H};
    bool _showGroup{false};
    friend class QWMMain;
    friend class QWMDataEditor;
    QHash<QString ,QStringList> _group2FieldMap;
    QHash<QString,QString> _fileld2GroupMap;
private slots:

};

#endif // QWMROTATEPROXYMODEL_H
