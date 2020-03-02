#ifndef APPINFODAO_H
#define APPINFODAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "mdltable.h"
#include "mdlfield.h"
#include "mdlunit.h"
#include "mdlfieldlookup.h"
class MDLDao : public QObject
{
    Q_OBJECT
public:
    enum PhysicalType{Integer=2,Long=3,Single=4,Double=5,Currency=6,DateTime=7,Boolean=11,Text=200,Memo=201,Bolob=205};
    Q_ENUM(PhysicalType)
    enum LookupType{NONE=0,LibEdit=1,LibOnly=2,DBDistinctValues=3,Icon=4,Date=5,Time=6,DateAndTime=7,Foreign=8,List=11,TabList=12,MaskEdit=14};
    Q_ENUM(LookupType)
    explicit MDLDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~MDLDao();
    static MDLDao * instance();
    QStringList tableGroup();
    QList<MDLTable*> tablesOfGroup(QString group);
    void readConfig(QHash<QString,QString>& );
    QString tableOrderKey(QString table);
    QStringList tableHeaders(QString table);
    QList<MDLFieldVisible*> tableMainHeadersVisible();
    QStringList tableMainHeadersVisibleKeys();
    QStringList units();
    QStringList datumPref();
    QList<MDLField*> tableFields(QString table);
    QSqlQuery tableFieldsQuery(QString table);
    bool tableHasField(QString table,QString field);
    QString parentTable(QString table);
    QString parentRefName(QString table);
    MDLUnitType* baseUnitOfField(QString table,QString field);
    QVariant unitBase2User(QString baseUnit,QString userUnit,QVariant v);
    QVariant unitUser2Base(QString baseUnit,QString userUnit,QVariant v);
    MDLUnitTypeSet *  userUnitKey(QString unitSet,QString unitType);
    MDLUnitType* baseUnitKey(QString unitType);
    MDLUnitConversion * unitConversion(QString baseUnitKey,QString userUnitKey);
    MDLTable * tableInfo(QString Table);
    MDLField * fieldInfo(QString table,QString field);
    QList<MDLTable*>  childTables(QString table,QStringList hidden,QString profile=QString());
    QString idField(QString table);
    QStringList fieldGroup(QString table);
    QStringList fieldOfGroup(QString table,QString field);
    QList<MDLFieldLookup *> fieldLookupinfo(QString table,QString field);
    QString filedCapl(QString table,QString field);
signals:
private:
    QSqlDatabase  _db;
    static  QHash<QString,QString> _sql;
    static MDLDao* _instance;
    static QHash<QString ,QVariant> _cache;
public slots:
};

#endif // APPINFODAO_H
