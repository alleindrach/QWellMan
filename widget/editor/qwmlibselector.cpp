#include "qwmlibselector.h"
#include "ui_qwmlibselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QDebug>
#include "common.h"
#include "libdao.h"
#include "mdldao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"


QWMLibSelector::QWMLibSelector(QString table,QString lib, QSqlQueryModel* model, QWidget *parent):
    QWidget(parent),ui(new Ui::QWMLibSelector),_table(table),_refLibName(lib),_model(model)
{
    ui->setupUi(this);

    init();
}
void QWMLibSelector::setEditable(bool v)
{
    _editable=v;
}

bool QWMLibSelector::editable()
{
    return _editable;
}
void QWMLibSelector::setText(QString text)
{
    _selectedValue=text;
    QSortFilterProxyModel * proxyModel=( QSortFilterProxyModel * )ui->tableView->model();
    QSqlQueryModel * sourceModel=(QSqlQueryModel*)proxyModel->sourceModel();
    if(!_lookupFld.isNull() && !_lookupFld.isEmpty()){
        _col=sourceModel->record().indexOf(_lookupFld);
    }
    if(_col>=0){
        QModelIndexList indexs= proxyModel->match(proxyModel->index(0,_col),Qt::DisplayRole,_selectedValue,1,Qt::MatchExactly);
        if(indexs.length()==1){
            QItemSelection selection;
            selection.select(proxyModel->index(indexs.first().row(),0),
                             proxyModel->index(indexs.first().row(),proxyModel->columnCount()-1) );
            ui->tableView->selectionModel()->select(selection,QItemSelectionModel::SelectCurrent);
            return;
        }
    }else{
        for(int c=0;c<proxyModel->columnCount();c++){
            QModelIndexList indexs= proxyModel->match(proxyModel->index(0,c),Qt::DisplayRole,_selectedValue,1,Qt::MatchExactly);
            if(indexs.length()==1){
                QItemSelection selection;
                selection.select(proxyModel->index(indexs.first().row(),0),
                                 proxyModel->index(indexs.first().row(),proxyModel->columnCount()-1) );
                ui->tableView->selectionModel()->select(selection,QItemSelectionModel::SelectCurrent);
                return;
            }
        }
    }
    if(!_editable){
        _selectedValue=QString();
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

void QWMLibSelector::setTitle(QString v)
{
    _title=v;
}

void QWMLibSelector::init()
{

    if(_model==nullptr){
        //根据当前表和检索表名，获取当前表需要用到的字段
        QStringList lookupFlds=MDL->lookupFields(_table,_refLibName);
        _model=LIB->libLookup(_table,_refLibName,QString(),lookupFlds);
    }
    this->ui->tableView->setStyleSheet(APP->style());
    //    connect(ui->buttonBox,&QDialogButtonBox::clicked,this,&QWMLibSelector::on_btn_clicked);
    TIMESTAMP(QWMLibSelectorINIT1);
    connect(ui->btnOK,&QPushButton::clicked,this,&QWMLibSelector::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMLibSelector::on_btn_clicked);
    connect(ui->lineEdit,&QLineEdit::textChanged,this,&QWMLibSelector::on_text_changed);
    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&QWMLibSelector::on_return_pressed);
    connect(ui->tableView,&QTableView::doubleClicked,this,&QWMLibSelector::on_item_doubleclick);
    ui->lineEdit->installEventFilter(this);;
    ui->tableView->installEventFilter(this);;
    TIMESTAMP(QWMLibSelectorINIT2);
    QSortFilterProxyModel * proxyModel=new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    ui->tableView->setModel(proxyModel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(12);
    TIMESTAMP(QWMLibSelectorINIT3);
    //    for(int i=0;i<_model->columnCount();i++){
    //        QString fieldName=_model->headerData(i,Qt::Horizontal,FIELD_ROLE).toString();
    //        if(LIBDao::hiddenFields.contains(fieldName)){
    //            ui->tableView->setColumnHidden(i,true);
    //        }else{

    //            if(!_visibleFlds.isEmpty()){
    //                if(!_visibleFlds.contains(fieldName)){
    //                    ui->tableView->setColumnHidden(i,true);
    //                }
    //            }
    //            if(_lookupFld.compare(fieldName,Qt::CaseInsensitive)==0){
    //                ui->tableView->setColumnHidden(i,false);
    //                //                proxyModel->setFilterKeyColumn(i);
    //                proxyModel->setFilterRole(Qt::DisplayRole);
    //                _col=i;
    //                proxyModel->setHeaderData(i,Qt::Horizontal,_title,Qt::DisplayRole);
    //                proxyModel->setHeaderData(i,Qt::Horizontal,_title,Qt::EditRole);
    //            }
    //            else{
    //                proxyModel->setHeaderData(i,Qt::Horizontal,fieldName,Qt::DisplayRole);
    //                proxyModel->setHeaderData(i,Qt::Horizontal,fieldName,Qt::EditRole);
    //            }
    //        }
    //    }
    TIMESTAMP(QWMLibSelectorINIT4);
    //    QModelIndexList indexs= proxyModel->match(proxyModel->index(0,_col),Qt::DisplayRole,_selectedValue,1,Qt::MatchExactly);
    //    TIMESTAMP(QWMLibSelectorINIT5);
    //    if(indexs.length()==1){
    //        ui->tableView->selectionModel()->select(indexs.first(),QItemSelectionModel::SelectCurrent);
    //    }
    TIMESTAMP(QWMLibSelectorINIT6);
    ui->lineEdit->setFocus();
    TIMESTAMP(QWMLibSelectorINIT7);
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

void QWMLibSelector::showEvent(QShowEvent */*event*/)
{

}

void QWMLibSelector::focusInEvent(QFocusEvent */*event*/)
{
    ui->lineEdit->setFocus();
}

QSqlRecord QWMLibSelector::selectedRecord()
{
    QItemSelectionModel *selectionModel=ui->tableView->selectionModel();
    if(selectionModel->hasSelection()){
        if(this->instanceof<QSortFilterProxyModel>(ui->tableView->model())){
            QSortFilterProxyModel * pModel=dynamic_cast<QSortFilterProxyModel*>(ui->tableView->model());
            if(this->instanceof<QSqlQueryModel>(pModel->sourceModel())){
                QSqlQueryModel * model=dynamic_cast<QSqlQueryModel*>( pModel->sourceModel());
                return model->record(pModel->mapToSource( selectionModel->currentIndex()).row());
            }
        }
    }
    return QSqlRecord();
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
    if(proxyModel->rowCount()>0){
        QModelIndex index=proxyModel->index(0,0);
        QItemSelectionModel * selectModel= ui->tableView->selectionModel();
        selectModel->select(index,QItemSelectionModel::SelectCurrent);
        ui->tableView->setFocus();
        //        emit this->accepted(this);
    }
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
