#ifndef UDLDAO_H
#define UDLDAO_H


#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QPair>
#include "mdltable.h"
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
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static UDLDao* _instance;
    QHash<QString ,QVariant> _cache;
public slots:
};
#endif // UDLDAO_H
