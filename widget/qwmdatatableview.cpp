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
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>(this->model());
    SX(sourcemodel,this->model());
    if(model->mode()==QWMRotatableProxyModel::H){
        for(int i=0;i< model->columnCount();i++)
        {
            MDLField * fieldInfo;
            QString fieldName=model->fieldName(model->index(0,i));
            QString tableName=sourcemodel->tableName();
            fieldInfo=MDL->fieldInfo(tableName,fieldName);
            if(fieldInfo!=nullptr &&  fieldInfo->PhysicalType()==MDLDao::DateTime )
            {
                if(fieldInfo->LookupTyp()==MDLDao::DateAndTime){
                    this->setItemDelegateForColumn(i,new QWMDateDelegate(QWMDateDelegate::DATETIME,"yyyy-MM-dd HH:mm:ss",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    this->setItemDelegateForColumn(i,new QWMDateDelegate(QWMDateDelegate::DATE,"yyyy-MM-dd",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    this->setItemDelegateForColumn(i,new QWMDateDelegate(QWMDateDelegate::TIME,"HH:mm:ss",this));
                }
            }

        }
        connect(this->horizontalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
    }else{
        for(int i=0;i< model->rowCount();i++)
        {
            MDLField * fieldInfo;
            QString fieldName=model->fieldName(model->index(i,0));
            QString tableName=sourcemodel->tableName();
            fieldInfo=MDL->fieldInfo(tableName,fieldName);
            if(fieldInfo!=nullptr &&  fieldInfo->PhysicalType()==MDLDao::DateTime )
            {
                if(fieldInfo->LookupTyp()==MDLDao::DateAndTime){
                    this->setItemDelegateForRow(i,new QWMDateDelegate(QWMDateDelegate::DATETIME,"yyyy-MM-dd HH:mm:ss",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    this->setItemDelegateForRow(i,new QWMDateDelegate(QWMDateDelegate::DATE,"yyyy-MM-dd",this));
                }else if(fieldInfo->LookupTyp()==MDLDao::Date){
                    this->setItemDelegateForRow(i,new QWMDateDelegate(QWMDateDelegate::TIME,"HH:mm:ss",this));
                }
            }

        }
        connect(this->verticalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
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
    bindDelegate();
    this->resize(this->size()+QSize(1,1));
}
