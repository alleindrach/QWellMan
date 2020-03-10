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
    void connectSignals();
    bool readonly();
    void setReadonly(bool v);
    void initFields(const QString &tableName);
    void setVisibleFields( QStringList lst);
    int  fieldVisibleOrder(const QString &field);
    const QString fieldInVisibleOrder(int);
    int  visibleFieldsCount();
    int  fieldsCount();
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

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
    inline QString parentID(){
        return _parentID;
    }
    inline void setParentID(QString v){
        _parentID=v;
    }
signals:
    void  rowsChanged();
protected:
//    virtual bool updateRowInTable(int row, const QSqlRecord &values) override;
public slots:
    void init_record_on_prime_insert(int row, QSqlRecord &record);
    void before_update_record(int row, QSqlRecord &record);
    void before_insert(QSqlRecord &record);
private :
    bool  _readonly{false};
    QString  _parentID{QString()};
    QStringList _fieldsInDisplayOrder;//显示顺序，大小写区分
    QHash<QString,int> _fieldsNameToNatureOrderMap;//显示名到自然顺序的映射，全小写
    QHash<QString,int> _fieldsNameToDisplayOrderMap;//显示名到自然顺序的映射,全小写
    QStringList _fieldsInNatureOrder;//自然顺序，大小写区分
    QStringList _calFields;//计算字段列表、全小写
    int _natureFieldCount;//物理字段数
//    QHash<QString,int> _fieldsCalcMap;
//    QList<QString> _fieldsOrigin;
    int _visibleFields{0};
    QString _idWell;
    friend class QWMDataEditor;
};
Q_DECLARE_METATYPE(QWMTableModel*)
#endif // QWMTABLEMODEL_H
