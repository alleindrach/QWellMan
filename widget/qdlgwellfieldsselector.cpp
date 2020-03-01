#include "qdlgwellfieldsselector.h"
#include "ui_qdlgwellfieldsselector.h"
#include "common.h"
#include "qwmapplication.h"
#include "mdldao.h"
#include "QSqlQuery"
#include "QSqlQueryModel"
#include "QSqlRecord"
#include "QListWidgetItem"
#include "QStringList"
#include "QDebug"
QDlgWellFieldsSelector::QDlgWellFieldsSelector(QStringList selected, QWidget *parent) :
    QDialog(parent),_selectedFields(selected),
    ui(new Ui::QDlgWellFieldsSelector)
{
    ui->setupUi(this);
    QSqlQuery q=MDL->tableFieldsQuery(CFG(KeyTblMain));
    QSqlQueryModel * model=new QSqlQueryModel(this);
    model->setQuery(q);
    ui->lvFields->setModel(model);
    ui->lvFields->setModelColumn(model->record().indexOf("CaptionLong"));
    q.first();
    while(true){
        QString v=q.value(q.record().indexOf("KeyFld")).toString();
        QString c=q.value(q.record().indexOf("CaptionLong")).toString();
//        qDebug()<<"FLD:"<<v<<",c:"<<c<<",selected:"<<selected;
        if(selected.contains(v,Qt::CaseInsensitive)){
            QListWidgetItem * item=new QListWidgetItem(c, ui->lvDisplayFields);
            item->setData(DATA_ROLE,q.value("KeyFld").toString());
            ui->lvDisplayFields->addItem(item);
        }
        if(!q.next())
            break;
    }
}

QDlgWellFieldsSelector::~QDlgWellFieldsSelector()
{
    delete ui;
}

QStringList QDlgWellFieldsSelector::selectedFields()
{
    return _selectedFields;
}

void QDlgWellFieldsSelector::setSelectedFields(QStringList v)
{
    _selectedFields=v;
}

void QDlgWellFieldsSelector::on_addBtn_clicked()
{
    QItemSelectionModel * selector=ui->lvFields->selectionModel();
    QSqlQueryModel * model=static_cast<QSqlQueryModel*>(ui->lvFields->model());

    foreach( QModelIndex item, selector->selection().indexes()){
        QString value=model->record(item.row()).value("KeyFld").toString();
//        qDebug()<<"item data|"<<value;
        if(!_selectedFields.contains(value,Qt::CaseInsensitive)){
            _selectedFields.append(value);
            QListWidgetItem * newItem=new QListWidgetItem(item.data(Qt::DisplayRole).toString(), ui->lvDisplayFields);
            newItem->setData(DATA_ROLE,value);
            ui->lvDisplayFields->addItem(newItem);
        }
    }
}

void QDlgWellFieldsSelector::on_delBtn_clicked()
{
    QItemSelectionModel * selector=ui->lvDisplayFields->selectionModel();
    foreach( QModelIndex item, selector->selection().indexes()){
        _selectedFields.removeOne(item.data(DATA_ROLE).toString());
        ui->lvDisplayFields->model()->removeRow(item.row());
//        qDebug()<<" fields:"<<_selectedFields;
    }
}
