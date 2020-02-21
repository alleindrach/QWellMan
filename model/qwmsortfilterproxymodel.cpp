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
QWMSortFilterProxyModel::QWMSortFilterProxyModel(int type,QObject *parent) : QSortFilterProxyModel(parent),_type(type)
{

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

    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
    return model->flags(index);
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
    QModelIndex srcIndex=QSortFilterProxyModel::mapToSource(index);
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
    return QSortFilterProxyModel::submit();
}

void QWMSortFilterProxyModel::revert()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return model->revert();
}


QSqlError QWMSortFilterProxyModel::lastError()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return  model->lastError();
}

QVariant QWMSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
//    if( role == Qt::DisplayRole){
//        if(this->mode()==Qt::Orientation::Horizontal){
//            if( orientation==Qt::Orientation::Horizontal)
//            {
//                Nut::FieldModel  * field=fieldBySection(section);
//                if(field)
//                    return field->displayName;
//            }else
//            {
//                return QString::number(section+1, 3);
//            }
//        }else if(this->mode()==Qt::Orientation::Vertical){
//            if( orientation==Qt::Orientation::Vertical){
//                Nut::FieldModel  * field=fieldBySection(section);
//                if(field)
//                    return field->displayName;
//            }else
//            {
//                return QString::number(section+1, 3);
//            }
//        }
//    }

//    Q_D(const QSortFilterProxyModel);
//    IndexMap::const_iterator it = d->create_mapping(QModelIndex());
//    if (it.value()->source_rows.count() * it.value()->source_columns.count() > 0)
//        return QAbstractProxyModel::headerData(section, orientation, role);
//    int source_section;
//    if (orientation == Qt::Vertical) {
//        if (section < 0 || section >= it.value()->source_rows.count())
//            return QVariant();
//        source_section = it.value()->source_rows.at(section);
//    } else {
//        if (section < 0 || section >= it.value()->source_columns.count())
//            return QVariant();
//        source_section = it.value()->source_columns.at(section);
//    }
    if(orientation==Qt::Horizontal){
        return QSortFilterProxyModel::headerData(section,orientation,role);
    }else
    {
        if(role==Qt::DisplayRole)
        {
            QString s=QString::number(section+1);
            return s;
        }else
        {
            return QSortFilterProxyModel::headerData(section,orientation,role);
        }
    }

}

bool QWMSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(_filterFunction==nullptr){
        return true;
    }else
    {
        return _filterFunction(sourceRow,sourceParent);
    }

}
