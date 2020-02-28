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

QWMRotatableProxyModel::QWMRotatableProxyModel(Mode mode,QObject *parent) : QExSortFilterProxyModel(parent),_mode(mode),_showGroup(mode==QWMRotatableProxyModel::V)
{

}

bool QWMRotatableProxyModel::insertRecord(int row, const QSqlRecord &record)
{

    P(model);
    bool success=model->insertRecord(row,record);
    if(!success && model->lastError().isValid()){
        qDebug()<<"QWMSortFilterProxyModel::insertRecord error:"<< model->lastError().text();
    }
    return success;
}
bool QWMRotatableProxyModel::insertRecordDirect(int row, const QSqlRecord &record)
{

    P(model);
    bool success=model->insertRecordDirect(row,record);
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

QVariant QWMRotatableProxyModel::data(const QModelIndex &item, int role) const
{
    //    if(_mode==H){
    return QExSortFilterProxyModel::data(item,role);
    //    }else
    //    {
    //        QModelIndex sourceIndex=mapToSource(item);
    //        return QExSortFilterProxyModel::data(sourceIndex,role);
    //    }
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
    //    QModelIndex sourceIndex=mapToSource(index);
    return QExSortFilterProxyModel::flags(index);
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

    //    if(_mode==H){
    return QExSortFilterProxyModel::index(row,column,parent);
    //    }else
    //    {
    //        return QExSortFilterProxyModel::index(column,row,parent);
    //    }

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
    S(sourceModel);
    QModelIndex source_parent = mapToSource(parent);
    if(_mode==H){
        return model->columnCount(source_parent);
    }else
    {
        if(sourceModel->tableName()=="wvJobReport"){
            //               qDebug()<<"error";
            int i=0;

        }
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

QModelIndex QWMRotatableProxyModel::firstEditableCell()
{
    P(model);
    if(model->rowCount()<=0)
        return  QModelIndex();
    else{
        int i=0;
        QString fn=QString();
        for(i=0;i<model->columnCount();i++){
            fn= model->headerData(i,Qt::Horizontal,FIELD_ROLE).toString();
            if(!fn.isNull())
                break;
            if(i>=model->columnCount())
                break;
        }
        if(!fn.isNull()){
            QModelIndex  index= mapFromSource( model->index(0,i));
            return index;
        }
    }
    return QModelIndex();
}

bool QWMRotatableProxyModel::showGroup()
{
    P(model);
    return model->showGroup() ;
}

void QWMRotatableProxyModel::setShowGroup(bool show)
{
    P(model);
    model->setShowGroup(show);

}

bool QWMRotatableProxyModel::submitAll()
{
    P(proxyModel);
    return proxyModel->submitAll();
}

bool QWMRotatableProxyModel::submit()
{
    return QExSortFilterProxyModel::submit();
}

void QWMRotatableProxyModel::revert()
{
    P(model);
    return model->revert();
}

int QWMRotatableProxyModel::visibleFieldsCount()
{
    P(model);
    return model->visibleFieldsCount();
}

bool QWMRotatableProxyModel::isFieldVisible(const QString &field)
{
    P(model);
    return model->isFieldVisible(field);
}

void QWMRotatableProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QExSortFilterProxyModel::setSourceModel(sourceModel);
    P(model);
    model->setShowGroup(_mode);
    disconnect(sourceModel,&QAbstractItemModel::dataChanged,0,0);
    connect(sourceModel,&QAbstractItemModel::dataChanged,this,&QWMRotatableProxyModel::on_source_model_data_changed);
}

QWMRotatableProxyModel::Mode QWMRotatableProxyModel::mode()
{
    return _mode;
}

void QWMRotatableProxyModel::setMode(QWMRotatableProxyModel::Mode m)
{
    _mode=m;
    beginResetModel();
    P(model);
    model->setShowGroup(m==QWMRotatableProxyModel::V);
    endResetModel();
    emit modeChange(m);
}

QSqlError QWMRotatableProxyModel::lastError()
{
    S(model);
    return  model->lastError();
}

bool QWMRotatableProxyModel::isDirty()
{
    S(model);
    return model->isDirty();
}

void QWMRotatableProxyModel::on_source_model_data_changed(QModelIndex  lefttop, QModelIndex rightbottom, QVector<int>roles)
{
    QModelIndex proxyLeftTop=mapFromSource(lefttop);
    QModelIndex proxyRightBottom=mapFromSource(rightbottom);
    emit dataChanged(proxyLeftTop,proxyRightBottom,roles);
}

bool QWMRotatableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
}
bool QWMRotatableProxyModel::removeRecord(QModelIndex index){
    P(model);
    QModelIndex  sourceIndex=mapToSource(index);
    model->removeRecord(sourceIndex.row());
}
