#ifndef WELLSDAO_H
#define WELLSDAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include "qwmrotatableproxymodel.h"
#include "qwmtablemodel.h"
class WellDao : public QObject
{
    Q_OBJECT
public:

    explicit WellDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~WellDao();
    static WellDao * instance();

    QAbstractItemModel * wells(int type);
    bool  processWells(QWMTableModel *);
    QWMRotatableProxyModel * tableForEdit(QString tablename,QString parentID);
    QWMRotatableProxyModel * table(QString tablename);
    bool processTable(QWMTableModel *);
    QSqlRecord well(QString idWell);
    int addRecord(QString table,QString parentId=QString());
    int addRecentWell(QString idWell);
    int addFavoriteWell(QString idWell);
    int removeFavoriteWell(QString idWell);
    int deleteItem(QString idWell,QString idRec);
    bool isRecentWell(QString idwell);
    bool isFavoriteWell(QString idwell);
    bool isDeletedWell(QString  idwell);
    QString recordDes(QString table,QSqlRecord record);
    void initRecord(QSqlRecord &,QString IDWell=QString(),QString parentID=QString() );

signals:
private:
    QSqlDatabase  _db;
    QHash<QString,QString> _sql;
    static WellDao* _instance;
    QHash<QString ,QVariant> _cache;
public slots:
};

#endif // WELLSDAO_H
