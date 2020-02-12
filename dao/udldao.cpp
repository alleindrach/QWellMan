#include "udldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include "QSqlRecord"
#include "QDebug"
#include <QSqlField>
UDLDao * UDLDao::_instance=nullptr;

UDLDao::UDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_profile_set,"select  * from pceUDLProfile p \
             order by p.DisplayOrder");
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
        result<<q.value("KeyProfile").toString();
    }
    return  result;
}

