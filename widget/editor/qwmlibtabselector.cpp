#include "qwmlibtabselector.h"
#include "ui_qwmlibtabselector.h"
#include "qwmlibselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include "common.h"
#include "libdao.h"
#include "udldao.h"
#include "mdldao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
#include "qwmreflookupdelegate.h"
#include "qwmuserdata.h"
#include "QFocusEvent"
//QWMLibTabSelector::QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editale,QString v,QWidget *parent)
//    : QWMAbstractEditor(parent),ui(new Ui::QWMLibTabSelector),_title(title),_selectedValue(v),_editable(editale)
//{

//    ui->setupUi(this);
//    QStringList tabs=LIB->libTabs(lib);
//    foreach(QString tab ,tabs){
//        QSqlQueryModel * model=LIB->libLookup(lib,tab);
//        QWMLibSelector * selector=new  QWMLibSelector(lib,lookupFld,title,model,QStringList() ,_editable,v,this);
//        ui->tabWidget->addTab(selector,tab);
//        connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
//        connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);

//    }

//    ui->tabWidget->setCurrentIndex(0);
//}
QWMLibTabSelector::QWMLibTabSelector(QString table, QString lib,QString lookupFld,QString title,bool editale,QString v,QWidget *parent)
    : QWMAbstractEditor(parent),ui(new Ui::QWMLibTabSelector),_table(table),_title(title),_selectedValue(v),_editable(editale)
{

    TIMESTAMP(QWMLibTabSelectorE);
    ui->setupUi(this);
    QString group=UDL->lookupGroup(APP->profile(),table);
    if(group.isNull()||group.isEmpty()){//单分组
        _type=S;
        QStringList tabs=LIB->libTabs(lib);
        QStringList lookupFlds=MDL->lookupFields(table,lib);
        foreach(QString tab ,tabs){
            QSqlQueryModel * model=LIB->libLookup(table,lib,tab,lookupFlds);
            QWMLibSelector * selector=new  QWMLibSelector(table,lib,lookupFld,title,model,QStringList() ,_editable,v,this);
            ui->tabWidget->addTab(selector,tab);
            connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
            connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);
        }
    }else{//横竖双向分组
        _type=B;
        QList<UDLLibGroup *> tabs= UDL->lookupTables(group);
        TIMESTAMP(QWMLibTabSelectorInitGROUP);
        foreach(UDLLibGroup* tab ,tabs){
            QTabWidget * subTabWidget=new QTabWidget(ui->tabWidget);//
            subTabWidget->setTabPosition(QTabWidget::North);
            subTabWidget->setProperty(USER_PROPERTY,tab->KeyTbl());
            TIMESTAMP(QWMLibTabSelectorTab);
            QList<UDLLibTab*> subTabs=UDL->lookupTableTabs(tab->KeySet(),tab->KeyTbl());
            TIMESTAMP(QWMLibTabSelectorTab2);
            foreach(UDLLibTab * subTab,subTabs){
                TIMESTAMP(QWMLibTabSelectorTab3);
                QList<UDLLibTabField*>  visibleFields=UDL->lookupTableFieldsOfTab(subTab->KeySet(),subTab->KeyTbl(),subTab->KeyTab());
                TIMESTAMP(QWMLibTabSelectorTab4);
                QStringList fields;
                foreach(UDLLibTabField * field,visibleFields){
                    fields<<field->KeyFld();
                }
                TIMESTAMP(QWMLibTabSelectorTab5);
                QSqlQueryModel * model=LIB->libLookup(table, subTab->KeyTbl(),subTab->KeyTab(),fields);
                TIMESTAMP(QWMLibTabSelectorTab6);
                QWMLibSelector * selector=new  QWMLibSelector(table,lib,lookupFld,title,model,QStringList() ,_editable,v,this);
                TIMESTAMP(QWMLibTabSelectorTab7);
                subTabWidget->addTab(selector,subTab->KeyTab()) ;
                TIMESTAMP(QWMLibTabSelectorTab8);
                connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
                connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);
                TIMESTAMP(QWMLibTabSelectorTab9);
            }
            subTabWidget->setCurrentIndex(0);
            ui->tabWidget->addTab(subTabWidget,tab->Caption());
        }
        TIMESTAMP(QWMLibTabSelectorENDGROUP);
        ui->tabWidget->setTabPosition(QTabWidget::West);
    }
    ui->tabWidget->setCurrentIndex(0);
}
void QWMLibTabSelector::setText(QString text)
{
    if(_type==S){
        for(int i=0;i<ui->tabWidget->count();i++){
            QWMLibSelector * selector=(QWMLibSelector*) ui->tabWidget->widget(i);
            selector->setText(text);
        }
    }
    else if(_type==B){
        for(int i=0;i<ui->tabWidget->count();i++){
            QTabWidget * subTab=(QTabWidget*)ui->tabWidget->widget(i);
            for(int j=0;j<subTab->count();j++){
                QWMLibSelector * selector=(QWMLibSelector*) subTab->widget(j);
                selector->setText(text);
            }
        }
    }
}

QWMLibSelector *QWMLibTabSelector::currentWidget()
{
    if(_type==S){
        return (QWMLibSelector*)ui->tabWidget->currentWidget();
    }
    else if(_type==B){
        return (QWMLibSelector*) ((QTabWidget*)(ui->tabWidget->currentWidget()))->currentWidget();
    }
}

void QWMLibTabSelector::focusInEvent(QFocusEvent *event)
{
    if(event->reason()!=Qt::NoFocusReason){
        if(_type==S){
            ui->tabWidget->currentWidget()->setFocus();
        }else if(_type==B){
            ((QTabWidget*)(ui->tabWidget->currentWidget()))->currentWidget()->setFocus();
        }
    }
}

QSqlRecord QWMLibTabSelector::selectedRecord()
{
    this->currentWidget()->selectedRecord();
}

QList<QWidget *> QWMLibTabSelector::taborders()
{
    QList<QWidget*> lists;
    lists<<ui->tabWidget;
    return lists;
}

QSize QWMLibTabSelector::sizeHint()
{
    return QWMAbstractEditor::sizeHint();
    //    if(_type==S){
    //        return QSize(350,250);
    //    }else
    //    {
    //        return QSize(500,350);
    //    }
}


void QWMLibTabSelector::on_tab_accepted(QWidget *tab)
{
    emit accepted(this);
}

void QWMLibTabSelector::on_tab_recjected(QWidget * tab)
{
    emit rejected(this);
}
