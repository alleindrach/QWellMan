#ifndef WELLSDAO_H
#define WELLSDAO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QList>
#include <QStringList>
#include "qwmrotatableproxymodel.h"
#include "qwmtablemodel.h"
struct PendingDuplicateItem{
    QString table;
    QString pkValue;
    QString pkField;
    QString field;
};

class WellDao : public QObject
{
    Q_OBJECT
public:

    explicit WellDao(QSqlDatabase & db, QObject *parent = nullptr);
    ~WellDao();
    static WellDao * instance();

    QAbstractItemModel * wells(int type);
    bool  processWells(QWMTableModel *);
    QWMRotatableProxyModel * tableForEdit(const QString  tablename,const QString  idWell,const  QString  parentID);
    QWMTableModel * table(QString tablename,QString idWell);
    bool processTable(QWMTableModel *);
    QSqlRecord well(QString idWell);
    int addRecord(QString table,QString parentId=QString());
    int addRecentWell(QString idWell);
    int addFavoriteWell(QString idWell);
    int removeFavoriteWell(QString idWell);
    int deleteItem(QString idWell,QString idRec,QString table);
    int undeleteItem(QString idWell,QString idRec);
    bool isRecentWell(QString idwell);
    bool isFavoriteWell(QString idwell);
    bool isDeletedWell(QString  idwell);
    bool isDeletedRecord(QString idWell,QString idRec);
    QString recordDes(QString table,QSqlRecord record);
    QSqlRecord refRecord(QString table, QString id);
    void initRecord(QSqlRecord &,QString IDWell=QString(),QString parentID=QString() );
    QStringList distinctValue(QString table,QString field);
    QSqlQuery records(QString table,QString idWell,QString parentID);
    QSqlRecord aRecord(QString table);
    bool hasRecord(QString table,QString idrec);

    QString PBTDAll(QString idWell);
    QString TDAll(QString idWell);
    double TD(QString idWell);
    static void resetCache();
    bool duplicateWellHeader(QString idWell,QList<QPair<QString,QStringList>> & stagedTables ,QHash<QString,QString> &   mapDuplicatedRecords,QHash<QString,int> & mapDuplicatedTables,QHash<QString ,QList<PendingDuplicateItem> >&pendingIDs,QStringList & errors);
    bool initStageTables(QString idWell,QList<QPair<QString,QStringList>> & stagedTables ,QHash<QString,QString> & mapDuplicatedRecords,QHash<QString,int> & mapDuplicatedTables,QHash<QString,QList<PendingDuplicateItem> >&pendingIDs,QStringList & errors);
    bool handleStageTables(QString idWell,QList<QPair<QString,QStringList>> & stagedTables ,QHash<QString,QString> & mapDuplicatedRecords,QHash<QString,int> & mapDuplicatedTables,QHash<QString,QList<PendingDuplicateItem> >&pendingIDs,QStringList & errors);
    bool duplicateTable(QString idWell,QString table,QList<QPair<QString,QStringList>> & stagedTables ,QHash<QString,QString> & mapDuplicatedRecords,QHash<QString,int> & mapDuplicatedTables,QHash<QString,QList<PendingDuplicateItem> >&pendingIDs,QStringList & errors);
    bool processPendingID(QString oriId,QString newId, QHash<QString ,QList<PendingDuplicateItem> >&pendingIDs,QStringList & errors);
    bool duplicateWell(QString idWell,QWidget* parent);
    QSqlRecord wellbore4SurvyData(QString survyId);//根据survy的idrec查找所属的wellbore的idrec
    QString wellboreActiveSurvyId(QString wellboreId);//根据wellbore的idrec查询绑定的survyId
    QPointF wellboreDepth(QString idBore,QDateTime till);

signals:
    void Duplicating(QString message,int progress);
private:
    QSqlDatabase  _db;
    static QHash<QString,QString> _sql;
    static WellDao* _instance;
    static QHash<QString ,QVariant> _cache;
public:
    static QStringList unpastebleFields;
    //lookup=8的参数
    //首先查找表特定的字段，比如wvWellBore.IDRecParent，如果没有，再查找无表前缀的字段，，如果形式为TblKeyxxx:a/b形式，则表明是根据本表字段TblKeyXXXX来判断是哪个表，并将表明写入此字段。具体从哪几个表取，是a/b...

public slots:
};

#endif // WELLSDAO_H
