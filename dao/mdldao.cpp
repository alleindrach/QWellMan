#include "mdldao.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include  "QSqlRecord"
#include "QDebug"
#include <QSqlField>
#include <QtCore/qmath.h>
//const auto SELECT_TABLE_GRP = QLatin1String(R"(select  keyGrp from pceMDLTableGrp order by  DisplayOrder)");

MDLDao * MDLDao::_instance=nullptr;

MDLDao::MDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_table_group,"select  keyGrp from pceMDLTableGrp order by  DisplayOrder");
    DECL_SQL(select_ini,"select  * from pceMDLINI ");
    DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group and l.KeyTbl=t.KeyTbl COLLATE NOCASE order by l.DisplayOrder ");
    DECL_SQL(select_table_order,"select  SQLOrderBy from pceMDLTable where KeyTbl=:table COLLATE NOCASE");
    DECL_SQL(select_table_long_headers,"select  CaptionLong from pceMDLTableField where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ");
    DECL_SQL(select_main_table_headers,"select  t.*,m.KeyFld as VisableFld from pceMDLTableField t left join pceMDLTableMainIDFields m  on "
                                       "t.KeyTbl=:table  COLLATE NOCASE and t.KeyFld=m.KeyFld COLLATE NOCASE order by m.DisplayOrder ")
            DECL_SQL(select_unit_set,"select  * from pceMDLUnitSet s   "
                                     "order by s.DisplayOrder");
    DECL_SQL(select_profile_set,"select  * from pceMDLUnitSet s   "
                                " order by s.DisplayOrder");
    DECL_SQL(select_table_fields,"select  * from pceMDLTableField where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ");
    DECL_SQL(select_base_unit_of_field,"select u.* from pceMDLTableField f ,pceMDLUnitType u  where f.KeyTbl=:table COLLATE NOCASE and  f.KeyFld=:field COLLATE NOCASE and u.KeyType=f.KeyUnit COLLATE NOCASE")
            DECL_SQL(select_user_unit,"select * from pceMDLUnitTypeSet us  where us.KeyType=:unitType and us.KeySet in (select KeySet from pceMDLUnitSet u where  u.KeySet=:unitSet COLLATE NOCASE union select KeySetInherit  from  pceMDLUnitSet uh where uh.KeySet=:unitSet COLLATE NOCASE) ");

    DECL_SQL(select_base_unit,"select * from pceMDLUnitType us \
             where us.KeyType=:unitType  COLLATE NOCASE");
            DECL_SQL(select_unit_conversion,"select * from pceMDLUnitConversion where BaseUnits=:baseUnitKey COLLATE NOCASE and  UserUnits=:userUnitKey COLLATE NOCASE");
            DECL_SQL(select_table_info,"select  * from pceMDLTable where KeyTbl=:table COLLATE NOCASE");
    DECL_SQL(select_child_tables,"select t2.* from pceMDLTableChildren c,pceMDLTable t,pceMDLTable t2 \
             where   c.KeyTbl=:table COLLATE NOCASE and t2.Calculated=false and t.KeyTbl=c.KeyTbl COLLATE NOCASE and t2.KeyTbl=c.KeyTblChild COLLATE NOCASE and not exists(select * from pceMDLTableGrpLink l where l.KeyTbl=c.KeyTblChild COLLATE NOCASE) \
            and c.KeyTblChild not in (%1) order by c.DisplayOrder ")
            DECL_SQL(select_table_field_count,"select count(1) as cnt from pceMDLTableField f where f.KeyTbl=:table COLLATE NOCASE and KeyFld=:field COLLATE NOCASE ")
            DECL_SQL(select_parent_table,"select KeyTbl  from pceMDLTableChildren c where c.KeyTblChild=:table COLLATE NOCASE ")
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
    config.insert("ID","IDRec");
    config.insert("ParentIDField","IDRecParent");
    config.insert("SysRecDelTable","wvSysRecDel");
    config.insert("SysRecentTable","wvSys01");
    config.insert("SysFavoriteTable","wvSys02");
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
bool MDLDao::tableHasField(QString table,QString field){
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_field_count));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    if(q.next()){
        return QS(q,cnt).toInt()>0;
    }
    return false;

}

QString MDLDao::parentTable(QString table)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_parent_table));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    if(q.next()){
        return QS(q,KeyTbl);
    }
    return QString();
}
QString MDLDao::parentRefName(QString table){
    QString parentTableName=parentTable(table);
    if(parentTableName.compare(CFG(KeyTblMain),Qt::CaseInsensitive)==0){
        return CFG(IDMainFieldName);
    }else if (!parentTableName.isNull()&& !parentTableName.isEmpty()){
        return CFG(ParentIDField);
    }
    return CFG(IDMainFieldName);
}
// unitType:比如长度度量： Length (0.00m,0.00ft)
// unitSet: 单位制，比如美制(US)，英制，公制(Metric)
// unitKey: 单位，比如 m
// baseUnit: 一个 UnitType，对应一个baseUnitKey，这个关系保存在 main.pceMDLUnitType，比如Length (0.00m,0.00ft) 对应的基本单位就是m
//  userUnit:在某个单位制unitSet下，某一个unitType，对应一个userUnit，用于用户切换当前单位制时的显示和录入，比如 Length (0.00m,0.00ft) 在US单位制下，其userUnit为ft,在Metric单位制下，其userUnit为空，取baseUnit
// 单位制-单位类型-用户单位(unitSet-unitType-userUnit）的关系保存在main.pceMDLUnitTypeSet 表中
QSqlRecord MDLDao::baseUnitOfField(QString table, QString field)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_base_unit_of_field));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();
    }

}
//单位转换
QVariant MDLDao::unitBase2User(QString baseUnitKey,QString userUnitKey, QVariant v)
{
    if(baseUnitKey==userUnitKey)
        return v;
    if(v.type()==QMetaType::QString)
        return v;
    if(v.type()==QMetaType::Double||v.type()==QMetaType::Int||v.type()==QMetaType::Float||v.type()==QMetaType::Long){
        QSqlRecord rec=unitConversion(baseUnitKey,userUnitKey);
        if(!rec.isEmpty()){
            double factor=RS(rec,Factor).toDouble();
            double exponent=RS(rec,Exponent).toDouble();
            double offset=RS(rec,Offset).toDouble();
            double result=qPow(v.toDouble()*factor,exponent)+offset;
            return result;
        }
    }
    return v;
}

QVariant MDLDao::unitUser2Base(QString baseUnitKey,QString userUnitKey, QVariant v)
{

    if(baseUnitKey==userUnitKey)
        return v;
    if(v.type()==QMetaType::Double||v.type()==QMetaType::Int||v.type()==QMetaType::Float||v.type()==QMetaType::Long){
        QSqlRecord rec=unitConversion(baseUnitKey,userUnitKey);
        if(!rec.isEmpty()){
            double factor=RS(rec,Factor).toDouble();
            double exponent=RS(rec,Exponent).toDouble();
            double offset=RS(rec,Offset).toDouble();
            double result=qPow(v.toDouble()-offset,-exponent)/factor;
            return result;
        }
    }
    return v;
}
//当在当前单位制下，无用户单位时，应使用基本单位
QSqlRecord MDLDao::userUnitKey(QString unitSet,QString unitType)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_user_unit));
    q.bindValue(":unitSet",unitSet);
    q.bindValue(":unitType",unitType);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();//当在当前单位制下，无用户单位时，应使用基本单位
    }
}
//获取当前单位类型的基本单位
QSqlRecord MDLDao::baseUnitKey(QString unitType){


    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_base_unit));
    q.bindValue(":unitType",unitType);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();
    }
}
//获取单位转换信息
QSqlRecord MDLDao::unitConversion(QString baseUnitKey, QString userUnitKey)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_unit_conversion));
    q.bindValue(":baseUnitKey",baseUnitKey);
    q.bindValue(":userUnitKey",userUnitKey);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();
    }
}

QSqlRecord MDLDao::tableInfo(QString table)
{
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_info));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    if(q.next())
        return  q.record();
    else {
        return QSqlRecord();
    }
}

QSqlQuery MDLDao::childTables(QString table,QStringList hidden, QString profile)
{
    QSqlQuery q(APP->mdl());
    QStringList critieal=hidden.replaceInStrings(QRegExp("^(\\w)"), "'\\1")
            .replaceInStrings(QRegExp("(\\w)$"), "\\1'");
    q.prepare(SQL(select_child_tables).arg(critieal.join(",")));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    return q;
}
