#ifndef QWMTABLEMODEL_H
#define QWMTABLEMODEL_H
#include <QSqlRelationalTableModel>
#include <QObject>
#include "common.h"
#include "qwmapplication.h"
class QWMTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    explicit QWMTableModel(QObject *parent = nullptr,QSqlDatabase db = APP->well());
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setTable(const QString &tableName) override;
    bool readonly();
    void setReadonly(bool v);
    void initFields(const QString &tableName);
    void setVisibleFields(const QHash<int,QString> lst);
    QHash<int,QString>  & visibleFields();
signals:

public slots:
    void init_record_on_prime_insert(int row, QSqlRecord &record);
private :
    bool  _readonly;
    QHash<int,QString> _visibleFieldsInOrder;
};

#endif // QWMTABLEMODEL_H
