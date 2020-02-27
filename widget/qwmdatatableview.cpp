#include "qwmdatatableview.h"
#include "QDebug"
#include "qwmsortfilterproxymodel.h"
#include "qwmtablemodel.h"
#include "qwmrotatableproxymodel.h"
#include "common.h"
#include "mdlfield.h"
#include "mdldao.h"
#include "qwmdatedelegate.h"
#include <QHeaderView>
#include "qwmliblookupdelegate.h"
#include "qwmdistinctvaluedelegate.h"
#include "qwmheaderview.h"
#include <QSettings>
#define CLEAR_DELEGATES(w) \
for(int i;i<w->model()->columnCount();i++){ \
    w->setItemDelegateForColumn(i,nullptr);\
}\
for(int i;i<w->model()->rowCount();i++){\
    w->setItemDelegateForRow(i,nullptr);\
}

QWMDataTableView::QWMDataTableView(QWidget *parent):QTableView(parent)
{
    QWMHeaderView * vHeaderView= new QWMHeaderView(Qt::Vertical, this);
    setVerticalHeader(vHeaderView);
    disconnect(vHeaderView, SIGNAL(sectionCountChanged(int,int)),0,0);
    connect(vHeaderView, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(rowCountChanged(int,int)));

    QWMHeaderView * hHeaderView= new QWMHeaderView(Qt::Horizontal, this);
    setHorizontalHeader(hHeaderView);
    disconnect(hHeaderView, SIGNAL(sectionCountChanged(int,int)),0,0);
    connect(hHeaderView, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(columnCountChanged(int,int)));
}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    QWMRotatableProxyModel * rotateModel=(QWMRotatableProxyModel*)model;
    disconnect(rotateModel,&QWMRotatableProxyModel::modeChange,0,0);
    connect(rotateModel,&QWMRotatableProxyModel::modeChange,this,&QWMDataTableView::on_mode_change);
}

void QWMDataTableView::bindDelegate()
{
    typedef  void (QAbstractItemView::* DelegateSettor)(int, QAbstractItemDelegate *) ;
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>(this->model());
    SX(sourcemodel,this->model());
    DelegateSettor  func;
    int counter=0;
    CLEAR_DELEGATES(this);
    if(model->mode()==QWMRotatableProxyModel::H){
        func =& QWMDataTableView::setItemDelegateForColumn;
        counter=model->columnCount();
        connect(this->horizontalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
    }else{
        func = & QWMDataTableView::setItemDelegateForRow;
        counter=model->rowCount();
        connect(this->verticalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
    }
    for(int i=0;i< counter;i++)
    {
        MDLField * fieldInfo;
        QString fieldName=model->fieldName(
                    model->mode()==QWMRotatableProxyModel::H?model->index(0,i): model->index(i,0)
                                                             );
        QString tableName=sourcemodel->tableName();
        fieldInfo=MDL->fieldInfo(tableName,fieldName);

        if(fieldInfo!=nullptr){
            if( fieldInfo->PhysicalType()==MDLDao::DateTime )
            {
                if(fieldInfo->LookupTyp()==MDLDao::DateAndTime){
                    (this->*func)(i,new QWMDateDelegate(QWMDateDelegate::DATETIME,"yyyy-MM-dd HH:mm:ss",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    (this->*func)(i,new QWMDateDelegate(QWMDateDelegate::DATE,"yyyy-MM-dd",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    (this->*func)(i,new QWMDateDelegate(QWMDateDelegate::TIME,"HH:mm:ss",this));
                }
            }else if( fieldInfo->LookupTyp()==MDLDao::LibEdit){
                (this->*func)(i,new QWMLibLookupDelegate(fieldInfo->LookupTableName(),fieldInfo->LookupFieldName(),fieldInfo->CaptionLong(),true,this));
            }
            else if(fieldInfo->LookupTyp()==MDLDao::LibOnly){
                (this->*func)(i,new QWMLibLookupDelegate(fieldInfo->LookupTableName(),fieldInfo->LookupFieldName(),fieldInfo->CaptionLong(),false,this));
            }
            else if(fieldInfo->LookupTyp()==MDLDao::DBDistinctValues){
                (this->*func)(i,new QWMDistinctValueDelegate(tableName,fieldName,this));
            }
        }
    }
}

QRect QWMDataTableView::visualRect(const QModelIndex &index) const{
    qDebug()<<"VisualRect:"<<index.row()<<","<<index.column();
    return QTableView::visualRect(index);
}

QModelIndex QWMDataTableView::indexAt(const QPoint &pos) const
{
    return QTableView::indexAt(pos);
}
bool QWMDataTableView::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event){
    return QTableView::edit(index,trigger,event);
}

void QWMDataTableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    qDebug()<<"dataChanged:["<<topLeft.row()<<","<<topLeft.column()<<"]-["<<bottomRight.row()<<","<<bottomRight.column()<<"],R["<<roles<<"]";
    return QTableView::dataChanged(topLeft,bottomRight,roles);
}

void QWMDataTableView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    qDebug()<<"rowsInserted:"<<start<<","<<end;
    return QTableView::rowsInserted(parent,start,end);
}

void QWMDataTableView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    qDebug()<<"rowsAboutToBeRemoved:"<<start<<","<<end;
    return QTableView::rowsAboutToBeRemoved(parent,start,end);
}

void QWMDataTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    return QTableView::selectionChanged(selected,deselected);
}

void QWMDataTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    return QTableView::currentChanged(current,previous);
}

void QWMDataTableView::updateEditorData()
{
    return QTableView::updateEditorData();
}

void QWMDataTableView::updateEditorGeometries()
{
    return QTableView::updateEditorGeometries();
}

void QWMDataTableView::updateGeometries()
{
    return QTableView::updateGeometries();
}

void QWMDataTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    return QTableView::closeEditor(editor,hint);
}

void QWMDataTableView::commitData(QWidget *editor)
{
    return QTableView::commitData(editor);
}

void QWMDataTableView::on_header_clicked(int section)
{
    qDebug()<<"on_header_clicked["<<section<<"]";
}

void QWMDataTableView::on_mode_change(QWMRotatableProxyModel::Mode)
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)this->model();
    PX(pmodel,this->model());
    SX(smodel,this->model());
    if(model->mode()==QWMRotatableProxyModel::H){
        for(int j=0;j<model->columnCount();j++){
            if(j<model->visibleFieldsCount()){
                setColumnHidden(j,false);
            }else
            {
                setColumnHidden(j,true);
            }
        }
        for(int i=0;i<model->rowCount();i++){
            setRowHidden(i,false);
        }
    }else{
        for(int i=0;i<model->rowCount();i++){
            if(i<model->visibleFieldsCount()){
                setRowHidden(i,false);
            }else
            {
                setRowHidden(i,true);
            }
        }
        for(int j=0;j<model->columnCount();j++){
            setColumnHidden(j,false);
        }
    }
    QSettings settings;
    settings.setValue(QString(EDITOR_TABLE_ENTRY_PREFIX).arg(smodel->tableName()),model->mode());
    bindDelegate();
    this->resize(this->size()+QSize(1,1));
}

void QWMDataTableView::rowCountChanged(int oldCount, int newCount)
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel *) this->verticalHeader()->model();
    if(model!=nullptr){
        if(model->mode()==QWMRotatableProxyModel::H){
            emit this->RecordCountChanged(oldCount,newCount);
        }
    }
    QTableView::rowCountChanged(oldCount,newCount);
}

void QWMDataTableView::columnCountChanged(int oldCount, int newCount)
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel *) this->horizontalHeader()->model();
    if(model!=nullptr){
        if(model->mode()==QWMRotatableProxyModel::V){
            emit this->RecordCountChanged(oldCount,newCount);
        }
    }
    QTableView::columnCountChanged(oldCount,newCount);
}
