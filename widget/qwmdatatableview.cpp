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
    _itemDelegate=new QWMStyledItemDelegate(this);
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
                (this->*func)(i,new QWMRefLookupDelegate(tableName, fieldInfo->LookupTableName(),fieldInfo->LookupFieldName(),fieldInfo->CaptionLong(),true,this));
            }
            else if(fieldInfo->LookupTyp()==MDLDao::LibOnly){
                (this->*func)(i,new QWMRefLookupDelegate(tableName, fieldInfo->LookupTableName(),fieldInfo->LookupFieldName(),fieldInfo->CaptionLong(),false,this));
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
            }else if(fieldInfo->LookupTyp()==MDLDao::TabList){
                QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
                QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
                foreach(MDLFieldLookup * l,fls){
                    if(!l->TableKey())
                    {
                        QPair<QString,QVariant> p(l->LookupItem(),l->LookupItem());
                        options<<p;
                    }else {
                        QString tableName=l->LookupItem();
                        QString caption=MDL->tableInfo(tableName)->CaptionLongP();
                        QPair<QString,QVariant> p(caption,tableName);
                        options<<p;
                    }
                }
                (this->*func)(i,new QWMComboBoxDelegate(options,true,this));
            }else if (fieldInfo->LookupTyp()==MDLDao::Foreign){
                QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
                //                QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
                QStringList tables;

                foreach(MDLFieldLookup * l,fls){
                    if(l->TableKey()){
                        QString tableName=l->LookupItem();
                        tables<<tableName;
                    }
                }
                TP(this,QWMDataEditor,parentDoc);
                if(IS_SPEC_REF_FIELD(fieldInfo)){
                    (this->*func)(i,new QWMRefLookupDelegate(tables,fieldInfo->caption(),parentDoc->idWell(),QWMRefLookupDelegate::TwoStepRecord,this));
                }else{
                    (this->*func)(i,new QWMRefLookupDelegate(tables,fieldInfo->caption(),parentDoc->idWell(),QWMRefLookupDelegate::SigleStepRecord,this));

                }
            }
        }
    }
}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    QWMRotatableProxyModel * rmodel=(QWMRotatableProxyModel*)this->model();
    connect(model,SIGNAL(modeChanged()),this,SLOT(onModeChange()));
    connect(model,SIGNAL(modelReset()),this,SLOT(onModeChange()));
    rmodel->reset();
}


void QWMDataTableView::onModeChange()
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)this->model();
    PX(pmodel,this->model());
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


