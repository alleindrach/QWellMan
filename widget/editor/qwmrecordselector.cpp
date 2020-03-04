#include "qwmrecordselector.h"
#include "ui_qwmrecordselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QDebug>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
#include "mdltablechildren.h"
#include "mdldao.h"
#include "welldao.h"
#include <QSqlQuery>
#include <QStyleFactory>
#include <QStack>
QWMRecordSelector::QWMRecordSelector(QStringList  tables,QString idwell,QString title,QWidget *parent) :
    QWidget(parent),ui(new Ui::QWMRecordSelector),_title(title),_tables(tables),_idWell(idwell)
{
    ui->setupUi(this);
    //    QSqlQueryModel*  model=LIB->libLookup(lib);
    //    QSqlRecord record=model->record();
    //    for(int i=0;i<record.count();i++){
    //        model->setHeaderData(i,Qt::Horizontal,record.fieldName(i),FIELD_ROLE);
    //    }
    init();
}

QWMRecordSelector::QWMRecordSelector(QString idwell,QString title,QWidget *parent ) :
    QWidget(parent),ui(new Ui::QWMRecordSelector),_title(title),_idWell(idwell){
    ui->setupUi(this);
    init();
}

void QWMRecordSelector::loadTables(QStringList tables)
{
    QStandardItemModel * model=qobject_cast<QStandardItemModel *> (ui->treeView->model());
    model->clear();
    _tables=tables;
    foreach(QString table,_tables){
        loadTable(table,model);
    }

    ui->treeView->expandAll();
}
void QWMRecordSelector::setText(QString text)
{
    _selectedValue=text;
    if(!_selectedValue.isNull()&&!_selectedValue.isEmpty()){
        QStringList parts=_selectedValue.split(".",Qt::SkipEmptyParts);
        if(parts.size()==2){
            QStandardItemModel * model = qobject_cast<QStandardItemModel*>( ui->treeView->model());
            QModelIndexList matched = model->match(model->index(0,0),PK_VALUE_ROLE,parts[1],1,Qt::MatchExactly|Qt::MatchRecursive);
            if(!matched.isEmpty()){
                QItemSelectionModel* sel = ui->treeView->selectionModel();
                sel->setCurrentIndex(matched.first(),QItemSelectionModel::SelectCurrent);
            }
        }
    }
}

QString QWMRecordSelector::text()
{
    QItemSelectionModel* sel = ui->treeView->selectionModel();
    if(sel->hasSelection()){
        QModelIndex curIndex=ui->treeView->currentIndex();
        if(curIndex.isValid() && curIndex.data(CAT_ROLE)==QWMApplication::RECORD){
            QString id = curIndex.data(PK_VALUE_ROLE).toString();
            QString tbl=curIndex.data(FIELD_ROLE).toString();
            return QString("%1.%2").arg(tbl,id);
        }
    }
    return _selectedValue;
}
void QWMRecordSelector::loadTable(QString childtable,QStandardItemModel* tv)
{
    QStack<QString> stack;
    stack.push(childtable);
    QString tn=childtable;
    QString pn=MDL->parentTable(tn);
    while(!pn.isNull() && pn.compare(CFG(KeyTblMain),Qt::CaseInsensitive) != 0){
        stack.push(pn);
        pn=MDL->parentTable(pn);
    }
    QString  table=stack.pop();
    QString  parentIDFld=CFG(ParentID);
    QSqlQuery query;
    ui->treeView->setStyle(QStyleFactory::create("windows"));
    parentIDFld=CFG(IDWell);
    query=WELL->records(table,_idWell,QString());
    int recordCnt=0;

    QStandardItem*  tableItem=new QStandardItem();
    MDLTable * tableInfo=MDL->tableInfo(table);
    QString des=tableInfo->CaptionLongP();
    //    des+=tr("(无数据)");
    tableItem->setText(des); //设置第1列的文字
    //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
    tableItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    tableItem->setData(QWMApplication::NA,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setData(QString(),PK_VALUE_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setData(tableInfo->KeyTbl(),FIELD_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setToolTip(QString("[%1]").arg(table));
    tv->appendRow(tableItem);

    while(query.next()){
        QSqlRecord rec=query.record();
        QString des=WELL->recordDes(table,rec);
        QStandardItem*  dataTableItem=new QStandardItem();
        PK_VALUE(pk,rec);
        dataTableItem->setText(des); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        dataTableItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable );
        dataTableItem->setData(QWMApplication::RECORD,CAT_ROLE);
        dataTableItem->setData(pk,PK_VALUE_ROLE);
        dataTableItem->setData(tableInfo->KeyTbl(),FIELD_ROLE); //设置节点第1列的Qt::UserRole的Data
        dataTableItem->setToolTip(QString("[%1]").arg(table));
        tableItem->appendRow(dataTableItem);
        loadChildTable(dataTableItem,stack);
        recordCnt++;
    }
    if(recordCnt==0){
        des+=tr("(无数据)");
        tableItem->setText(des); //设置第1列的文字
    }

    return ;
}

void QWMRecordSelector::loadChildTable(QStandardItem *parent,QStack<QString> trace)
{
    if(trace.isEmpty())
        return;
    QString parentID=parent->data(PK_VALUE_ROLE).toString();
    QString table=trace.pop();
    QSqlQuery query=WELL->records(table,_idWell,parentID);
    int recordCnt=0;
    QStandardItem*  tableItem=new QStandardItem();
    MDLTable * tableInfo=MDL->tableInfo(table);
    QString des=tableInfo->CaptionLongP();
    //    des+=tr("(无数据)");
    tableItem->setText(des); //设置第1列的文字
    tableItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    tableItem->setData(QWMApplication::NA,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setData(QString(),PK_VALUE_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setData(tableInfo->KeyTbl(),FIELD_ROLE); //设置节点第1列的Qt::UserRole的Data
    tableItem->setToolTip(QString("[%1]").arg(table));
    parent->appendRow(tableItem);

    while(query.next()){
        QSqlRecord rec=query.record();
        QString des=WELL->recordDes(table,rec);
        QStandardItem*  dataTableItem=new QStandardItem();
        PK_VALUE(pk,rec);
        dataTableItem->setText(des); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        dataTableItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        dataTableItem->setData(QWMApplication::RECORD,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
        dataTableItem->setData(pk,PK_VALUE_ROLE); //设置节点第1列的Qt::UserRole的Data
        dataTableItem->setData(tableInfo->KeyTbl(),FIELD_ROLE); //设置节点第1列的Qt::UserRole的Data
        dataTableItem->setToolTip(QString("[%1]").arg(table));
        tableItem->appendRow(dataTableItem);
        if(!trace.empty())
            loadChildTable(dataTableItem,trace);
        recordCnt++;
    }
    if(recordCnt==0){
        des+=tr("(无数据)");
        tableItem->setText(des); //设置第1列的文字
    }

}

void QWMRecordSelector::init()
{
    connect(ui->treeView,&QTableView::doubleClicked,this,&QWMRecordSelector::on_item_doubleclick);
    ui->treeView->installEventFilter(this);;
    //    QSortFilterProxyModel * proxyModel=new QSortFilterProxyModel(this);
    QStandardItemModel * model=new QStandardItemModel(this);
    foreach(QString table,_tables){
        loadTable(table,model);
    }
    ui->treeView->setModel(model);
    ui->treeView->expandAll();
}


QItemSelectionModel* QWMRecordSelector::selectionModel()
{
    return ui->treeView->selectionModel();
}

bool QWMRecordSelector::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress){
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Escape){
            emit this->rejected(this);
            return true;
        }
    }
    if( event->type()==QEvent::KeyPress ){
        //        qDebug()<<"TableView:"<<event->type();
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Return){
            emit this->accepted(this);
            return true;
        }
    }
    return false;
}

void QWMRecordSelector::on_item_doubleclick(const QModelIndex &)
{
    emit this->accepted(this);
}
QList<QWidget *> QWMRecordSelector::taborders() {
    QList<QWidget*> results;
    results<<ui->treeView;
    return results;
}

