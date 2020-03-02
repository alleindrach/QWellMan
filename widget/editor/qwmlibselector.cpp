#include "qwmlibselector.h"
#include "ui_qwmlibselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QDebug>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
QWMLibSelector::QWMLibSelector(QString lib,QString lookupFld,QString title,bool editale,QString v,QWidget *parent) : QWidget(parent),ui(new Ui::QWMLibSelector),_lookupFld(lookupFld),_title(title),_selectedValue(v),_editable(editale)
{
    ui->setupUi(this);
    QSqlQueryModel*  model=LIB->libLookup(lib);
    init(model);
}

QWMLibSelector::QWMLibSelector(QString lib, QString lookupFld,QString title, QSqlQueryModel* model, bool editable, QString v, QWidget *parent):
    QWidget(parent),ui(new Ui::QWMLibSelector),_lookupFld(lookupFld),_selectedValue(v),_editable(editable),_title(title)
{

    ui->setupUi(this);
    init(model);
}

void QWMLibSelector::setText(QString text)
{
    _selectedValue=text;
    QSortFilterProxyModel * proxyModel=( QSortFilterProxyModel * )ui->tableView->model();

    QModelIndexList indexs= proxyModel->match(proxyModel->index(0,_col),Qt::DisplayRole,_selectedValue,1,Qt::MatchExactly);
    if(indexs.length()==1){
        QItemSelection selection;
        selection.select(proxyModel->index(indexs.first().row(),0),
                         proxyModel->index(indexs.first().row(),proxyModel->columnCount()-1) );
        ui->tableView->selectionModel()->select(selection,QItemSelectionModel::SelectCurrent);
    }else{
        if(!_editable){
            _selectedValue=QString();
        }
    }
}

QString QWMLibSelector::text()
{
    QSortFilterProxyModel * proxyModel=( QSortFilterProxyModel * )ui->tableView->model();
    QItemSelection selection=ui->tableView->selectionModel()->selection();
    if(!selection.isEmpty()&& !selection.indexes().isEmpty()){
        QModelIndex selected=selection.indexes().first();
        QModelIndex selectedAsCol=proxyModel->index(selected.row(),_col);
        _selectedValue=selectedAsCol.data().toString();
    }else if(_editable){
        _selectedValue=ui->lineEdit->text();
    }
    return _selectedValue;
}

void QWMLibSelector::init(QSqlQueryModel * model)
{


    //    connect(ui->buttonBox,&QDialogButtonBox::clicked,this,&QWMLibSelector::on_btn_clicked);
    connect(ui->btnOK,&QPushButton::clicked,this,&QWMLibSelector::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMLibSelector::on_btn_clicked);
    connect(ui->lineEdit,&QLineEdit::textChanged,this,&QWMLibSelector::on_text_changed);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&QWMLibSelector::on_return_pressed);
    connect(ui->tableView,&QTableView::doubleClicked,this,&QWMLibSelector::on_item_doubleclick);
    ui->lineEdit->installEventFilter(this);;
    ui->tableView->installEventFilter(this);;
    QSortFilterProxyModel * proxyModel=new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    ui->tableView->setModel(proxyModel);

    ui->tableView->verticalHeader()->setDefaultSectionSize(12);
    for(int i=0;i<model->columnCount();i++){
        if(LIBDao::hiddenFields.contains(model->record().fieldName(i))){
            ui->tableView->setColumnHidden(i,true);
        }else{
            if(_lookupFld.compare(model->record().fieldName(i),Qt::CaseInsensitive)==0){
                ui->tableView->setColumnHidden(i,false);
                //                proxyModel->setFilterKeyColumn(i);
                proxyModel->setFilterRole(Qt::DisplayRole);
                _col=i;
                proxyModel->setHeaderData(i,Qt::Horizontal,_title,Qt::DisplayRole);
                proxyModel->setHeaderData(i,Qt::Horizontal,_title,Qt::EditRole);
            }
            else{
                proxyModel->setHeaderData(i,Qt::Horizontal,model->record().fieldName(i),Qt::DisplayRole);
                proxyModel->setHeaderData(i,Qt::Horizontal,model->record().fieldName(i),Qt::EditRole);
            }
        }
    }

    QModelIndexList indexs= proxyModel->match(proxyModel->index(0,_col),Qt::DisplayRole,_selectedValue,1,Qt::MatchExactly);
    if(indexs.length()==1){
        ui->tableView->selectionModel()->select(indexs.first(),QItemSelectionModel::SelectCurrent);
    }
    ui->lineEdit->setFocus();
}

const QItemSelectionModel* QWMLibSelector::selectionModel()
{
    return ui->tableView->selectionModel();
}

bool QWMLibSelector::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress){
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Escape){
            emit this->rejected(this);
            return true;
        }
    }
    if(watched==ui->lineEdit && event->type()==QEvent::KeyPress){
//        qDebug()<<"LineEdit:"<<event->type();
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Tab){
            if(ui->lineEdit->nextInFocusChain()!=nullptr)
                ui->lineEdit->nextInFocusChain()->setFocus();
            return true;
        }
    }
    if(watched==ui->tableView && event->type()==QEvent::KeyPress ){
//        qDebug()<<"TableView:"<<event->type();
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Return){
            emit this->accepted(this);
            return true;
        }
        if(keyEvent->key()==Qt::Key_Tab){
            if(ui->lineEdit->nextInFocusChain()!=nullptr){
                ui->lineEdit->nextInFocusChain()->setFocus();
                return true;
            }
        }
    }
    return false;
    //    return true;
}

void QWMLibSelector::showEvent(QShowEvent *event)
{

}

void QWMLibSelector::focusInEvent(QFocusEvent *event)
{
    ui->lineEdit->setFocus();
}


void QWMLibSelector::on_btn_clicked()
{
    if(sender()->objectName()=="btnOK"){
        emit this->accepted(this);
    }
    if(sender()->objectName()=="btnCancel"){
        emit this->rejected(this);
    }
}
//void QWMLibSelector::on_btn_clicked(QAbstractButton *button)
//{
////    if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::AcceptRole){
////        emit this->accepted(this);
////    }else if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::RejectRole){
////        emit this->rejected(this);
////    }

//}

void QWMLibSelector::on_text_changed(const QString & text)
{
    QSortFilterProxyModel * proxyModel=(QSortFilterProxyModel*)ui->tableView->model();
    proxyModel->setFilterRegExp(".*"+text+".*");
    if(_editable){

    }

}

void QWMLibSelector::on_return_pressed()
{
    QSortFilterProxyModel * proxyModel=(QSortFilterProxyModel*)ui->tableView->model();
    int qc=proxyModel->rowCount();
//    qDebug()<<"RowCount:"<<proxyModel->rowCount();
    if(proxyModel->rowCount()>0){
        QModelIndex index=proxyModel->index(0,0);
        QItemSelectionModel * selectModel= ui->tableView->selectionModel();
        selectModel->select(index,QItemSelectionModel::SelectCurrent);
        ui->tableView->setFocus();
        //        emit this->accepted(this);
    }

    //    emit this->accepted(this);

}

void QWMLibSelector::on_item_doubleclick(const QModelIndex &)
{
    emit this->accepted(this);
}
QList<QWidget *> QWMLibSelector::taborders() {
    QList<QWidget*> results;
    results<<ui->lineEdit<<ui->tableView;
    return results;
}
