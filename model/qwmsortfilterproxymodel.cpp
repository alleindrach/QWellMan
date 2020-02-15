#include "qwmsortfilterproxymodel.h"
#include <QSqlRelationalTableModel>
#include "QSqlRecord"
#include "mdldao.h"
#include "udldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "common.h"
#include "qwmapplication.h"
QWMSortFilterProxyModel::QWMSortFilterProxyModel(int type,QObject *parent) : QSortFilterProxyModel(parent),_type(type)
{

}

bool QWMSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QString idwell=index0.data(PK_ROLE).toString();
    if(_type==QWMApplication::RECENT){
        bool isvalid=WELL->isRecentWell(idwell);
        return isvalid;
    }else if(_type==QWMApplication::FAVORITE){
        bool isvalid=WELL->isFavoriteWell(idwell);
        return isvalid;
    }else{
        return true;
    }

}
