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
#include "qwmrotatableproxymodel.h"
#include "qwmtablemodel.h"
#include "QSqlTableModel"
#include "QUuid"
#include "QDateTime"
#include <QSettings>
#include <QSqlDriver>
#include <QDir>
#include <QPair>
#include <QMessageBox>
#include "qwmprogressdialog.h"
WellDao * WellDao::_instance=nullptr;
QHash<QString ,QVariant> WellDao::_cache={};

BEGIN_SQL_DECLARATION(WellDao)
DECL_SQL(select_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 and w.%3=d.IDRec COLLATE NOCASE )  order by %5 ")
DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group COLLATE NOCASE and l.KeyTbl=t.KeyTbl COLLATE NOCASE order by l.DisplayOrder ")
DECL_SQL(select_spec_wells,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE and w.%3=d.IDRec  COLLATE NOCASE) and  exists(select *  from %6 r where w.%3=r.%7 COLLATE NOCASE)  order by %5 COLLATE NOCASE")
DECL_SQL(insert_well_to_catlog,"insert into %1 (%2) values(?)")
DECL_SQL(delete_well_from_catlog,"delete from %1 where %2=? COLLATE NOCASE")
DECL_SQL(select_record,"select  w.* from %1  w  where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE  and w.%3=d.IDRec COLLATE NOCASE) and w.%3=:id COLLATE NOCASE")
DECL_SQL(select_record2,"select  w.* from %1  w  where  not exists(select * from %2 d where  w.%3=d.IDRec COLLATE NOCASE) and w.%3=:id COLLATE NOCASE")
DECL_SQL(select_records,"select  w.* from %1  w  where  not exists(select * from %2 d where  w.%3=d.IDRec COLLATE NOCASE) %4  order by %5")
DECL_SQL(insert_record,"insert into %1 (%2) values( %3)")
DECL_SQL(delete_record,"delete from %1 where %2")
DECL_SQL(select_well_cnt_in_cat,"select  count(1) as cnt from %1  w  "
                                "where  not exists(select * from %2 d where w.%3=d.%4 COLLATE NOCASE and w.%3=d.IDRec  COLLATE NOCASE) "
                                "and w.%3=:idwell")
DECL_SQL(select_is_deleted,"select  count(1) as cnt from %1  w  "
                           "where   w.%2=:idwell  COLLATE NOCASE and w.%3=:idrec COLLATE NOCASE")
DECL_SQL(select_distinct_value,"select  distinct %2 as fv from %1  w  where fv is not null"
                               " order by fv ")
DECL_SQL(select_a_record ,"select * from %1 limit 1")
DECL_SQL(select_record_count_even_deleted,"select count(*) as c from %1 w where w.%2=:id COLLATE NOCASE")
DECL_SQL(select_pbtd_all,"select  depth, b.des,max(p.dttm) dttm,count(*) c from wvWellborePBTD  p,wvWellbore b where b.idrec=p.idrecparent and b.idwell=:idwell ")
DECL_SQL(select_td_all,"select b.des,max(DepthBtmActual)  depth from wvWellboreSize s,wvWellbore  b"
                       " where b.idwell=:idwell and b.IDRec=s.IDRecParent")


END_SQL_DECLARATION

WellDao::WellDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
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
//model是QWMTableModel
bool WellDao::processWells(QWMTableModel * model)
{
    model->setVisibleFields(APP->wellDisplayList());
    return true;
}
bool WellDao::processTable(QWMTableModel *sourceModel)
{

    return true;
}
//针对此井的过滤表
QWMTableModel *WellDao::table(QString tablename,QString idWell)
{
    QString key=QString("table.%1.%2").arg(tablename).arg(idWell);
    CS(key,QWMTableModel*);
    QWMTableModel *  model=new QWMTableModel(idWell,this,APP->well());
    QString filter=QString(" %1='%2' ").arg(CFG(IDWell)).arg(idWell);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(tablename);
    model->setFilter(filter);
    model->setSort(model->fieldIndex( UDL->tableOrderKey(tablename)),Qt::AscendingOrder);
    int cols=model->columnCount();
    model->select();
    int cols2=model->columnCount();
    while(model->canFetchMore())
        model->fetchMore();

    //    model->select();
    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    model->connectSignals();
    CI(key,model);
}
//获取table，并设置过滤，
//如果是顶级节点，则只过滤IDWell
//如果是子节点，则过滤IDRecParent

QWMRotatableProxyModel *WellDao::tableForEdit(const QString tablename,const QString  idWell,const  QString parentID)
{
    QString key=QString("tableForEdit.%1.%2.%3").arg(tablename).arg(idWell).arg(parentID);
    CS(key,QWMRotatableProxyModel*);
    QWMTableModel * sourceModel=table(tablename,idWell);


    QWMSortFilterProxyModel * proxyModel=new QWMSortFilterProxyModel(idWell,sourceModel);
    proxyModel->setSourceModel(sourceModel);
    proxyModel->setParentId(parentID);
    //    proxy->setShowGroup(true);
    SETTINGS;
    int mode= settings.value(QString(EDITOR_TABLE_ENTRY_PREFIX).arg(tablename),QWMRotatableProxyModel::V).toInt();
    proxyModel->setShowGroup(mode==QWMRotatableProxyModel::V);
    QWMRotatableProxyModel * rotateProxy=new QWMRotatableProxyModel(idWell,(QWMRotatableProxyModel::Mode)mode,proxyModel);
    rotateProxy->setSourceModel(proxyModel);

    //如果有parentid，则根据IDRecParent字段进行过滤
    QSqlRecord record=sourceModel->record();


    proxyModel->setFilterFunction( [=](int sourceRow, const QModelIndex &sourceParent)->bool {
        //        QModelIndex index0 = model->index(sourceRow, 0, sourceParent);
        //        QString idwell=index0.data(PK_ROLE).toString();

        QSqlRecord record=sourceModel->record(sourceRow);
        if(!parentID.isEmpty()){
            if(record.indexOf(CFG(ParentID))>=0){
                QString pidOfRow=record.value(CFG(ParentID)).toString();
                if(parentID.isNull()&& !pidOfRow.isNull()){
                    return false;
                }
                if(parentID.compare(pidOfRow,Qt::CaseInsensitive)!=0){
                    return false;
                }
            }
        }
        QString idWellThis=record.value(CFG(IDWell)).toString();
        QString idRec=record.value(CFG(ID)).toString();
        bool isDeleted=WELL->isDeletedRecord(idWell,idRec);
        return !isDeleted;
    });
    //    QVariant l = (source_left.model() ? source_left.model()->data(source_left, d->sort_role) : QVariant());
    //    QVariant r = (source_right.model() ? source_right.model()->data(source_right, d->sort_role) : QVariant());
    //        return QAbstractItemModelPrivate::isVariantLessThan(l, r, d->sort_casesensitivity, d->sort_localeaware);
    MDLTable * tableInfo=UDL->tableInfo(tablename);
    QStringList sortFieldsList,sortFieldsListTrimed;
    QString strOrderby=tableInfo->SQLOrderBy();
    if(!strOrderby.isNull()){
        sortFieldsList=strOrderby.split(",",Qt::SkipEmptyParts);
        foreach(QString field,sortFieldsList){
            sortFieldsListTrimed<<field.trimmed();
        }
    }
    if(sortFieldsListTrimed.isEmpty())
        sortFieldsListTrimed<<CFG(SysCD)<<CFG(SysMD);
    proxyModel->setSortFunction( [=](const QModelIndex &left, const QModelIndex &right)->bool {
        QStringList leftValues;
        QStringList rightValues;
        QWMTableModel * model=(QWMTableModel*)(left.model());
        QSqlRecord  leftRecord=model->record();
        foreach(QString field,sortFieldsListTrimed){
            int  fldIndex=leftRecord.indexOf(field);
            if(fldIndex<0)
                continue;
            QVariant lv= model->data(model->index(left.row(),fldIndex),SORT_ROLE);
            QVariant rv=model->data(model->index(right.row(),fldIndex),SORT_ROLE);
            int c=Utility::compare(lv,rv);
            if(c<0)
                return true;
            else if(c>0)
                return false;
        }
        return false;
    });
    proxyModel->sort(1);
    processTable(sourceModel);
    //    proxyModel->sort(1);
    CI(key,rotateProxy);
    //    return model;
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

bool WellDao::isDeletedWell(QString idwell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_is_deleted)
              .arg(CFG(SysRecDelTable)) //%1 wvSysDelRec
              .arg(CFG(IDMainFieldName)) //%2 IDWell
              .arg(CFG(ID))) //%4 IDWell
            ;
    q.bindValue(":idwell",idwell);
    q.bindValue(":idrec",idwell);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  QS(q,cnt).toUInt()>0;
    return false;
}
bool WellDao::isDeletedRecord(QString idWell, QString idRec)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_is_deleted)
              .arg(CFG(SysRecDelTable)) //%1 wvSysDelRec
              .arg(CFG(IDWell)) //%2 IDWell
              .arg(CFG(ID))) //%4 IDWell
            ;
    q.bindValue(":idwell",idWell);
    q.bindValue(":idrec",idRec);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  QS(q,cnt).toUInt()>0;
    return false;
}
int WellDao::addRecord(QString , QString )
{
    return 0;
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

int WellDao::undeleteItem(QString idWell, QString idRec)
{
    QSqlQuery q(APP->well());
    QString tableName=CFG(SysRecDelTable);
    QString where=QString(" %1='%2'  and  %3='%4' ")
            .arg(CFG(IDWell)).arg(idWell).arg(CFG(ID)).arg(idRec);
    q.prepare(SQL(delete_record)
              .arg(tableName) //%1 wvSysDelRec
              .arg(where)) //%2 where
            ;
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}

int WellDao::deleteItem(QString idWell, QString idRec,QString table)
{

    QSqlQuery q(APP->well());
    QStringList fields,values;
    fields<<QString("'%1'").arg(CFG(IDMainFieldName))<<QString("'%1'").arg(CFG(ID))<<QString("'%1'").arg("TblKey");
    values<<QString("'%1'").arg(idWell)<<QString("'%1'").arg(idRec)<<QString("'%1'").arg(table);
    q.prepare(SQL(insert_record)
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(fields.join(",")) //%1 wvWellHeader
              .arg(values.join(","))) //%4 IDWell
            ;
    q.exec();
    PRINT_ERROR(q);
    return q.numRowsAffected();
}
//选中的记录的显示信息，比如<NSDist>(<NSDist.unit>) -->25(m)
QString WellDao::recordDes(QString table, QSqlRecord record)
{
    MDLTable* tableInfo=UDL->tableInfo(table);
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
                    MDLField * fieldInfo=UDL->fieldInfo(table,var);
                    if(fieldInfo!=nullptr){
                        value=fieldInfo->refValue(value.toString());
                        if(Utility::isNumber(value))
                        {
                            //单位转换
                            value=fieldInfo->displayValue(value);
                        }
                    }
                    cachedValue.insert(var,value);
                    cachedTransferedValue.insert(var,value);
                    //                    rdResult.replace("<"+var+">",value);
                }else{
                    QString refVarName=var;
                    refVarName.replace(".unit","");
                    MDLUnitType *unitType=UDL->baseUnitOfField(table,refVarName);
                    QString baseUnit=unitType->BaseUnits();
                    MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType->KeyType());
                    if(userUnitInfo!=nullptr){
                        //显示为用户profile的单位制
                        QString userUnit=userUnitInfo->UserUnits();
                        cachedValue.insert(var,userUnit);
                        cachedTransferedValue.insert(var,userUnit);
                        //                        QVariant value=cachedValue[refVarName];
                        //                        value=MDL->unitBase2User(baseUnit,userUnit,value);
                        //                        QString fmtstr=userUnitInfo->UserFormat();
                        //                        cachedTransferedValue.remove(refVarName);
                        //                        cachedTransferedValue.insert(refVarName,Utility::format(fmtstr,value));//这个是转换后的数值,<NSDist.Unit>:10(ft)
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

QSqlRecord WellDao::refRecord(QString table, QString id)
{
    QSqlQuery q(APP->well());
    QString keyFld=CFG(ID);
    if(!UDL->tableHasField(table,keyFld)){
        keyFld=CFG(IDWell);
    }
    q.prepare(SQL(select_record2)
              .arg(table) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(keyFld) //%3 IDREC
              );
    q.bindValue(":id",id);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();
    }
}

QAbstractItemModel*  WellDao::wells(int type)
{
    QString key=QString("wells.%1").arg(type);
    CS(key,QAbstractItemModel*);

    QWMTableModel *  model=new QWMTableModel(QString(),this,APP->well());
    model->setTable(CFG(KeyTblMain));
    model->setSort(model->fieldIndex( UDL->tableOrderKey(CFG(KeyTblMain))),Qt::AscendingOrder);
    model->select();

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    processWells(model);

    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(QString(),model);
    proxy->setSourceModel(model);

    proxy->setFilterFunction( [model,type](int sourceRow, const QModelIndex &sourceParent)->bool {
        QModelIndex index0 = model->index(sourceRow, 0, sourceParent);
        QString idwell=index0.data(PK_ROLE).toString();
        if(type==QWMApplication::RECENT){
            bool isvalid=WELL->isRecentWell(idwell);
            return isvalid;
        }else if(type==QWMApplication::FAVORITE){
            bool isvalid=WELL->isFavoriteWell(idwell);
            return isvalid;
        }else{
            bool isvalid=WELL->isDeletedWell(idwell);
            return !isvalid;
        }
    });

    CI(key,proxy);
}
void WellDao::initRecord(QSqlRecord & record,QString idWell,QString parentID){
    //初始化记录
    // record:空白记录
    //    idWell：为null时，说明此记录是在well还没有创建时进行的初始化，需要新建一个uuid，如果有值，且记录无idrec字段，说明和well是1：1的，需要赋值给idWell。
    //    parentID:父表的id，如果parentID为空，则IDRecParent=IDRec

    int idIndex=record.indexOf(CFG(ID));
    QUuid id=QUuid::createUuid();
    QString strID=UUIDToString(id);
    if(idIndex>=0){// 如果有idrec，则idrec是初始化值，idwell需要引用 赋值
        record.setValue(idIndex,strID);
        INITFLD(record,CFG(IDWell),idWell);
        if(parentID.isNull()){
            INITFLD(record,CFG(ParentID),strID);
        }else
        {
            INITFLD(record,CFG(ParentID),parentID);
        }
    }else{ //如果 只有 idwell，则idwell需要初始化
        if(idWell.isNull()){ //wvWellHeader
            INITFLD(record,CFG(IDWell),strID);
            INITFLD(record,"WellName",strID);
        }else
        {
            INITFLD(record,CFG(IDWell),idWell);
        }
    }
    QDateTime now=QDateTime::currentDateTimeUtc();
    INITFLD(record,CFG(SysCD),now);
    INITFLD(record,CFG(SysCU),QString(""));
    INITFLD(record,CFG(SysMD),now);
    INITFLD(record,CFG(SysMU),QString(""));
    for(int i=0;i<record.count();i++){
        if(record.field(i).type()==QVariant::Bool){
            record.setValue(i,false);
            record.setGenerated(i,true);
        }
    }

}

QStringList WellDao::distinctValue(QString table,QString field){
    QString key=QString("distinctValue.%1.%2").arg(table).arg(field);
    //    CS(key,QStringList);

    QStringList result;
    QSqlQuery q(SQL(select_distinct_value).arg(table).arg(field),APP->well());
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,fv);
    }
    return result;
    //    CI(key,result);
}

QSqlQuery WellDao::records(QString table,QString idWell,QString parentID)
{
    MDLTable * tableInfo=UDL->tableInfo(table);
    QSqlRecord record=aRecord(table);
    PARENT_ID_FLD(parentFld,record);
    PK_FLD(idFld,record);
    QString additionalCri;
    //    DECL_SQL(select_records,"select  w.* from %1  w  where  not exists(select * from %2 d where  w.%3=d.IDRec COLLATE NOCASE) %4  order by %5")
    if(parentID.isNull() && parentFld==CFG(ParentID)){
        //        顶级记录，如果有IDRecParent，则应该=IDRec， w.IDwell=：idwell and  w.IDRec=w.IDRecParent
        additionalCri=QString(" and w.%1='%2' COLLATE NOCASE and  w.%3=w.%4  COLLATE NOCASE ").arg(CFG(IDWell)).arg(idWell).arg(CFG(ID)).arg(CFG(ParentID));

    }else if(parentID.isNull() && parentFld==CFG(IDWell)){
        //顶级记录，无IDRecParent,w.IDWell=:idwell
        additionalCri=QString(" and w.%1='%2' COLLATE NOCASE  ").arg(CFG(IDWell)).arg(idWell);

    }else if(!parentID.isNull() && parentFld==CFG(ParentID)){
        //子记录，同时要满足IDWell和IDRecPARENT ,w.IDwell=:idWell and w.IDRecParent=parentID
        additionalCri=QString(" and w.%1='%2' COLLATE NOCASE and  w.%3='%4'  COLLATE NOCASE ").arg(CFG(IDWell)).arg(idWell).arg(CFG(ParentID)).arg(parentID);
    }else if(!parentID.isNull() && parentFld==CFG(IDWell)){
        //子记录，无IDRecPARENT ,w.IDwell=:idWell
        additionalCri=QString(" and w.%1='%2' COLLATE NOCASE  ").arg(CFG(IDWell)).arg(idWell);
    }

    //    QSqlQueryModel* model=new QSqlQueryModel(this);
    //    DECL_SQL(select_records,"select  w.* from %1  w  where  not exists(select * from %2 d where  w.%3=d.IDRec COLLATE NOCASE) and w.%4=:parentID  COLLATE NOCASE %6  order by %5")
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_records)
              .arg(table) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(idFld) //%3 IDREC
              .arg(additionalCri) //%4 idrecparent/idwell
              .arg(tableInfo->SQLOrderBy())// order by
              );
    q.bindValue(":parentID",parentID);
    q.exec();
    PRINT_ERROR(q);
    return q;
}

QSqlRecord WellDao::aRecord(QString table)
{
    QSqlQuery q(SQL(select_a_record).arg(table),APP->well());
    q.exec();
    return q.record();
}

bool WellDao::hasRecord(QString table, QString idrec)
{
    //    select count(*) as c from %1 w where w.%2=:id COLLATE NOCASE
    QStringList result;
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_record_count_even_deleted).arg(table).arg(CFG(ID)));
    q.bindValue(":id",idrec);
    q.exec();
    PRINT_ERROR(q);
    if(q.next()){
        return q.value("c").toInt()>0;
    }
    return false;
}

void WellDao::resetCache()
{
    _cache.clear();
}

bool WellDao::duplicateWellHeader(QString idWell, QList<QPair<QString, QStringList> > &stagedTables,
                                  QHash<QString, QString> & mapDuplicatedRecords,
                                  QHash<QString, int> & mapDuplicatedTables,
                                  QHash<QString ,QList<PendingDuplicateItem>>&mapPendingIDs,
                                  QStringList & errors)
{
    QString mainTable=CFG(KeyTblMain);
    QSqlTableModel * model=new QSqlTableModel(nullptr,APP->well());
    model->setTable(mainTable);
    QString filter=QString(" %1='%2' ").arg(CFG(IDWell)).arg(idWell);
    model->setFilter(filter);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    bool result=false;
    if(model->rowCount()>0){
        QSqlRecord record=model->record(0);
        QUuid uuid=QUuid::createUuid();
        QString newIdWell=UUIDToString(uuid);
        QString des=WELL->recordDes(mainTable,record);
        record.setValue("WellName",QString("%1-%2").arg(tr("副本 - ")).arg(des));
        record.setValue(CFG(IDWell),newIdWell);
        bool success = model->insertRecord(0,record);
        ADD_DUP_ERROR(success,model,errors,mainTable);
        if(success){
            success=model->submitAll();
            ADD_DUP_ERROR(success,model,errors,mainTable);
            if(success){
                mapDuplicatedRecords.insert(idWell,newIdWell);
                mapDuplicatedTables.insert(mainTable.toLower(),1);
                result=true;
            }else{

            }
        }else{

        }
    }else
    {
        errors<<tr("%1查找失败。").arg(mainTable);
    }
    model->deleteLater();
    return result;
}

bool WellDao::initStageTables(QString idWell,QList<QPair<QString, QStringList> > &stagedTables,
                              QHash<QString, QString> & mapDuplicatedRecords,
                              QHash<QString, int> &mapDuplicatedTables,
                              QHash<QString ,QList<PendingDuplicateItem>>&mapPendingIDs,
                              QStringList & errors)
{
    QStringList depends;
    QList<MDLTable*> tables=MDL->phyicalTables();
    QString mainTable=CFG(KeyTblMain);
    bool result=false;
    foreach(MDLTable * table,tables){
        depends.clear();
        if(table->KeyTbl().compare(mainTable,Qt::CaseInsensitive)==0)
        {
            continue;
        }
        QString  parentTable= MDL->parentTable(table->KeyTbl());
        if(!parentTable.isNull() && parentTable.compare(mainTable,Qt::CaseInsensitive)!=0){
            depends<<parentTable.toLower();
        }
        QList<MDLField*> fields=UDL->tableRefFields(table->KeyTbl());
        foreach(MDLField * refField,fields){
            if(IS_SPEC_REF_FIELD(refField)){
                depends<<"*";
            }else{
                QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(refField->KeyTbl(),refField->KeyFld());
                QStringList tables;
                foreach(MDLFieldLookup * l,fls){
                    if(l->TableKey()){
                        QString tableName=l->LookupItem();
                        if(tableName.compare(table->KeyTbl(),Qt::CaseInsensitive)!=0){
                            depends<<tableName.toLower();
                        }
                    }
                }
            }
        }
        depends.removeDuplicates();
        stagedTables.append(QPair<QString,QStringList>(table->KeyTbl(),depends));
    }

    result=true;
    return result;

}

bool WellDao::handleStageTables(QString idWell, QList<QPair<QString, QStringList> > &stagedTables,
                                QHash<QString, QString> & mapDuplicatedRecords,
                                QHash<QString, int> & mapDuplicatedTables,
                                QHash<QString ,QList<PendingDuplicateItem>>&mapPendingIDs,
                                QStringList & errors)
{
    bool result=false;//只要有新复制的表，就返回true，否则返回false
    for(int i=0;i<stagedTables.size();i++){
        bool success=duplicateTable(idWell,stagedTables[i].first,stagedTables,mapDuplicatedRecords,mapDuplicatedTables ,mapPendingIDs,errors);
    }
    if(mapPendingIDs.size()>0){
        errors<<tr("未决的记录:%1").arg(mapPendingIDs.keys().join("/"));
        return false;
    }else{
        return true;
    }
    //    int i=0;
    //    bool hasOneNotDependAll=false;
    //    int duplicatedRecords=0;
    //    int stage=0;//0 正常，1 松弛
    //    QStringList localErrors;
    //    while(stagedTables.length()>0){
    //        //        if(stagedTables.length()==1){
    //        //            mapDuplicatedTables.insert("*","*");
    //        //        }
    //        QPair<QString, QStringList> stagedTable=stagedTables[i];
    //        bool depnedsResolved=true;

    //        if(stage==0){
    //            foreach(QString depend,stagedTable.second){
    //                if(!mapDuplicatedTables.contains(depend)){
    //                    depnedsResolved=false;
    //                    break;
    //                }
    //            }
    //        }else if(stage==1)
    //        {
    //            depnedsResolved=true;
    //        }
    //        if(depnedsResolved){
    //            int duplicatedRecsOfTable;
    //            bool tableDupSuccessed=duplicateTable(idWell,stagedTable.first,stagedTables,mapDuplicatedRecords,mapDuplicatedTables,mapPendingIDs,localErrors,duplicatedRecsOfTable);
    //            duplicatedRecords+=duplicatedRecsOfTable;
    //            if(tableDupSuccessed){
    //                result=true;
    //                stagedTables.removeAt(i);
    //            }else{
    //                i++;
    //            }
    //            if(stage==0)
    //                hasOneNotDependAll=true;
    //        }else{
    //            i++;
    //        }

    //        if(i==stagedTables.size()){
    //            if(duplicatedRecords==0&&stage==1){
    //                errors<<localErrors;
    //                return result;
    //            }
    //            if(!hasOneNotDependAll && stage==0){//如果没有不依赖于其他项的存在，只有硬上这些依赖于其他项的了
    //                mapDuplicatedTables.insert("*",0);
    //                stage=1;
    //            }else if(stage==0){
    //                hasOneNotDependAll=false;
    //            }
    //            localErrors.clear();
    //            duplicatedRecords=0;
    //        }
    //        i=i%stagedTables.size();
    //    }
    //    errors<<localErrors;
    return result;
}

bool WellDao::duplicateTable(QString idWell, QString table,
                             QList<QPair<QString, QStringList> > &stagedTables,
                             QHash<QString, QString> & mapDuplicatedRecords,
                             QHash<QString, int> & mapDuplicatedTables,
                             QHash<QString ,QList<PendingDuplicateItem>>&mapPendingIDs,
                             QStringList & errors)
{
    QString parentTable=MDL->parentTable(table);
    QSqlTableModel * model=new QSqlTableModel(nullptr,APP->well());
    MDLTable * tableInfo=UDL->tableInfo(table);
    model->setTable(table);
    model->setFilter(QString(" %1='%2' ").arg(CFG(IDWell)).arg(idWell));
    model->select();
    while(model->canFetchMore())
        model->fetchMore();

    int recordcount=model->rowCount();
    QStringList refs;
    QSqlRecord rec=model->record();
    QString pkField=CFG(ID);
    bool useIdWellAsPk=false;
    if(!UDL->tableHasField(table,CFG(ID))){
        pkField=CFG(IDWell);
        useIdWellAsPk=true;
    }
    for(int f=0;f<rec.count();f++){
        QString fieldname=rec.fieldName(f);
        if(fieldname.compare(CFG(ParentID),Qt::CaseInsensitive)==0){
            refs<<fieldname;
            continue;
        }

    }
    QList<MDLField *> reffields=UDL->tableRefFields(table);
    foreach(MDLField* f,reffields){
        refs<<f->KeyFld();
    }
    refs.removeDuplicates();
    bool hasProcessANewRecord=false;;
    int processedRecords=0;
    int processedPending=0;
    int i=0;

    QStringList unduplicatedRecords;
    while(true){

        if(i>=recordcount){
            if(processedRecords==recordcount){
                mapDuplicatedTables.insert(table.toLower(),processedRecords);//复制完毕
                //                duplicatedRecs=processedRecords;
                return true;
            }
            if(!hasProcessANewRecord && processedRecords<recordcount){//无可处理的记录，也未复制完毕，报错
                errors<<QString(tr("表 %1 有未解决依赖项的条目:%2")).arg(table).arg(unduplicatedRecords.join("/"));
                //                mapDuplicatedTables.insert(table.toLower(),processedRecords);//复制完毕
                //                duplicatedRecs=processedRecords;
                return false;
            }
            hasProcessANewRecord=false;
            unduplicatedRecords.clear();
        }
        i=i%recordcount;
        QSqlRecord rec=model->record(i);
        QSqlRecord dupRec(rec);
        QUuid uuid=QUuid::createUuid();
        QString newIdrec=UUIDToString(uuid);
        QString oldIdRec=rec.value(pkField).toString();

        INITFLD(dupRec,CFG(IDWell),mapDuplicatedRecords[idWell]);
        if(!useIdWellAsPk){
            dupRec.setValue(pkField,newIdrec);
        }else{
            newIdrec=mapDuplicatedRecords[idWell];
        }

        bool resolved=true;
        QString oriIdrec=rec.value(pkField).toString();
        if(mapDuplicatedRecords.contains(oriIdrec)){
            i++;
            continue;
        }

        foreach(QString rf,refs){
            QString refValue=rec.value(rf).toString();
            if(!refValue.isNull()&&!refValue.isEmpty()){
                if(refValue.compare(oriIdrec,Qt::CaseInsensitive)==0){//指向自身的引用
                    dupRec.setValue(rf,newIdrec);
                }else{
                    if(!mapDuplicatedRecords.contains(refValue)){
                        PendingDuplicateItem pi;
                        pi.field=rf;
                        pi.table=table;
                        pi.pkField=pkField;
                        pi.pkValue=newIdrec;
                        if(!mapPendingIDs.contains(refValue)){
                            mapPendingIDs.insert(refValue,QList<PendingDuplicateItem>());
                        }
                        QList<PendingDuplicateItem> pendings=mapPendingIDs[refValue];
                        pendings.append(pi);
                        mapPendingIDs.insert(refValue,pendings);
                        dupRec.setValue(rf,QString());//未决的，设为空
                        resolved=false;
                        break;//有未解决的依赖
                    }else{
                        //从已经复制的记录中替换引用
                        dupRec.setValue(rf,mapDuplicatedRecords[refValue]);
                    }
                }
            }
        }
        //        if(resolved){
        processedRecords++;
        hasProcessANewRecord=true;
        int insertPos=model->rowCount();
        bool success=model->insertRecord(insertPos,dupRec);
        ADD_DUP_ERROR(success,model,errors,table);
        if(success){
            success=model->submitAll();
            ADD_DUP_ERROR(success,model,errors,table);
            if(success){
                mapDuplicatedRecords.insert(rec.value(CFG(ID)).toString(),newIdrec);
                success= processPendingID(oldIdRec,newIdrec,mapPendingIDs,errors);
                if(success){
                    processedPending++;
                }
                i++;
            }
            else{
                //                    duplicatedRecs=processedRecords+processedPending;
                return false;
            }
        }else{
            //                duplicatedRecs=processedRecords+processedPending;
            return false;
        }
        //        }else{
        //            PK_VALUE(pk,rec);
        //            unduplicatedRecords<<pk;
        //            i++;
        //        }

    }
    model->deleteLater();
    //    duplicatedRecs=processedRecords;
    return false;
}

bool WellDao::processPendingID(QString oriId, QString newId, QHash<QString, QList<PendingDuplicateItem> > & pendingIDs, QStringList &errors)
{
    bool success=true;
    if(pendingIDs.contains(oriId)){
        //处理遗留的依赖
        QList<PendingDuplicateItem> listOfPendingItems=pendingIDs[oriId];
        foreach(PendingDuplicateItem pi,listOfPendingItems){
            QSqlQuery q(APP->well());
            QString stmt=QString("update %1 set  %2='%3' where %4='%5'  COLLATE NOCASE ").arg(pi.table).arg(pi.field).arg(newId).arg(pi.pkField).arg(pi.pkValue);
            q.prepare(stmt);
            success=q.exec();
            PRINT_ERROR(q);
            if(q.numRowsAffected()<=0){
                success=false;
                break;
            }
        }
        if(success){
            pendingIDs.remove(oriId);
        }
    }
    return success;
}

bool WellDao::duplicateWell(QString idWell, QWidget *parent)
{
    QWMProgressDialog * progressBar=new  QWMProgressDialog(parent);
    QStringList errors;
    QList<QPair<QString, QStringList> > stagedTables;
    QHash<QString, QString> mapDuplicatedRecords;
    QHash<QString, int> mapDuplicatedTables;
    QHash<QString,QList<PendingDuplicateItem>> mapPendingIDs;
    progressBar->setWindowModality(Qt::WindowModal);
    progressBar->show();
    int progress=0;
    bool success=false;
    progressBar->on_progress(tr("复制WellHeader"),progress);
    APP->well().transaction();
    success=duplicateWellHeader(idWell,stagedTables,mapDuplicatedRecords,mapDuplicatedTables,mapPendingIDs,errors);
    QCoreApplication::processEvents();
    if(!success)
    {
        progressBar->on_progress(tr("复制失败"),100);
        QCoreApplication::processEvents();
        APP->well().rollback();
        QMessageBox::warning(parent,tr("失败"),errors.join(";"));
        return false;
    }
    progressBar->on_progress(tr("复制WellHeader完成"),10);
    QCoreApplication::processEvents();
    success =initStageTables(idWell,stagedTables,mapDuplicatedRecords,mapDuplicatedTables,mapPendingIDs,errors);
    QCoreApplication::processEvents();
    if(!success)
    {
        progressBar->on_progress(tr("复制失败"),100);
        QCoreApplication::processEvents();
        QMessageBox::warning(parent,tr("失败"),errors.join(";"));
        APP->well().rollback();
        return false;
    }
    progressBar->on_progress(tr("初始化表完成"),30);
    QCoreApplication::processEvents();
    success=handleStageTables(idWell,stagedTables,mapDuplicatedRecords,mapDuplicatedTables,mapPendingIDs,errors);
    QCoreApplication::processEvents();
    APP->well().commit();
    if(!success)
    {
        progressBar->on_progress(tr("复制完成"),100);
        QMessageBox::warning(parent,tr("存在问题"),errors.join(";"));
        QCoreApplication::processEvents();
        //        APP->well().rollback();
        //        return false;
    }else{
        progressBar->on_progress(tr("复制成功结束"),100);
        QCoreApplication::processEvents();
    } ;
}


QString WellDao::PBTDAll(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_pbtd_all));
    q.bindValue(":idwell",idWell);
    q.exec();
    PRINT_ERROR(q);
    QString result;

    if(q.next()){
        int count=q.value("c").toInt();
        QString des=q.value("des").toString();
        QString depth=q.value("depth").toString();
        if(count>0){
            result=QString("%1-%2").arg(q.value("des").toString()).arg(q.value("depth").toString());
        }
    }
    return result;
}

QString WellDao::TDAll(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_td_all));
    q.bindValue(":idwell",idWell);
    q.exec();
    PRINT_ERROR(q);
    QString result;

    if(q.next()){
        result=QString("%1-%2").arg(q.value("des").toString()).arg(q.value("depth").toString());
    }
    return result;
}
double WellDao::TD(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_td_all));
    q.bindValue(":idwell",idWell);
    q.exec();
    PRINT_ERROR(q);
    double result;

    if(q.next()){
        result=q.value("depth").toDouble();
    }
    return result;
}


