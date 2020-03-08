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
QWMLibTabSelector::QWMLibTabSelector(QString table, QString lib,QWidget *parent)
    : QWMAbstractEditor(parent),ui(new Ui::QWMLibTabSelector),_table(table)
{
    ui->setupUi(this);
    ui->tabWidget->setStyleSheet(APP->style());
    QString group=UDL->lookupGroup(APP->profile(),table);
    if(group.isNull()||group.isEmpty()){//单分组
        _type=Tab;
        QStringList tabs=LIB->libTabs(lib);
        QStringList lookupFlds=MDL->lookupFields(table,lib);
        foreach(QString tab ,tabs){
            QSqlQueryModel * model=LIB->libLookup(table,lib,tab,lookupFlds);
            QWMLibSelector * selector=new  QWMLibSelector(table,lib,model,this);
            ui->tabWidget->addTab(selector,tab);
            connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
            connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);
        }
    }else{//横竖双向分组
        _type=BiTab;
        QList<UDLLibGroup *> tabs= UDL->lookupTables(group);
        foreach(UDLLibGroup* tab ,tabs){
            QTabWidget * subTabWidget=new QTabWidget(ui->tabWidget);//
            subTabWidget->setTabPosition(QTabWidget::North);
            subTabWidget->setProperty(USER_PROPERTY,tab->KeyTbl());
            QList<UDLLibTab*> subTabs=UDL->lookupTableTabs(tab->KeySet(),tab->KeyTbl());
            foreach(UDLLibTab * subTab,subTabs){
                QList<UDLLibTabField*>  visibleFields=UDL->lookupTableFieldsOfTab(subTab->KeySet(),subTab->KeyTbl(),subTab->KeyTab());
                QStringList fields;
                foreach(UDLLibTabField * field,visibleFields){
                    fields<<field->KeyFld();
                }
                QSqlQueryModel * model=LIB->libLookup(table, subTab->KeyTbl(),subTab->KeyTab(),fields);
                QWMLibSelector * selector=new  QWMLibSelector(table,lib,model,this);
                subTabWidget->addTab(selector,subTab->KeyTab()) ;
                connect(selector,&QWMLibSelector::accepted,this,&QWMLibTabSelector::on_tab_accepted);
                connect(selector,&QWMLibSelector::rejected,this,&QWMLibTabSelector::on_tab_recjected);
            }
            subTabWidget->setCurrentIndex(0);
            ui->tabWidget->addTab(subTabWidget,tab->Caption());
        }
        ui->tabWidget->setTabPosition(QTabWidget::West);
    }
    ui->tabWidget->setCurrentIndex(0);
}
void QWMLibTabSelector::setText(QString text)
{
    if(_type==Tab){
        for(int i=0;i<ui->tabWidget->count();i++){
            QWMLibSelector * selector=(QWMLibSelector*) ui->tabWidget->widget(i);
            selector->setText(text);
        }
    }
    else if(_type==BiTab){
        for(int i=0;i<ui->tabWidget->count();i++){
            QTabWidget * subTab=(QTabWidget*)ui->tabWidget->widget(i);
            for(int j=0;j<subTab->count();j++){
                QWMLibSelector * selector=(QWMLibSelector*) subTab->widget(j);
                selector->setText(text);
            }
        }
    }
}

void QWMLibTabSelector::setValue(QVariant v)
{
    this->setText(v.toString());
}

QVariant QWMLibTabSelector::value()
{
    return QVariant();
}

QWMLibSelector *QWMLibTabSelector::currentWidget()
{
    if(_type==Tab){
        return (QWMLibSelector*)ui->tabWidget->currentWidget();
    }
    else if(_type==BiTab){
        return (QWMLibSelector*) ((QTabWidget*)(ui->tabWidget->currentWidget()))->currentWidget();
    }
}

void QWMLibTabSelector::focusInEvent(QFocusEvent *event)
{
    if(event->reason()!=Qt::NoFocusReason){
        if(_type==Tab){
            ui->tabWidget->currentWidget()->setFocus();
        }else if(_type==BiTab){
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

QWMAbstractEditor::Type QWMLibTabSelector::type()
{
    return _type;
}

void QWMLibTabSelector::init()
{

}

void QWMLibTabSelector::setLookupFld(QString fld)
{
    if(_type==Tab){
        for(int i=0;i<ui->tabWidget->count();i++){
            QWMLibSelector * selector=(QWMLibSelector*) ui->tabWidget->widget(i);
            selector->setLookupFld(fld);
        }
    }
    else if(_type==BiTab){
        for(int i=0;i<ui->tabWidget->count();i++){
            QTabWidget * subTab=(QTabWidget*)ui->tabWidget->widget(i);
            for(int j=0;j<subTab->count();j++){
                QWMLibSelector * selector=(QWMLibSelector*) subTab->widget(j);
                selector->setLookupFld(fld);
            }
        }
    }
}


void QWMLibTabSelector::on_tab_accepted(QWidget */*tab*/)
{
    emit accepted(this);
}

void QWMLibTabSelector::on_tab_recjected(QWidget * /*tab*/)
{
    emit rejected(this);
}
