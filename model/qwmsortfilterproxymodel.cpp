#include "qwmsortfilterproxymodel.h"
#include <QSqlRelationalTableModel>
#include "QSqlRecord"
#include "mdldao.h"
#include "udldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "common.h"
#include "qwmapplication.h"
#include "qwmtablemodel.h"
#include "QSqlError"
#include "QDebug"
#include "QSqlIndex"
#define S(model)\
    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
QWMSortFilterProxyModel::QWMSortFilterProxyModel(QObject *parent) : QExSortFilterProxyModel(parent)
{

}

QModelIndex QWMSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();
    S(model);

    QString sourceColName= model->fieldInPosByOrder(proxyIndex.column());
    if(sourceColName.isNull()||sourceColName.isEmpty()){
        return QModelIndex();
    }
    //计算字段没有field对应，这里sourceColPos就可能等于-1
    int sourceColPos=model->fieldIndexEx(sourceColName);

//    QModelIndex index=this->index(proxyIndex.row(),sourceColPos);
    QModelIndex proxyIndex2=this->index(proxyIndex.row(),0);
    QModelIndex sourceIndex2=QExSortFilterProxyModel::mapToSource(proxyIndex2);

    QModelIndex index= model->createIndex(sourceIndex2.row(),sourceColPos);
    return index;
    //    if(sourceColPos>=model->record().count()){
    //        return model->index(proxyIndex.row(),sourceColPos);
    //    }else
//    return QExSortFilterProxyModel::mapToSource(index);
    //    return model->index(proxyIndex.row(),sourceColPos);
}

QModelIndex QWMSortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();
    S(model);
    QString sourceColName= model->fieldNameEx(sourceIndex.column());
    int visiblePos=model->fieldPosByOrder(sourceColName);
    if(visiblePos<0)
        return QModelIndex();
    QModelIndex index=model->index(sourceIndex.row(),visiblePos);
    return  QExSortFilterProxyModel::mapFromSource(index);
    //    return createIndex(sourceIndex.row(),visiblePos);
}


bool QWMSortFilterProxyModel::insertRecord(int row, const QSqlRecord &record)
{
    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
    bool success=model->insertRecord(row,record);
    if(!success && model->lastError().isValid()){
        qDebug()<<"QWMSortFilterProxyModel::insertRecord error:"<< model->lastError().text();
    }
    return success;
}
Qt::ItemFlags QWMSortFilterProxyModel::flags(const QModelIndex &index) const
{


//    QModelIndex sourceIndex=mapToSource(index);
    return QExSortFilterProxyModel::flags(index);
}

QSqlRecord QWMSortFilterProxyModel::record() const
{
    S(model);
    return model->record();
}
QSqlRecord QWMSortFilterProxyModel::record(int row) const
{
    S(model);
    QModelIndex index=this->index(row,0);
    QModelIndex srcIndex=QExSortFilterProxyModel::mapToSource(index);
    return model->record(srcIndex.row());
}

void QWMSortFilterProxyModel::setFilterFunction( std::function<bool (int, const QModelIndex &)>  acceptor)
{
    _filterFunction=acceptor;
}



bool QWMSortFilterProxyModel::submitAll()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    qDebug()<<" is dirt:"<<model->isDirty();

    model->database().transaction();
    if(model->submitAll())
    {
        model->database().commit();
        return  true;
    }else
    {
        model->database().rollback();
        model->revertAll();
        return false;
    }
    if(model->lastError().isValid()){
        qDebug()<<" Submit all:"<<model->lastError().text();
    }
}

bool QWMSortFilterProxyModel::submit()
{
    return QExSortFilterProxyModel::submit();
}

void QWMSortFilterProxyModel::revert()
{
    S(model);//QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return model->revert();
}

int QWMSortFilterProxyModel::visibleFieldsCount()
{
    S(model);
    return model->visibleFieldsCount();
}

bool QWMSortFilterProxyModel::isFieldVisible(const QString &field)
{
    S(model);
    return model->isFieldVisible(field);
}


QSqlError QWMSortFilterProxyModel::lastError()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return  model->lastError();
}

QVariant QWMSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    S(model);
    if(orientation==Qt::Horizontal){
        return QExSortFilterProxyModel::headerData(section,orientation,role);
    }else
    {
        if(role==Qt::DisplayRole)
        {
            QString s=QString::number(section+1);
            return s;
        }else
        {
            return QExSortFilterProxyModel::headerData(section,orientation,role);
        }
    }

}

bool QWMSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(_filterFunction==nullptr){
        return QExSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);
    }else
    {
        return _filterFunction(sourceRow,sourceParent);
    }

}
