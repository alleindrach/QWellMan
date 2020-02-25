#ifndef LIBDAO_H
#define LIBDAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "mdltable.h"
#include "mdlfield.h"
#include "mdlunit.h"

class LIBDao : public QObject
{
    Q_OBJECT
public:
    explicit LIBDao(QSqlDatabase &db,QObject *parent);
    ~LIBDao();
    static LIBDao * instance();
//    QStringList libLookup(QString table,QString displayFld);
    QStringList libTabs(QString table);

    QSqlQueryModel * libLookup(QString table);
    QSqlQueryModel * libLookup(QString table,QString tab);
    static QVector<QString> hiddenFields;
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static LIBDao* _instance;

    QHash<QString ,QVariant> _cache;
};

#endif // LIBDAO_H
