#include "welldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "mdldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlRecord"
#include "QDebug"
#include "QSqlField"
WellDao * WellDao::_instance=nullptr;

WellDao::WellDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 )  order by %5 ");
    DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group and l.KeyTbl=t.KeyTbl order by l.DisplayOrder");
    DECL_SQL(select_spec_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 ) and  exists(select *  from %6 r where w.%3=r.%7)  order by %5 ");
    DECL_SQL(insert_well_to_catlog,"insert into %1 (%2) values(?)");
    DECL_SQL(delete_well_from_catlog,"delete from %1 where %2=?");
}

WellDao::~WellDao()
{

}

WellDao *WellDao::instance()
{
    if(_instance==nullptr){
        _instance=new WellDao(APP->mdl(),APP);
    }
    return _instance;
}

QSqlQueryModel *WellDao::recentWells()
{

    QSqlQueryModel*  model=new QSqlQueryModel(this);
    QStringList result;
    QString keytableMain=CFG(KeyTblMain);
    QString orderKey=MDL->tableOrderKey(keytableMain);


    QSqlQuery q(SQL(select_spec_wells)
                .arg(keytableMain)
                .arg(SYS_DEL_REC)
                .arg(CFG(IDMainFieldName))
                .arg(CFG(IDMainFieldName))
                .arg(orderKey)
                .arg(SYS_RECENT_WELL)
                .arg(CFG(IDMainFieldName))
                ,APP->well());
    q.exec();
    PRINT_ERROR(q);
    model->setQuery(q);
    return processWells(model);
}

QSqlQueryModel *WellDao::favoriteWells()
{
    QSqlQueryModel*  model=new QSqlQueryModel(this);
    QStringList result;
    QString keytableMain=CFG(KeyTblMain);
    QString orderKey=MDL->tableOrderKey(keytableMain);


    QSqlQuery q(SQL(select_spec_wells)
                .arg(keytableMain)
                .arg(SYS_DEL_REC)
                .arg(CFG(IDMainFieldName))
                .arg(CFG(IDMainFieldName))
                .arg(orderKey)
                .arg(SYS_FAVORITE_WELL)
                .arg(CFG(IDMainFieldName))
                ,APP->well());
    q.exec();
    PRINT_ERROR(q);
    model->setQuery(q);
    return processWells(model);
}

QSqlQueryModel *WellDao::processWells(QSqlQueryModel * model)
{

    QSqlQuery visibleFieldsQuery=MDL->tableMainHeadersVisible();
    QStringList visibleFields;
    QStringList visibleHeaders;
    QHash<QString,QString> visibleHeaderData;
    QHash<QString,QString> headerData;
    QSqlRecord rec=visibleFieldsQuery.record();
    while(visibleFieldsQuery.next()){
        QString fieldName=visibleFieldsQuery.value(rec.indexOf("KeyFld")).toString();
        bool isVisible=!visibleFieldsQuery.value(rec.indexOf("VisibleFld")).toString().isEmpty();
        QString fieldNameUpper=fieldName.toUpper();
        QString fieldHeader=visibleFieldsQuery.value(rec.indexOf("CaptionLong")).toString();
        visibleFields<<fieldName;
        visibleHeaders<<fieldHeader;
        headerData.insert(fieldNameUpper,fieldHeader);
        if(isVisible)
            visibleHeaderData.insert(fieldNameUpper,fieldHeader);
    }
    for(int i=0;i<model->query().record().count();i++){
        bool isVisible=visibleHeaderData.contains(model->query().record().field(i).name().toUpper());
//        qDebug()<<"Field["<<i<<"]="<<model->query().record().field(i).name()<<"  is visible["<<isVisible<<"]";
        model->setHeaderData(i,Qt::Horizontal, headerData[model->query().record().field(i).name().toUpper()],Qt::DisplayRole);
        model->setHeaderData(i,Qt::Horizontal, isVisible,VISIBLE_ROLE);
    }
    return model;

}

int WellDao::addRecentWell(QString idWell)
{
    QSqlQuery q(SQL(insert_well_to_catlog)
                .arg(SYS_RECENT_WELL)
                .arg(CFG(IDMainFieldName))
                ,APP->well());
    q.bindValue(0,idWell);
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}

int WellDao::addFavoriteWell(QString idWell)
{
    QSqlQuery q(SQL(insert_well_to_catlog)
                .arg(SYS_FAVORITE_WELL)
                .arg(CFG(IDMainFieldName))
                ,APP->well());
    q.bindValue(0,idWell);
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}

int WellDao::removeFavoriteWell(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(delete_well_from_catlog)
              .arg(SYS_FAVORITE_WELL)
              .arg(CFG(IDMainFieldName))
              );
    q.bindValue(0,idWell);
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}

QSqlQueryModel*  WellDao::wells()
{
    QSqlQueryModel*  model=new QSqlQueryModel(this);
    QStringList result;
    QString keytableMain=CFG(KeyTblMain);
    QString orderKey=MDL->tableOrderKey(keytableMain);


    QSqlQuery q(SQL(select_wells)
                .arg(keytableMain)
                .arg(SYS_DEL_REC)
                .arg(CFG(IDMainFieldName))
                .arg(CFG(IDMainFieldName))
                .arg(orderKey)
                ,APP->well());
    q.exec();
    PRINT_ERROR(q);
    model->setQuery(q);
    return processWells(model);
}
