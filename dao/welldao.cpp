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
#include  <QSqlDriver>
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
DECL_SQL(select_pbtd_all,"select  depth, b.des,max(dttm) from wvWellborePBTD  p,wvWellbore b where b.idrec=p.idrecparent and b.idwell=:idwell ")
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
    QList<MDLFieldVisible*> visibleFieldsList=MDL->tableMainHeadersVisible();
    model->setVisibleFields(APP->wellDisplayList());
    return true;
}
bool WellDao::processTable(QWMTableModel *sourceModel)
{
    sourceModel->select();

    while(sourceModel->canFetchMore())
        sourceModel->fetchMore();

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
    //    proxy->setShowGroup(true);
    QSettings settings;
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
    q.prepare(SQL(select_record2)
              .arg(table) //%1 wvWellHeader
              .arg(CFG(SysRecDelTable)) //%2 wvSysDelRec
              .arg(CFG(ID)) //%3 IDREC
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

QString WellDao::PBTDAll(QString idWell)
{
    QSqlQuery q(APP->well());
    q.prepare(SQL(select_pbtd_all));
    q.bindValue(":idwell",idWell);
    q.exec();
    PRINT_ERROR(q);
    QString result;

    if(q.next()){
        result=QString("%1-%2").arg(q.value("des").toString()).arg(q.value("depth").toString());
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


