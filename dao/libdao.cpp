#include "libdao.h"
#include "mdldao.h"
#include "mdlfield.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlError"
#include "QDebug"
#include "qwmlibquerymodel.h"
LIBDao * LIBDao::_instance=nullptr;
QVector<QString> LIBDao::hiddenFields={"IDRec","sysOrderBy","sysModUser","sysHelp","sysModDate","sysTab"};
QHash<QString ,QVariant> LIBDao::_cache={};

BEGIN_SQL_DECLARATION(LIBDao)
DECL_SQL(select_table,"select  *  from %1 order by  sysOrderBy")
DECL_SQL(select_table_as_struct,"select  *  from %1 order by  sysOrderBy  limit 1")
DECL_SQL(select_table_tabs,"select  distinct sysTab  from %1 order by  sysTab")
DECL_SQL(select_table_by_tab,"select  *  from %1 where sysTab='%2' COLLATE NOCASE order by  sysOrderBy ")
DECL_SQL(select_table_by_tab_of_fields,"select  %3  from %1 %2 order by  sysOrderBy ")
END_SQL_DECLARATION

LIBDao::LIBDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
}
LIBDao::~LIBDao()
{

}
LIBDao *LIBDao::instance()
{
    if(_instance==nullptr){
        _instance=new LIBDao(APP->lib(),APP);
    }
    return _instance;
}

//QStringList LIBDao::libLookup(QString table, QString displayFld)
//{
//    QString key=QString("libLookup.%1.%2").arg(table).arg(displayFld);
//    CS(key,QStringList);
//    QSqlQuery q(APP->lib());
//    q.prepare(SQL(select_table).arg(table));
//    q.exec();
//    PRINT_ERROR(q);
//    QStringList result;

//    while(q.next()){
//        result<<q.value(displayFld).toString();
//    }
//    CI(key,result)
//}

QStringList LIBDao::libTabs(QString table)
{
    QString key=QString("libTabs.%1").arg(table);
    CS(key,QStringList);
    QSqlQuery q(APP->lib());
    q.prepare(SQL(select_table_tabs).arg(table));
    q.exec();
    PRINT_ERROR(q);
    QStringList result;

    while(q.next()){
        result<<q.value("sysTab").toString();
    }
    CI(key,result)
}
QSqlQueryModel* LIBDao::libLookupAsStruct(QString table)
{
    QString key=QString("libLookup.%1").arg(table);
    CS(key,QSqlQueryModel *);
    QSqlQueryModel* model=new QSqlQueryModel(this);
    model->setQuery(SQL(select_table_as_struct).arg(table),APP->lib());
    CI(key,model)
}

//QSqlQueryModel *LIBDao::libLookup(QString table, QString tab)
//{
//    QString key=QString("libLookup.tab.%1.%2").arg(table).arg(tab);
//    CS(key,QSqlQueryModel *);
//    QSqlQueryModel* model=new QSqlQueryModel(this);
//    model->setQuery(SQL(select_table_by_tab).arg(table).arg(tab),APP->lib());
//    QSqlRecord rec=model->record();
//    for(int i=0;i<rec.count();i++){
//        model->setHeaderData(i,Qt::Horizontal,rec.fieldName(i),FIELD_ROLE);
//    }
//    CI(key,model)
//}

QSqlQueryModel *LIBDao::libLookup(QString table,QString lib, QString tab, QStringList fields)
{
    QString key=QString("libLookup.tabs.%1.%2.%3").arg(table).arg(lib).arg(tab);
    CS(key,QSqlQueryModel *);
    QWMLibQueryModel * model=new QWMLibQueryModel(table,lib, this);
    QString where="";
    if(!tab.isNull() && !tab.isEmpty()){
        where=QString("where sysTab='%1' COLLATE NOCASE").arg(tab);
    }
    model->setQuery(SQL(select_table_by_tab_of_fields).arg(lib).arg(where).arg(fields.join(",")),APP->lib());
    CI(key,model)
}
