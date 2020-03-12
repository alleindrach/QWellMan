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
#define UPPER_LEVEL_ORIENT(uo,o) \
    Qt::Orientation  uo; \
    if(_mode==QWMRotatableProxyModel::H) \
{ \
    uo=o; \
    }else \
{ \
    uo=o==Qt::Horizontal?Qt::Vertical:Qt::Horizontal;\
    }

#define SOURCE_COLUMN(section,index) \
    if(_mode==H){ \
        section=index.column(); \
    }else \
    { \
        section=index.row(); \
    }

QWMRotatableProxyModel::QWMRotatableProxyModel(QString idWell,Mode mode ,QObject *parent)
    : QAbstractProxyModel(parent),_idWell(idWell),_mode(mode),_showGroup(mode==QWMRotatableProxyModel::V)
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
    return QAbstractProxyModel::data(item,role);
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
    if(_mode==H){
        return createIndex(sourceIndex.row(),sourceIndex.column());
    }else
    {
        return createIndex(sourceIndex.column(),sourceIndex.row());
    }
}

int QWMRotatableProxyModel::mapToSourceTable(QModelIndex index)
{
    P(pmodel);
    QModelIndex pIndex=this->mapToSource(index);
    return pmodel->mapRowToSource(pIndex.row());
}

Qt::ItemFlags QWMRotatableProxyModel::flags(const QModelIndex &index) const
{
    return QAbstractProxyModel::flags(index);
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

QModelIndex QWMRotatableProxyModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
    return createIndex(row,column);
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
        int columnCount=model->rowCount(source_parent);
        return columnCount;
    }
}

int QWMRotatableProxyModel::recordCount()
{
     P(model);
     return model->rowCount();
}

QVariant QWMRotatableProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    P(model);
    if(_mode==H)
    {
        if(orientation==Qt::Horizontal&& role==Qt::DecorationRole){
            QString table=tableName();
            QString field=this->fieldName(this->index(0,section));
            MDLField *  fieldInfo=UDL->fieldInfo(table,field);
            if(fieldInfo!=nullptr&& (fieldInfo->LookupTyp()==MDLDao::Icon||
                                     fieldInfo->LookupTyp()==MDLDao::Foreign||
                                     fieldInfo->LookupTyp()==MDLDao::LibEdit||
                                     fieldInfo->LookupTyp()==MDLDao::LibOnly||
                                     fieldInfo->LookupTyp()==MDLDao::List||
                                     fieldInfo->LookupTyp()==MDLDao::DateAndTime)
                    )
            {
                return APP->icon("lookup");
            }
        }
        return model->headerData(section,orientation,role);
    }else
    {
        if(orientation==Qt::Vertical&& role==Qt::DecorationRole){
                QString table=tableName();
                QString field=this->fieldName(this->index(section,0));
                MDLField *  fieldInfo=UDL->fieldInfo(table,field);
                if(fieldInfo!=nullptr&& (fieldInfo->LookupTyp()==MDLDao::Icon||
                                         fieldInfo->LookupTyp()==MDLDao::Foreign||
                                         fieldInfo->LookupTyp()==MDLDao::LibEdit||
                                         fieldInfo->LookupTyp()==MDLDao::LibOnly||
                                         fieldInfo->LookupTyp()==MDLDao::List||
                                         fieldInfo->LookupTyp()==MDLDao::DateAndTime)
                        )
                {
                    return APP->icon("lookup");
                }
            }

        return model->headerData(section,orientation==Qt::Horizontal?Qt::Vertical:Qt::Horizontal,role);
    }
}

QString QWMRotatableProxyModel::fieldName(QModelIndex index) const
{
    P(model);
    int section=-1;
    SOURCE_COLUMN(section,index);
    QVariant fieldInfo= model->headerData(section,Qt::Horizontal,FIELD_ROLE);
    return fieldInfo.toString();
}

QString QWMRotatableProxyModel::fieldTitle(QModelIndex index)
{
    P(model);
    int section=-1;
    SOURCE_COLUMN(section,index);
    QVariant fieldInfo= model->headerData(section,Qt::Horizontal,Qt::DisplayRole);
    return fieldInfo.toString();
}

QModelIndex QWMRotatableProxyModel::indexOfSameRecord(QModelIndex index, QString fieldName)
{
    P(pmodel);
    S(smodel);
    QModelIndex pindex=this->mapToSource(index);
    QModelIndex sindex=pmodel->mapToSource(pindex);

    int col=smodel->record().indexOf(fieldName);
    if(col>=0){
        QModelIndex saindex=smodel->index(sindex.row(),col);
        QModelIndex paindex=pmodel->mapFromSource(saindex);
        QModelIndex taindex=this->mapFromSource(paindex);
        return taindex;
    }

    return QModelIndex();
}
QString QWMRotatableProxyModel::tableName() const
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
    return QAbstractProxyModel::submit();
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

    //    disconnect(sourceModel,&QAbstractItemModel::dataChanged,0,0);
    //    connect(sourceModel,&QAbstractItemModel::dataChanged,this,&QWMRotatableProxyModel::on_source_model_data_changed);


    if (sourceModel ==this->sourceModel())
        return;
    QWMSortFilterProxyModel  * prevModel=qobject_cast<QWMSortFilterProxyModel*>(this->sourceModel());
    QWMSortFilterProxyModel  * thisModel=qobject_cast<QWMSortFilterProxyModel*>(sourceModel);

    beginResetModel();

    disconnect(prevModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,0);
    disconnect(prevModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, 0);
    disconnect(prevModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), this, 0);
    disconnect(prevModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this,0);
    disconnect(prevModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),this, 0);
    disconnect(prevModel, SIGNAL(columnsInserted(QModelIndex,int,int)), this, 0);
    disconnect(prevModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this, 0);
    disconnect(prevModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, 0);
    disconnect(prevModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)), this, 0);
    disconnect(prevModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),  this, 0);
    disconnect(prevModel, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), this, 0);
    disconnect(prevModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),this, 0);
    disconnect(prevModel, SIGNAL(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), this, 0);
    disconnect(prevModel, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)),this, 0);
    disconnect(prevModel, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),this, 0);
    disconnect(prevModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)), this, 0);
    disconnect(prevModel, SIGNAL(modelAboutToBeReset()), this, 0);
    disconnect(prevModel, SIGNAL(modelReset()), this, 0);


    QAbstractProxyModel::setSourceModel(sourceModel);

    //    P(proxyModel);
    //    proxyModel->setShowGroup(_mode);

    connect(thisModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(thisModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(sourceHeaderDataChanged(Qt::Orientation,int,int)));
    connect(thisModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(sourceColumnsAboutToBeInserted(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(sourceColumnsInserted(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceColumnsAboutToBeRemoved(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceColumnsRemoved(QModelIndex,int,int)));
    connect(thisModel, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(thisModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(thisModel, SIGNAL(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceColumnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(thisModel, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceColumnsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(thisModel, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(sourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
    connect(thisModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(sourceLayoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
    connect(thisModel, SIGNAL(modelAboutToBeReset()), this, SLOT(_q_sourceAboutToBeReset()));
    connect(thisModel, SIGNAL(modelReset()), this, SLOT(_q_sourceReset()));
    endResetModel();

    P(pmodel);
    connect(pmodel,&QWMSortFilterProxyModel::rowsChanged,this,&QWMRotatableProxyModel::on_rows_changed);
}

QWMRotatableProxyModel::Mode QWMRotatableProxyModel::mode() const
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
    //    emit modeChanged();
}

void QWMRotatableProxyModel::reset()
{
    beginResetModel();
    endResetModel();
    //    emit modeChanged();
}

void QWMRotatableProxyModel::calc()
{
    P(model);
    model->calcAll();
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

void QWMRotatableProxyModel::source_items_inserted(const QModelIndex &source_parent, int start, int end, Qt::Orientation orient)
{

    UPPER_LEVEL_ORIENT(upperOrient,orient);
    QModelIndex mappedParent=mapFromSource(source_parent);
    if (upperOrient == Qt::Vertical)
        beginInsertRows(mappedParent, start, end);
    else
        beginInsertColumns(mappedParent, start, end);

    if (upperOrient == Qt::Vertical)
        endInsertRows();
    else
        endInsertColumns();

    this->reset();
}

//void QWMRotatableProxyModel::on_source_model_data_changed(QModelIndex  lefttop, QModelIndex rightbottom, QVector<int>roles)
//{
//    QModelIndex proxyLeftTop=mapFromSource(lefttop);
//    QModelIndex proxyRightBottom=mapFromSource(rightbottom);
//    emit dataChanged(proxyLeftTop,proxyRightBottom,roles);
//}

//void QWMRotatableProxyModel::rowsInsertedAdaptor(const QModelIndex &index, int start, int end)
//{
//    if(_mode==QWMRotatableProxyModel::H){
//        emit this->rowsInserted(index,start,end);
//    }else
//    {
//        emit this->columnsInserted(index,start,end);
//    }
//}

//void QWMRotatableProxyModel::rowsRemovedAdaptor(const QModelIndex &index, int start, int end)
//{
//    if(_mode==QWMRotatableProxyModel::H){
//        emit this->rowsRemoved(index,start,end);
//    }else
//    {
//        emit this->columnsRemoved(index,start,end);
//    }
//}

//void QWMRotatableProxyModel::columnsInsertedAdaptor(const QModelIndex &index, int start, int end)
//{
//    if(_mode==QWMRotatableProxyModel::H){
//        emit this->columnsInserted(index,start,end);
//    }else
//    {
//        emit this->rowsInserted(index,start,end);
//    }
//}

//void QWMRotatableProxyModel::columnsRemovedAdaptor(const QModelIndex &index, int start, int end)
//{
//    if(_mode==QWMRotatableProxyModel::H){
//        emit this->columnsRemoved(index,start,end);
//    }else
//    {
//        emit this->rowsRemoved(index,start,end);
//    }
//}

struct QSortFilterProxyModelDataChanged
{
    QSortFilterProxyModelDataChanged(const QModelIndex &tl, const QModelIndex &br)
        : topLeft(tl), bottomRight(br) { }

    QModelIndex topLeft;
    QModelIndex bottomRight;
};

void QWMRotatableProxyModel::sourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QVector<int> &roles)
{
    if (!source_top_left.isValid() || !source_bottom_right.isValid())
        return;
    QModelIndex rotatedTopLeft=mapFromSource(source_top_left);
    QModelIndex rotatedBottomRight=mapFromSource(source_bottom_right);
    emit dataChanged(rotatedTopLeft, rotatedBottomRight, roles);
}

void QWMRotatableProxyModel::sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end)
{
    Q_ASSERT(start <= end);
    if( _mode==QWMRotatableProxyModel::H){
        emit headerDataChanged(orientation, start, end);
    }else{
        emit headerDataChanged(orientation==Qt::Horizontal?Qt::Vertical:Qt::Horizontal, start, end);
    }
}

void QWMRotatableProxyModel::sourceRowsAboutToBeInserted(const QModelIndex &/*source_parent*/, int /*start*/, int /*end*/)
{
    //no mapping ,do nothing
}

void QWMRotatableProxyModel::sourceRowsInserted(const QModelIndex &source_parent, int start, int end)
{
    source_items_inserted(source_parent,start,end,Qt::Vertical);
    // last_top_source should now become visible
    sourceDataChanged(source_parent, source_parent, QVector<int>());
}

void QWMRotatableProxyModel::sourceColumnsAboutToBeInserted(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
}

void QWMRotatableProxyModel::sourceColumnsInserted(const QModelIndex &source_parent, int start, int end)
{
    source_items_inserted(source_parent,start,end,Qt::Horizontal);
    sourceDataChanged(source_parent, source_parent, QVector<int>());
}

void QWMRotatableProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end)
{
    source_items_about_to_be_removed(source_parent,start,end,Qt::Vertical);
}

void QWMRotatableProxyModel::sourceRowsRemoved(const QModelIndex &source_parent, int start, int end)
{
    source_items_removed(source_parent, start, end, Qt::Vertical);
}

void QWMRotatableProxyModel::sourceColumnsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end)
{
    source_items_about_to_be_removed(source_parent,start,end,Qt::Horizontal);
}

void QWMRotatableProxyModel::sourceColumnsRemoved(const QModelIndex &source_parent, int start, int end)
{
    source_items_removed(source_parent, start, end, Qt::Horizontal);
}

void QWMRotatableProxyModel::sourceRowsAboutToBeMoved(const QModelIndex &/*sourceParent*/, int /*sourceStart*/, int /*sourceEnd*/, const QModelIndex &/*destParent*/, int /*dest*/)
{

}

void QWMRotatableProxyModel::sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    saved_layoutChange_parents.clear();
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            saved_layoutChange_parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = mapFromSource(parent);
        // Might be filtered out.
        if (mappedParent.isValid())
            saved_layoutChange_parents << mappedParent;
    }

    // All parents filtered out.
    if (!sourceParents.isEmpty() && saved_layoutChange_parents.isEmpty())
        return;

    emit layoutAboutToBeChanged(saved_layoutChange_parents);

}

void QWMRotatableProxyModel::sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    if (!sourceParents.isEmpty() && saved_layoutChange_parents.isEmpty())
        return;
    emit layoutChanged(saved_layoutChange_parents);
    saved_layoutChange_parents.clear();
}

void QWMRotatableProxyModel::sourceAboutToBeReset()
{
    beginResetModel();
}

void QWMRotatableProxyModel::sourceReset()
{
    endResetModel();
}

void QWMRotatableProxyModel::on_source_model_submitted(QString table)
{
    emit submitted(table);
}

void QWMRotatableProxyModel::source_items_about_to_be_removed(const QModelIndex &source_parent, int start, int end, Qt::Orientation orient)
{
    remove_source_items(source_parent,start,end,orient,true);
}

void QWMRotatableProxyModel::source_items_removed(const QModelIndex &/*source_parent*/, int /*start*/, int /*end*/, Qt::Orientation /*orient*/)
{
    this->reset();
}

void QWMRotatableProxyModel::remove_source_items( const QModelIndex &source_parent,  int start, int end,Qt::Orientation orient, bool emit_signal)
{
    if (emit_signal) {
        UPPER_LEVEL_ORIENT(uplevelOrient,orient);
        QModelIndex mappedParent=mapFromSource(source_parent);
        if (uplevelOrient == Qt::Vertical)
            beginRemoveRows(mappedParent, start, end);
        else
            beginRemoveColumns(mappedParent, start, end);

        if (uplevelOrient == Qt::Vertical)
            endRemoveRows();
        else
            endRemoveColumns();

    }
}
//bool QWMRotatableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
//{
//    return true;
//}
bool QWMRotatableProxyModel::removeRecord(QModelIndex index){
    P(model);
    QModelIndex  sourceIndex=mapToSource(index);
    model->removeRecord(sourceIndex.row());
}

void QWMRotatableProxyModel::on_rows_changed(){
    this->reset();
    emit rowsChanged();
}
