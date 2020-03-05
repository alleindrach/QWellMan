#include "qwmrecordtwostepselector.h"

#include "qwmrecordsinglestepselector.h"
#include "qwmlibsingleselector.h"
#include "qwmlibselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QDebug>
#include <QHBoxLayout>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
#include "ui_qwmrecordtowstepselector.h"
#include "qwmtableselector.h"
#include "qwmrecordselector.h"
#include "mdldao.h"
#include "QSizePolicy"
#include "QHBoxLayout"
QWMRecordTwoStepSelector::QWMRecordTwoStepSelector(QString idwell,QString title,QWidget *parent )
    : QWMAbstractEditor(parent),ui(new Ui::QWMRecordTwoStepSelector) ,
      _idWell(idwell)
{
    ui->setupUi(this);
    for(int i=0;i<ui->stackedWidget->count();i++)
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(i));
    qDebug()<<"i:"<<ui->stackedWidget->count();
    QWMTableSelector * tableSelector=new QWMTableSelector(title,this);
    int c=ui->stackedWidget->addWidget(tableSelector);
    qDebug()<<"c:"<<c;

    QWMRecordSelector * recordSelector=new QWMRecordSelector(idwell,title,this);
    ui->stackedWidget->addWidget(recordSelector);
    ui->stackedWidget->setCurrentIndex(0);

    connect(tableSelector,&QWMTableSelector::accepted,this,&QWMRecordTwoStepSelector::on_tab_accepted);
    connect(tableSelector,&QWMTableSelector::rejected,this,&QWMRecordTwoStepSelector::on_tab_recjected);
    connect(recordSelector,&QWMRecordSelector::accepted,this,&QWMRecordTwoStepSelector::on_tab_accepted);
    connect(recordSelector,&QWMRecordSelector::rejected,this,&QWMRecordTwoStepSelector::on_tab_recjected);

    connect(ui->btnOK,&QPushButton::clicked,this,&QWMRecordTwoStepSelector::on_btn_clicked);
    connect(ui->btnNav,&QPushButton::clicked,this,&QWMRecordTwoStepSelector::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMRecordTwoStepSelector::on_btn_clicked);
}

QWMRecordTwoStepSelector::~QWMRecordTwoStepSelector()
{
       qDebug()<<"QWMRecordTwoStepSelector destory!";
}

void QWMRecordTwoStepSelector::on_btn_clicked()
{
    QPushButton * bttn=qobject_cast<QPushButton*>(sender());
    if((bttn->objectName()=="btnNav"||bttn->objectName()=="btnOK") && ui->stackedWidget->currentIndex()==0){
        nav2RecordSelector();
    }else if(bttn->objectName()=="btnNav" && ui->stackedWidget->currentIndex()==1){
        nav2TableSelector();
    }else if(bttn->objectName()=="btnOK" &&  ui->stackedWidget->currentIndex()==1){
        emit this->accept();
    }else if(bttn->objectName()=="btnCancel"){
        emit this->reject();
    }
}

void QWMRecordTwoStepSelector::nav2RecordSelector()
{
    QWMTableSelector* tableSelector= qobject_cast<QWMTableSelector*> (ui->stackedWidget->widget(0));
    if(!tableSelector->text().isNull()&&!tableSelector->text().isEmpty()){
        QString table=tableSelector->text();
        QWMRecordSelector * recordSelector=qobject_cast<QWMRecordSelector*>(ui->stackedWidget->widget(1));
        recordSelector->loadTables(QStringList(table));
        ui->stackedWidget->setCurrentIndex(1);
        ui->btnNav->setText(tr("Prev"));
    }
}

void QWMRecordTwoStepSelector::nav2TableSelector()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnNav->setText(tr("Next"));
}

void QWMRecordTwoStepSelector::setText(QString text)
{
    _selectedValue=text;
    QWMTableSelector* tableSelector= qobject_cast<QWMTableSelector*> (ui->stackedWidget->widget(0));
    tableSelector->setText(text);
    QStringList parts=text.split(".",Qt::SkipEmptyParts);
    QWMRecordSelector* recordSelector= qobject_cast<QWMRecordSelector*> (ui->stackedWidget->widget(1));
    if(parts.size()==2){
        recordSelector->loadTables(QStringList(parts[0]));
    }
    recordSelector->setText(text);
}

QString QWMRecordTwoStepSelector::text()
{
    QWMRecordSelector * selector=qobject_cast<QWMRecordSelector*>(ui->stackedWidget->widget(1));
    return selector->text();
}

const QItemSelectionModel* QWMRecordTwoStepSelector::selectionModel()
{
    QWMRecordSelector * selector=qobject_cast<QWMRecordSelector*>(ui->stackedWidget->widget(1));
    return selector->selectionModel();
}

QList<QWidget *> QWMRecordTwoStepSelector::taborders()
{
    return QList<QWidget*>();
    //    return QList<QWidget*>()<<ui->stackedWidget;
}

QSize QWMRecordTwoStepSelector::sizeHint()
{
    return QSize(450,350);
}
void QWMRecordTwoStepSelector::on_tab_accepted(QWidget *sel)
{
    if(instanceof<QWMTableSelector>(sel)){
        nav2RecordSelector();
    }else{
        emit accepted(this);
    }
}

void QWMRecordTwoStepSelector::on_tab_recjected(QWidget */*sel*/)
{
    emit rejected(this);
}

