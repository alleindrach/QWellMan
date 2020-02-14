#ifndef APPINFODAO_H
#define APPINFODAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class MDLDao : public QObject
{
    Q_OBJECT
public:

    explicit MDLDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~MDLDao();
    static MDLDao * instance();
    QStringList tableGroup();
    QSqlQuery tablesOfGroup(QString group);
    void readConfig(QHash<QString,QString>& );
    QString tableOrderKey(QString table);
    QSqlQuery tableHeaders(QString table);
    QSqlQuery tableMainHeadersVisible();
    QStringList tableMainHeadersVisibleKeys();
    QStringList units();
    QStringList datumPref();
    QSqlQuery tableFields(QString table);
    bool tableHasField(QString table,QString field);
    QString parentTable(QString table);
    QString parentRefName(QString table);
    QSqlRecord baseUnitOfField(QString table,QString field);
    QVariant unitBase2User(QString baseUnit,QString userUnit,QVariant v);
    QVariant unitUser2Base(QString baseUnit,QString userUnit,QVariant v);
    QSqlRecord userUnitKey(QString unitSet,QString unitType);
    QSqlRecord baseUnitKey(QString unitType);
    QSqlRecord unitConversion(QString baseUnitKey,QString userUnitKey);
    QSqlRecord tableInfo(QString Table);
    QSqlQuery childTables(QString table,QStringList hidden,QString profile=QString());
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static MDLDao* _instance;
public slots:
};

#endif // APPINFODAO_H
