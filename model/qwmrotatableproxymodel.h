#ifndef QWMROTATEPROXYMODEL_H
#define QWMROTATEPROXYMODEL_H
#include <QAbstractProxyModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QItemSelection>
#include "common.h"
class QWMRotatableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(int visibleFieldsCount READ visibleFieldsCount )
public:
    enum Mode{H=Qt::Horizontal,V=Qt::Vertical};
    Q_ENUM(Mode);
    explicit QWMRotatableProxyModel(Mode mode=H,QObject *parent = nullptr);

    bool insertRecord(int row, const QSqlRecord &record);
    bool insertRecordDirect(int row, const QSqlRecord &record);
    bool removeRecord(QModelIndex index);
    QSqlRecord record() const;
    QSqlRecord record(QModelIndex) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const  override;
    Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const   override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Q_INVOKABLE virtual QItemSelection mapSelectionToSource(const QItemSelection &selection) const override;
    Q_INVOKABLE virtual QItemSelection mapSelectionFromSource(const QItemSelection &selection) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QString fieldName(QModelIndex  index);
    QString fieldTitle(QModelIndex  index);
    QModelIndex indexOfSameRecord(QModelIndex  index,QString fieldName);
    QString tableName();
    QModelIndex firstEditableCell() ;
    bool showGroup();
    void setShowGroup(bool show);
    bool submitAll() ;
    bool submit() override;
    void revert() override;
    int  visibleFieldsCount();
    bool isFieldVisible(const QString & field);
    virtual void setSourceModel(QAbstractItemModel *sourceModel)  override;
    Mode mode();
    void setMode(Mode m);
    void reset();
    QSqlError  lastError() ;
    bool isDirty();
    void source_items_inserted(const QModelIndex &source_parent,
                               int start, int end, Qt::Orientation orient);
    void source_items_about_to_be_removed(const QModelIndex &source_parent,
                                          int start, int end, Qt::Orientation orient);
    void source_items_removed(const QModelIndex &source_parent,
                              int start, int end, Qt::Orientation orient);
    void remove_source_items(  const QModelIndex &source_parent, int start, int end,
                               Qt::Orientation orient, bool emit_signal = true);
public  slots:
    //    void  on_source_model_data_changed(QModelIndex,QModelIndex,QVector<int>);
    //    void rowsInsertedAdaptor(const QModelIndex &index, int start, int end);
    //    void rowsRemovedAdaptor(const QModelIndex &index, int start, int end);
    //    void columnsInsertedAdaptor(const QModelIndex &index, int start, int end);
    //    void columnsRemovedAdaptor(const QModelIndex &index, int start, int end);
    void sourceHeaderDataChanged(Qt::Orientation orientation,int start, int end);
    void sourceDataChanged(const QModelIndex &source_top_left,const QModelIndex &source_bottom_right,
                           const QVector<int> &roles);
    void sourceRowsAboutToBeInserted(
            const QModelIndex &source_parent, int start, int end);
    void sourceRowsInserted(
            const QModelIndex &source_parent, int start, int end);
    void sourceColumnsAboutToBeInserted(
            const QModelIndex &source_parent, int start, int end);
    void sourceColumnsInserted(const QModelIndex &source_parent,
                               int start, int end);
    void sourceRowsAboutToBeRemoved(
            const QModelIndex &source_parent, int start, int end);
    void sourceRowsRemoved(
            const QModelIndex &source_parent, int start, int end);

    void sourceColumnsAboutToBeRemoved(const QModelIndex &source_parent,
                                       int start, int end);
    void sourceColumnsRemoved(
            const QModelIndex &source_parent, int start, int end);
    void sourceRowsAboutToBeMoved(const QModelIndex &sourceParent,
                                  int sourceStart, int sourceEnd,
                                  const QModelIndex &destParent, int dest);

    void sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);

    void sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);

    void sourceAboutToBeReset();
    void sourceReset();

signals:
    void  modeChanged();

    //    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    //    void rowsAboutToBeInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void rowsInserted(const QModelIndex &parent, int first, int last);

    //    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void rowsRemoved(const QModelIndex &parent, int first, int last);

    //    void columnsAboutToBeInserted(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void columnsInserted(const QModelIndex &parent, int first, int last);

    //    void columnsAboutToBeRemoved(const QModelIndex &parent, int first, int last, QPrivateSignal);
    void columnsRemoved(const QModelIndex &parent, int first, int last);


protected:
    //    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    //    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private :
    Mode _mode{H};
    bool _showGroup;

    friend class QWMDataEditor;
    QHash<QString ,QStringList> _group2FieldMap;
    QHash<QString,QString> _fileld2GroupMap;
    QList<QPersistentModelIndex> saved_layoutChange_parents;
private slots:

};
Q_DECLARE_METATYPE(QWMRotatableProxyModel*);
#endif // QWMROTATEPROXYMODEL_H
