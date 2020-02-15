#include "welldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "mdldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlRecord"
#include "QDebug"
#include "QSqlField"
#include "utility.h"
#include "qwmsortfilterproxymodel.h"
#include "qwmtablemodel.h"
#include "QSqlTableModel"
WellDao * WellDao::_instance=nullptr;

WellDao::WellDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 and w.%3=d.IDRec COLLATE NOCASE )  order by %5 ");
    DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group COLLATE NOCASE and l.KeyTbl=t.KeyTbl COLLATE NOCASE order by l.DisplayOrder ");
    DECL_SQL(select_spec_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE and w.%3=d.IDRec  COLLATE NOCASE) and  exists(select *  from %6 r where w.%3=r.%7 COLLATE NOCASE)  order by %5 COLLATE NOCASE");
    DECL_SQL(insert_well_to_catlog,"insert into %1 (%2) values(?)");
    DECL_SQL(delete_well_from_catlog,"delete from %1 where %2=? COLLATE NOCASE");
    DECL_SQL(select_record,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE  and w.%3=d.IDRec COLLATE NOCASE) and w.%3=:id COLLATE NOCASE");
    DECL_SQL(insert_record,"insert into %1 (%2) values( %3)")
    DECL_SQL(select_well_cnt_in_cat,"select  count(1) as cnt from %1  w  \
where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE and w.%3=d.IDRec  COLLATE NOCASE) \
and w.%3=:idwell");
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

QAbstractItemModel *WellDao::recentWells()
{

//    QSqlQueryModel*  model=new QSqlQueryModel(this);
//    QStringList result;
//    QString keytableMain=CFG(KeyTblMain);
//    QString orderKey=MDL->tableOrderKey(keytableMain);


//    QSqlQuery q(SQL(select_spec_wells)
//                .arg(keytableMain)
//                .arg(CFG(SysRecDelTable))
//                .arg(CFG(IDMainFieldName))
//                .arg(CFG(IDMainFieldName))
//                .arg(orderKey)
//                .arg(CFG(SysRecentTable))
//                .arg(CFG(IDMainFieldName))
//                ,APP->well());
//    q.exec();
//    PRINT_ERROR(q);
//    model->setQuery(q);
//    return processWells(model);

    QSqlTableModel *  model=new QWMTableModel(this,APP->well());
    model->setTable(CFG(KeyTblMain));
    model->select();

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    processWells(model);
    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(QWMApplication::RECENT,this);
    proxy->setSourceModel(model);
    return proxy;

}

QAbstractItemModel *WellDao::favoriteWells()
{
//    QSqlQueryModel*  model=new QSqlQueryModel(this);
//    QStringList result;
//    QString keytableMain=CFG(KeyTblMain);
//    QString orderKey=MDL->tableOrderKey(keytableMain);


//    QSqlQuery q(SQL(select_spec_wells)
//                .arg(keytableMain)
//                .arg(CFG(SysRecDelTable))
//                .arg(CFG(IDMainFieldName))
//                .arg(CFG(IDMainFieldName))
//                .arg(orderKey)
//                .arg(CFG(SysFavoriteTable))
//                .arg(CFG(IDMainFieldName))
//                ,APP->well());
//    q.exec();
//    PRINT_ERROR(q);
//    model->setQuery(q);
//    return processWells(model);
    QSqlTableModel *  model=new QWMTableModel(this,APP->well());
    model->setTable(CFG(KeyTblMain));
    model->select();

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    processWells(model);
    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(QWMApplication::FAVORITE,this);
    proxy->setSourceModel(model);
    return proxy;
}

QAbstractItemModel *WellDao::processWells(QSqlQueryModel * model)
{

    QList<MDLFieldVisible*> visibleFieldsList=MDL->tableMainHeadersVisible();
    QStringList visibleFields;
    QStringList visibleHeaders;
    QHash<QString,QString> visibleHeaderData;
    QHash<QString,QString> headerData;

    foreach(MDLFieldVisible * field,visibleFieldsList){
        QString fieldName=field->KeyFld();
        bool isVisible=field->Visible()>0;
        QString fieldNameUpper=fieldName.toUpper();
        QString fieldHeader=field->CaptionLong();
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
        QString fieldName=model->record().field(i).name();
        if(APP->wellDisplayList().contains( fieldName,Qt::CaseInsensitive)||isVisible){
            model->setHeaderData(i,Qt::Horizontal, true,VISIBLE_ROLE);
        }else
        {
           model->setHeaderData(i,Qt::Horizontal, false,VISIBLE_ROLE);
        }
    }
    return model;

}

QSqlRecord WellDao::well(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_record)
              .arg(CFG(KeyTblMain)) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(CFG(IDMainFieldName)) //%3 IDWell
              .arg(CFG(IDMainFieldName))) //%4 IDWell
              ;
    q.bindValue(":id",idWell);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();//当在当前单位制下，无用户单位时，应使用基本单位
    }
}
bool WellDao::isRecentWell(QString idwell){

    QSqlQuery q(APP->well());
    q.prepare(SQL(select_well_cnt_in_cat)
               .arg(CFG(SysRecentTable)) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(CFG(IDMainFieldName)) //%3 IDWell
              .arg(CFG(IDMainFieldName))) //%4 IDWell
              ;
    q.bindValue(":idwell",idwell);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  QS(q,cnt).toUInt()>0;
    return false;
}
bool WellDao::isFavoriteWell(QString idwell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_well_cnt_in_cat)
              .arg(CFG(SysFavoriteTable)) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(CFG(IDMainFieldName)) //%3 IDWell
              .arg(CFG(IDMainFieldName))) //%4 IDWell
              ;
    q.bindValue(":idwell",idwell);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  QS(q,cnt).toUInt()>0;
    return false;
}
int WellDao::addRecord(QString table, QString parentId)
{

}

int WellDao::addRecentWell(QString idWell)
{
    QSqlQuery q(SQL(insert_well_to_catlog)
                .arg(CFG(SysRecentTable))
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
                .arg(CFG(SysFavoriteTable))
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
              .arg(CFG(SysFavoriteTable))
              .arg(CFG(IDMainFieldName))
              );
    q.bindValue(0,idWell);
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}
//选中的记录的显示信息，比如<NSDist>(<NSDist.unit>) -->25(m)
QString WellDao::recordDes(QString table, QSqlRecord record)
{
    MDLTable* tableInfo=MDL->tableInfo(table);
    QString rd=tableInfo->RecordDes();
    QString rdResult=rd;
    QHash<QString,QVariant> cachedValue;
    QHash<QString,QVariant> cachedTransferedValue;
    if(!rd.isNull()&&!rd.isEmpty()){
        QRegExp pat("<([\\w\.]*)>");
        int count = 0;
        int pos = 0;
        while ((pos = pat.indexIn(rd, pos)) != -1) {
            ++count;
            QString var=pat.cap(1);
            if(!cachedValue[var].isNull()){
//                rdResult.replace("<"+var+">",cachedValue[var]);
            }else{
                if(!var.contains(".unit")){
                    QVariant value=record.value(record.indexOf(var));
                    cachedValue.insert(var,value);
                    cachedTransferedValue.insert(var,value);
//                    rdResult.replace("<"+var+">",value);
                }else{
                    QString refVarName=var.replace(".unit","");
                    MDLUnitType *unitType=MDL->baseUnitOfField(table,refVarName);
                    QString baseUnit=unitType->BaseUnits();
                    MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType->KeyType());
                    if(userUnitInfo!=nullptr){

                        //需要转换
                        QString userUnit=userUnitInfo->UserUnits();
                        cachedValue.insert(var,userUnit);
                        cachedTransferedValue.insert(var,userUnit);
                        QVariant value=cachedValue[refVarName];
                        value=MDL->unitBase2User(baseUnit,userUnit,value);
                        QString fmtstr=userUnitInfo->UserFormat();
                        cachedTransferedValue.remove(refVarName);
                        cachedTransferedValue.insert(refVarName,Utility::format(fmtstr,value));//这个是转换后的数值,<NSDist.Unit>:10(ft)
                    }else{
                        cachedValue.insert(var,baseUnit);
                        cachedTransferedValue.insert(var,baseUnit);
                    }
                }
            }
//            qDebug()<<"pos:"<<pos<<",c:"<<count<<","<<pat.cap(1).toUtf8().data();
            pos += pat.matchedLength();
        }
        foreach(QString key,cachedTransferedValue.keys()){
            QVariant value=cachedTransferedValue[key];
            rdResult.replace("<"+key+">",value.toString());
        }
    }
    return rdResult;
}

QAbstractItemModel*  WellDao::wells()
{
//    QSqlQueryModel*  model=new QSqlQueryModel(this);
//    QStringList result;
//    QString keytableMain=CFG(KeyTblMain);
//    QString orderKey=MDL->tableOrderKey(keytableMain);


//    QSqlQuery q(SQL(select_wells)
//                .arg(keytableMain)
//                .arg(CFG(SysRecDelTable))
//                .arg(CFG(IDMainFieldName))
//                .arg(CFG(IDMainFieldName))
//                .arg(orderKey)
//                ,APP->well());
//    q.exec();
//    PRINT_ERROR(q);
//    model->setQuery(q);
    QSqlTableModel *  model=new QWMTableModel(this,APP->well());
    model->setTable(CFG(KeyTblMain));
    model->select();

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    processWells(model);
    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(QWMApplication::ALL,this);
    proxy->setSourceModel(model);
    return proxy;
}
