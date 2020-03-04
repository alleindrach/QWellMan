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
    QWMRotatableProxyModel * tableForEdit(const QString  tablename,const QString  IDWell,const  QString  parentID);
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
    QSqlRecord refRecord(QString table, QString id);
    void initRecord(QSqlRecord &,QString IDWell=QString(),QString parentID=QString() );
    QStringList distinctValue(QString table,QString field);
    QSqlQuery records(QString table,QString idWell,QString parentID);
    QSqlRecord aRecord(QString table);
signals:
private:
    QSqlDatabase  _db;
    static QHash<QString,QString> _sql;
    static WellDao* _instance;
    static QHash<QString ,QVariant> _cache;
    //lookup=8的参数
    //首先查找表特定的字段，比如wvWellBore.IDRecParent，如果没有，再查找无表前缀的字段，，如果形式为TblKeyxxx:a/b形式，则表明是根据本表字段TblKeyXXXX来判断是哪个表，并将表明写入此字段。具体从哪几个表取，是a/b...
    QHash<QString,QString> _RefTable={{"IDRecString","TblKeyString:wvCas/wvJobDrillString"},
                                      {"IDRecJobPull","wvJob"},
                                      {"IDRecJobRun","wvJob"},
                                      {"IDRecWellBore","wvWellbore"},
                                      {"IDRecJob","wvJob"},
                                      {"IDRecBit","wvJobDrillBit"},
                                      {"IDRecJobContact","wvJobContact"},
                                      {"IDRecPhaseCustom","wvJobProgramPhase"},
                                      {"IDRecJobRentalItem","wvJobRentalItem"},
                                      {"IDRecZone","wvZone"},
                                      {"IDRecJobContactContractor","wvJobContact"},
                                      {"IDRecTub","wvTub"},
                                      {"IDRecFailedItem","TblKeyFailedItem:wvcascomp/wvtubcomp"},//这个比较特殊，表名根据TblKeyFailedItem来判断，存在于wvProblem
                                      {"IDRecProblem","wvProblem"},
                                      {"IDRecTubComp","wvTubComp"},
                                      {"IDRecFluid","wvStimTreatFluid"},
                                      {"wvTask.IDRecParent","TblKeyParent:wvTask"},
                                      {"IDRecTestItem","TblKeyTestItem:wvwellheadcomp/wvwellhead"},
                                      {"IDRecCas","wvCas"},
                                      {"IDRecFrm","wvWellboreFormation"},
                                      {"IDRecRod","wvRod"},
                                      {"wvWellBore.IDRecParent","wvWellBore"},
                                      {"IDRecGauge","wvWellTestPresTravGauge"},
                                      {"IDRecGaugeUsed","wvWellTestTransGauge"},
                                      {"IDRecDirSrvyActual","wvWellboreDirSurvey"},
                                      {"IDRecDirSrvyProp","wvWellboreDirSurvey"},
                                      {"IDRecTubular","TblKeyTubular:?"},
                                      {"wvZoneLink.IDRecItem","TblKeyItem:?"},

                                     };
public slots:
};

#endif // WELLSDAO_H
