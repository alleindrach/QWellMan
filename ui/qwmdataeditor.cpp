#include "QToolBar"
#include "QCloseEvent"
#include "QStyleFactory"
#include <QSqlQuery>
#include <QSqlRecord>
#include "qwmdataeditor.h"
#include "ui_qwmdataeditor.h"
#include "qwmmain.h"
#include "common.h"
#include "mdldao.h"
#include "udldao.h"
#include "qwmapplication.h"
#include "welldao.h"

QWMDataEditor::QWMDataEditor(QString idWell,QString name,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_wellName(name),
    ui(new Ui::QWMDataEditor)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,4);
    ui->trwTables->setStyle(QStyleFactory::create("windows"));
    this->loadDataTree();

    //    QToolBar * toolBar =new QToolBar(this);
    //    toolBar->setObjectName(QString::fromUtf8("toolBar"));
    //    toolBar->setIconSize(QSize(16, 16));
    //    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    //    ui->verticalLayout->insertWidget(0,m_dataToolbar);
    //    m_dataToolbar->setIconSize(size);
    //    m_dataToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //    m_dataToolbar->addAction(ui->actAddItem);
    //    m_dataToolbar->addAction(ui->actInsertItem);
    //    m_dataToolbar->addAction(ui->actDelItem);
    //    m_dataToolbar->addAction(ui->actRotate);
    //    m_dataGrid=tableviewData;

}

QWMDataEditor::~QWMDataEditor()
{
    delete ui;
}

void QWMDataEditor::loadDataTree()
{
    QStringList groups=UDL->tableGroup(APP->profile());

    //初始化Tree
    //QTreeWidgetItem *item; //节点
    QString dataStr=""; // Item的Data 存储的string
    ui->trwTables->clear();//清除目录树所有节点
    QIcon icon;
    QSqlRecord wellRec=WELL->well(_idWell);
    QString wellTitle=WELL->recordDes(CFG(KeyTblMain),wellRec);
    this->setWindowTitle(wellTitle);
    foreach(QString group ,groups){
        QTreeWidgetItem*  item=new QTreeWidgetItem(ui->trwTables); //新建节点时设定类型为 itTopItem
        item->setIcon(0,APP->icons()["folder-open@4x"]); //设置第1列的图标

        item->setText(0,group); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        item->setFlags(Qt::ItemIsEnabled );
        item->setData(0,CAT_ROLE,QWMApplication::GROUP); //设置节点第1列的Qt::UserRole的Data
        ui->trwTables->addTopLevelItem(item);//添加顶层节点
        QSqlQuery qTables=UDL->tablesOfGroup(group,APP->profile());
        while(qTables.next()){
            QString text=QS(qTables,CaptionLongP);
            QString key=QS(qTables,KeyTbl);
            QTreeWidgetItem*  tableItem=new QTreeWidgetItem(item); //新建节点时设定类型为 itTopItem
            tableItem->setIcon(0,APP->icons()["data@4x"]); //设置第1列的图标

            tableItem->setText(0,text); //设置第1列的文字
            //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
            tableItem->setFlags(Qt::ItemIsEnabled );
            tableItem->setData(0,CAT_ROLE,QWMApplication::TABLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(0,DATA_ROLE,key); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(0,TEXT_ROLE,text); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(0,RECORD_DES_ROLE,""); //设置节点第1列的Qt::UserRole的Data
            item->addChild(tableItem);
             QSqlQuery childTables=UDL->childTables(key,APP->profile());

             loadChildTable(tableItem);

        }

    }
}

void QWMDataEditor::loadChildTable(QTreeWidgetItem * parent)
{
    QString strTblKey=parent->data(0,DATA_ROLE).toString();

    QSqlQuery childTables=UDL->childTables(strTblKey,APP->profile());
    while(childTables.next()){
        QString strChildTblKey=QS(childTables,KeyTbl);
        QString strChildTblName=QS(childTables,CaptionLongP);
        QTreeWidgetItem*  tableItem=new QTreeWidgetItem(parent); //新建节点时设定类型为 itTopItem
        tableItem->setIcon(0,APP->icons()["data@4x"]); //设置第1列的图标

        tableItem->setText(0,strChildTblName); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        tableItem->setFlags(Qt::ItemIsEnabled );
        tableItem->setData(0,CAT_ROLE,QWMApplication::TABLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(0,DATA_ROLE,strChildTblKey); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(0,TEXT_ROLE,strChildTblName); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(0,RECORD_DES_ROLE,""); //设置节点第1列的Qt::UserRole的Data
        parent->addChild(tableItem);
        loadChildTable(tableItem);
    }

}

void QWMDataEditor::closeEvent(QCloseEvent *event)
{

    QWMMain * main= static_cast<QWMMain *>(this->parent());
    main->show();
    event->accept();
}


void QWMDataEditor::on_actionSaveExit_triggered()
{
    this->close();
}
