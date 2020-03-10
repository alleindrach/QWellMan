#ifndef EDDAOL_H
#define EDDAOL_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "mdltable.h"
#include "mdlfield.h"
#include "mdlunit.h"

class EDLDao : public QObject
{
    Q_OBJECT
public:
    explicit EDLDao(QSqlDatabase &db,QObject *parent);
    ~EDLDao();
    QString equation(QString table,QString field);
    static EDLDao * instance();
    static void resetCache();

signals:
private:
    QSqlDatabase  _db;
    static QHash<QString,QString> _sql;
    static EDLDao* _instance;
    static QHash<QString ,QVariant> _cache;
};


#endif // EDL_H
