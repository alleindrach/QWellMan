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
#include "qlogging.h"

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
UDLDao * UDLDao::_instance=nullptr;

UDLDao::UDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_profile_set,"select  * from pceUDLProfile p \
             order by p.DisplayOrder");
             DECL_SQL(select_table_group_by_profile,"select d.* from pceUDLSetTblGroupData d,pceUDLSetTblGroup g,pceUDLProfile p \
                      where p.KeyProfile=:profile and p.KeySetTblGroup=g.KeySet and g.KeySet=d.KeySet \
            order by d.DisplayOrder");
             DECL_SQL(select_tables_of_group_profile,"select tl.* from pceUDLSetTblGroupTblData t,pceUDLProfile p,pceListTbl tl \
                      where p.KeyProfile=:profile and tl.Calculated=false and p.KeySetTblGroup=t.KeySet and t.GroupName=:group and tl.KeyTbl=t.KeyTbl  order by t.DisplayOrder");
//qInstallMessageHandler(outputMessage);
            DECL_SQL(select_hidden_tables_of_profile,"select hd.KeyTbl from pceUDLProfile p ,pceUDLSetTblHiddenData hd \
                     where p.KeyProfile=:profile and p.KeySetTblHidden=hd.KeySet")
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
    QStringList result;
    QSqlQuery q(SQL(select_profile_set),APP->udl());
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyProfile);
    }
    return  result;
}

QStringList UDLDao::tableGroup(QString profile)
{


    bool isDefult=profile.compare(QString(DEFAULT_PROFILE),Qt::CaseInsensitive)==0;
    qDebug()<<"Profile"<<isDefult<< endl<<flush;

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
        return  result;
    }
}

QSqlQuery UDLDao::tablesOfGroup(QString group, QString profile)
{

    bool isDefult=profile.compare(QString(DEFAULT_PROFILE),Qt::CaseInsensitive)==0;
    qDebug()<<"Profile"<<isDefult;
    if(profile.isNull()||profile.isEmpty()||isDefult){
        return MDL->tablesOfGroup(group);
    }else{
        QStringList result;
        QSqlQuery q(APP->udl());
        q.prepare(SQL(select_tables_of_group_profile));
        q.bindValue(":profile",profile);
        q.bindValue(":group",group);
        q.exec();
        PRINT_ERROR(q);
        return  q;
    }

}

QSqlQuery UDLDao::childTables(QString table, QString profile)
{
    QStringList tablesHidden=this->tablesHidden(profile);
    QSqlQuery childTablesQuery=MDL->childTables(table,tablesHidden,APP->profile());
    return childTablesQuery;
}

QStringList UDLDao::tablesHidden(QString profile){
    QStringList result;
    QSqlQuery q(APP->udl());
    q.prepare(SQL(select_hidden_tables_of_profile));
    q.bindValue(":profile",profile);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<QS(q,KeyTbl);
    }
    return  result;
}
