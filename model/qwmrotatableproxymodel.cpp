#include "qwmrotatableproxymodel.h"
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
#include "qwmsortfilterproxymodel.h"

QWMRotatableProxyModel::QWMRotatableProxyModel(Mode mode,QObject *parent) : QSortFilterProxyModel(parent),_mode(mode)
{

}

bool QWMRotatableProxyModel::insertRecord(int row, const QSqlRecord &record)
{

    S(model);
    bool success=model->insertRecord(row,record);
    if(!success && model->lastError().isValid()){
        qDebug()<<"QWMSortFilterProxyModel::insertRecord error:"<< model->lastError().text();
    }
    return success;
}
QSqlRecord QWMRotatableProxyModel::record() const
{
    S(model);
    return model->record();
}
QSqlRecord QWMRotatableProxyModel::record(QModelIndex pos) const
{
    P(model);
    if(_mode==H){
        QModelIndex index=this->index(pos.row(),0);
        QModelIndex srcIndex=mapToSource(index);
        return model->record(srcIndex.row());
    }else{
        QModelIndex index=this->index(0,pos.column());
        QModelIndex srcIndex=mapToSource(index);
        return model->record(srcIndex.row());
    }
}
QModelIndex QWMRotatableProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();

    P(model);
    if(_mode==H){
        return model->index(proxyIndex.row(),proxyIndex.column());
    }else
    {
        return model->index(proxyIndex.column(),proxyIndex.row());
    }
}

QModelIndex QWMRotatableProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    P(model);
    if(_mode==H){
        return createIndex(sourceIndex.row(),sourceIndex.column());
    }else
    {
        return createIndex(sourceIndex.column(),sourceIndex.row());
    }
}

Qt::ItemFlags QWMRotatableProxyModel::flags(const QModelIndex &index) const
{
    P(model);
    QModelIndex sourceIndex=mapToSource(index);
    return model->flags(sourceIndex);
}

QItemSelection QWMRotatableProxyModel::mapSelectionToSource(const QItemSelection &selection) const
{
    QItemSelection convertedSelection;
    foreach(QModelIndex index, selection.indexes()){
        convertedSelection.indexes().append(mapToSource(index));
    }
    return convertedSelection;
}

QItemSelection QWMRotatableProxyModel::mapSelectionFromSource(const QItemSelection &selection) const
{
    QItemSelection convertedSelection;
    foreach(QModelIndex index, selection.indexes()){
        convertedSelection.indexes().append(mapFromSource(index));
    }
    return convertedSelection;
}

QModelIndex QWMRotatableProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    //    P(model);
    //    QModelIndex source_parent = mapToSource(parent);
    //    if(_mode==H){
    //        QModelIndex source_index = model->index( row,
    //                                                 column,source_parent);
    //        return source_index;
    //    }else{
    //        QModelIndex source_index = model->index( column,
    //                                                 row,source_parent);
    //        return source_index;
    //    }
    return QSortFilterProxyModel::index(row,column,parent);
}

QModelIndex QWMRotatableProxyModel::parent(const QModelIndex &child) const
{
    P(model);
    QModelIndex source_child  = mapToSource( child );
    QModelIndex source_parent = model->parent( source_child );
    return mapFromSource(source_parent );
}

int QWMRotatableProxyModel::rowCount(const QModelIndex &parent) const
{

    P(model);
    QModelIndex source_parent = mapToSource(parent);
    if(_mode==H){
        return model->rowCount(source_parent);
    }else
    {
        int rowCount=model->columnCount(source_parent);
//        qDebug()<<"row:"<<rowCount;
        return rowCount;
    }
}

int QWMRotatableProxyModel::columnCount(const QModelIndex &parent) const
{
    P(model);
    QModelIndex source_parent = mapToSource(parent);
    if(_mode==H){
        return model->columnCount(source_parent);
    }else
    {
        int columnCount=model->rowCount(source_parent);
//        qDebug()<<"column:"<<columnCount;
        return columnCount;
    }
}

QVariant QWMRotatableProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    P(model);
    if(_mode==H)
    {
        return model->headerData(section,orientation,role);
    }else
    {
        return model->headerData(section,orientation==Qt::Horizontal?Qt::Vertical:Qt::Horizontal,role);
    }
}

QString QWMRotatableProxyModel::fieldName(QModelIndex index)
{
    P(model);
    QModelIndex preProxyIndex=mapToSource(index);

    QVariant fieldInfo= model->headerData(preProxyIndex.column(),Qt::Horizontal,FIELD_ROLE);
    return fieldInfo.toString();
}

QString QWMRotatableProxyModel::tableName()
{
    S(model);
    return model->tableName();
}

bool QWMRotatableProxyModel::showGroup()
{
    return _showGroup;
}

void QWMRotatableProxyModel::setShowGroup(bool show)
{
    _showGroup=show;
    return;
}

bool QWMRotatableProxyModel::submitAll()
{
    P(proxyModel);
    return proxyModel->submitAll();
}

bool QWMRotatableProxyModel::submit()
{
    return QSortFilterProxyModel::submit();
}

void QWMRotatableProxyModel::revert()
{
    S(model);
    return model->revert();
}

QWMRotatableProxyModel::Mode QWMRotatableProxyModel::mode()
{
    return _mode;
}

void QWMRotatableProxyModel::setMode(QWMRotatableProxyModel::Mode m)
{
    _mode=m;

}

QSqlError QWMRotatableProxyModel::lastError()
{
    S(model);
    return  model->lastError();
}

bool QWMRotatableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
}
