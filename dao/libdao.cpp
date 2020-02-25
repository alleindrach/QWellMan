#include "libdao.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlError"
#include "QDebug"
LIBDao * LIBDao::_instance=nullptr;
QVector<QString> LIBDao::hiddenFields={"IDRec","sysOrderBy","sysModUser","sysHelp","sysModDate","sysTab"};
LIBDao::LIBDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{
    DECL_SQL(select_table,"select  *  from %1 order by  sysOrderBy");
    DECL_SQL(select_table_tabs,"select  distinct sysTab  from %1 order by  sysTab");
    DECL_SQL(select_table_by_tab,"select  *  from %1 where sysTab='%2' COLLATE NOCASE order by  sysOrderBy ");
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
QSqlQueryModel* LIBDao::libLookup(QString table)
{
    QString key=QString("libLookup.%1").arg(table);
    CS(key,QSqlQueryModel *);
    QSqlQueryModel* model=new QSqlQueryModel(this);
    model->setQuery(SQL(select_table).arg(table),APP->lib());
    CI(key,model)
}

QSqlQueryModel *LIBDao::libLookup(QString table, QString tab)
{
    QString key=QString("libLookup.tab.%1.%2").arg(table).arg(tab);
    CS(key,QSqlQueryModel *);
    QSqlQueryModel* model=new QSqlQueryModel(this);
    model->setQuery(SQL(select_table_by_tab).arg(table).arg(tab),APP->lib());
    CI(key,model)
}
