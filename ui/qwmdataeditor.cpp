#include <QToolBar>
#include <QCloseEvent>
#include <QStyleFactory>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include <QStandardItem>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlField>
#include <QFileInfo>
#include "qwmdataeditor.h"
#include "ui_qwmdataeditor.h"
#include "qwmmain.h"
#include "common.h"
#include "mdldao.h"
#include "udldao.h"
#include "qwmapplication.h"
#include "welldao.h"

#include "mdltable.h"
#include "qwmdatatableview.h"

#include "qwmrotatableproxymodel.h"
#include "qwmfieldeditcommand.h"
#include "qwmrecordeditcommand.h"
#include "qwmsortfilterproxymodel.h"

#define CHECK_UNDO_STATE \
    ui->actionRedo->setEnabled(undoStack()->canRedo());\
    ui->actionUndo->setEnabled(undoStack()->canUndo());\
    ui->actionSave->setEnabled(isCurrentTableDirty());\
    ui->actionSaveAll->setEnabled(isDirty());

#define CUR_UNDO_STACK_KEY(key) \
    QString key; \
{\
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model(); \
    PX(proxyModel,model); \
    SX(sourceModel,model);  \
    key=QString("%1.%2.%3").arg(_idWell).arg(sourceModel->tableName()).arg(proxyModel->parentId());\
    }

#define UNDO_STACK_KEY(key,table,parent) \
    QString key=QString("%1.%2.%3").arg(_idWell).arg(table).arg(parent);

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

QWMDataEditor::QWMDataEditor(QString idWell,QString name,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_wellName(name),
    ui(new Ui::QWMDataEditor)
{
    ui->setupUi(this);
}

QWMDataEditor::~QWMDataEditor()
{
    delete ui;
}

void QWMDataEditor::init()
{
    _tbvData = new QWMDataTableView(ui->frame_3);
    _tbvData->setObjectName(QString::fromUtf8("tbvData"));
    //    ui->splitter->addWidget(_tbvData);
    ui->horizontalLayout_2->addWidget(_tbvData);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,8);
    ui->trvTables->setStyle(QStyleFactory::create("windows"));

    _tableOpToolBar=new QToolBar(this);
    _tableOpToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _tableOpToolBar->setIconSize(QSize(12,12));

    ui->verticalLayout_2->insertWidget(0,_tableOpToolBar);

    _tableOpToolBar->addAction(ui->actionNew);
    _tableOpToolBar->addAction(ui->actionDelete);
    _tableOpToolBar->addAction(ui->actionSave);
    _tableOpToolBar->addSeparator();
    _navActions<<ui->actionactPrevRec;
    _tableOpToolBar->addAction(ui->actionactPrevRec);
    _tableOpToolBar->addAction(ui->actionParent);
    _navActions<<ui->actionParent;
    //    _lblParentTable=new QLabel(this);
    //    _lblParentTable->setText("");
    //    _lblParentTable->setVisible(false);
    //    _tableOpToolBar->addWidget(_lblParentTable);
    _navActions<<ui->actionactNextRec;
    _tableOpToolBar->addAction(ui->actionactNextRec);
    _navActions<<_tableOpToolBar->addSeparator();
    _tableOpToolBar->addAction(ui->actionCalc);
    _tableOpToolBar->addSeparator();
    _tableOpToolBar->addAction(ui->actionUndo);
    _tableOpToolBar->addAction(ui->actionRedo);
    _tableOpToolBar->addSeparator();
    _tableOpToolBar->addAction(ui->actionSort);
    _tableOpToolBar->addSeparator();

    _tableOpToolBar->addAction(ui->actionOpen);
    _tableOpToolBar->addAction(ui->actionSaveTo);
    ui->actionOpen->setVisible(false);
    ui->actionSaveTo->setVisible(false);

    _lblStatus = new QLabel(this);
    QFont font=_lblStatus->font();
    font.setPixelSize(10);
    ui->trvTables->setFont(font);
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

    _tbvData->setStyleSheet(APP->style());
    _tbvData->verticalHeader()->setDefaultSectionSize(12);
    _tbvData->setFont(font);
    ui->trvTables->setStyleSheet(APP->style());
    connect(ui->trvTables,&QTreeView::clicked,this,&QWMDataEditor::on_trv_table_node_clicked);
    connect(ui->actionRedo,&QAction::triggered,this,&QWMDataEditor::redo);
    connect(ui->actionUndo,&QAction::triggered,this,&QWMDataEditor::undo);
    connect(_tbvData,&QWMDataTableView::RecordCountChanged,this,&QWMDataEditor::on_data_record_changed);

    this->loadDataTree();
    CHECK_UNDO_STATE
}

void QWMDataEditor::undo()
{
    undoStack()->undo();
    CHECK_UNDO_STATE;

}

void QWMDataEditor::addUndoCommand(QUndoCommand *command,QString table,QString parentId)
{
    UNDO_STACK_KEY(key,table,parentId);
    QUndoStack * stack=nullptr;
    if(_undoStacks.contains(key)){
        stack=new QUndoStack(this);
        _undoStacks.insert(key,stack);
    }else
    {
        stack=_undoStacks[key];
    }
    stack->push(command);
    CHECK_UNDO_STATE;

}

void QWMDataEditor::redo()
{
    undoStack()->redo();
    CHECK_UNDO_STATE;

}

void QWMDataEditor::on_model_submitted(QString tablename)
{
    _dirtyTables.remove(tablename);
}

void QWMDataEditor::on_model_data_modified(QString tablename, QString parentId)
{
    QWMTableModel * model=WELL->table(tablename,_idWell);
    if(model->isDirty()){
        _dirtyTables.insert(tablename,model);
    }
    CHECK_UNDO_STATE;
}

void QWMDataEditor::loadDataTree()
{
    _wellRec=WELL->well(_idWell);
    _wellDes=WELL->recordDes(CFG(KeyTblMain),_wellRec);
    this->setWindowTitle(_wellDes);
    QStandardItemModel * model= MDL->loadDataTree(true,this);
    ui->trvTables->setModel(model);
    ui->trvTables->expandAll();
    QItemSelectionModel * selectModel= ui->trvTables->selectionModel();
    QString tablename=CFG(KeyTblMain);
    QModelIndexList matchedList=model->match(model->index(0,0),TABLE_NAME_ROLE, CFG(KeyTblMain),1,Qt::MatchCaseSensitive|Qt::MatchRecursive);
    if(!matchedList.isEmpty()){
        selectModel->setCurrentIndex(matchedList.first(),QItemSelectionModel::SelectCurrent);
        editTable(matchedList.first());
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

QList<QWMRotatableProxyModel *> QWMDataEditor::dirtyTables(QModelIndex index)
{
    QList<QWMRotatableProxyModel *> result;
    QAbstractItemModel * model=ui->trvTables->model();
    QString parentID=index.data(PK_VALUE_ROLE).toString();
    QString parentTableName=index.data(TABLE_NAME_ROLE).toString();
    //    qDebug()<<"dirtyTables["<<index.row()<<","<<index.column()<<"]."<<parentTableName<<"."<<parentID;
    if(!parentID.isNull()){
        for(int i=0;i<model->rowCount(index);i++){
            QModelIndex childIndex=index.child(i,0);
            QString tableName=model->data(childIndex,TABLE_NAME_ROLE).toString();
            //            if(tableName=="wvJobReportSupportVes"){
            //                qDebug()<<"main.wvJobReportSupportVes";
            //            }
            int modelTypeId=QMetaType::type((QString(QWMRotatableProxyModel::staticMetaObject.className())+"*").toStdString().c_str());
            QVariant value=model->data(childIndex,MODEL_ROLE);

            //            qDebug()<<"\tdirtySub Tables["<<i<<"]."<<tableName<<"."<<parentID;

            if(!value.isNull()){
                QWMRotatableProxyModel * model=value.value<QWMRotatableProxyModel*>();//  WELL->tableForEdit(tableName,_idWell,parentID);
                //                qDebug()<<"\t\tdirtySub Tables["<<i<<"].modeldirty."<<(model!=nullptr? QString(model->isDirty()):"null");
                if(model!=nullptr && model->isDirty()){
                    result<<model;
                }
            }
            result<<dirtyTables(childIndex);
        }
    }
    return result;
}

void QWMDataEditor::closeEvent(QCloseEvent *event)
{

    //    bool canceled=false;
    if(isDirty()){
        int result=QMessageBox::warning(this, tr("提示"),tr("是否保存之前已经修改过的数据？"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(result==QMessageBox::Yes){
            QStringList errors;
            bool success=this->saveAll(errors);
            if(!success){
                result=QMessageBox::warning(this, tr("提示"),tr("提交错误，关闭可能导致数据丢失，是否关闭?"),QMessageBox::Yes|QMessageBox::No);
                if(result==QMessageBox::No){
                    event->ignore();
                    return;
                }
            }
        }else if(result==QMessageBox::Cancel){
            event->ignore();
            return;
        }else if(result==QMessageBox::No){
            QStringList errors;
            this->revertAll(errors);
        }
    }
    WELL->resetCache();
    QWMMain * main= static_cast<QWMMain *>(this->parent());
    main->show();
    main->setCurrentEditor(nullptr);
    event->accept();
}

bool QWMDataEditor::nodeParentInfo(const QModelIndex &index, QString &parentTable, QString &parentDes, QString &parentID, QString &lastError)
{
    parentID=QString();
    parentTable=CFG(KeyTblMain);
    parentDes=_wellDes;
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        parentTable = MDL->parentTable(tableName);
        //        MDLTable * tableInfo=UDL->tableInfo(tableName);
        if(!parentTable.isNull()  && !parentTable.isEmpty()){
            if(index.parent().isValid() && index.parent().data(CAT_ROLE)==QWMApplication::TABLE)//如果有父节点
            {
                QString pv=index.parent().data(PK_VALUE_ROLE).toString();
                QString ptn=index.parent().data().toString();
                parentDes=index.parent().data(RECORD_DES_ROLE).toString();

                if(pv.isNull()){
                    lastError=tr("未选中[%1]表的记录！").arg(ptn);
                    return false;
                }else{
                    parentID=pv;
                    return true;
                }
            }else{//如果无父节点，则认为 其父为wvWellheader
                //                parentDes=WELL->recordDes(parentTable,)
            }
        }
        else{
            //            parentID=_idWell;
        }
        return false;
    }
    return false;
}
void QWMDataEditor::revertAll(QStringList & errors){
    bool result=true;
    foreach(QWMTableModel * model,_dirtyTables){
        if(model->isDirty()){
            model->revertAll();
        }
    }
}
bool QWMDataEditor::saveAll(QStringList & errors){
    //    QHash<QWMTableModel*,int> updatedModels;
    bool result=true;
    foreach(QWMTableModel * model,_dirtyTables){
        if(model->isDirty()){
            bool success=model->submitAll();
            if(!success){
                qDebug()<<"Submit Statement:["+model->selectStatement()<<"],Error["<<model->lastError().text()<<"]";
                result=false;
                errors<<QString(tr("提交[%1] 字段修改，错误[%2]")).arg(model->tableName()).arg(model->lastError().text());
            }
        }
    }
    foreach(QUndoStack * stack,_undoStacks){
        for(int i=0;i<stack->count();i++){
            const QUndoCommand * cmd=stack->command(i);
            if(instanceof<QWMRecordEditCommand>(cmd)){
                QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
                reCommand->setSubmitted(true);
            }
        }
    }

    //    for(int i=0;i<_undoStack.count();i++){
    //        const QUndoCommand * cmd=_undoStack.command(i);

    //        if(instanceof<QWMFieldEditCommand>(cmd)){
    //            QWMFieldEditCommand* feCommand=(QWMFieldEditCommand*) cmd;
    //            QWMTableModel * model=feCommand->model();
    //            if(model->isDirty())
    //            {
    //                bool success=model->submitAll();
    //                if(!success){
    //                    qDebug()<<"Submit Statement:["+model->selectStatement()<<"],Error["<<model->lastError().text()<<"]";
    //                    result=false;
    //                    errors<<QString(tr("提交[%1] 字段修改，错误[%2]")).arg(model->tableName()).arg(model->lastError().text());
    //                }
    //            }
    //        }else if(instanceof<QWMRecordEditCommand>(cmd)){
    //            QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
    //            QWMTableModel * model=reCommand->model();
    //            if(model->isDirty())
    //            {
    //                bool success=model->submitAll();
    //                if(!success){
    //                    qDebug()<<"Submit Statement:["+model->selectStatement()<<"],Error["<<model->lastError().text()<<"]";
    //                    result=false;
    //                    errors<<QString(tr("提交[%1] 记录增删 错误[%2]")).arg(model->tableName()).arg(model->lastError().text());
    //                    continue;
    //                }
    //            }
    //            reCommand->setSubmitted(true);
    //        }
    //    }
    CHECK_UNDO_STATE;
    return result;
}

bool QWMDataEditor::isDirty()
{
    //    for(int i=0;i<_undoStack.count();i++){
    //        const QUndoCommand * cmd=_undoStack.command(i);

    //        if(instanceof<QWMFieldEditCommand>(cmd)){
    //            QWMFieldEditCommand* feCommand=(QWMFieldEditCommand*) cmd;
    //            QWMTableModel * model=feCommand->model();
    //            if(model->isDirty())
    //            {
    //                return true;
    //            }
    //        }else if(instanceof<QWMRecordEditCommand>(cmd)){
    //            QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
    //            QWMTableModel * model=reCommand->model();
    //            if(model->isDirty())
    //            {
    //                return true;
    //            }
    //        }
    //    }
    foreach(QWMTableModel  * model,_dirtyTables){
        if(model->isDirty())
        {
            return true;
        }
    }
    return false;
}

bool QWMDataEditor::isCurrentTableDirty()
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model();
    if(model!=nullptr)
        return  model->isDirty();
    return false;
}
void QWMDataEditor::on_actionSaveExit_triggered()
{
    QStringList errors;
    if(!this->saveAll(errors)){
        int result=QMessageBox::warning(this, tr("提示"),tr("提交错误，关闭可能导致数据丢失，是否关闭?"),QMessageBox::Yes|QMessageBox::No);
        if(result==QMessageBox::No){
            return ;
        }
    }
    this->close();
}



MDLTable *QWMDataEditor::nodeTableInfo(const QModelIndex &index)
{
    if(!index.isValid())
        return nullptr;
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        QString parentTableName=MDL->parentTable(tableName);
        MDLTable * tableInfo=UDL->tableInfo(tableName);
        return tableInfo;
    }
    return nullptr;
}

void QWMDataEditor::clearChildSelection(const QModelIndex &index)
{
    //    QItemSelectionModel*  selection= ui->trvTables->selectionModel();
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

void QWMDataEditor::editTable(const QModelIndex &tableNodeIndex)
{
    //1 如果当前的表和well是1：1的，且无记录，则必须生成一个新纪录
    //2 如果当前表有记录，且当前节点无选中历史，则 选中第一条记录 ，将当前记录的key保存 到treeview node的key_field中 。当前几点的RecordDes显示
    //3 如果当前表有父表，则必须父表的节点有选中记录，否则报警，如果当前的父表有选中记录，则将父表的id设置为当前表的parentid进行过滤 。
    //4 选中当前节点对应的表的其中一条记录后，子节点的所有选中记录清空
    if(tableNodeIndex.data(CAT_ROLE)==QWMApplication::TABLE){
        QString lastError;
        QString parentTable,parentTableDes;
        QString parentID;
        bool hasParent=nodeParentInfo(tableNodeIndex,parentTable,parentTableDes,parentID,lastError);
        if((parentID.isNull()) && !lastError.isEmpty()){
            QMessageBox::information(this,tr("错误"),lastError);
            return ;
        }

        QString tableName=tableNodeIndex.data(TABLE_NAME_ROLE).toString();
        QWMRotatableProxyModel * model=WELL->tableForEdit(tableName,_idWell,parentID);
        QVariant v=QVariant::fromValue(model);

        ui->trvTables->model()->setData(tableNodeIndex,v,MODEL_ROLE);
        SX(sourceModel,model);
        PX(sortableProxyModel,model);

        this->_tbvData->setModel(model);

        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(sourceModel,&QWMTableModel::submitted,nullptr,nullptr);
        connect(sourceModel,&QWMTableModel::submitted,this, &QWMDataEditor::on_model_submitted);
        disconnect(sortableProxyModel,&QWMSortFilterProxyModel::dataModified,nullptr,nullptr);
        connect(sortableProxyModel,&QWMSortFilterProxyModel::dataModified,this, &QWMDataEditor::on_model_data_modified);

        //        面包屑导航、表帮助信息显示
        MDLTable * tableInfo=nodeTableInfo(tableNodeIndex);
        QString  breadCrumb;
        if(tableName.compare(parentTable)==0){
            breadCrumb=QString("%1[%2]").arg(parentTable,parentTableDes);
        }else{
            breadCrumb=QString("%1[%2]-%3").arg(parentTable,parentTableDes,tableInfo->CaptionLongP());
        }
        ui->lblBread->setText(breadCrumb);
        ui->lblTableInfo->setText(tableInfo->Help());
        //        删除、新增按钮的状态
        if(sortableProxyModel->rowCount()>0){
            ui->actionNew->setEnabled(true);
            ui->actionDelete->setEnabled(true);
        }else{
            if(tableInfo->OneToOne()){//1:1的情况，默认选择首行
                QSqlRecord record=model->record();
                WELL->initRecord(record,_idWell,parentID);
                model->insertRecord(0,record);
            }
            ui->actionDelete->setEnabled(false);
            ui->actionNew->setEnabled(true);
        }
        if(tableInfo->OneToOne()){//1:1的情况
            ui->actionNew->setEnabled(false);
            ui->actionDelete->setEnabled(false);
        }
        //        方向按钮的状态恢复
        if(model->mode()==QWMRotatableProxyModel::H ){
            ui->actionRotate->setChecked(false);
        }else
        {
            ui->actionRotate->setChecked(true);
        }

        //恢复当前选择行、列
        QVariant  pos=tableNodeIndex.data(SELECT_ROLE);
        if(tableNodeIndex.data(SELECT_ROLE).isNull()){
            QItemSelectionModel * selection=_tbvData->selectionModel();
            if(!selection->hasSelection()){
                if(model->rowCount()*model->columnCount()>0){
                    selection->setCurrentIndex(model->firstEditableCell(),QItemSelectionModel::SelectCurrent);
                }
            }
        }else{
            QPoint pos=tableNodeIndex.data(SELECT_ROLE).toPoint();
            QItemSelectionModel * selection=_tbvData->selectionModel();
            if(model->mode()==QWMRotatableProxyModel::H){
                selection->setCurrentIndex(_tbvData->model()->index(pos.x(),pos.y()),QItemSelectionModel::SelectCurrent);
            }else{
                selection->setCurrentIndex(_tbvData->model()->index(pos.y(),pos.x()),QItemSelectionModel::SelectCurrent);
            }

        }
        //恢复当前工具栏保存按钮状态
        if(model->isDirty()){
            ui->actionSave->setEnabled(false);
        }
        if(this->isDirty()){
            ui->actionSave->setEnabled(false);
        }
        // 当前左侧树形节点的单复数变化，如果右侧的记录数>1，则显示为复数
        QStandardItemModel * tblModel= (QStandardItemModel *) ui->trvTables->model();
        QStandardItem *item= tblModel->itemFromIndex(tableNodeIndex);
        if(sortableProxyModel->rowCount()>1){
            item->setIcon(APP->icons()["files@4x"]);
        }
        else{
            item->setIcon(APP->icons()["file@4x"]);
        }
        //排序按钮的状态恢复
        ui->actionSort->setChecked(sortableProxyModel->sortOrder()==Qt::DescendingOrder);

        showRecordNav();
        if(tableName.compare(ATTACHMENT_TABLE,Qt::CaseInsensitive)==0){
            ui->actionOpen->setVisible(true);
            ui->actionSaveTo->setVisible(true);
        }else{
            ui->actionOpen->setVisible(false);
            ui->actionSaveTo->setVisible(false);
        }
        CHECK_UNDO_STATE;
        //        sortableProxyModel->sort(1, ui->actionSort->isChecked()?Qt::DescendingOrder:Qt::AscendingOrder);
    }
}

QUndoStack* QWMDataEditor::undoStack()
{
    CUR_UNDO_STACK_KEY(key);
    if(!_undoStacks.contains(key)){
        QUndoStack * aundoStack=new QUndoStack(this);
        _undoStacks.insert(key,aundoStack);
    }
    return _undoStacks[key];
}

void QWMDataEditor::addRecord(const QModelIndex &index)
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model();
    SX(sourceModel,model);
    sourceModel->setParentID(QString());
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){

        QString lastError;
        QString parentID,parentTable,parentTableDes;
        nodeParentInfo(index,parentTable,parentTableDes,parentID,lastError);
        MDLTable * tableInfo=nodeTableInfo(index);
        QString  breadCrumb=QString("%1-%2").arg(parentTableDes,tableInfo->CaptionLongP());
        ui->lblBread->setText(breadCrumb);

        sourceModel->setParentID(QString());
        if(parentID.isNull()&& !lastError.isEmpty()){
            QMessageBox::information(this,tr("错误"),lastError);
            return ;
        }
        QString tableName=index.data(TABLE_NAME_ROLE).toString();

        if(tableInfo->OneToOne()){//1:1的情况，默认选择首行
            if(sourceModel->rowCount()>0){
                return;
            }
        }
        QSqlRecord record=model->record();
        WELL->initRecord(record,_idWell,parentID);
        bool success=model->insertRecord(-1,record);

    }

}

void QWMDataEditor::removeRecord(const QModelIndex &index)
{
    if(index.isValid()){
        QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model();
        model->removeRecord(index);
    }

}

void QWMDataEditor::breadcrumbInfo(const QModelIndex &index)
{

}

void QWMDataEditor::checkUndoStacks(QWMTableModel *commitedModel)
{
    foreach(QUndoStack * stack,_undoStacks.values()){
        for(int i=0;i<stack->count();i++){
            const QUndoCommand * cmd=stack->command(i);
            if(instanceof<QWMRecordEditCommand>(cmd)){
                QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
                QWMTableModel * model=reCommand->model();
                if(commitedModel->tableName()==model->tableName()){
                    reCommand->setSubmitted(true);
                }
            }
        }
    }

}

void QWMDataEditor::parentRecordNavigate(int step)
{
    QItemSelectionModel * selection= ui->trvTables->selectionModel();
    QModelIndex  curTableNodeIndex=selection->currentIndex();
    QString  parentTable,parentDes,parentId,lastError;
    QAbstractItemModel * catModel=ui->trvTables->model();
    QWMRotatableProxyModel *  model=(QWMRotatableProxyModel*)_tbvData->model();
    PX(proxyModel,model);
    //如果有父节点，且父节点的记录数>1 ，且当前记录位置<记录数
    bool hasParent=this->nodeParentInfo(selection->currentIndex(),parentTable,parentDes,parentId,lastError);
    if(hasParent){
        QModelIndex parentTableIndex=curTableNodeIndex.parent();
        if(parentTableIndex.isValid() && parentTableIndex.data(CAT_ROLE)==QWMApplication::TABLE){
            QWMRotatableProxyModel * parentModel=parentTableIndex.data(MODEL_ROLE).value<QWMRotatableProxyModel*>();

            QPoint curPos;
            if(parentTableIndex.data(SELECT_ROLE).isNull()){
                curPos=QPoint(0,0);
            }else{
                curPos=parentTableIndex.data(SELECT_ROLE).value<QPoint>();
            }

            if(parentModel->recordCount()>1 && (curPos.x()+step) <= parentModel->recordCount() && (curPos.x()+step) >=  0){
                PX(proxyParentModel,parentModel);
                QPoint nextPos=QPoint(curPos.x()+step,curPos.y());
                QSqlRecord parentRecord=proxyParentModel->record(proxyParentModel->index(nextPos.x(),nextPos.y()));
                QString parentRecordDes=WELL->recordDes(parentModel->tableName(),parentRecord);
                MDLTable *  parentTableInfo=UDL->tableInfo(parentModel->tableName());
                PK_VALUE(pk,parentRecord);

                QString dispText=QString("%1  [%2]").arg(parentTableInfo->CaptionLongP()).arg(parentRecordDes);
                catModel->setData(parentTableIndex,nextPos,SELECT_ROLE);
                catModel->setData(parentTableIndex,parentRecordDes,RECORD_DES_ROLE);
                catModel->setData(parentTableIndex,dispText,Qt::DisplayRole);
                catModel->setData(parentTableIndex,pk,PK_VALUE_ROLE);
                editTable(curTableNodeIndex);
            }
        }
    }
    //    ui->actionactNextRec->setEnabled(false);
}

bool QWMDataEditor::saveDirtTables(QModelIndex index,QStringList & errors){
    //此处要获取已经脏了的数据库表，更新
    if(!index.isValid())
        return true;
    QList<QWMRotatableProxyModel *> tablesDirt=dirtyTables(index);
    bool result=true;
    if(!tablesDirt.isEmpty()){

        int result=QMessageBox::warning(this, tr("提示"),tr("是否保存之前已经修改过的数据？"),QMessageBox::Yes|QMessageBox::No);
        if(result==QMessageBox::Yes){
            foreach(QWMRotatableProxyModel* table,tablesDirt){
                bool success=table->submitAll();
                if(!success){
                    result=false;
                    SX(sourceTable,table);
                    PX(proxyTable,table);
                    if(table->lastError().isValid()){
                        errors<<tr("表[")<<sourceTable->tableName()<<"] 提交异常["<<table->lastError().text()<<",IDWELL["<<_idWell<<",PARENTID["<<proxyTable->filterRegExp().pattern()<<"]";
                    }else
                    {
                        errors<<tr("表[")<<sourceTable->tableName()<<"] 提交异常,IDWELL["<<_idWell<<",PARENTID["<<proxyTable->filterRegExp().pattern()<<"]";
                    }
                }else{
                    SX(sourceTable,table);
                    checkUndoStacks(sourceTable);
                }
            }
        }
    }
    return result;
}
void QWMDataEditor::on_current_record_changed(const QModelIndex &current, const QModelIndex &previous){
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>(_tbvData->model());
    SX(sourceModel,model);

    if((model->mode()==QWMRotatableProxyModel::H && current.row()!=previous.row()  && current.row()>=0 && model->rowCount()>0) ||
            (model->mode()==QWMRotatableProxyModel::V && current.column()!=previous.column() && current.column()>=0 && model->columnCount()>0)
            ){
        if(previous.isValid()){
            QStringList errors;
            bool result=
                    saveDirtTables(ui->trvTables->currentIndex(),errors);
            clearChildSelection(ui->trvTables->currentIndex());
        }
        QSqlRecord record=model->record(current);
        PK_VALUE(pk,record);
        QString tableName=sourceModel->tableName();
        MDLTable * tableInfo=UDL->tableInfo(tableName);
        QString des=WELL->recordDes(tableName,record);

        QString lastError;
        QString parentID,parentTable,parentTableDes;
        nodeParentInfo(current,parentTable,parentTableDes,parentID,lastError);
        if(!des.isNull()&& !des.isEmpty())
        {
            QString breadCrumb;
            if(parentTable.compare(tableName)==0){
                breadCrumb=QString("%1[%2]").arg(parentTable).arg(parentTableDes);
            }else{
                breadCrumb=QString("%1[%2]-%3[%4]").arg(parentTable).arg(parentTableDes).arg(tableInfo->CaptionLongP()).arg(des);
            }
            ui->lblBread->setText(breadCrumb);
        }

        QString dispText=tableInfo->CaptionLongP();
        if(!des.isNull()&& !des.isEmpty())
        {
            dispText=QString("%1  [%2]").arg(tableInfo->CaptionLongP()).arg(des);
        }
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),dispText,Qt::DisplayRole);
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),pk,PK_VALUE_ROLE);
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),des,RECORD_DES_ROLE);
        //        clearChildSelection(ui->trvTables->currentIndex());
    }else if((model->mode()==QWMRotatableProxyModel::H && current.column()!=previous.column()  && current.row()>=0 && model->rowCount()>0) ||
             (model->mode()==QWMRotatableProxyModel::V && current.row()!=previous.row() && current.column()>=0 && model->columnCount()>0)
             ){
        QString field=model->fieldName(current);
        MDLField * fieldInfo=UDL->fieldInfo(model->tableName(),field);
        ui->lblFieldInfo->setText(fieldInfo->Help());
    }
    if(ui->trvTables->currentIndex().isValid()){
        QPoint select ;
        if(model->mode()==QWMRotatableProxyModel::H){
            select=QPoint(current.row(),current.column());
        }
        else
        {
            select=QPoint(current.column(),current.row());
        }
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),select,SELECT_ROLE);
    }

}
void QWMDataEditor::on_trv_table_node_clicked(const QModelIndex &index)
{
    editTable(index);
}

void QWMDataEditor::on_actionRotate_triggered(bool checked)
{
    QWMRotatableProxyModel * model=static_cast<QWMRotatableProxyModel*>( _tbvData->model());
    //    model->beginResetModel();
    if(checked)
    {
        model->setMode(QWMRotatableProxyModel::V);
    }
    else{
        model->setMode(QWMRotatableProxyModel::H);
    }
    //    model->endResetModel();
    //    showDataGrid(model);
}

void QWMDataEditor::on_actionNew_triggered()
{
    QItemSelectionModel * selectionModel= ui->trvTables->selectionModel();
    if(selectionModel->currentIndex().isValid()){
        QModelIndex index=selectionModel->currentIndex();
        addRecord(index);
    }
}

void QWMDataEditor::on_actionDelete_triggered()
{
    QItemSelectionModel * selectionModel=_tbvData->selectionModel();
    if(!selectionModel->selectedIndexes().isEmpty()){
        QModelIndex index=selectionModel->selectedIndexes().first();
        removeRecord(index);
    }
}


void QWMDataEditor::on_data_record_changed(int /*oldCount*/, int newCount)
{
    MDLTable * tableInfo=nodeTableInfo(ui->trvTables->currentIndex());
    if(tableInfo->OneToOne()){
        if(newCount>0){
            ui->actionNew->setEnabled(false);
        }else
        {
            ui->actionNew->setEnabled(true);
        }
        ui->actionDelete->setEnabled(false);
    }else{
        if(newCount<=0){
            ui->actionDelete->setEnabled(false);
            ui->actionNew->setEnabled(true);
        }else{
            ui->actionDelete->setEnabled(true);
            ui->actionNew->setEnabled(true);
        }
    }

}

void QWMDataEditor::on_actionSave_triggered()
{
    QWMRotatableProxyModel * model=qobject_cast<QWMRotatableProxyModel*>( _tbvData->model());
    bool success=model->submitAll();
    if(!success){
        QString msg="";
        if(model->lastError().isValid()){
            msg=model->lastError().text();
        }
        QMessageBox::critical(this,tr("错误"),tr("提交错误[%1]").arg(msg));
    }else{
        SX(sourceModel,model);
        this->checkUndoStacks(sourceModel);
    }
    CHECK_UNDO_STATE
}

void QWMDataEditor::on_actionExit_triggered()
{
    QCloseEvent *event=new QCloseEvent();
    QApplication::postEvent(this,event);
}

void QWMDataEditor::on_actionSaveAll_triggered()
{
    QStringList errors;
    bool success=this->saveAll(errors);
    if(!success){
        QString msg=errors.join("\n");
        QMessageBox::critical(this,tr("错误"),tr("提交错误[%1]").arg(msg));
    }
}

void QWMDataEditor::on_actionSort_triggered(bool checked)
{
    QWMRotatableProxyModel * model=qobject_cast<QWMRotatableProxyModel*>( _tbvData->model());
    PX(sortableModel,model);
    sortableModel->sort(1,checked?Qt::DescendingOrder:Qt::AscendingOrder);
}

//void QWMDataEditor::before_update_record(int /*row*/, QSqlRecord &record)
//{
//     int sysMDIndex=record.indexOf(CFG(SysMD));
//     if(sysMDIndex>=0){
//         record.setValue(sysMDIndex,QDateTime::currentDateTime());
//         record.setGenerated(sysMDIndex,true);
//     }
//}

void QWMDataEditor::on_actionCalc_triggered()
{
    QWMRotatableProxyModel * model=qobject_cast<QWMRotatableProxyModel*>( _tbvData->model());
    //    PX(sortableModel,model);
    //    SX(sourceModel,model);
    model->calc();
}

void QWMDataEditor::on_actionactNextRec_triggered()
{
    this->parentRecordNavigate(1);
}

void QWMDataEditor::on_actionactPrevRec_triggered()
{
    this->parentRecordNavigate(-1);
}

void QWMDataEditor::showRecordNav()
{
    QItemSelectionModel * selection= ui->trvTables->selectionModel();
    QModelIndex tableNodeIndex=selection->currentIndex();
    QString  parentTable,parentDes,parentId,lastError;
    //    QAbstractItemModel * catModel=ui->trvTables->model();
    //    QWMRotatableProxyModel *  model=(QWMRotatableProxyModel*)_tbvData->model();
    //    PX(proxyModel,model);
    //如果有父节点，且父节点的记录数>1 ，且当前记录位置<记录数
    bool hasParent=this->nodeParentInfo(selection->currentIndex(),parentTable,parentDes,parentId,lastError);
    if(hasParent){
        QModelIndex parentTableNodeIndex=tableNodeIndex.parent();
        if(parentTableNodeIndex.isValid() && parentTableNodeIndex.data(CAT_ROLE)==QWMApplication::TABLE){
            QWMRotatableProxyModel * parentModel=parentTableNodeIndex.data(MODEL_ROLE).value<QWMRotatableProxyModel*>();
            QPoint curPos;
            if(parentTableNodeIndex.data(SELECT_ROLE).isNull()){
                curPos=QPoint(0,0);
            }else{
                curPos=parentTableNodeIndex.data(SELECT_ROLE).value<QPoint>();
            }
            int prc=parentModel->recordCount();
            if(prc > 1){
                MDLTable *  parentTableInfo=UDL->tableInfo(parentModel->tableName());

                ui->actionParent->setText(parentTableInfo->CaptionLongP());

                foreach(QAction * act ,_navActions){
                    act->setVisible(true);
                }

                if(curPos.x() < prc-1){
                    ui->actionactNextRec->setEnabled(true);
                }else{
                    ui->actionactNextRec->setEnabled(false);
                }
                if(curPos.x() > 0){
                    ui->actionactPrevRec->setEnabled(true);
                }else{
                    ui->actionactPrevRec->setEnabled(false);
                }
                return;
            }
        }
    }
    foreach(QAction * act ,_navActions){
        act->setVisible(false);
    }
}

void QWMDataEditor::on_actionOpen_triggered()
{
    QWMRotatableProxyModel * model=qobject_cast<QWMRotatableProxyModel*>( _tbvData->model());
    QModelIndex index=_tbvData->currentIndex();
    if(!index.isValid()){
        return ;
    }
    QString curPath=QDir::homePath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle=tr("选择文件位置"); //对话框标题
    QString filter=tr("all files(*.*)"); //文件过滤器
    QString strFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (!strFileName.isEmpty()){
        QFileInfo fileInfo(strFileName);
        QByteArray ba;
        QFile file(strFileName);
        if (!file.open(QIODevice::ReadOnly))
            return;
        QByteArray blob = file.readAll();
        file.close();
        QList<QPair<QString,QVariant>> spv;
        spv.append(QPair<QString,QVariant>("Des",fileInfo.baseName()));
        spv.append(QPair<QString,QVariant>("AttachURL",fileInfo.filePath()));
        spv.append(QPair<QString,QVariant>("AttachExtension",fileInfo.suffix()));
        spv.append(QPair<QString,QVariant>("TblKeyParent",CFG(KeyTblMain)));
        spv.append(QPair<QString,QVariant>("AttachBLOB",blob));
        model->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
    }
}

void QWMDataEditor::on_actionSaveTo_triggered()
{
    QWMRotatableProxyModel * model=qobject_cast<QWMRotatableProxyModel*>( _tbvData->model());
    QModelIndex index=_tbvData->currentIndex();
    if(!index.isValid()){
        return ;
    }
    QSqlRecord record=model->record(index);
    if(record.isEmpty()){
        return;
    }
    QString curPath=QDir::homePath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle=tr("选择文件保存位置"); //对话框标题
    QString filter=tr("all files(*.*)"); //文件过滤器
    QString strFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (!strFileName.isEmpty()){
        QFileInfo fileInfo(strFileName);
        QString fn=QString("%1%2%3.%4").arg(fileInfo.dir().path()).arg(QDir::separator()).arg(fileInfo.baseName()).arg(record.value("AttachExtension").toString());
        QFileInfo fileInfoSave(fn);

        QByteArray ba=record.value("AttachBLOB").toByteArray();
        QFile file(fileInfoSave.absoluteFilePath());
        if (!file.open(QIODevice::WriteOnly))
            return;
        long writed=file.write(ba);
        file.close();
    }
}
