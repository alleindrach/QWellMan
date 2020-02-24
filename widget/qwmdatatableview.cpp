#include "qwmdatatableview.h"
#include "QDebug"
#include "qwmsortfilterproxymodel.h"
#include "qwmtablemodel.h"
#include "qwmrotatableproxymodel.h"
#include "common.h"
#include "mdlfield.h"
#include "mdldao.h"
#include "qwmdatedelegate.h"
QWMDataTableView::QWMDataTableView(QWidget *parent):QTableView(parent)
{

}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
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
            if(fieldInfo!=nullptr &&  fieldInfo->PhysicalType()==MDLDao::DateTime){
               this->setItemDelegateForColumn(i,new QWMDateDelegate(QWMDateDelegate::DATETIME,"yyyy-MM-dd HH:mm:ss",this));
            }

        }
    }else{
        for(int i=0;i< model->rowCount();i++)
        {
            MDLField * fieldInfo;
            QString fieldName=model->fieldName(model->index(i,0));
            QString tableName=sourcemodel->tableName();
            fieldInfo=MDL->fieldInfo(tableName,fieldName);
            if(fieldInfo!=nullptr &&  fieldInfo->PhysicalType()==MDLDao::DateTime){
               this->setItemDelegateForRow(i,new QWMDateDelegate(QWMDateDelegate::DATETIME,"yyyy-MM-dd HH:mm:ss",this));
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

void QWMDataTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    return QTableView::closeEditor(editor,hint);
}

void QWMDataTableView::commitData(QWidget *editor)
{
    return QTableView::commitData(editor);
}
