#include "udldao.h"
#include "mdldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include "QSqlRecord"
#include "QDebug"
#include <QSqlField>
#include "iostream"
#include <QtMsgHandler>
#include <QMessageLogContext>
#include "QMetaObject"
#include "QMetaType"
#include "qlogging.h"
#include "mdldao.h"
//;
QHash<QString ,QVariant> UDLDao::_cache={};

BEGIN_SQL_DECLARATION(UDLDao)

DECL_SQL(select_profile_set,"select  * from pceUDLProfile p          order by p.DisplayOrder")
DECL_SQL(select_table_group_by_profile,"select d.* from pceUDLSetTblGroupData d,pceUDLSetTblGroup g,pceUDLProfile p "
                                       "         where p.KeyProfile=:profile COLLATE NOCASE and p.KeySetTblGroup=g.KeySet COLLATE NOCASE and g.KeySet=d.KeySet COLLATE NOCASE "
                                       "        order by d.DisplayOrder ")

DECL_SQL(select_tables_of_group_profile,"select tl.* from pceUDLSetTblGroupTblData t,pceUDLProfile p,pceListTbl tl "
                                        "where p.KeyProfile=:profile COLLATE NOCASE and tl.Calculated=false  and p.KeySetTblGroup=t.KeySet COLLATE NOCASE and t.GroupName=:group COLLATE NOCASE and tl.KeyTbl=t.KeyTbl COLLATE NOCASE order by t.DisplayOrder ")
//qInstallMessageHandler(outputMessage);
DECL_SQL(select_hidden_tables_of_profile,"select hd.KeyTbl from pceUDLProfile p ,pceUDLSetTblHiddenData hd "
                                         "         where p.KeyProfile=:profile COLLATE NOCASE and p.KeySetTblHidden=hd.KeySet COLLATE NOCASE")
DECL_SQL(select_hidden_fields_of_profile,"select hd.KeyFld from pceUDLProfile p ,pceUDLSetFldHiddenData hd"
                                         "where p.KeyProfile=:profile COLLATE NOCASE "
                                         "and p.KeySetFldHidden=hd.KeySet COLLATE NOCASE "
                                         "and hd.KeyTbl=:table COLLATE  NOCASE")

DECL_SQL(select_table_visible_fields_in_order_by_group,"select * from pceListTblFld f where  f.GroupName=:group COLLATE NOCASE "
                                                       "and f.KeyTbl=:table COLLATE NOCASE  and not  exists ( "
                                                       "select hd.KeyFld from pceUDLProfile p ,pceUDLSetFldHiddenData hd "
                                                       " where p.KeyProfile=:profile COLLATE NOCASE "
                                                       "and p.KeySetFldHidden=hd.KeySet COLLATE NOCASE "
                                                       "and hd.KeyTbl=f.KeyTbl COLLATE  NOCASE "
                                                       "and hd.KeyFld=f.KeyFLd COLLATE  NOCASE "
                                                       ")  order by f.DisplayOrder")
DECL_SQL(select_table_visible_fields_in_order,"select * from pceListTblFld f where  "
                                              "f.KeyTbl=:table  and not  exists ( "
                                              " select hd.KeyFld from pceUDLProfile p ,pceUDLSetFldHiddenData hd "
                                              " where p.KeyProfile=:profile COLLATE NOCASE "
                                              "and p.KeySetFldHidden=hd.KeySet COLLATE NOCASE "
                                              "and hd.KeyTbl=f.KeyTbl COLLATE  NOCASE "
                                              "and hd.KeyFld=f.KeyFLd COLLATE  NOCASE "
                                              ")  order by f.DisplayOrder")
DECL_SQL(select_table_property,"select  * from pceUDLGenTblProp p    where p.KeyTbl=:table")
DECL_SQL(select_table_lookup_group,"select KeyGroup from pceUDLSetLib l ,pceUDLSetLibGroup g ,pceUDLProfile p"
                                   " where p.KeyProfile=:profile and p.KeySetLib=l.KeySet and l.KeySet=g.KeySet and g.TblApp=:table COLLATE  NOCASE ")
DECL_SQL(select_libs_of_lookup_group,"select  * from pceUDLSetLibGroup g   where g.KeyGroup=:group COLLATE  NOCASE  order by DisplayOrder")
DECL_SQL(select_tabs_of_lookup_lib,"select  * from pceUDLSetLibTab t   where t.KeySet=:set COLLATE  NOCASE and t.KeyTbl=:table  COLLATE  NOCASE  order by DisplayOrder")
DECL_SQL(select_fields_of_lookup_tab,"select  * from pceUDLSetLibTabField f   where f.KeySet=:set COLLATE  NOCASE and f.KeyTbl=:lib  COLLATE  NOCASE and f.KeyTab=:tab  COLLATE  NOCASE  order by DisplayOrder")

DECL_SQL(select_fields_of_group,"select  * from pceListTblFld where KeyTbl=:table  COLLATE NOCASE and GroupName=:groupName  COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_table_fields,"select  * from pceListTblFld where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_table_ref_fields,"select  * from pceListTblFld where KeyTbl=:table COLLATE NOCASE  and LookupTyp=8 order by DisplayOrder ")
DECL_SQL(select_table_long_headers,"select  CaptionLong from pceListTblFld where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_table_order,"select  SQLOrderBy from pceListTbl where KeyTbl=:table COLLATE NOCASE")
DECL_SQL(select_table_field_count,"select count(1) as cnt from pceListTblFld f where f.KeyTbl=:table COLLATE NOCASE and KeyFld=:field COLLATE NOCASE ")
DECL_SQL(select_table_field,"select  * from pceListTblFld where KeyTbl=:table  COLLATE NOCASE  and KeyFld=:field COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_field_groups,"select KeyTbl,GroupName,DisplayOrder   from pceUDLGenTblFldGroup g where g.KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_field_by_lookup,"select f.* from pceListTblFld f where f.LookupTableName=:lib COLLATE NOCASE and f.LookupFieldName=:fld COLLATE NOCASE  and f.KeyTbl=:table COLLATE NOCASE")
DECL_SQL(select_table_info,"select  * from pceListTbl where KeyTbl=:table COLLATE NOCASE")

END_SQL_DECLARATION

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
UDLDao * UDLDao::_instance=nullptr;

UDLDao::UDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{


}

UDLDao::~UDLDao()
{
    //    if(_instance)
    //        delete _instance;
}

UDLDao *UDLDao::instance()
{
    if(_instance==nullptr){
        _instance=new UDLDao(APP->udl(),APP);
    }
    return _instance;
}

QStringList UDLDao::profiles()
{
    QString key=QString("profiles");
    CS(key,QStringList);

    QStringList result;
    QSqlQuery q(SQL(select_profile_set),APP->udl());
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyProfile);
    }
    CI(key,result);
}

QStringList UDLDao::tableGroup(QString profile)
{

    QString key=QString("tableGroup.%1").arg(profile);
    CS(key,QStringList);

    bool isDefult=profile.compare(QString(DEFAULT_PROFILE),Qt::CaseInsensitive)==0;
    //    qDebug()<<"Profile"<<isDefult<< endl<<flush;

    if(profile.isNull()||profile.isEmpty()||isDefult){
        return MDL->tableGroup();
    }else{
        QStringList result;
        QSqlQuery q(APP->udl());
        q.prepare(SQL(select_table_group_by_profile));
        q.bindValue(":profile",profile);
        q.exec();
        PRINT_ERROR(q);
        while(q.next()){
            result<<QS(q,GroupName);
        }
        CI(key,result)
    }
}

QList<MDLTable*> UDLDao::tablesOfGroup(QString group, QString profile)
{

    bool isDefult=profile.compare(QString(DEFAULT_PROFILE),Qt::CaseInsensitive)==0;
    //    qDebug()<<"Profile"<<isDefult;
    if(profile.isNull()||profile.isEmpty()||isDefult){
        return MDL->tablesOfGroup(group);
    }else{
        QString key=QString("tablesOfGroup.%1.%2").arg(group).arg(profile);
        CS_LIST(key,MDLTable);
        QStringList result;
        QSqlQuery q(APP->udl());
        q.prepare(SQL(select_tables_of_group_profile));
        q.bindValue(":profile",profile);
        q.bindValue(":group",group);
        q.exec();
        PRINT_ERROR(q);
        auto r=Record::fromSqlQuery<MDLTable>(MDLTable::staticMetaObject.className(),q,this);
        CI(key,r);
    }

}

QList<MDLTable*> UDLDao::childTables(QString table, QString profile)
{
    QStringList tablesHidden=this->tablesHidden(profile);
    QList<MDLTable*> childTablesQuery=MDL->childTables(table,tablesHidden,APP->profile());
    return childTablesQuery;
}

QStringList UDLDao::tablesHidden(QString profile){
    QString key=QString("tablesHidden.%1").arg(profile);
    CS(key,QStringList);

    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_hidden_tables_of_profile));
    q.bindValue(":profile",profile);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyTbl);
    }
    CI(key,result);
}
QStringList UDLDao::fieldsHidden(QString profile,QString tablename){
    QString key=QString("fieldsHidden.%1.%2").arg(profile).arg(tablename);
    CS(key,QStringList);

    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_hidden_fields_of_profile));
    q.bindValue(":profile",profile);
    q.bindValue(":table",tablename);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyFld);
    }
    CI(key,result);
}

QStringList UDLDao::fieldsVisibleInOrder(QString profile, QString table)
{
    QString key=QString("fieldsVisibleInOrder.%1.%2").arg(profile).arg(table);
    CS(key,QStringList);
    QStringList result;

    QSqlQuery q(APP->udl());
    QStringList groups=UDL->fieldGroup(table);
    if(groups.size()>0){
        foreach(QString group,groups){
            QStringList fields=fieldsVisibleInOrderByGroup(profile,table,group);
            result<<fields;
        }
    }else
    {
        q.prepare(SQL(select_table_visible_fields_in_order));
        q.bindValue(":profile",profile);
        q.bindValue(":table",table);
        q.exec();
        PRINT_ERROR(q);
        while(q.next()){
            result<<QS(q,KeyFld);
        }
    }

    CI(key,result);
}

QStringList UDLDao::fieldsVisibleInOrderByGroup(QString profile, QString table, QString group)
{
    QString key=QString("fieldsVisibleInOrderByGroup.%1.%2.%3").arg(profile).arg(table).arg(group);
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_visible_fields_in_order_by_group));
    q.bindValue(":profile",profile);
    q.bindValue(":table",table);
    q.bindValue(":group",group);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyFld);
    }
    CI(key,result);

}

UDLTableProperty *UDLDao::tableProperty(QString table)
{
    QString key=QString("tableProperty.%1").arg(table);
    CS(key,UDLTableProperty*);

    UDLTableProperty * result=nullptr;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_property));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    if(q.next()){
        result=R(q.record(),UDLTableProperty);
    }
    CI(key,result);
}

QString UDLDao::lookupGroup(QString profile, QString table)
{
    QString key=QString("lookupGroup.%1.%2").arg(profile).arg(table);
    CS(key,QString);

    QString result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_lookup_group));
    q.bindValue(":profile",profile);
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result=QS(q,KeyGroup);
        break;
    }
    CI(key,result);
}

QList<UDLLibGroup *> UDLDao::lookupTables(QString group)
{
    QString key=QString("lookupTables.%1").arg(group);
    CS_LIST(key,UDLLibGroup);
    QList<UDLLibGroup *> result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_libs_of_lookup_group));
    q.bindValue(":group",group);
    q.exec();
    PRINT_ERROR(q);
    auto r=Record::fromSqlQuery<UDLLibGroup>(UDLLibGroup::staticMetaObject.className(),q,this);
    CI(key,r);
}


QList<UDLLibTab *> UDLDao::lookupTableTabs(QString set,QString lib)
{
    QString key=QString("lookupTableTabs.%1.%2").arg(set).arg(lib);
    CS_LIST(key,UDLLibTab);
    QList<UDLLibTab *> result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_tabs_of_lookup_lib));
    q.bindValue(":set",set);
    q.bindValue(":table",lib);
    q.exec();
    PRINT_ERROR(q);
    auto r=Record::fromSqlQuery<UDLLibTab>(UDLLibTab::staticMetaObject.className(),q,this);
    CI(key,r);
}

QList<UDLLibTabField *> UDLDao::lookupTableFieldsOfTab(QString set, QString lib, QString tab)
{
    QString key=QString("lookupTableFieldsOfTab.%1.%2.%3").arg(set).arg(lib).arg(tab);
    CS_LIST(key,UDLLibTabField);
    QList<UDLLibTabField *> result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_fields_of_lookup_tab));
    q.bindValue(":set",set);
    q.bindValue(":lib",lib);
    q.bindValue(":tab",tab);
    q.exec();
    PRINT_ERROR(q);
    auto r=Record::fromSqlQuery<UDLLibTabField>(UDLLibTabField::staticMetaObject.className(),q,this);
    CI(key,r);
}

QStringList UDLDao::fieldOfGroup(QString table, QString group)
{
    QString key=QString("fieldOfGroup.%1.%2").arg(table).arg(group);
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_fields_of_group));
    q.bindValue(":table",table);
    q.bindValue(":groupName",group);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("KeyFld").toString();
    }
    if(result.isEmpty()){
        return MDL->fieldOfGroup(table,group);
    }
    CI(key,result)
}

QList<MDLField *> UDLDao::tableFields(QString table)
{
    QString key="tableFields."+table;
    CS_LIST(key,MDLField);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QList<MDLField*> result=Record::fromSqlQuery<MDLField>(MDLField::staticMetaObject.className(),q,this);
    if(result.size()<=0)
        result=MDL->tableFields(table);
    CI(key,result);
}

QList<MDLField *> UDLDao::tableRefFields(QString table)
{
    QString key="tableRefFields."+table;
    CS_LIST(key,MDLField);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_ref_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QList<MDLField*> result=Record::fromSqlQuery<MDLField>(MDLField::staticMetaObject.className(),q,this);
    if(result.size()<=0)
        result=MDL->tableRefFields(table);
    CI(key,result);
}

QStringList UDLDao::tableHeaders(QString table)
{
    QString key="tableHeaders."+table;
    CS(key,QStringList)
            QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_long_headers));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QStringList result;
    while(q.next()){
        result<<q.value("CaptionLong").toString();
    }
    if(result.size()<=0)
        result=MDL->tableHeaders(table);
    CI(key,result)
}

QString UDLDao::tableOrderKey(QString table)
{
    QString key="tableOrderKey."+table;
    CS(key,QString)
            QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_order));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    //    qDebug()<<"OrderKey of table["<<table<<"],isActive:"<<q.isActive()<<",Size:"<<q.size()<<","<<q.lastQuery();
    //    qDebug()<<"Driver:has QSqlDriver::QuerySize|"<<APP->mdl().driver()->hasFeature(QSqlDriver::QuerySize);
    QString result=QString();
    if(q.next()){
        result=q.value(0).toString();
    }else{
        result=MDL->tableOrderKey(table);
    }
    CI(key,result)
}
QSqlQuery UDLDao::tableFieldsQuery(QString table)
{

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);

    return q;
}
bool UDLDao::tableHasField(QString table,QString field){
    QString key=QString("tableHasField.%1.%2").arg(table).arg(field);
    CS(key,bool);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_field_count));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    bool result=false;
    if(q.next()){
        result= QS(q,cnt).toInt()>0;
    }
    if(result==false){
        result=MDL->tableHasField(table,field);
    }
    CI(key,result);
}
MDLUnitType* UDLDao::baseUnitOfField(QString table, QString field)
{

    QString key=QString("baseUnitOfField.%1.%2").arg(table).arg(field);
    CS(key,MDLUnitType*);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_base_unit_of_field));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    MDLUnitType *result=nullptr;
    if(q.next())
    {
        result=R(q.record(),MDLUnitType);
    }
    else {
        result=MDL->baseUnitOfField(table,field);
    }
    CI(key,result);
}
MDLField * UDLDao::fieldInfo(QString table, QString field)
{
    QString key=QString("fieldInfo.%1.%2").arg(table).arg(field);
    CS(key,MDLField*);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_field));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    MDLField * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLField);
    }else{
        result=MDL->fieldInfo(table,field);
    }
    CI(key,result);
}
QStringList UDLDao::fieldGroup(QString table)
{
    QString key=QString("fieldGroup.%1").arg(table);
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_field_groups));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("GroupName").toString();
    }
    if(result.isEmpty())
        result=MDL->fieldGroup(table);
    CI(key,result)
}

MDLField *UDLDao::fieldByLookup(QString table,QString lib, QString fld)
{
    QString key=QString("fieldByLookup.%1.%2.%3").arg(table).arg(lib).arg(fld);
    CS(key,MDLField*);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_field_by_lookup));
    q.bindValue(":table",table);
    q.bindValue(":lib",lib);
    q.bindValue(":fld",fld);
    q.exec();
    PRINT_ERROR(q);
    MDLField * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLField);
    }else
    {
        result=MDL->fieldByLookup(table,lib,fld);
    }
    CI(key,result);
}

bool UDLDao::isLookupAllField(MDLField *fieldInfo)
{
    if(MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld()).isEmpty()){
        QString tableNameField=lookupTableNameField(fieldInfo);
        if(!tableNameField.isNull() && !tableNameField.isEmpty()){
            if( MDL->tableHasField(fieldInfo->KeyTbl(),tableNameField)){
                return true;
            }
        }
    }
    return false;
}

bool UDLDao::isLookupMultiTableField(MDLField *fieldInfo)
{
    if(MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld()).size()>1){
        QString tableNameField=lookupTableNameField(fieldInfo);
        if(!tableNameField.isNull() && !tableNameField.isEmpty()){
            if( MDL->tableHasField(fieldInfo->KeyTbl(),tableNameField)){
                return true;
            }
        }
    }
    return false;
}

bool UDLDao::isLookupField(MDLField *fieldInfo)
{
    if(MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld()).size()>0){
        return true;
    }
    return false;
}

QString UDLDao::lookupTableNameField(MDLField *fieldInfo)
{
    QString fieldTable=fieldInfo->KeyFld().replace("IDRec","TblKey",Qt::CaseInsensitive);
    return fieldTable;
}
MDLTable* UDLDao::tableInfo(QString table)
{
    QString key=QString("tableInfo.%1").arg(table);
    CS(key,MDLTable*);

    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_table_info));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    MDLTable * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLTable);
    }else{
        result=MDL->tableInfo(table);
    }
    CI(key,result);
}

void UDLDao::resetCache()
{
    _cache.clear();
}
