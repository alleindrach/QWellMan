#include "mdldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include  "QSqlRecord"
#include "QDebug"
#include <QSqlField>
//const auto SELECT_TABLE_GRP = QLatin1String(R"(select  keyGrp from pceMDLTableGrp order by  DisplayOrder)");

MDLDao * MDLDao::_instance=nullptr;

MDLDao::MDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_table_group,"select  keyGrp from pceMDLTableGrp order by  DisplayOrder");
    DECL_SQL(select_ini,"select  * from pceMDLINI ");
    DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group and l.KeyTbl=t.KeyTbl order by l.DisplayOrder");
    DECL_SQL(select_table_order,"select  SQLOrderBy from pceMDLTable where KeyTbl=:table ");
    DECL_SQL(select_table_long_headers,"select  CaptionLong from pceMDLTableField where KeyTbl=:table order by DisplayOrder");
    DECL_SQL(select_main_table_headers,"select  t.*,m.KeyFld as VisableFld from pceMDLTableField t left join pceMDLTableMainIDFields m  on "
                                       "t.KeyTbl=:table and upper(t.KeyFld)=upper(m.KeyFld) order by m.DisplayOrder")
            DECL_SQL(select_unit_set,"select  * from pceMDLUnitSet s   "
                                     "order by s.DisplayOrder");
    DECL_SQL(select_profile_set,"select  * from pceMDLUnitSet s   "
                                " order by s.DisplayOrder");
    DECL_SQL(select_table_fields,"select  * from pceMDLTableField where KeyTbl=:table order by DisplayOrder");
}

MDLDao::~MDLDao()
{
    //    if(_instance)
    //        delete _instance;
}

MDLDao *MDLDao::instance()
{
    if(_instance==nullptr){
        _instance=new MDLDao(APP->mdl(),APP);
    }
    return _instance;
}

QStringList MDLDao::tableGroup()
{
    QStringList result;
    QSqlQuery q(SQL(select_table_group),APP->mdl());
    q.exec();
    while(q.next()){
        result<<q.value("KeyGrp").toString();
    }
    return  result;
}

QSqlQuery MDLDao::tablesOfGroup(QString group)
{

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_of_group));
    q.bindValue(":group",group);
    q.exec();
    return q;


}

void MDLDao::readConfig(QHash<QString,QString>&  config){

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_ini));
    q.exec();
    PRINT_ERROR(q);
    QSqlRecord  rec=q.record();
    while(q.next()){
        for(int i=0;i<rec.count();i++){
            QString colname=rec.field(i).name();
            config.insert(colname,q.value(rec.indexOf(colname)).toString());
        }
    }
    return ;

}

QString MDLDao::tableOrderKey(QString table)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_order));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
//    qDebug()<<"OrderKey of table["<<table<<"],isActive:"<<q.isActive()<<",Size:"<<q.size()<<","<<q.lastQuery();
//    qDebug()<<"Driver:has QSqlDriver::QuerySize|"<<APP->mdl().driver()->hasFeature(QSqlDriver::QuerySize);
    if(q.next())
        return q.value(0).toString();
    return QString();
}

QSqlQuery MDLDao::tableHeaders(QString table)
{

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_long_headers));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    return q;

}

QSqlQuery MDLDao::tableMainHeadersVisible()
{

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_main_table_headers));
    q.bindValue(":table",CFG(KeyTblMain));
    q.exec();
    PRINT_ERROR(q);
    return q;
}

QStringList MDLDao::tableMainHeadersVisibleKeys()
{
    QStringList list;
    QSqlQuery q=tableMainHeadersVisible();
    while(q.next()){
//        qDebug()<<" field :"<<q.value("KeyFld" ).toString()<<","<<q.value("VisableFld").toString();
        if(!q.value("VisableFld").toString().isEmpty())
            list<<q.value("KeyFld").toString();
    }
    return list;
}

QStringList MDLDao::units()
{
    QStringList result;
    QSqlQuery q(SQL(select_unit_set),APP->mdl());
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("KeySet").toString();
    }
    return  result;
}

QStringList MDLDao::datumPref()
{
    QStringList  result;
    result<< tr("Original KB Elevation(KB)")<<tr("Casing Flange Elevation(CF)")
          <<tr("Ground Elevation(GRD)")<< tr("Tubing Head Elevation(TH)")
         <<tr("Mud Line Elevation(ML)")<<tr("Mean Sea Level(MSL)")<< tr("Other Elevation(Well specfic)");
    return result;
}

QSqlQuery MDLDao::tableFields(QString table)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    return q;
}
