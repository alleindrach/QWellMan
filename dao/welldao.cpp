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

            DECL_SQL(select_is_deleted,"select  count(1) as cnt from %1  w  \
                     where   w.%2=:idwell  COLLATE NOCASE and w.%3=:idrec COLLATE NOCASE  \
            ");
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
bool WellDao::processTable(QWMTableModel *model)
{
    return true;
}
QWMRotatableProxyModel *WellDao::table(QString tablename)
{
    QString key=QString("tableForEdit.%1.%2").arg(tablename);
    CS(key,QWMRotatableProxyModel*);


    QSqlTableModel *  model=new QWMTableModel(this,APP->well());
    model->setTable(tablename);
    model->setSort(model->fieldIndex( MDL->tableOrderKey(tablename)),Qt::AscendingOrder);

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }

    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(model);
    proxy->setSourceModel(model);
    proxy->setShowGroup(true);
    QWMRotatableProxyModel * rotateProxy=new QWMRotatableProxyModel(QWMRotatableProxyModel::V,proxy);
    rotateProxy->setSourceModel(proxy);
    CI(key,rotateProxy);

}
//获取table，并设置过滤，
//如果是顶级节点，则只过滤IDWell
//如果是子节点，则过滤IDRecParent

QWMRotatableProxyModel *WellDao::tableForEdit(const QString& tablename,const QString & IDWell,const  QString& parentID)
{
    QWMRotatableProxyModel * model=table(tablename);
    PX(proxyModel,model);
    SX(sourceModel,model);
    //如果有parentid，则根据IDRecParent字段进行过滤
    QSqlRecord record=sourceModel->record();
    int recn=record.count();
    if(!parentID.isNull() && !parentID.isEmpty()){

        if(record.indexOf(CFG(ParentID))>=0)
        {
            proxyModel->setFilterKeyColumn(record.indexOf(CFG(ParentID)));
            proxyModel->setFilterFixedString(parentID);
            proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        }else{
            proxyModel->setFilterFixedString(parentID);
            proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            proxyModel->setFilterKeyColumn(record.indexOf(CFG(IDWell)));
        }
    }else
    {
        proxyModel->setFilterFixedString(IDWell);
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterKeyColumn(record.indexOf(CFG(IDWell)));
    }
    processTable(sourceModel);
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

int WellDao::deleteItem(QString idWell, QString idRec)
{
    QSqlQuery q(APP->well());
    QStringList fields,values;
    fields<<QString("%1").arg(CFG(IDMainFieldName))<<QString("%1").arg(CFG(ID));
    values<<QString("'%1'").arg(idWell)<<QString("'%1'").arg(idRec);
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

QAbstractItemModel*  WellDao::wells(int type)
{
    QString key=QString("wells.%1").arg(type);
    CS(key,QAbstractItemModel*);

    QWMTableModel *  model=new QWMTableModel(this,APP->well());
    model->setTable(CFG(KeyTblMain));
    model->setSort(model->fieldIndex( MDL->tableOrderKey(CFG(KeyTblMain))),Qt::AscendingOrder);
    model->select();

    if(model->lastError().isValid())
    {
        QString e=model->lastError().text();
        qDebug()<<e;
    }
    processWells(model);

    QWMSortFilterProxyModel * proxy=new QWMSortFilterProxyModel(model);
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
    QWMRotatableProxyModel * rotateProxy=new QWMRotatableProxyModel(QWMRotatableProxyModel::H,proxy);
    rotateProxy->setSourceModel(proxy);
    CI(key,rotateProxy);
}
void WellDao::initRecord(QSqlRecord & record,QString idWell,QString parentID){
    //初始化记录
    // record:空白记录
//    idWell：为null时，说明此记录是在well还没有创建时进行的初始化，需要新建一个uuid，如果有值，且记录无idrec字段，说明和well是1：1的，需要赋值给idWell。
//    parentID:父表的id
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
