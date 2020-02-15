#ifndef APPINFODAO_H
#define APPINFODAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "mdltable.h"
#include "mdlfield.h"
#include "mdlunit.h"
class MDLDao : public QObject
{
    Q_OBJECT
public:

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
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static MDLDao* _instance;
    QHash<QString ,QVariant> _cache;
public slots:
};

#endif // APPINFODAO_H
