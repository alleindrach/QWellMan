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
#include "qwmreflookupdelegate.h"
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
    } \
    w->setItemDelegate(_itemDelegate);

QWMDataTableView::QWMDataTableView(QWidget *parent):QTableView(parent)
{
    _itemDelegate=new QWMRefLookupDelegate(this);
//    _reflookupDelegate=new QWMRefLookupDelegate(this);
    QWMHeaderView *vertical = new QWMHeaderView(Qt::Vertical, this);
    vertical->setSectionsClickable(true);
    vertical->setHighlightSections(true);
    setVerticalHeader(vertical);

    QWMHeaderView *horizontal = new QWMHeaderView(Qt::Horizontal, this);
    horizontal->setSectionsClickable(true);
    horizontal->setHighlightSections(true);
    setHorizontalHeader(horizontal);
}
void QWMDataTableView::setHorizontalHeader(QHeaderView *header)
{

    QTableView::setHorizontalHeader(header);

    disconnect(header, SIGNAL(sectionCountChanged(int,int)),0,0);
    connect(header, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(rowCountChanged(int,int)));
}

void QWMDataTableView::setVerticalHeader(QHeaderView *header)
{
    QTableView::setHorizontalHeader(header);

    disconnect(header, SIGNAL(sectionCountChanged(int,int)),0,0);
    connect(header, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(columnCountChanged(int,int)));

}


void QWMDataTableView::bindDelegate()
{
//    typedef  void (QAbstractItemView::* DelegateSettor)(int, QAbstractItemDelegate *) ;
//    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>(this->model());
//    SX(sourcemodel,this->model());
//    DelegateSettor  func;
//    int counter=0;
    CLEAR_DELEGATES(this);
//    if(model->mode()==QWMRotatableProxyModel::H){
//        func =& QWMDataTableView::setItemDelegateForColumn;
//        counter=model->columnCount();
//        //        connect(this->horizontalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
//    }else{
//        func = & QWMDataTableView::setItemDelegateForRow;
//        counter=model->rowCount();
//        //        connect(this->verticalHeader(),&QHeaderView::sectionDoubleClicked,this,&QWMDataTableView::on_header_clicked);
//    }
//    for(int i=0;i< counter;i++)
//    {
//        MDLField * fieldInfo;
//        QString fieldName=model->fieldName(
//                    model->mode()==QWMRotatableProxyModel::H?model->index(0,i): model->index(i,0)
//                                                             );
//        QString tableName=sourcemodel->tableName();
//        fieldInfo=UDL->fieldInfo(tableName,fieldName);

//        if(fieldInfo!=nullptr){
//            if( fieldInfo->PhysicalType()==MDLDao::DateTime ||
//                    fieldInfo->LookupTyp()==MDLDao::LibEdit||
//                    fieldInfo->LookupTyp()==MDLDao::LibOnly||
//                    fieldInfo->LookupTyp()==MDLDao::DBDistinctValues||
//                    fieldInfo->LookupTyp()==MDLDao::Icon||
//                    fieldInfo->LookupTyp()==MDLDao::List||
//                    fieldInfo->LookupTyp()==MDLDao::TabList||
//                    fieldInfo->LookupTyp()==MDLDao::Foreign){
//                    (this->*func)(i,_reflookupDelegate);
//            }
//        }
//    }
}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    QWMRotatableProxyModel * rmodel=(QWMRotatableProxyModel*)this->model();
    connect(model,SIGNAL(modeChanged()),this,SLOT(onModeChange()));
    connect(model,SIGNAL(modelReset()),this,SLOT(onModeChange()));
    connect(rmodel,&QWMRotatableProxyModel::rowsChanged,this,&QWMDataTableView::onRowsChanged);
    rmodel->reset();
}


void QWMDataTableView::onModeChange()
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)this->model();
    SX(smodel,this->model());
    QSettings settings;
    settings.setValue(QString(EDITOR_TABLE_ENTRY_PREFIX).arg(smodel->tableName()),model->mode());
    bindDelegate();
    this->resize(this->size()+QSize(1,1));
}

void QWMDataTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    return QTableView::closeEditor(editor,hint);
}

void QWMDataTableView::onRowsChanged()
{
    emit this->RecordCountChanged(0,1);
}
void QWMDataTableView::rowCountChanged(int oldCount, int newCount)
{
    QAbstractItemModel  * model=this->verticalHeader()->model();
    if(model!=nullptr){
        QVariant vMode=model->property("mode");
        if(!vMode.isNull()&& vMode.isValid()){
            int mode = vMode.toInt();
            if(Qt::Horizontal == mode){
                emit this->RecordCountChanged(oldCount,newCount);
            }
        }
    }
    QTableView::rowCountChanged(oldCount,newCount);
}

void QWMDataTableView::columnCountChanged(int oldCount, int newCount)
{
    QAbstractItemModel * model=this->horizontalHeader()->model();
    if(model!=nullptr){
        QVariant vMode=model->property("mode");
        if(!vMode.isNull()&& vMode.isValid()){
            int mode = vMode.toInt();
            if(Qt::Vertical == mode){
                emit this->RecordCountChanged(oldCount,newCount);
            }
        }
    }
    QTableView::columnCountChanged(oldCount,newCount);
}


