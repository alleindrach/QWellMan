#ifndef WELLSDAO_H
#define WELLSDAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class WellDao : public QObject
{
    Q_OBJECT
public:

    explicit WellDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~WellDao();
    static WellDao * instance();

    QSqlQueryModel * wells();
    QSqlQueryModel * recentWells();
    QSqlQueryModel * favoriteWells();
    QSqlQueryModel * processWells(QSqlQueryModel *);
    QSqlRecord well(QString idWell);
    int addRecord(QString table,QString parentId=QString());
    int addRecentWell(QString idWell);
    int addFavoriteWell(QString idWell);
    int removeFavoriteWell(QString idWell);
    QString recordDes(QString table,QSqlRecord record);

signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static WellDao* _instance;
public slots:
};

#endif // WELLSDAO_H
