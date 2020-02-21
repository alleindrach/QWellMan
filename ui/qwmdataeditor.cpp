#include "QToolBar"
#include "QCloseEvent"
#include "QStyleFactory"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include <QStandardItem>
#include "qwmdataeditor.h"
#include "ui_qwmdataeditor.h"
#include "qwmmain.h"
#include "common.h"
#include "mdldao.h"
#include "udldao.h"
#include "qwmapplication.h"
#include "welldao.h"
#include "QLabel"
#include "mdltable.h"
#include  "qwmdatatableview.h"
QWMDataEditor::QWMDataEditor(QString idWell,QString name,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_wellName(name),
    ui(new Ui::QWMDataEditor)
{
    ui->setupUi(this);

    _tbvData = new QWMDataTableView(ui->splitter);
    _tbvData->setObjectName(QString::fromUtf8("tbvData"));
    ui->splitter->addWidget(_tbvData);

    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,4);
    ui->trvTables->setStyle(QStyleFactory::create("windows"));
    this->loadDataTree();


    _lblStatus = new QLabel(this);
    QFont font=_lblStatus->font();
    font.setPixelSize(10);
    _lblStatus->setFont(font);
    _lblProfile = new QLabel(this);
    _lblProfile->setFont(font);
    _lblUnit=new  QLabel(this);
    _lblUnit->setFont(font);
    _lblReferenceDatum =new QLabel(this);
    _lblReferenceDatum->setFont(font);
    _lblMessage= new QLabel(this);
    _lblMessage->setFont(font);

    ui->statusbar->addWidget(_lblStatus,1);
    ui->statusbar->addWidget(_lblProfile,1);
    ui->statusbar->addWidget(_lblUnit,1);
    ui->statusbar->addWidget(_lblReferenceDatum,1);
    ui->statusbar->addPermanentWidget(_lblMessage,1);

    this->showProfile(APP->profile());
    this->showReferenceDatum(APP->referenceDatumName( APP->datumPreference()));
    this->showUnitSetting(APP->unit());
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
    connect(ui->trvTables,&QTreeView::clicked,this,&QWMDataEditor::on_trv_data_table_node_clicked);
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
    //    ui->trvTables->clear();//清除目录树所有节点
    QIcon icon;
    QSqlRecord wellRec=WELL->well(_idWell);
    QString wellTitle=WELL->recordDes(CFG(KeyTblMain),wellRec);
    QStandardItemModel * model=new QStandardItemModel(ui->trvTables);
    this->setWindowTitle(wellTitle);
    foreach(QString group ,groups){
        QStandardItem*  item=new QStandardItem(); //新建节点时设定类型为 itTopItem
        item->setIcon(APP->icons()["folder-open@4x"]); //设置第1列的图标

        item->setText(group); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        item->setFlags(Qt::ItemIsEnabled );
        item->setData(CAT_ROLE,QWMApplication::GROUP); //设置节点第1列的Qt::UserRole的Data
        model->appendRow(item);//添加顶层节点
        QList<MDLTable*> tables=UDL->tablesOfGroup(group,APP->profile());
        foreach(MDLTable * table ,tables){
            QString text=table->CaptionLongP();
            QString key=table->KeyTbl();
            QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
            tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标

            tableItem->setText(text); //设置第1列的文字
            //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
            tableItem->setFlags(Qt::ItemIsEnabled );
            tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(key,TABLE_NAME_ROLE);
//            tableItem->setData(text,TEXT_ROLE);
            tableItem->setData("",RECORD_DES_ROLE);
            tableItem->setData("",PK_VALUE_ROLE);
            tableItem->setToolTip(QString("[%1] %2").arg(table->KeyTbl()).arg(table->Help()));
            item->appendRow(tableItem);

            loadChildTable(tableItem);
        }

    }
    ui->trvTables->setModel(model);
    ui->trvTables->expandAll();
}

void QWMDataEditor::loadChildTable(QStandardItem * parent)
{
    QString strTblKey=parent->data(DATA_ROLE).toString();
    QStandardItemModel * model=parent->model();
    QList<MDLTable *> childTables=UDL->childTables(strTblKey,APP->profile());
    foreach(MDLTable * table,childTables){
        QString strChildTblKey=table->KeyTbl();
        QString strChildTblName=table->CaptionLongP();
        QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
        tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标

        tableItem->setText(strChildTblName); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        tableItem->setFlags(Qt::ItemIsEnabled );
        tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(strChildTblKey,TABLE_NAME_ROLE); //设置节点第1列的Qt::UserRole的Data
//        tableItem->setData(strChildTblName,TEXT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData("",RECORD_DES_ROLE);
        tableItem->setData("",PK_VALUE_ROLE);
        tableItem->setToolTip(QString("[%1] %2").arg(table->KeyTbl()).arg(table->Help()));
        parent->appendRow(tableItem);
        loadChildTable(tableItem);
    }

}
void QWMDataEditor::showStatus(QString status)
{
    this->_lblStatus->setText(status);
}

void QWMDataEditor::showProfile(QString profile)
{
    this->_lblProfile->setText(profile);
}

void QWMDataEditor::showUnitSetting(QString unit)
{
    this->_lblUnit->setText(unit);
}

void QWMDataEditor::showReferenceDatum(QString datum)
{
    this->_lblReferenceDatum->setText(datum);
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

void QWMDataEditor::on_trv_data_table_node_clicked(const QModelIndex &index)
{
    //1 如果当前的表和well是1：1的，且无记录，则必须生成一个新纪录
    //2 如果当前表有记录，且当前节点无选中历史，则 选中第一条记录 ，将当前记录的key保存 到treeview node的key_field中 。当前几点的RecordDes显示
    //3 如果当前表有父表，则必须父表的节点有选中记录，否则报警，如果当前的父表有选中记录，则将父表的id设置为当前表的parentid进行过滤 。
    QString tableName=index.data(TABLE_NAME_ROLE).toString();
    QString parentTableName=MDL->parentTable(tableName);
    MDLTable * tableInfo=MDL->tableInfo(tableName);

    if(!parentTableName.isNull()  && !parentTableName.isEmpty()){

    }

}
