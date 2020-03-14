#ifndef UDLDAO_H
#define UDLDAO_H


#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QPair>
#include "mdltable.h"
#include "udltableproperty.h"
#include "udllibgroup.h"
#include "udllibtab.h"
#include "udllibtabfield.h"
#include "mdlfield.h"
#include "mdlunit.h"
class UDLDao : public QObject
{
    Q_OBJECT
public:

    explicit UDLDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~UDLDao();
    static UDLDao * instance();
    QStringList profiles();
    QStringList tableGroup(QString profile);
    QList<MDLTable*>  tablesOfGroup(QString group,QString profile=QString());
    QList<MDLTable*> childTables(QString table,QString profile=QString());
    QStringList tablesHidden(QString profile);
    QStringList fieldsHidden(QString profile,QString tablename);
    QStringList fieldsVisibleInOrder(QString profile, QString table);
    QStringList fieldsVisibleInOrderByGroup(QString profile,QString table,QString group);
    UDLTableProperty * tableProperty(QString table);
    QString lookupGroup(QString profile,QString table);
    QList<UDLLibGroup*> lookupTables(QString group);
    QList<UDLLibTab *> lookupTableTabs(QString set,QString lib);
    QList<UDLLibTabField*> lookupTableFieldsOfTab(QString set,QString lib,QString tab);
    QStringList fieldOfGroup(QString table,QString field);

    MDLTable * tableInfo(QString Table);
    QList<MDLField*> tableFields(QString table);
    QList<MDLField*> tableRefFields(QString table);
    QStringList tableHeaders(QString table);
    QString tableOrderKey(QString table);
    QSqlQuery tableFieldsQuery(QString table);
    bool tableHasField(QString table,QString field);
    MDLUnitType* baseUnitOfField(QString table,QString field);
    MDLField * fieldInfo(QString table,QString field);
    QStringList fieldGroup(QString table);
//    QString filedCapl(QString table,QString field);
    MDLField * fieldByLookup(QString table,QString lib,QString libfld);
//如果一个lookuptyp=8的字段，没有main.pceMDLTableFieldLookupList对应的记录，且有Tblkey***的字段，则判定为引用全部的idrec
    bool isLookupAllField(MDLField *fieldInfo);
    bool isLookupMultiTableField(MDLField *fieldInfo);
//    IDRecxxxx对应的tableName字段KeyTblxxxx
    QString lookupTableNameField(MDLField * fieldInfo);
    static void resetCache();
signals:
private:
    QSqlDatabase  _db;
    static QHash<QString,QString> _sql;
    static UDLDao* _instance;
    static QHash<QString ,QVariant> _cache;

public slots:
};
#endif // UDLDAO_H
