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
//#include "qexsortfilterproxymodel.h"
#include "qwmfieldeditcommand.h"
#include "qwmrecordeditcommand.h"
#include "qwmmain.h"
#define S1(model)\
    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());

QWMSortFilterProxyModel::QWMSortFilterProxyModel(QString idWell,QObject *parent)
    : QSortFilterProxyModel(parent),_idWell(idWell)
{

}

QModelIndex QWMSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();
    S1(model);

    //    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
    //1     过滤 掉 分栏列
    int realCol=realColumn(proxyIndex.column());
    if(realCol<0)
        return QModelIndex();
    //2     对应的原始字段
    QString sourceColName= model->fieldInVisibleOrder(realCol);
    if(sourceColName.isNull()||sourceColName.isEmpty()){
        return QModelIndex();
    }
    // 3    对应的原始列号
    //    计算字段没有field对应，这里sourceColPos就可能等于-1
    int cols=this->columnCount();
    int sourceColPos=model->fieldIndexEx(sourceColName);

    //    QModelIndex index=this->index(proxyIndex.row(),sourceColPos);
    // 4    取该行对应的原始行号
    QModelIndex proxyIndex2=this->index(proxyIndex.row(),0);
    QModelIndex sourceIndex2=QSortFilterProxyModel::mapToSource(proxyIndex2);
    //5     拼凑成index
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
    //    1   取原始列名
    QString sourceColName= model->fieldNameEx(sourceIndex.column());
    //    2 取显示的列号
    int visiblePos=model->fieldVisibleOrder(sourceColName);
    if(visiblePos<0)
        return QModelIndex();
    //    3   取加上分栏列 后的列号
    int visiblePosGrouped=groupedColumn(visiblePos);
    QModelIndex index=model->index(sourceIndex.row(),0);
    //    4 取过滤后 的 行号
    QModelIndex mappedIndex=  QSortFilterProxyModel::mapFromSource(index);

    QModelIndex proxyIndex=createIndex(mappedIndex.row(),visiblePosGrouped);
    return proxyIndex;
    //    return createIndex(sourceIndex.row(),visiblePos);
}

int QWMSortFilterProxyModel::mapColumnToSource(const int  col) const
{
    if(col<0)
        return col;
    S1(model);

    //    QWMTableModel * model=static_cast<QWMTableModel *>(this->sourceModel());
    //1     过滤 掉 分栏列
    int realCol=realColumn(col);
    if(realCol<0)
        return -1;
    //2     对应的原始字段
    QString sourceColName= model->fieldInVisibleOrder(realCol);
    if(sourceColName.isNull()||sourceColName.isEmpty()){
        return -1;
    }
    // 3    对应的原始列号
    //    计算字段没有field对应，这里sourceColPos就可能等于-1
    int sourceColPos=model->fieldIndexEx(sourceColName);

    return sourceColPos;
}

int QWMSortFilterProxyModel::mapRowToSource(int row) const
{
    if(row<0)
        return row;
    if(row>=this->rowCount())
        return -1;
    // 4    取该行对应的原始行号
    QModelIndex proxyIndex2=this->index(row,0);
    QModelIndex sourceIndex2=QSortFilterProxyModel::mapToSource(proxyIndex2);

    return sourceIndex2.row();
}


bool QWMSortFilterProxyModel::insertRecord(int /*row*/, const QSqlRecord &record)
{
    S1(model);
    QWMRecordEditCommand * command=new QWMRecordEditCommand( model,_idWell,record,QWMRecordEditCommand::insert);
    DOC->addUndoCommand(command,model->tableName(),_parentId);
    emit dataModified(model->tableName(),_parentId);
    return true;
}

bool QWMSortFilterProxyModel::insertRecordDirect(int row, const QSqlRecord &record)
{
    S1(model);
    bool success=model->insertRecord(row,record);
    if(!success && model->lastError().isValid()){
        qDebug()<<"QWMSortFilterProxyModel::insertRecord error:"<< model->lastError().text();
    }
    return success;
}

bool QWMSortFilterProxyModel::removeRecord(int row)
{
    S1(model);
    int sourceRow=mapRowToSource(row);
    QSqlRecord  record=model->record(sourceRow);
    PK_VALUE(id,record);
    QWMRecordEditCommand * command=new QWMRecordEditCommand(model,_idWell,record,QWMRecordEditCommand::remove);
    DOC->addUndoCommand(command,model->tableName(),_parentId);
    emit dataModified(model->tableName(),_parentId);
    return true;
}
Qt::ItemFlags QWMSortFilterProxyModel::flags(const QModelIndex &index) const
{


    //    QModelIndex sourceIndex=mapToSource(index);
    if(groupTitle(index.column()).isNull()){
        //        int col=realColumn(index.column());
        //        QModelIndex realIndex=createIndex(index.row(),col);
        return QSortFilterProxyModel::flags(index);
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
    QModelIndex srcIndex=QSortFilterProxyModel::mapToSource(index);
    return model->record(srcIndex.row());
}

QSqlRecord QWMSortFilterProxyModel::record(QModelIndex index) const
{
    return record(index.row());
}

void QWMSortFilterProxyModel::setFilterFunction( std::function<bool (int, const QModelIndex &)>  acceptor)
{
    _filterFunction=acceptor;
}

void QWMSortFilterProxyModel::setSortFunction( std::function<bool (const QModelIndex &, const QModelIndex &)>  acceptor)
{
    _sortFunction=acceptor;
}


bool QWMSortFilterProxyModel::submitAll()
{
    QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    //    qDebug()<<" is dirt:"<<model->isDirty();

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
    S1(model);//QWMTableModel *model=static_cast<QWMTableModel *>(this->sourceModel());
    return model->revert();
}

void QWMSortFilterProxyModel::reset()
{
    this->beginResetModel();
    this->endResetModel();
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

int QWMSortFilterProxyModel::realColumn(const int col) const
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

int QWMSortFilterProxyModel::groupedColumn(const int col) const
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

int QWMSortFilterProxyModel::columnCount(const QModelIndex &/*parent*/) const
{
    S1(model);
    int cols=model->visibleFieldsCount();
    if(_showGroup){
        cols+= _groupIndex.count();
    }
    //    qDebug()<<"QWMSortFilterProxyModel:"<<model->tableName()<<",columnCount="<<cols;
    return cols;
}

const QString QWMSortFilterProxyModel::groupTitle(const int col) const
{
    if(_showGroup){
        if( _groupIndex.contains(col))
            return _groupIndex[col];
    }
    return QString();
}

QString QWMSortFilterProxyModel::fieldName(QModelIndex index)
{
    S1(model);
    QModelIndex  sourceIndex=mapToSource(index);
    return model->fieldNameEx(sourceIndex.column());
}

void QWMSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QWMTableModel  * model=(QWMTableModel *) sourceModel;
    QStringList groups=UDL->fieldGroup(model->tableName());
    int c=0;

    foreach(QString group,groups){
        QStringList fields=UDL->fieldsVisibleInOrderByGroup(APP->profile(),model->tableName(),group);
        if(fields.size()>0){
            _groupIndex.insert(c,group);
            _fieldGroup.insert(group,fields);
            c+=(fields.count()+1);
        }
    }
    QSortFilterProxyModel::setSourceModel(sourceModel);
    disconnect(sourceModel,&QAbstractItemModel::dataChanged,0,0);
    connect(sourceModel,&QAbstractItemModel::dataChanged,this,&QWMSortFilterProxyModel::on_source_model_data_changed);
    disconnect(model,&QWMTableModel::rowsChanged,0,0);
    connect(model,&QWMTableModel::rowsChanged,this,&QWMSortFilterProxyModel::on_rows_changed);
    //    disconnect(model,&QWMTableModel::submitted,0,0);
    //    connect(model,&QWMTableModel::submitted,this,&QWMSortFilterProxyModel::on_source_model_data_submitted);
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
                int sourceSection=mapColumnToSource(section);
                QString fieldName=model->fieldNameEx(sourceSection);
                return fieldName;
            }else{
                int sourceSection=mapColumnToSource(section);
                QVariant v= model->headerData(sourceSection,orientation,role);
                return v;
            }
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
            return QSortFilterProxyModel::headerData(section,orientation,role);
        }
    }

}

QModelIndex QWMSortFilterProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return QSortFilterProxyModel::index(row,column,parent);

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
        return QSortFilterProxyModel::data(index,role);
    }
}

QVariant QWMSortFilterProxyModel::data(const int row, QString fieldname, int role) const
{
    S1(model);
    int sourceRow=mapRowToSource(row);
    int sourceCol=model->fieldIndexEx(fieldname);
    return model->data(model->index(sourceRow,sourceCol),role);
}

bool QWMSortFilterProxyModel::setData(const QModelIndex &item, const QVariant &value, int role)
{
    S1(model);
    if(role==Qt::EditRole){
        QModelIndex sourceIndex=mapToSource(item);
        Modifier m;
        m.col=sourceIndex.column();
        m.row=sourceIndex.row();
        m.newValue=value;
        m.oldValue=model->data(sourceIndex,Qt::EditRole);
        QList<Modifier> list;
        list.append(m);
        QWMFieldEditCommand * command=new QWMFieldEditCommand(model,list);

        DOC->addUndoCommand(command,model->tableName(),_parentId);
        emit dataModified(model->tableName(),_parentId);
    }else if(role==Qt::CheckStateRole){
        QModelIndex sourceIndex=mapToSource(item);
        Modifier m;
        m.col=sourceIndex.column();
        m.row=sourceIndex.row();
        m.newValue=value==Qt::Checked;
        m.oldValue=(model->data(sourceIndex,Qt::CheckStateRole)==Qt::Checked);
        QList<Modifier> list;
        list.append(m);
        QWMFieldEditCommand * command=new QWMFieldEditCommand(model,list);
        DOC->addUndoCommand(command,model->tableName(),_parentId);
        emit dataModified(model->tableName(),_parentId);
    }else if(role==LINKED_FIELDS){
        QModelIndex sourceIndex=mapToSource(item);
        Modifier m;
        m.col=sourceIndex.column();
        m.row=sourceIndex.row();
        QList<QPair<QString,QVariant>> newValue=value.value<QList<QPair<QString,QVariant>>>();
        QList<QPair<QString,QVariant>> oldValue;
        for(int i=0;i<newValue.size();i++){
            QString fieldName=newValue[i].first;
            int col=model->record().indexOf(fieldName);
            QModelIndex aIndex=model->index(m.row,col);
            QVariant oldv=model->data(aIndex,Qt::EditRole);
            oldValue.append(QPair<QString,QVariant>(fieldName,oldv));
        }
        m.newValue=value;
        m.oldValue=QVariant::fromValue(oldValue);
        QList<Modifier> list;
        list.append(m);
        QWMFieldEditCommand * command=new QWMFieldEditCommand(model,list,QWMFieldEditCommand::LinkedFields);
        DOC->addUndoCommand(command,model->tableName(),_parentId);
        emit dataModified(model->tableName(),_parentId);
    }else
    {
        return QSortFilterProxyModel::setData(item,value,role);
    }
    return true;

}

void QWMSortFilterProxyModel::calc(int row)
{

    int sourceRow=mapRowToSource(row);
    int preRow=row-1;
    int sourcePreRow=mapRowToSource(preRow);

    S1(model);
    model->calc(sourceRow,sourcePreRow);
}

void QWMSortFilterProxyModel::calcAll()
{
    for(int i=0;i<rowCount();i++){
        calc(i);
    }
}

void QWMSortFilterProxyModel::setParentId(QString v)
{
    _parentId=v;
}

QString QWMSortFilterProxyModel::parentId()
{
    return _parentId;
}

void QWMSortFilterProxyModel::on_source_model_data_changed(QModelIndex lefttop, QModelIndex rightbottom, QVector<int> roles)
{
    QModelIndex proxyLeftTop=mapFromSource(lefttop);
    QModelIndex proxyRightBottom=mapFromSource(rightbottom);
    emit dataChanged(proxyLeftTop,proxyRightBottom,roles);

}

void QWMSortFilterProxyModel::on_rows_changed()
{
    this->reset();
    emit rowsChanged();
}

void QWMSortFilterProxyModel::on_source_model_data_submitted(QString table)
{
    emit submitted(table);
}

bool QWMSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(_filterFunction==nullptr){
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);
    }else
    {
        return _filterFunction(sourceRow,sourceParent);
    }

}

bool QWMSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if(_sortFunction==nullptr){
        return QSortFilterProxyModel::lessThan(left,right);
    }else
    {
        return _sortFunction(left,right);
    }
}
