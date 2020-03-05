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
#include "ui_qwmrecordsinglestepselector.h"

QWMRecordSingleStepSelector::QWMRecordSingleStepSelector(QStringList tables,QString idwell,QString title,QWidget *parent )
    : QWMAbstractEditor(parent),ui(new Ui::QWMRecordSingleStepSelector) ,
      _idWell(idwell),_tables(tables)
{
    ui->setupUi(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(ui->frameInner);
    horizontalLayout->setMargin(0);
//    this->setStyleSheet("background-color:rgb(150,150,150);");
    horizontalLayout->setObjectName("horizontalLayout");
    _selector=new QWMRecordSelector(tables,idwell,title,this);
    horizontalLayout->addWidget(_selector);
    connect(_selector,&QWMRecordSelector::accepted,this,&QWMRecordSingleStepSelector::on_tab_accepted);
    connect(_selector,&QWMRecordSelector::rejected,this,&QWMRecordSingleStepSelector::on_tab_recjected);
    connect(ui->btnOK,&QPushButton::clicked,this,&QWMAbstractEditor::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMAbstractEditor::on_btn_clicked);
}

void QWMRecordSingleStepSelector::setText(QString text)
{
    _selector->setText(text);
}

QString QWMRecordSingleStepSelector::text()
{
   return _selector->text();
}

const QItemSelectionModel* QWMRecordSingleStepSelector::selectionModel()
{
    return _selector->selectionModel();
}

QList<QWidget *> QWMRecordSingleStepSelector::taborders()
{
    return _selector->taborders();
}

QSize QWMRecordSingleStepSelector::sizeHint()
{
    return QWMAbstractEditor::sizeHint();
//    return QSize(350,250);
}
void QWMRecordSingleStepSelector::on_tab_accepted(QWidget */*sel*/)
{
    emit accepted(this);
}

void QWMRecordSingleStepSelector::on_tab_recjected(QWidget */*sel*/)
{
    emit rejected(this);
}

