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
                                                       "and f.KeyTbl=:table  and not  exists ( "
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
    QStringList groups=MDL->fieldGroup(table);
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
