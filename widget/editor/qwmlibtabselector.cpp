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
QWMLibTabSelector::QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editale,QString v,QWidget *parent) : QWidget(parent),ui(new Ui::QWMLibTabSelector),_title(title),_selectedValue(v),_editable(editale)
{

    ui->setupUi(this);
    QStringList tabs=LIB->libTabs(lib);
    foreach(QString tab ,tabs){
        QSqlQueryModel * model=LIB->libLookup(lib,tab);

        QWMLibSelector * selector=new  QWMLibSelector(lib,lookupFld,title,model,_editable,v,this);

        //        QHBoxLayout * horizontalLayout = new QHBoxLayout(this);
        //        //                horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        //        horizontalLayout->setContentsMargins(0, 4, 0, 4);
        //        QFrame* frame = new QFrame(this);
        //        //                frame->setObjectName(QString::fromUtf8("frame"));
        //        frame->setFrameShape(QFrame::StyledPanel);
        //        frame->setFrameShadow(QFrame::Raised);
        //        QHBoxLayout* horizontalLayout_2 = new QHBoxLayout(frame);
        //        //                horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        //        horizontalLayout_2->setContentsMargins(4, -1, 4, -1);
        //        horizontalLayout_2->addWidget(selector);
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


void QWMLibTabSelector::on_tab_accepted(QWMLibSelector *tab)
{
    emit accepted(tab);
}

void QWMLibTabSelector::on_tab_recjected(QWMLibSelector * tab)
{
    emit rejected(tab);
}

void QWMLibTabSelector::on_tabWidget_tabBarClicked(int index)
{

}
