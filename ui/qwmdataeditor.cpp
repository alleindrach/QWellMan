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
#include "qwmdatatableview.h"
#include <QMessageBox>
#include "qwmrotatableproxymodel.h"
#include "qwmsortfilterproxymodel.h"
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

    _tbvData->setStyleSheet(_TableStyle);
    _tbvData->verticalHeader()->setDefaultSectionSize(12);
    _tbvData->setFont(font);

    connect(ui->trvTables,&QTreeView::clicked,this,&QWMDataEditor::on_trv_table_node_clicked);
    connect(ui->actionRedo,&QAction::triggered,this,&QWMDataEditor::redo);
    connect(ui->actionUndo,&QAction::triggered,this,&QWMDataEditor::undo);
}

QWMDataEditor::~QWMDataEditor()
{
    delete ui;
}

void QWMDataEditor::undo()
{
    _undoStack.undo();
}

void QWMDataEditor::addUndoCommand(QUndoCommand *command)
{
    _undoStack.push(command);
}

void QWMDataEditor::redo()
{
    _undoStack.redo();
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
            tableItem->setData(text,TEXT_ROLE);
            tableItem->setData(QString(),RECORD_DES_ROLE);
            tableItem->setData(QString(),PK_VALUE_ROLE);
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
    QString strTblKey=parent->data(TABLE_NAME_ROLE).toString();
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
        tableItem->setData(strChildTblName,TEXT_ROLE); //设置节点第1列的Qt::UserRole的Data
        tableItem->setData(QString(),RECORD_DES_ROLE);
        tableItem->setData(QString(),PK_VALUE_ROLE);
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

void QWMDataEditor::showDataGrid(QWMRotatableProxyModel *model)
{

    PX(pmodel,model);

    if(model->mode()==QWMRotatableProxyModel::H){
        for(int j=0;j<model->columnCount();j++){
            if(j<model->visibleFieldsCount()){
                _tbvData->setColumnHidden(j,false);
            }else
            {
                _tbvData->setColumnHidden(j,true);
            }
        }
        for(int i=0;i<model->rowCount();i++){
            _tbvData->setRowHidden(i,false);
        }
    }else{
        for(int i=0;i<model->rowCount();i++){
            if(i<model->visibleFieldsCount()){
                _tbvData->setRowHidden(i,false);
            }else
            {
                _tbvData->setRowHidden(i,true);
            }
        }
        for(int j=0;j<model->columnCount();j++){
            _tbvData->setColumnHidden(j,false);
        }
    }
    _tbvData->bindDelegate();
    this->resize(this->size()+QSize(1,1));
}

void QWMDataEditor::closeEvent(QCloseEvent *event)
{

    QWMMain * main= static_cast<QWMMain *>(this->parent());
    main->show();
    main->setCurrentEditor(nullptr);
    event->accept();
}


void QWMDataEditor::on_actionSaveExit_triggered()
{
    this->close();
}

QString QWMDataEditor::nodeParentID(const QModelIndex &index,QString &lastError){
    QString parentID=QString();
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        QString parentTableName=MDL->parentTable(tableName);
        MDLTable * tableInfo=MDL->tableInfo(tableName);
        if(!parentTableName.isNull()  && !parentTableName.isEmpty()){
            if(index.parent().isValid() && index.parent().data(CAT_ROLE)==QWMApplication::TABLE)//如果有父节点
            {
                QString pv=index.parent().data(PK_VALUE_ROLE).toString();
                QString ptn=index.parent().data().toString();
                if(pv.isNull()){
                    lastError=tr("未选中[%1]表的记录！").arg(ptn);
                    return QString();
                }else{
                    parentID=ptn;
                }
            }else{//如果无父节点，则认为 其父为wvWellheader
//                parentID=_idWell;
            }
        }
        else{
//            parentID=_idWell;
        }
        return parentID;
    }
    return parentID;
}

MDLTable *QWMDataEditor::nodeTableInfo(const QModelIndex &index)
{
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        QString parentTableName=MDL->parentTable(tableName);
        MDLTable * tableInfo=MDL->tableInfo(tableName);
        return tableInfo;
    }
    return nullptr;
}

void QWMDataEditor::clearChildSelection(const QModelIndex &index)
{
    QItemSelectionModel*  selection= ui->trvTables->selectionModel();
    QAbstractItemModel * model=ui->trvTables->model();


    for(int i=0;i<model->rowCount(index);i++){
        QModelIndex childIndex=index.child(i,0);
        QString tableName=model->data(childIndex,TABLE_NAME_ROLE).toString();
        QString textDisplay=model->data(childIndex,TEXT_ROLE).toString();
//        MDLTable * tableInfo=MDL->tableInfo(tableName);
        model->setData(childIndex,textDisplay,Qt::DisplayRole);
        model->setData(childIndex,QString(),PK_VALUE_ROLE);
        model->setData(childIndex,QString(),RECORD_DES_ROLE);
        clearChildSelection(index.child(i,0));
    }
}
void QWMDataEditor::on_current_record_changed(const QModelIndex &current, const QModelIndex &previous){
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>(_tbvData->model());
    SX(sourceModel,model);
    if((model->mode()==QWMRotatableProxyModel::H && current.row()!=previous.row()  && current.row()>=0 && model->rowCount()>0) ||
            (model->mode()==QWMRotatableProxyModel::V && current.column()!=previous.column() && current.column()>=0 && model->columnCount()>0)
            ){

        QSqlRecord record=model->record(current);
        PK_VALUE(pk,record);
        QString tableName=sourceModel->tableName();
        MDLTable * tableInfo=MDL->tableInfo(tableName);
        QString des=WELL->recordDes(tableName,record);
        QString dispText=QString("%1  [%2]").arg(tableInfo->CaptionLongP()).arg(des);
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),dispText,Qt::DisplayRole);
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),pk,PK_VALUE_ROLE);
        clearChildSelection(ui->trvTables->currentIndex());
    }

}
void QWMDataEditor::on_trv_table_node_clicked(const QModelIndex &index)
{
    //1 如果当前的表和well是1：1的，且无记录，则必须生成一个新纪录
    //2 如果当前表有记录，且当前节点无选中历史，则 选中第一条记录 ，将当前记录的key保存 到treeview node的key_field中 。当前几点的RecordDes显示
    //3 如果当前表有父表，则必须父表的节点有选中记录，否则报警，如果当前的父表有选中记录，则将父表的id设置为当前表的parentid进行过滤 。
    //4 选中当前节点对应的表的其中一条记录后，子节点的所有选中记录清空
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString lastError;
        QString parentID=nodeParentID(index,lastError);
        if(parentID.isNull()&& !lastError.isEmpty()){
            QMessageBox::information(this,tr("错误"),lastError);
            return ;
        }

        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        QWMRotatableProxyModel * model=WELL->tableForEdit(tableName,_idWell,parentID);
        SX(sourceModel,model);
        sourceModel->select();
        this->_tbvData->setModel(model);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,this, &QWMDataEditor::on_current_record_changed);

        if(sourceModel->rowCount()>0){
            QItemSelectionModel * selectModel=_tbvData->selectionModel();
            QItemSelection prevSelect=selectModel->selection();
            if(prevSelect.isEmpty()){
                selectModel->select(model->index(0,0),QItemSelectionModel::SelectCurrent);
                if(model->mode()==QWMRotatableProxyModel::H)
                    emit selectModel->currentRowChanged(model->index(0,0), QModelIndex());
                else
                    emit selectModel->currentColumnChanged(model->index(0,0), QModelIndex());
            }
        }
        MDLTable * tableInfo=nodeTableInfo(index);
        if(tableInfo->OneToOne()){//1:1的情况，默认选择首行
            if( (model->mode()==QWMRotatableProxyModel::H && model->rowCount()==0)||(model->mode()==QWMRotatableProxyModel::V && model->columnCount()==0)) {
                //新建一条1：1的记录
                QSqlRecord record=model->record();
                WELL->initRecord(record,_idWell,parentID);
                model->insertRecord(0,record);
            }
            ui->actionNew->setEnabled(false);
            ui->actionDelete->setEnabled(false);
        }else
        {
            ui->actionNew->setEnabled(true);
            ui->actionDelete->setEnabled(false);
        }

        //        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
        //        void currentColumnChanged(const QModelIndex &current, const QModelIndex &previous);
        showDataGrid(model);
    }
}

void QWMDataEditor::on_actionRotate_triggered(bool checked)
{
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>( _tbvData->model());
//    model->beginResetModel();
    if(checked)
        model->setMode(QWMRotatableProxyModel::V);
    else
        model->setMode(QWMRotatableProxyModel::H);
//    model->endResetModel();
//    showDataGrid(model);
}
