#include "qwmtableselector.h"
#include "ui_qwmtableselector.h"
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
QWMTableSelector::QWMTableSelector(QString title,QWidget *parent) :
    QWidget(parent),ui(new Ui::QWMTableSelector),_title(title)
{
    ui->setupUi(this);
    init();
}


void QWMTableSelector::setText(QString text)
{
    _selectedValue=text;
    if(!_selectedValue.isNull()&&!_selectedValue.isEmpty()){
        QStringList parts=_selectedValue.split(".",Qt::SkipEmptyParts);
        if(parts.size()==2){
            QStandardItemModel * model = qobject_cast<QStandardItemModel*>( ui->treeView->model());
            QModelIndexList matched = model->match(model->index(0,0),TABLE_NAME_ROLE,text,1,Qt::MatchExactly|Qt::MatchRecursive);
            if(!matched.isEmpty()){
                QItemSelectionModel* sel = ui->treeView->selectionModel();
                sel->setCurrentIndex(matched.first(),QItemSelectionModel::SelectCurrent);
            }
        }
    }

}

QString QWMTableSelector::text()
{
    QItemSelectionModel* sel = ui->treeView->selectionModel();
    if(sel->hasSelection()){
        QModelIndex curIndex=ui->treeView->currentIndex();
        if(curIndex.isValid() && curIndex.data(CAT_ROLE)==QWMApplication::TABLE){
            QString  v = curIndex.data(TABLE_NAME_ROLE).toString();
            return v;
        }
    }
    if(!_selectedValue.isNull()&&!_selectedValue.isEmpty()){
        QStringList parts=_selectedValue.split(".",Qt::SkipEmptyParts);
        if(parts.size()==2){
            return parts[0];
        }
    }
    return _selectedValue;
}

void QWMTableSelector::init()
{
    connect(ui->treeView,&QTableView::doubleClicked,this,&QWMTableSelector::on_item_doubleclick);
    //    ui->treeView->installEventFilter(this);;
    //    QSortFilterProxyModel * proxyModel=new QSortFilterProxyModel(this);
    QStandardItemModel * model=MDL->loadDataTree(false,this);
    int rows=model->rowCount();
    ui->treeView->setModel(model);
    ui->treeView->expandAll();
    ui->treeView->setStyle(QStyleFactory::create("windows"));

}


QItemSelectionModel* QWMTableSelector::selectionModel()
{
    return ui->treeView->selectionModel();
}

bool QWMTableSelector::eventFilter(QObject *watched, QEvent *event)
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

void QWMTableSelector::on_item_doubleclick(const QModelIndex &)
{
    emit this->accepted(this);
}
QList<QWidget *> QWMTableSelector::taborders() {
    QList<QWidget*> results;
    results<<ui->treeView;
    return results;
}

