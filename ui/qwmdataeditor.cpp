#include "QToolBar"
#include "QCloseEvent"
#include "QStyleFactory"
#include <QSqlQuery>
#include <QSqlRecord>
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
QWMDataEditor::QWMDataEditor(QString idWell,QString name,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_wellName(name),
    ui(new Ui::QWMDataEditor)
{
    ui->setupUi(this);
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
        QSqlQuery qTables=UDL->tablesOfGroup(group,APP->profile());
        while(qTables.next()){
            QString text=QS(qTables,CaptionLongP);
            QString key=QS(qTables,KeyTbl);
            QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
            tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标

            tableItem->setText(text); //设置第1列的文字
            //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
            tableItem->setFlags(Qt::ItemIsEnabled );
            tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(key,DATA_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData(text,TEXT_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setData("",RECORD_DES_ROLE); //设置节点第1列的Qt::UserRole的Data
            tableItem->setToolTip(QString("[%1] %2").arg(QS(qTables,KeyTbl)).arg(QS(qTables,Help)));
            item->appendRow(tableItem);
            QSqlQuery childTables=UDL->childTables(key,APP->profile());
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
    QSqlQuery childTables=UDL->childTables(strTblKey,APP->profile());
    while(childTables.next()){
        QString strChildTblKey=QS(childTables,KeyTbl);
        QString strChildTblName=QS(childTables,CaptionLongP);
        QStandardItem*  tableItem=new QStandardItem(); //新建节点时设定类型为 itTopItem
        tableItem->setIcon(APP->icons()["data@4x"]); //设置第1列的图标

        tableItem->setText(strChildTblName); //设置第1列的文字
        //        item->setText(MainWindow::colItemType,"type=itTopItem");  //设置第2列的文字
        tableItem->setFlags(Qt::ItemIsEnabled );
        tableItem->setData(QWMApplication::TABLE,CAT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(strChildTblKey,DATA_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(strChildTblName,TEXT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData("",RECORD_DES_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setToolTip(QString("[%1] %2").arg(QS(childTables,KeyTbl)).arg(QS(childTables,Help)));
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
