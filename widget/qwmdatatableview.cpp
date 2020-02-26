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
#include <QSettings>
QWMDataTableView::QWMDataTableView(QWidget *parent):QTableView(parent)
{

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
                (this->*func)(i,new QWMDistinctValueDelegate(fieldInfo->LookupTableName(),fieldInfo->LookupFieldName(),this));
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
    return QTableView::dataChanged(topLeft,bottomRight,roles);
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
