#ifndef QWMTABLEMODEL_H
#define QWMTABLEMODEL_H
#include <QSqlTableModel>
#include <QObject>
#include "qwmapplication.h"
#include "common.h"

class QWMTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit QWMTableModel(QString idWell,QObject *parent = nullptr,QSqlDatabase db = APP->well());
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setTable(const QString &tableName) override;
    bool readonly();
    void setReadonly(bool v);
    void initFields(const QString &tableName);
    void setVisibleFields( QStringList lst);
    void mergeVisibleFields( QStringList lst);
    int  fieldPosByOrder(const QString &field);
    int  visibleFieldsCount();
    int  fieldsCount();
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    const QString fieldInPosByOrder(int);
    bool isFieldVisible(const QString & field);
    int fieldIndexEx(const QString &fieldName) ;
    QString   fieldNameEx(const int  index) const;
    QModelIndex createIndex(int row,int col) ;
    inline void  setIDWell(QString idWell){
        _idWell=idWell;
    }
    inline QString idWell(){
        return _idWell;
    }
signals:
    void  rowsChanged();
protected:
//    virtual bool updateRowInTable(int row, const QSqlRecord &values) override;
public slots:
    void init_record_on_prime_insert(int row, QSqlRecord &record);
private :
    bool  _readonly{false};
    QList<QString> _fieldsInOrder;
    QHash<QString,int> _fieldsInOrderVice;
    QList<QString> _fieldsCalcInOrder;
    QHash<QString,int> _fieldsCalcMap;
    QList<QString> _fieldsOrigin;
    int _visibleFields{0};
    QString _idWell;
    friend class QWMDataEditor;
};
Q_DECLARE_METATYPE(QWMTableModel*)
#endif // QWMTABLEMODEL_H
