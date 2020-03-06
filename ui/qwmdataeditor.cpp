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
#include "qwmfieldeditcommand.h"
#include "qwmrecordeditcommand.h"
#include "qwmsortfilterproxymodel.h"
#include <QDebug>
#include <QSqlField>
#define CHECK_UNDO_STATE \
    ui->actionRedo->setEnabled(_undoStack.canRedo());\
    ui->actionUndo->setEnabled(_undoStack.canUndo());\
    ui->actionSave->setEnabled(isCurrentTableDirty());\
    ui->actionSaveAll->setEnabled(isDirty());

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

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
    connect(_tbvData,&QWMDataTableView::RecordCountChanged,this,&QWMDataEditor::on_data_record_changed);
    CHECK_UNDO_STATE
}

QWMDataEditor::~QWMDataEditor()
{
    delete ui;
}

void QWMDataEditor::undo()
{
    _undoStack.undo();
    CHECK_UNDO_STATE;

}

void QWMDataEditor::addUndoCommand(QUndoCommand *command)
{
    _undoStack.push(command);
    CHECK_UNDO_STATE;

}

void QWMDataEditor::redo()
{
    _undoStack.redo();
    CHECK_UNDO_STATE;

}

void QWMDataEditor::loadDataTree()
{
    QSqlRecord wellRec=WELL->well(_idWell);
    QString wellTitle=WELL->recordDes(CFG(KeyTblMain),wellRec);
    this->setWindowTitle(wellTitle);
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

    bool canceled=false;
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
        }
    }
    QWMMain * main= static_cast<QWMMain *>(this->parent());
    main->show();
    main->setCurrentEditor(nullptr);
    event->accept();
}

bool QWMDataEditor::saveAll(QStringList & errors){
    //    QHash<QWMTableModel*,int> updatedModels;
    bool result=true;
    for(int i=0;i<_undoStack.count();i++){
        const QUndoCommand * cmd=_undoStack.command(i);

        if(instanceof<QWMFieldEditCommand>(cmd)){
            QWMFieldEditCommand* feCommand=(QWMFieldEditCommand*) cmd;
            QWMTableModel * model=feCommand->model();
            if(model->isDirty())
            {
                bool success=model->submitAll();
                if(!success){
                    qDebug()<<"Submit Statement:["+model->selectStatement()<<"],Error["<<model->lastError().text()<<"]";
                    result=false;
                    errors<<QString(tr("提交[%1] 字段修改，错误[%2]")).arg(model->tableName()).arg(model->lastError().text());
                }
            }
        }else if(instanceof<QWMRecordEditCommand>(cmd)){
            QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
            QWMTableModel * model=reCommand->model();
            if(model->isDirty())
            {
                bool success=model->submitAll();
                if(!success){
                    qDebug()<<"Submit Statement:["+model->selectStatement()<<"],Error["<<model->lastError().text()<<"]";
                    result=false;
                    errors<<QString(tr("提交[%1] 记录增删 错误[%2]")).arg(model->tableName()).arg(model->lastError().text());
                    continue;
                }
            }
            reCommand->setSubmitted(true);
        }
    }
    CHECK_UNDO_STATE
            return result;
}

bool QWMDataEditor::isDirty()
{
    for(int i=0;i<_undoStack.count();i++){
        const QUndoCommand * cmd=_undoStack.command(i);

        if(instanceof<QWMFieldEditCommand>(cmd)){
            QWMFieldEditCommand* feCommand=(QWMFieldEditCommand*) cmd;
            QWMTableModel * model=feCommand->model();
            if(model->isDirty())
            {
                return true;
            }
        }else if(instanceof<QWMRecordEditCommand>(cmd)){
            QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
            QWMTableModel * model=reCommand->model();
            if(model->isDirty())
            {
                return true;
            }
        }
    }
    return false;
}

bool QWMDataEditor::isCurrentTableDirty()
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model();
    return  model->isDirty();
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
                    parentID=pv;
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
    if(!index.isValid())
        return nullptr;
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

void QWMDataEditor::editTable(const QModelIndex &tableNodeIndex)
{
    //1 如果当前的表和well是1：1的，且无记录，则必须生成一个新纪录
    //2 如果当前表有记录，且当前节点无选中历史，则 选中第一条记录 ，将当前记录的key保存 到treeview node的key_field中 。当前几点的RecordDes显示
    //3 如果当前表有父表，则必须父表的节点有选中记录，否则报警，如果当前的父表有选中记录，则将父表的id设置为当前表的parentid进行过滤 。
    //4 选中当前节点对应的表的其中一条记录后，子节点的所有选中记录清空
    if(tableNodeIndex.data(CAT_ROLE)==QWMApplication::TABLE){
        QString lastError;
        QString parentID=nodeParentID(tableNodeIndex,lastError);
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

        if(sourceModel->tableName()=="wvJobReport"){
            //qDebug()<<"error"<<proxyModel->filterRegExp();
            QModelIndex index =sortableProxyModel->index(0,0,QModelIndex());
        }


        this->_tbvData->setModel(model);

        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentRowChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentColumnChanged,this, &QWMDataEditor::on_current_record_changed);
        disconnect(_tbvData->selectionModel(),&QItemSelectionModel::currentChanged,nullptr,nullptr);
        connect(_tbvData->selectionModel(),&QItemSelectionModel::currentChanged,this, &QWMDataEditor::on_current_record_changed);

        int sourceCount=sourceModel->rowCount();
        int proxyCount=sortableProxyModel->rowCount();
        //        qDebug()<<"Table:"<<sourceModel->tableName()<<",Filter:"<<parentID<<",SourceCNT:"<<sourceCount<<",ProxyCNT:"<<proxyCount;
        MDLTable * tableInfo=nodeTableInfo(tableNodeIndex);
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
        if(model->mode()==QWMRotatableProxyModel::H ){
            ui->actionRotate->setChecked(false);
        }else
        {
            ui->actionRotate->setChecked(true);
        }
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
            selection->setCurrentIndex(_tbvData->model()->index(pos.x(),pos.y()),QItemSelectionModel::SelectCurrent);
        }
        if(!sourceModel->isSignalConnected(QMetaMethod::fromSignal(&QWMTableModel::primeInsert))){
            connect(sourceModel,&QWMTableModel::primeInsert,this,&QWMDataEditor::init_record_on_prime_insert);
        }
        if(!sourceModel->isSignalConnected(QMetaMethod::fromSignal(&QWMTableModel::beforeUpdate))){
            connect(sourceModel,&QWMTableModel::beforeUpdate,this,&QWMDataEditor::before_update_record);
        }

        if(model->isDirty()){
            ui->actionSave->setEnabled(false);
        }
        if(this->isDirty()){
            ui->actionSave->setEnabled(false);
        }

        sortableProxyModel->sort(1, ui->actionSort->isChecked()?Qt::DescendingOrder:Qt::AscendingOrder);
    }
}

QUndoStack& QWMDataEditor::undoStack()
{
    return _undoStack;
}

void QWMDataEditor::addRecord(const QModelIndex &index)
{
    _parentID=QString();
    if(index.data(CAT_ROLE)==QWMApplication::TABLE){
        QString lastError;
        QString parentID=nodeParentID(index,lastError);
        _parentID=parentID;
        if(parentID.isNull()&& !lastError.isEmpty()){
            QMessageBox::information(this,tr("错误"),lastError);
            return ;
        }

        QString tableName=index.data(TABLE_NAME_ROLE).toString();
        QWMRotatableProxyModel * model=(QWMRotatableProxyModel*)_tbvData->model();
        SX(sourceModel,model);
        //        PX(proxyModel,model);

        MDLTable * tableInfo=nodeTableInfo(index);
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

void QWMDataEditor::checkUndoStacks(QWMTableModel *commitedModel)
{
    for(int i=0;i<_undoStack.count();i++){
        const QUndoCommand * cmd=_undoStack.command(i);

        if(instanceof<QWMRecordEditCommand>(cmd)){
            QWMRecordEditCommand* reCommand=(QWMRecordEditCommand*) cmd;
            QWMTableModel * model=reCommand->model();
            if(commitedModel->tableName()==model->tableName()){
                reCommand->setSubmitted(true);
            }
        }
    }

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
        MDLTable * tableInfo=MDL->tableInfo(tableName);
        QString des=WELL->recordDes(tableName,record);
        QString dispText=tableInfo->CaptionLongP();
        if(!des.isNull()&& !des.isEmpty())
        {
            dispText=QString("%1  [%2]").arg(tableInfo->CaptionLongP()).arg(des);
        }
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),dispText,Qt::DisplayRole);
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),pk,PK_VALUE_ROLE);
        //        clearChildSelection(ui->trvTables->currentIndex());
    }
    if(ui->trvTables->currentIndex().isValid()){
        ui->trvTables->model()->setData(ui->trvTables->currentIndex(),QPoint(current.row(),current.column()),SELECT_ROLE);
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

void QWMDataEditor::init_record_on_prime_insert(int row, QSqlRecord &record)
{
    if(record.indexOf(CFG(ParentID))>=0){
        record.setValue(record.indexOf(CFG(ParentID)),_parentID);
    }
    if(record.indexOf(CFG(IDWell))>=0){
        record.setValue(record.indexOf((CFG(IDWell))),_idWell);
    }
}

void QWMDataEditor::on_data_record_changed(int oldCount, int newCount)
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

void QWMDataEditor::before_update_record(int row, QSqlRecord &record)
{
     int sysMDIndex=record.indexOf(CFG(SysMD));
     if(sysMDIndex>=0){
         record.setValue(sysMDIndex,QDateTime::currentDateTime());
         record.setGenerated(sysMDIndex,true);
     }
}
