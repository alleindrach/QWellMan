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
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static MDLDao* _instance;
public slots:
};

#endif // APPINFODAO_H
