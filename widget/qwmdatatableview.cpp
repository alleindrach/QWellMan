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
#include "qwmicondelegate.h"
#include "qwmcomboboxdelegate.h"

#define CLEAR_DELEGATES(w) \
    for(int i=0;i<w->model()->columnCount();i++){ \
    w->setItemDelegateForColumn(i,nullptr);\
    }\
    for(int i=0;i<w->model()->rowCount();i++){\
    w->setItemDelegateForRow(i,nullptr);\
    }

QWMDataTableView::QWMDataTableView(QWidget *parent):QRotatableTableView(parent)
{

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
        //        connect(this->horizontalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
    }else{
        func = & QWMDataTableView::setItemDelegateForRow;
        counter=model->rowCount();
        //        connect(this->verticalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
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
            else if(fieldInfo->LookupTyp()==MDLDao::Icon){
                (this->*func)(i,new QWMIconDelegate(this));
            }else if(fieldInfo->LookupTyp()==MDLDao::List){
                QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
                QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
                foreach(MDLFieldLookup * l,fls){
                    if(!l->TableKey())
                    {
                        QPair<QString,QVariant> p(l->LookupItem(),l->LookupItem());
                        options<<p;
                    }
                }
                (this->*func)(i,new QWMComboBoxDelegate(options,true,this));
            }
        }
    }
}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QRotatableTableView::setModel(model);
    QWMRotatableProxyModel * rmodel=(QWMRotatableProxyModel*)this->model();
    rmodel->reset();
}


void QWMDataTableView::onModeChange()
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)this->model();
    PX(pmodel,this->model());
    SX(smodel,this->model());
    //    if(model->mode()==QWMRotatableProxyModel::H){
    //        for(int j=0;j<model->columnCount();j++){
    //            if(j<pmodel->columnCount()){
    //                setColumnHidden(j,false);
    //            }else
    //            {
    //                setColumnHidden(j,true);
    //            }
    //        }
    //        for(int i=0;i<model->rowCount();i++){
    //            setRowHidden(i,false);
    //        }
    //    }else{
    //        int columns=model->rowCount();
    //        int vis=pmodel->columnCount();
    //        qDebug()<<"V:"<<",Total:"<<columns<<",Visible:"<<vis;
    //        for(int i=0;i<model->rowCount();i++){
    //            if(i<pmodel->columnCount()){
    //                setRowHidden(i,false);
    //            }else
    //            {
    //                setRowHidden(i,true);
    //            }
    //        }
    //        for(int j=0;j<model->columnCount();j++){
    //            setColumnHidden(j,false);
    //        }
    //    }
    QSettings settings;
    settings.setValue(QString(EDITOR_TABLE_ENTRY_PREFIX).arg(smodel->tableName()),model->mode());
    bindDelegate();
    this->resize(this->size()+QSize(1,1));
}

void QWMDataTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    return QRotatableTableView::closeEditor(editor,hint);
}

