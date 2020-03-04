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
#include "udldao.h"
#include "welldao.h"
//
QHash<QString ,QVariant> MDLDao::_cache={};

BEGIN_SQL_DECLARATION(MDLDao)
DECL_SQL(select_table_group,"select  keyGrp from pceMDLTableGrp order by  DisplayOrder")
DECL_SQL(select_ini,"select  * from pceMDLINI ")
DECL_SQL(select_table_of_group,"select t.* from pceMDLTableGrpLink l,pceMDLTable t where KeyGrp=:group and l.KeyTbl=t.KeyTbl COLLATE NOCASE order by l.DisplayOrder ")
DECL_SQL(select_table_order,"select  SQLOrderBy from pceMDLTable where KeyTbl=:table COLLATE NOCASE")
DECL_SQL(select_table_long_headers,"select  CaptionLong from pceMDLTableField where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_main_table_headers,"select  t.*, case ifnull(m.KeyFld,'0') when '0' then 0 else 1 end as Visible "
                                   "from pceMDLTableField t left join pceMDLTableMainIDFields m  on "
                                   "t.KeyTbl=:table  COLLATE NOCASE and t.KeyFld=m.KeyFld COLLATE NOCASE order by m.DisplayOrder ")
DECL_SQL(select_unit_set,"select  * from pceMDLUnitSet s   "
                         "order by s.DisplayOrder")
DECL_SQL(select_profile_set,"select  * from pceMDLUnitSet s   "
                            " order by s.DisplayOrder")
DECL_SQL(select_table_fields,"select  * from pceMDLTableField where KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_table_field,"select  * from pceMDLTableField where KeyTbl=:table  COLLATE NOCASE  and KeyFld=:field COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_base_unit_of_field,"select u.* from pceMDLTableField f ,pceMDLUnitType u  where f.KeyTbl=:table COLLATE NOCASE and  f.KeyFld=:field COLLATE NOCASE and u.KeyType=f.KeyUnit COLLATE NOCASE")
DECL_SQL(select_user_unit,"select * from pceMDLUnitTypeSet us  where us.KeyType=:unitType and us.KeySet in (select KeySet from pceMDLUnitSet u where  u.KeySet=:unitSet COLLATE NOCASE union select KeySetInherit  from  pceMDLUnitSet uh where uh.KeySet=:unitSet COLLATE NOCASE) ")

DECL_SQL(select_base_unit,"select * from pceMDLUnitType us "
                          "where us.KeyType=:unitType  COLLATE NOCASE")
DECL_SQL(select_unit_conversion,"select * from pceMDLUnitConversion where BaseUnits=:baseUnitKey COLLATE NOCASE and  UserUnits=:userUnitKey COLLATE NOCASE")
DECL_SQL(select_table_info,"select  * from pceMDLTable where KeyTbl=:table COLLATE NOCASE")
DECL_SQL(select_child_tables,"select t2.* from pceMDLTableChildren c,pceMDLTable t,pceMDLTable t2 "
                             "where   c.KeyTbl=:table COLLATE NOCASE and t2.Calculated=false and t.KeyTbl=c.KeyTbl COLLATE NOCASE and t2.KeyTbl=c.KeyTblChild COLLATE NOCASE and not exists(select * from pceMDLTableGrpLink l where l.KeyTbl=c.KeyTblChild COLLATE NOCASE) "
                             "and c.KeyTblChild not in (%1) order by c.DisplayOrder ")
DECL_SQL(select_table_field_count,"select count(1) as cnt from pceMDLTableField f where f.KeyTbl=:table COLLATE NOCASE and KeyFld=:field COLLATE NOCASE ")
DECL_SQL(select_parent_table,"select KeyTbl  from pceMDLTableChildren c where c.KeyTblChild=:table COLLATE NOCASE ")
DECL_SQL(select_field_groups,"select KeyTbl,GroupName,DisplayOrder   from pceMDLTableFieldGrp g where g.KeyTbl=:table COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_fields_of_group,"select  * from pceMDLTableField where KeyTbl=:table  COLLATE NOCASE and GroupName=:groupName  COLLATE NOCASE order by DisplayOrder ")
DECL_SQL(select_field_lookup,"select  * from pceMDLTableFieldLookupList where KeyTbl=:table  COLLATE NOCASE and KeyFld=:field  COLLATE NOCASE order by DisplayOrder ")

END_SQL_DECLARATION


MDLDao * MDLDao::_instance=nullptr;

MDLDao::MDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{

}

MDLDao::~MDLDao()
{
    //    if(_instance)
    //        delete _instance;
    //    if(!_cache.isEmpty())
    //    {
    //        qDeleteAll(_cache);
    //    }
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
    CS("tableGroup",QStringList)


            QSqlQuery q(SQL(select_table_group),APP->mdl());
    q.exec();
    while(q.next()){
        result<<q.value("KeyGrp").toString();
    }
    CI("tableGroup",result)
}

QList<MDLTable*> MDLDao::tablesOfGroup(QString group)
{
    QString key="tablesOfGroup."+group;
    CS_LIST(key,MDLTable)
            QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_of_group));
    q.bindValue(":group",group);
    q.exec();

    auto x=Record::fromSqlQuery<MDLTable>(MDLTable::staticMetaObject.className(),q,this);
    CI(key,x)

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
    config.insert("IDWell",config["IDMainFieldName"]);
    config.insert("ParentID","IDRecParent");
    config.insert("SysRecDelTable","wvSysRecDel");
    config.insert("SysRecentTable","wvSys01");
    config.insert("SysFavoriteTable","wvSys02");
    config.insert("SysCD","sysCreateDate");
    config.insert("SysMD","sysModDate");
    config.insert("SysCU","sysCreateUser");
    config.insert("SysMU","sysModUser");
    config.insert("SysLD","sysLockDate");
    config.insert("LibTab","sysTab");
    config.insert("TblKeyParent","TblKeyParent");
    return ;

}

QString MDLDao::tableOrderKey(QString table)
{
    QString key="tableOrderKey."+table;
    CS(key,QString)
            QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_order));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    //    qDebug()<<"OrderKey of table["<<table<<"],isActive:"<<q.isActive()<<",Size:"<<q.size()<<","<<q.lastQuery();
    //    qDebug()<<"Driver:has QSqlDriver::QuerySize|"<<APP->mdl().driver()->hasFeature(QSqlDriver::QuerySize);
    QString result=QString();
    if(q.next()){
        result=q.value(0).toString();
    }
    CI(key,result)
}

QStringList MDLDao::tableHeaders(QString table)
{
    QString key="tableHeaders."+table;
    CS(key,QStringList)
            QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_long_headers));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QStringList result;
    while(q.next()){
        result<<q.value("CaptionLong").toString();
    }
    CI(key,result)
}

QList<MDLFieldVisible*> MDLDao::tableMainHeadersVisible()
{
    QString key="tableMainHeadersVisible";
    CS_LIST(key,MDLFieldVisible);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_main_table_headers));
    q.bindValue(":table",CFG(KeyTblMain));
    q.exec();
    PRINT_ERROR(q);
    QList<MDLFieldVisible*> result=Record::fromSqlQuery<MDLFieldVisible >(MDLFieldVisible::staticMetaObject.className(),q,this );
    CI(key,result);
}

QStringList MDLDao::tableMainHeadersVisibleKeys()
{
    QStringList list;

    QString key="tableMainHeadersVisibleKeys";
    CS(key,QStringList);

    QList<MDLFieldVisible*> v=tableMainHeadersVisible();
    foreach(MDLFieldVisible * f,v){
        //        qDebug()<<" field :"<<q.value("KeyFld" ).toString()<<","<<q.value("VisableFld").toString();
        if(f->Visible()>0)
            list<<f->KeyFld();
    }
    CI(key,list);
}

QStringList MDLDao::units()
{
    QString key="units";
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(SQL(select_unit_set),APP->mdl());
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("KeySet").toString();
    }
    CI(key,result)
}

QStringList MDLDao::datumPref()
{

    QStringList  result;
    result<< tr("Original KB Elevation(KB)")<<tr("Casing Flange Elevation(CF)")
          <<tr("Ground Elevation(GRD)")<< tr("Tubing Head Elevation(TH)")
         <<tr("Mud Line Elevation(ML)")<<tr("Mean Sea Level(MSL)")<< tr("Other Elevation(Well specfic)");
    return result;
}

QList<MDLField*> MDLDao::tableFields(QString table)
{
    QString key="tableFields."+table;
    CS_LIST(key,MDLField);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QList<MDLField*> result=Record::fromSqlQuery<MDLField>(MDLField::staticMetaObject.className(),q,this);
    CI(key,result);
}
QSqlQuery MDLDao::tableFieldsQuery(QString table)
{

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_fields));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);

    return q;
}
bool MDLDao::tableHasField(QString table,QString field){
    QString key=QString("tableHasField.%1.%2").arg(table).arg(field);
    CS(key,bool);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_field_count));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    bool result=false;
    if(q.next()){
        result= QS(q,cnt).toInt()>0;
    }
    CI(key,result);
}

QString MDLDao::parentTable(QString table)
{
    QString key=QString("parentTable.%1").arg(table);
    CS(key,QString);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_parent_table));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QString result=QString();
    if(q.next()){
        result= QS(q,KeyTbl);
    }
    CI(key,result);
}
QString MDLDao::parentRefName(QString table){
    QString parentTableName=parentTable(table);
    if(parentTableName.compare(CFG(KeyTblMain),Qt::CaseInsensitive)==0){
        return CFG(IDMainFieldName);
    }else if (!parentTableName.isNull()&& !parentTableName.isEmpty()){
        return CFG(ParentID);
    }
    return CFG(IDMainFieldName);
}
// unitType:比如长度度量： Length (0.00m,0.00ft)
// unitSet: 单位制，比如美制(US)，英制，公制(Metric)
// unitKey: 单位，比如 m
// baseUnit: 一个 UnitType，对应一个baseUnitKey，这个关系保存在 main.pceMDLUnitType，比如Length (0.00m,0.00ft) 对应的基本单位就是m
//  userUnit:在某个单位制unitSet下，某一个unitType，对应一个userUnit，用于用户切换当前单位制时的显示和录入，比如 Length (0.00m,0.00ft) 在US单位制下，其userUnit为ft,在Metric单位制下，其userUnit为空，取baseUnit
// 单位制-单位类型-用户单位(unitSet-unitType-userUnit）的关系保存在main.pceMDLUnitTypeSet 表中
MDLUnitType* MDLDao::baseUnitOfField(QString table, QString field)
{

    QString key=QString("baseUnitOfField.%1.%2").arg(table).arg(field);
    CS(key,MDLUnitType*);

    QSqlQuery q(APP->mdl());
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

    }
    CI(key,result);
}
//单位转换
QVariant MDLDao::unitBase2User(QString baseUnitKey,QString userUnitKey, QVariant v)
{
    if(baseUnitKey==userUnitKey)
        return v;
    if(v.type()==QMetaType::QString)
        return v;
    if(v.type()==QMetaType::Double||v.type()==QMetaType::Int||v.type()==QMetaType::Float||v.type()==QMetaType::Long){
        MDLUnitConversion * rec=unitConversion(baseUnitKey,userUnitKey);
        if(rec!=nullptr){
            double factor=rec->Factor();
            double exponent=rec->Exponent();
            double offset=rec->Offset();
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
        MDLUnitConversion* rec=unitConversion(baseUnitKey,userUnitKey);
        if(rec!=nullptr){
            double factor=rec->Factor();
            double exponent=rec->Exponent();
            double offset=rec->Offset();
            double result=qPow(v.toDouble()-offset,-exponent)/factor;
            return result;
        }
    }
    return v;
}
//当在当前单位制下，无用户单位时，应使用基本单位
MDLUnitTypeSet* MDLDao::userUnitKey(QString unitSet,QString unitType)
{
    QString key=QString("userUnitKey.%1.%2").arg(unitSet).arg(unitType);
    CS(key,MDLUnitTypeSet*);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_user_unit));
    q.bindValue(":unitSet",unitSet);
    q.bindValue(":unitType",unitType);
    q.exec();
    PRINT_ERROR(q);
    MDLUnitTypeSet * result=nullptr;
    if(q.next()){
        result=Record::fromSqlRecord<MDLUnitTypeSet>(MDLUnitTypeSet::staticMetaObject.className(),q.record(),this);
    }

    CI(key,result);
}
//获取当前单位类型的基本单位
MDLUnitType * MDLDao::baseUnitKey(QString unitType){
    QString key=QString("baseUnitKey.%1").arg(unitType);
    CS(key,MDLUnitType*);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_base_unit));
    q.bindValue(":unitType",unitType);
    q.exec();
    PRINT_ERROR(q);
    MDLUnitType * result=nullptr;
    if(q.next())
    {
        result=Record::fromSqlRecord<MDLUnitType>(MDLUnitType::staticMetaObject.className(), q.record(),this);
    }
    CI(key,result);
}
//获取单位转换信息
MDLUnitConversion * MDLDao::unitConversion(QString baseUnitKey, QString userUnitKey)
{
    QString key=QString("unitConversion.%1.%2").arg(baseUnitKey).arg(userUnitKey);
    CS(key,MDLUnitConversion*);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_unit_conversion));
    q.bindValue(":baseUnitKey",baseUnitKey);
    q.bindValue(":userUnitKey",userUnitKey);
    q.exec();
    PRINT_ERROR(q);
    MDLUnitConversion * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLUnitConversion);
    }
    CI(key,result);
}

MDLTable* MDLDao::tableInfo(QString table)
{
    QString key=QString("tableInfo.%1").arg(table);
    CS(key,MDLTable*);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_info));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    MDLTable * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLTable);
    }
    CI(key,result);
}

MDLField * MDLDao::fieldInfo(QString table, QString field)
{
    QString key=QString("fieldInfo.%1.%2").arg(table).arg(field);
    CS(key,MDLField*);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_table_field));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    MDLField * result=nullptr;
    if(q.next()){
        result=R(q.record(),MDLField);
    }
    CI(key,result);
}

QList<MDLTable*> MDLDao::childTables(QString table,QStringList hidden, QString profile)
{
    QString key=QString("childTables.%1.%2").arg(table).arg(profile);
    CS_LIST(key,MDLTable);

    QSqlQuery q(APP->mdl());
    QStringList critieal=hidden.replaceInStrings(QRegExp("^(\\w)"), "'\\1")
            .replaceInStrings(QRegExp("(\\w)$"), "\\1'");
    q.prepare(SQL(select_child_tables).arg(critieal.join(",")));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    QList<MDLTable*> result;
    result=Q(q,MDLTable);
    CI(key,result)
}

QList<MDLTable *> MDLDao::topTables(QStringList hidden, QString profile)
{
    QString key=QString("topTables.%1").arg(profile);
    CS_LIST(key,MDLTable);

    QSqlQuery q(APP->mdl());
    QStringList critieal=hidden.replaceInStrings(QRegExp("^(\\w)"), "'\\1")
            .replaceInStrings(QRegExp("(\\w)$"), "\\1'");
    q.prepare(SQL(select_child_tables).arg(critieal.join(",")));
    q.exec();
    PRINT_ERROR(q);
    QList<MDLTable*> result;
    result=Q(q,MDLTable);
    CI(key,result)
}

QString MDLDao::idField(QString table)
{
    if(table.compare(CFG(KeyTblMain),Qt::CaseInsensitive)==0){
        return CFG(IDMainFieldName);
    }else{
        return CFG(ID);
    }
}

QStringList MDLDao::fieldGroup(QString table)
{
    QString key=QString("fieldGroup.%1").arg(table);
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_field_groups));
    q.bindValue(":table",table);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("GroupName").toString();
    }
    CI(key,result)
}

QStringList MDLDao::fieldOfGroup(QString table, QString group)
{
    QString key=QString("fieldOfGroup.%1.%2").arg(table).arg(group);
    CS(key,QStringList);
    QStringList result;
    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_fields_of_group));
    q.bindValue(":table",table);
    q.bindValue(":groupName",group);
    q.exec();
    PRINT_ERROR(q);
    while(q.next()){
        result<<q.value("KeyFld").toString();
    }
    CI(key,result)
}

QList<MDLFieldLookup *> MDLDao::fieldLookupinfo(QString table, QString field)
{
    QString key=QString("fieldLookupinfo.%1.%2").arg(table).arg(field);
    CS_LIST(key,MDLFieldLookup);

    QSqlQuery q(APP->mdl());
    q.prepare(SQL(select_field_lookup));
    q.bindValue(":table",table);
    q.bindValue(":field",field);
    q.exec();
    PRINT_ERROR(q);
    QList<MDLFieldLookup *> result;
    while(q.next()){
        result<<R(q.record(),MDLFieldLookup);
    }
    CI(key,result);
}

void MDLDao::loadChildTable(QStandardItem * parent,bool showGroup)
{
    QString strTblKey=parent->data(TABLE_NAME_ROLE).toString();
    QList<MDLTable *> childTables=UDL->childTables(strTblKey,APP->profile());
    foreach(MDLTable * table,childTables){
        QString strChildTblKey=table->KeyTbl();
        QString strChildTblName=table->CaptionLongP();
        QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
        if(showGroup)
            tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标
        tableItem->setText(strChildTblName); //设置第1列的文字
        tableItem->setFlags(Qt::ItemIsEnabled |Qt::ItemIsSelectable);
        tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(strChildTblKey,TABLE_NAME_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(strChildTblName,TEXT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(QString(),RECORD_DES_ROLE);
        tableItem->setData(QString(),PK_VALUE_ROLE);
        tableItem->setToolTip(QString("[%1] %2").arg(table->KeyTbl()).arg(table->Help()));
        parent->appendRow(tableItem);
        loadChildTable(tableItem,showGroup);
    }
}

QStandardItemModel * MDLDao::loadDataTree(bool showGroup,QObject * /*parent*/)
{
    //    QString key=QString("fieldLookupinfo.%1.%2").arg(table).arg(field);
    //    CS_LIST(key,MDLFieldLookup);

    QStandardItemModel * model=new QStandardItemModel(this);
    QStringList groups=UDL->tableGroup(APP->profile());
     QStandardItem*  item;
    foreach(QString group ,groups){
        if(showGroup){
            item=new QStandardItem(); //新建节点时设定类型为 itTopItem
            item->setIcon(APP->icons()["folder-open@4x"]); //设置第1列的图标

            item->setText(group); //设置第1列的文字
            item->setFlags(Qt::ItemIsEnabled |Qt::ItemIsSelectable);
            item->setData(CAT_ROLE,QWMApplication::GROUP); //设置节点第1列的Qt::UserRole的Data
            model->appendRow(item);//添加顶层节点
        }
        QList<MDLTable*> tables=UDL->tablesOfGroup(group,APP->profile());
        foreach(MDLTable * table ,tables){
            QString text=table->CaptionLongP();
            QString key=table->KeyTbl();
            QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
            if(showGroup)
                tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标

            tableItem->setText(text); //设置第1列的文字
            //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
            tableItem->setFlags(Qt::ItemIsEnabled |Qt::ItemIsSelectable);
            tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(key,TABLE_NAME_ROLE);
            tableItem->setData(text,TEXT_ROLE);
            tableItem->setData(QString(),RECORD_DES_ROLE);
            tableItem->setData(QString(),PK_VALUE_ROLE);
            tableItem->setToolTip(QString("[%1] %2").arg(table->KeyTbl()).arg(table->Help()));
            if(showGroup){
                item->appendRow(tableItem);
            }else{
                model->appendRow(tableItem);
            }

            loadChildTable(tableItem,showGroup);
        }

    }
    return model;
}
