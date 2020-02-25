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
#include "qexsortfilterproxymodel.h"
#define S1(model)\
    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
QWMSortFilterProxyModel::QWMSortFilterProxyModel(QObject *parent) : QExSortFilterProxyModel(parent)
{

}

QModelIndex QWMSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();
    S1(model);

    //    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());

    int realCol=realColumn(proxyIndex.column());
    if(realCol<0)
        return QModelIndex();
    QString sourceColName= model->fieldInPosByOrder(realCol);
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
    S1(model);
    QString sourceColName= model->fieldNameEx(sourceIndex.column());
    int visiblePos=model->fieldPosByOrder(sourceColName);
    if(visiblePos<0)
        return QModelIndex();

    QModelIndex index=model->index(sourceIndex.row(),visiblePos);
    QModelIndex mappedIndex=  QExSortFilterProxyModel::mapFromSource(index);
    QModelIndex proxyIndex=createIndex(mappedIndex.row(),groupedColumn(mappedIndex.column()));
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
    if(groupTitle(index.column()).isNull()){
        int col=realColumn(index.column());
        QModelIndex realIndex=createIndex(index.row(),col);
        return QExSortFilterProxyModel::flags(index);
    }else{
        return 0;
    }
}

QSqlRecord QWMSortFilterProxyModel::record() const
{
    S1(model);
    return model->record();
}
QSqlRecord QWMSortFilterProxyModel::record(int row) const
{
    S1(model);
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
    S1(model);//QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return model->revert();
}

int QWMSortFilterProxyModel::visibleFieldsCount()
{
    S1(model);
    return model->visibleFieldsCount();
}

bool QWMSortFilterProxyModel::isFieldVisible(const QString &field)
{
    S1(model);
    return model->isFieldVisible(field);
}

bool QWMSortFilterProxyModel::showGroup()
{
    return _showGroup;
}

void QWMSortFilterProxyModel::setShowGroup(bool v)
{
    _showGroup=v;
}

const int QWMSortFilterProxyModel::realColumn(const int col) const
{

    int realCol=col;
    if(_showGroup){
        if(_groupIndex.contains(col))
            return -1;
        QList<int> keys=_groupIndex.keys();
        qSort(keys);
        foreach(int key,keys){
            if(key>col)
                break;
            else
                realCol--;
        }
    }
    return realCol;
}

const int QWMSortFilterProxyModel::groupedColumn(const int col) const
{
    int gCol=col;
    if(_showGroup){

        QList<int> keys=_groupIndex.keys();
        qSort(keys);
        foreach(int key,keys){
            if(key>gCol)
                break;
            else
                gCol++;
        }
    }
    return gCol;
}

const QString QWMSortFilterProxyModel::groupTitle(const int col) const
{
    if(_showGroup){
        if( _groupIndex.contains(col))
            return _groupIndex[col];
    }
    return QString();
}

void QWMSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QWMTableModel  * model=(QWMTableModel *) sourceModel;
    QStringList groups=MDL->fieldGroup(model->tableName());
    int c=0;

    foreach(QString group,groups){
        QStringList fields=MDL->fieldOfGroup(model->tableName(),group);
        _groupIndex.insert(c,group);
        _fieldGroup.insert(group,fields);
        c+=(fields.count()+1);
    }
    QExSortFilterProxyModel::setSourceModel(sourceModel);
}


QSqlError QWMSortFilterProxyModel::lastError()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return  model->lastError();
}

QVariant QWMSortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    S1(model);
    if(orientation==Qt::Horizontal){
        if(this->groupTitle(section).isNull()){
            if(role==FIELD_ROLE){
                QModelIndex index=this->index(0,section);
                QModelIndex sourceIndex=this->mapToSource(index);
                QString fieldName=model->fieldNameEx(sourceIndex.column());
                return fieldName;
            }else
                return QExSortFilterProxyModel::headerData(section,orientation,role);
        }else{
            //分组
            if(role==Qt::DisplayRole){
                return this->groupTitle(section);
            }else if(role==Qt::BackgroundRole){
                return QColor(200,200,200);
            }else if(role==Qt::ForegroundRole){
                 return QColor(200,0,0);
            }
            return QVariant();
        }
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

QModelIndex QWMSortFilterProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return QExSortFilterProxyModel::index(row,column,parent);

}

QVariant QWMSortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if(!this->groupTitle(index.column()).isNull()){
        //分组
        if(role==Qt::DisplayRole){
            return QVariant();
        }else if(role==Qt::BackgroundRole){
            return QColor(200,200,200);
        }else if(role==Qt::ForegroundRole){
             return QColor(200,0,0);
        }
        return QVariant();
    }else{
        return QExSortFilterProxyModel::data(index,role);
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
