#ifndef UDLDAO_H
#define UDLDAO_H


#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QPair>
class UDLDao : public QObject
{
    Q_OBJECT
public:

    explicit UDLDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~UDLDao();
    static UDLDao * instance();
    QStringList profiles();
    QStringList tableGroup(QString profile);
    QSqlQuery  tablesOfGroup(QString group,QString profile=QString());
signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static UDLDao* _instance;
public slots:
};
#endif // UDLDAO_H
