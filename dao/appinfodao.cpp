#include "appinfodao.h"

AppInfoDao::AppInfoDao(QSqlDatabase *db,QObject *parent) : QObject(parent),_db(db)
{

}
