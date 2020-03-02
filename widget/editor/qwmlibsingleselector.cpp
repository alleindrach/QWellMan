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

QWMLibSingleSelector::QWMLibSingleSelector(QString lib,QString lookupFld,QString title,bool editable,QString v,QWidget *parent)
    : QWMAbstractEditor(parent),ui(new Ui::QWMLibSingleSelector) ,
      _lookupFld(lookupFld),_title(title),_selectedValue(v),_editable(editable)
{
    ui->setupUi(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setMargin(0);
    this->setStyleSheet("background-color:rgb(150,150,150);");
    horizontalLayout->setObjectName("horizontalLayout");
    _selector=new QWMLibSelector(lib,lookupFld,title,editable,v,parent);
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

QList<QWidget *> QWMLibSingleSelector::taborders()
{
    return _selector->taborders();
}
void QWMLibSingleSelector::on_tab_accepted(QWidget *sel)
{
    emit accepted(this);
}

void QWMLibSingleSelector::on_tab_recjected(QWidget * sel)
{
    emit rejected(this);
}

