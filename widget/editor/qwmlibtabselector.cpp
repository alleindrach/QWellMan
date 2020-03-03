#include "qwmlibtabselector.h"
#include "ui_qwmlibtabselector.h"
#include "qwmlibselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
QWMLibTabSelector::QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editale,QString v,QWidget *parent)
    : QWMAbstractEditor(parent),ui(new Ui::QWMLibTabSelector),_title(title),_selectedValue(v),_editable(editale)
{

    ui->setupUi(this);
    QStringList tabs=LIB->libTabs(lib);
    foreach(QString tab ,tabs){
        QSqlQueryModel * model=LIB->libLookup(lib,tab);
        QWMLibSelector * selector=new  QWMLibSelector(lib,lookupFld,title,model,QStringList() ,_editable,v,this);
        ui->tabWidget->addTab(selector,tab);
        connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
        connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);

    }
    ui->tabWidget->setCurrentIndex(0);
}

void QWMLibTabSelector::setText(QString text)
{
    for(int i=0;i<ui->tabWidget->count();i++){
       QWMLibSelector * selector=(QWMLibSelector*) ui->tabWidget->widget(i);
       selector->setText(text);
    }
}

QWMLibSelector *QWMLibTabSelector::currentWidget()
{
    return (QWMLibSelector*)ui->tabWidget->currentWidget();
}

void QWMLibTabSelector::focusInEvent(QFocusEvent *event)
{
    ui->tabWidget->currentWidget()->setFocus();
}

QList<QWidget *> QWMLibTabSelector::taborders()
{
    QList<QWidget*> lists;
    lists<<ui->tabWidget;
    return lists;
}


void QWMLibTabSelector::on_tab_accepted(QWidget *tab)
{
    emit accepted(this);
}

void QWMLibTabSelector::on_tab_recjected(QWidget * tab)
{
    emit rejected(this);
}
