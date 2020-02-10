#ifndef APPINFODAO_H
#define APPINFODAO_H

#include <QObject>
#include <QSqlDatabase>

class AppInfoDao : public QObject
{
    Q_OBJECT
public:
    explicit AppInfoDao(QSqlDatabase * db, QObject *parent = nullptr);

signals:
private:
    QSqlDatabase * _db;

public slots:
};

#endif // APPINFODAO_H
