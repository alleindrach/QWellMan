#include "edldao.h"
#include "libdao.h"
#include "mdldao.h"
#include "mdlfield.h"
#include "common.h"
#include "qwmapplication.h"
#include "QSqlError"
#include "QDebug"
#include "qwmlibquerymodel.h"
EDLDao * EDLDao::_instance=nullptr;
//QVector<QString> EDLDao::hiddenFields={"IDRec","sysOrderBy","sysModUser","sysHelp","sysModDate","sysTab"};
QHash<QString ,QVariant> EDLDao::_cache={};

BEGIN_SQL_DECLARATION(EDLDao)
DECL_SQL(select_table,"select  *  from %1 order by  sysOrderBy")
DECL_SQL(select_table_as_struct,"select  *  from %1 order by  sysOrderBy  limit 1")
DECL_SQL(select_table_tabs,"select  distinct sysTab  from %1 order by  sysTab")
DECL_SQL(select_table_by_tab,"select  *  from %1 where sysTab='%2' COLLATE NOCASE order by  sysOrderBy ")
DECL_SQL(select_table_by_tab_of_fields,"select  %3  from %1 %2 order by  sysOrderBy ")


END_SQL_DECLARATION

EDLDao::~EDLDao()
{

}
EDLDao *EDLDao::instance()
{
    if(_instance==nullptr){
        _instance=new EDLDao(APP->edl(),APP);
    }
    return _instance;
}


EDLDao::EDLDao(QSqlDatabase &db,QObject *parent) : QObject(parent),_db(db)
{

}
