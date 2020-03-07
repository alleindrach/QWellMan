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
#include "ui_qwmlibsingleselector.h"

QWMLibSingleSelector::QWMLibSingleSelector(QString table,QString lib,QWidget *parent)
    : QWMAbstractEditor(parent),ui(new Ui::QWMLibSingleSelector) ,
    _table(table)
{
    ui->setupUi(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
//    this->setStyleSheet("background-color:rgb(150,150,150);");
    horizontalLayout->setObjectName("horizontalLayout");

    _selector=new QWMLibSelector(table,lib,nullptr,parent);
    horizontalLayout->addWidget(_selector);
    connect(_selector,&QWMLibSelector::accepted,this,&QWMLibSingleSelector::on_tab_accepted);
    connect(_selector,&QWMLibSelector::rejected,this,&QWMLibSingleSelector::on_tab_recjected);

}

void QWMLibSingleSelector::setText(QString text)
{
    _selector->setText(text);
}

QString QWMLibSingleSelector::text()
{
   return _selector->text();
}

const QItemSelectionModel* QWMLibSingleSelector::selectionModel()
{
    return _selector->selectionModel();
}

QSqlRecord QWMLibSingleSelector::selectedRecord()
{
    return _selector->selectedRecord();
}

QList<QWidget *> QWMLibSingleSelector::taborders()
{
    return _selector->taborders();
}

void QWMLibSingleSelector::setValue(QVariant v)
{
    this->setText(v.toString());
}

QVariant QWMLibSingleSelector::value()
{
    return  text();
}

void QWMLibSingleSelector::init()
{
    _selector->setEditable(_editable);
    _selector->init();
}

QWMAbstractEditor::Type QWMLibSingleSelector::type()
{
    return QWMAbstractEditor::Simple;
}
void QWMLibSingleSelector::on_tab_accepted(QWidget */*sel*/)
{
    emit accepted(this);
}

void QWMLibSingleSelector::on_tab_recjected(QWidget * /*sel*/)
{
    emit rejected(this);
}

