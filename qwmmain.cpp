#include <QFile>
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QStyleFactory>
#include "qwmmain.h"
#include "ui_qwmmain.h"
#include "common.h"
#include "qwmapplication.h"
#include "mdldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlField"
#include <QDebug>
#include <QStandardItem>
#include <QUuid>
#include <QRegExp>
#include <QLabel>
#include <QAction>
#include "udldao.h"
#include "qdlgwellfieldsselector.h"
#include "qwmdataeditor.h"
#include "QSqlTableModel"
#include "qwmtablemodel.h"
#include "qwmsortfilterproxymodel.h"
//#include "qwmrotatableproxymodel.h"
#include "QTableView"
#include "QUuid"
#include <QEvent>
#include <QKeyEvent>
#include "qwmabout.h"
#include "qwmdatetimeeditor.h"
#include "qwmiconselector.h"
#include "qwmlibtabselector.h"
QWMMain::QWMMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QWMMain)
{
    ui->setupUi(this);
}

QWMMain::~QWMMain()
{
    delete ui;
}

void QWMMain::loadAllWells()
{
    QAbstractItemModel * model= WELL->wells(QWMApplication::ALL);
    ui->tbvWells->setModel(model);
    SA(tableModel,model);
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(tableModel,&QWMTableModel::beforeInsert,this,&QWMMain::before_insert);
    showWellGrid(static_cast<QWMSortFilterProxyModel*>(model));
}

void QWMMain::loadFavoriateWells()
{
    QAbstractItemModel * model= WELL->wells(QWMApplication::FAVORITE);
    ui->tbvWells->setModel(model);
    showWellGrid(static_cast<QWMSortFilterProxyModel*>(model));
}

void QWMMain::loadRecentWells()
{
    QAbstractItemModel * model= WELL->wells(QWMApplication::RECENT);
    ui->tbvWells->setModel(model);
    showWellGrid(static_cast<QWMSortFilterProxyModel*>(model));
}



void QWMMain::on_actionChangeDB_triggered()
{
    QString error;
    APP->selectWellDB(error,this);
    this->setWindowTitle(APP->applicationName()+" "+APP->well().databaseName());
}

void QWMMain::on_trvCatalogs_clicked(const QModelIndex &index)
{
    switch(index.data(CAT_ROLE).toInt()){
    case QWMApplication::ALL:
        loadAllWells();
        break;
    case QWMApplication::FAVORITE:
        loadFavoriateWells();
        break;
    case  QWMApplication::RECENT:
        loadRecentWells();
        break;
    case  QWMApplication::QUERY:
        break;
    case QWMApplication::QUICK_QUERY:
        break;
    }

}

void QWMMain::on_actionFavorite_triggered()
{

    //    QSqlQueryModel* model= (QSqlQueryModel *)ui->tbvWells->model();
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel*>(ui->tbvWells->model());
    QItemSelectionModel * selection=ui->tbvWells->selectionModel();
    switch(ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt()){
    case QWMApplication::ALL:
    case QWMApplication::RECENT:
        foreach(QModelIndex index ,selection->selectedRows()){
            QSqlRecord rec=model->record(index);
            int fieldIndex=rec.indexOf(CFG(IDMainFieldName));
            QString idwell=rec.value(fieldIndex).toString();
            int affected=WELL->addFavoriteWell(idwell);
            qDebug()<<"insert:"<<affected;
        }
        break;
    case QWMApplication::FAVORITE:
        foreach(QModelIndex index ,selection->selectedRows()){
            QSqlRecord rec=model->record(index);
            int fieldIndex=rec.indexOf(CFG(IDMainFieldName));
            QString idwell=rec.value(fieldIndex).toString();
            int affected=WELL->removeFavoriteWell(idwell);
            ui->tbvWells->hideRow(index.row());
            emit model->dataChanged(model->index(index.row(),0),model->index(index.row(),index.column()-1));
        }
        break;
    }
    return;
}

void QWMMain::resizeEvent(QResizeEvent */*event*/)
{
    //列宽随窗口大小改变而改变，每列平均分配，充满整个表，但是此时列宽不能拖动进行改变
    ui->tbvWells->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void QWMMain::showStatus(QString status)
{
    this->_lblStatus->setText(status);
}

void QWMMain::showProfile(QString profile)
{
    this->_lblProfile->setText(profile);
}

void QWMMain::showUnitSetting(QString unit)
{
    this->_lblUnit->setText(unit);
}

void QWMMain::showReferenceDatum(QString datum)
{
    this->_lblReferenceDatum->setText(datum);
}

QWMDataEditor *QWMMain::currentEditor() const
{
    return _currentEditor;
}

void QWMMain::showSplash()
{
    _splash=new QWMAbout(this);
    _splash->setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    _splash->size(),
                    APP->desktop()->availableGeometry()
                    ));
    _splash->show();
    init();
}

void QWMMain::hideSplash()
{
    _splash->hide();
//    _splash->deleteLater();
}

void QWMMain::init()
{
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,4);
    QFont font=ui->tbvWells->font();
    font.setPixelSize(10);
    ui->tbvWells->setFont(font);
    ui->tbvWells->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbvWells->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->trvCatalogs->setStyleSheet(APP->style());
    ui->tbvWells->setStyleSheet(APP->style());
    ui->tbvWells->verticalHeader()->setDefaultSectionSize(12);
    ui->trvCatalogs->setFont(font);
    // create objects for the label and progress bar
    _lblStatus = new QLabel(this);
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


    if(APP->initMDLDB()&&APP->initUDLDB()&&APP->initLIBDB()&&APP->initWellDB()&&APP->initEDLDB()){
        MDL->readConfig(APP->config());
        APP->loadPreference();
        this->setWindowTitle(APP->applicationName()+" "+APP->well().databaseName());
        qDebug()<<"\n************************"
               <<"\nUDL:"<<APP->udl().databaseName()
              <<"\nMDL:"<<APP->mdl().databaseName()
             <<"\nLIB:" <<APP->lib().databaseName()
            <<"\nEDL:"<<APP->edl().databaseName()
           <<"\nwell:"<<APP->well().databaseName()
          <<"\n************************";

        //        ui->trvCatalogs->model()->removeRows(0,ui->trvCatalogs->model()->rowCount());
        ui->trvCatalogs->setIconSize(QSize(16,16));
        ui->trvCatalogs->setStyle(QStyleFactory::create("windows"));
        _catalogModel = new QStandardItemModel(ui->trvCatalogs);
        //        QIcon icon=APP->icons()["files@1x"];
        _qsiRecentOpenWell= new QStandardItem(APP->icons()["folder"], tr("最近打开"));
        _qsiRecentOpenWell->setData(QWMApplication::RECENT,CAT_ROLE);
        _catalogModel->appendRow(_qsiRecentOpenWell);

        _qsiFavoriate = new QStandardItem(APP->icons()["folder"], tr("收藏"));
        _qsiFavoriate->setData(QWMApplication::FAVORITE,CAT_ROLE);
        _catalogModel->appendRow(_qsiFavoriate);

        _qsiAllWells = new QStandardItem(APP->icons()["folder"], tr("全部"));
        _qsiAllWells->setData(QWMApplication::ALL,CAT_ROLE);
        _catalogModel->appendRow(_qsiAllWells);

        _qsiQuery = new QStandardItem(APP->icons()["query"], tr("查询"));
        _qsiQuery->setData(QWMApplication::QUERY,CAT_ROLE);
        _catalogModel->appendRow(_qsiQuery);

        _qsiQuickQuery = new QStandardItem(APP->icons()["query"], tr("快速查询"));
        _qsiQuickQuery->setData(QWMApplication::QUICK_QUERY,CAT_ROLE);
        _qsiQuery->appendRow(_qsiQuickQuery);

        ui->trvCatalogs->setModel(_catalogModel);


        QStringList  groups=MDLDao::instance()->tableGroup();

        //        foreach(QString group ,groups){
        //            //            qDebug()<<"Group --:"<<group;
        //            QList<MDLTable*> tables=MDLDao::instance()->tablesOfGroup(group);

        //        }
        ui->menuUnit->clear();
        QStringList units=MDL->units();
        QActionGroup *unitsGroup = new QActionGroup(this);
        unitsGroup->setExclusive(true);
        foreach(QString unit,units){
            QAction* act=new QAction(unit,unitsGroup);
            act->setCheckable(true);
            act->setChecked(unit==APP->unit());
            act->setData(unit);
            ui->menuUnit->addAction(act);
            connect(act,&QAction::triggered,this,&QWMMain::on_actionUnit_triggered);
        }
        ui->menuUser_Profile->clear();
        QStringList profiles=UDL->profiles();
        QActionGroup *profileGroup = new QActionGroup(this);
        profileGroup->setExclusive(true);
        foreach(QString profile,profiles){
            QAction* act=new QAction(profile,profileGroup);
            act->setCheckable(true);
            act->setChecked(profile==APP->profile());
            act->setData(profile);
            ui->menuUser_Profile->addAction(act);
            connect(act,&QAction::triggered,this,&QWMMain::on_actionProfile_triggered);
        }
        ui->menuReference_Datum->clear();
        QStringList datumPrefs=MDL->datumPref();
        QActionGroup *datumGroup = new QActionGroup(this);
        datumGroup->setExclusive(true);
        foreach(QString datumPref,datumPrefs){
            QAction* act=new QAction(datumPref,datumGroup);
            act->setCheckable(true);
            act->setChecked(datumPref== APP->referenceDatumName( APP->datumPreference()));
            ui->menuReference_Datum->addAction(act);
            act->setData(APP->referenceDatumValue(datumPref));
            connect(act,&QAction::triggered,this,&QWMMain::on_actionDatumPref_triggered);
        }
        showUnitSetting(APP->unit());
        showProfile(APP->profile());
        showReferenceDatum(APP->referenceDatumName( APP->datumPreference()));
        this->setWindowTitle(APP->applicationName()+" "+APP->well().databaseName());
        QItemSelectionModel * selection=this->ui->trvCatalogs->selectionModel();
        selection->setCurrentIndex(_qsiAllWells->index(),QItemSelectionModel::SelectCurrent);
        //        selection->select(_qsiAllWells->index(),QItemSelectionModel::SelectCurrent);
        this->loadAllWells();
        ui->tbvWells->installEventFilter(this);
    }

    initEditors();
    hideSplash();
    this->showMaximized();

}

void QWMMain::initEditors()
{
    QString key=QWMDateTimeEditor::staticMetaObject.className();
    QWMDateTimeEditor *timeSelector = new QWMDateTimeEditor();
    EI(key,timeSelector);

    key=QWMIconSelector::staticMetaObject.className();
    QWMIconSelector *icoSelector = new QWMIconSelector( );
    EI(key,icoSelector);

    key=QString("%1.wvCasComp.libCasComp").arg(QWMLibTabSelector::staticMetaObject.className());
    QWMLibTabSelector * lts=new QWMLibTabSelector("wvCasComp","libCasComp");
    EI(key,lts);

}

void QWMMain::setCurrentEditor(QWMDataEditor * v)
{
    this->_currentEditor=v;
}

void QWMMain::editWell(QString idWell)
{
    QSqlRecord  rec= WELL->well(idWell);

    int fieldIndex=rec.indexOf(CFG(IDMainFieldName));
    QString idwell=rec.value(fieldIndex).toString();
    QString wellName=RS(rec,WellName);

    QWMDataEditor * editor=new QWMDataEditor(idwell,wellName,this);
    this->_currentEditor=editor;
    _currentEditor->init();
    editor->showMaximized();
    this->hide();
    WELL->addRecentWell(idwell);
}

void QWMMain::showWellGrid(QWMSortFilterProxyModel * model)
{
    SA(smodel,model);
    smodel->setVisibleFields(APP->wellDisplayList());
    for(int j=0;j<model->columnCount();j++){
        if(j<model->visibleFieldsCount()){
            ui->tbvWells->setColumnHidden(j,false);
        }else
        {
            ui->tbvWells->setColumnHidden(j,true);
        }
    }
    for(int i=0;i<model->rowCount();i++){
        ui->tbvWells->setRowHidden(i,false);
    }
    //        ui->tbvWells->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->resize(this->size()+QSize(1,1));
}

bool QWMMain::eventFilter(QObject *watched, QEvent *event)
{
    if(watched->objectName() =="tbvWells")
    {
        if(event->type()==QEvent::KeyPress ){
            QKeyEvent *keyEvent=static_cast<QKeyEvent *>(event);
            if(keyEvent->key()==Qt::Key_Return){
                this->on_actionEdit_triggered();
            }
        }
    }
}

void QWMMain::on_tbvWells_entered(const QModelIndex &/*index*/)
{
    //设置列宽可拖动
    ui->tbvWells->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

}

void QWMMain::on_actionUnFavorite_triggered()
{
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel*>(ui->tbvWells->model());

    QItemSelectionModel * selection=ui->tbvWells->selectionModel();

    switch(ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt()){

    case QWMApplication::FAVORITE:
        foreach(QModelIndex index ,selection->selectedRows()){
            QSqlRecord rec=model->record(index);
            int fieldIndex=rec.indexOf(CFG(IDMainFieldName));
            QString idwell=rec.value(fieldIndex).toString();
            int affected=WELL->removeFavoriteWell(idwell);
            ui->tbvWells->hideRow(index.row());
            emit model->dataChanged(model->index(index.row(),0),model->index(index.row(),index.column()-1));
        }
        break;
    }
    return;
}

void QWMMain::on_actionUnit_triggered()
{
    QAction *  act=static_cast<QAction*>( sender());
    QString unit=act->data().toString();
    APP->setUnit(unit);

}

void QWMMain::on_actionProfile_triggered()
{
    QAction *  act=static_cast<QAction*>( sender());
    QString profile=act->data().toString();
    APP->setProfile(profile);
}

void QWMMain::on_actionDatumPref_triggered()
{
    QAction *  act=static_cast<QAction*>( sender());
    QString datumPref=act->data().toString();
    APP->setDatumPref(datumPref);
}

void QWMMain::on_actionListColumn_triggered()
{
    QDlgWellFieldsSelector *  fieldSelector=new QDlgWellFieldsSelector(APP->wellDisplayList(),this);
    int result=fieldSelector->exec();
    if(result==QDialog::Accepted){
        APP->setWellDisplayList(fieldSelector->selectedFields());
        this->on_well_view_header_fields_change();
    }

}

void QWMMain::on_well_view_header_fields_change()
{
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel*>(ui->tbvWells->model());
    SA(sourceModel,ui->tbvWells->model());
    model->beginResetModel();
    sourceModel->setVisibleFields(APP->wellDisplayList());
    model->endResetModel();
    showWellGrid(model);
    this->resize(this->size()+QSize(1,1));
}

void QWMMain::on_actionEdit_triggered()
{
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel*>(ui->tbvWells->model());
    QItemSelectionModel * selection=ui->tbvWells->selectionModel();
    QModelIndex index=selection->selectedRows().first();
    if(index.isValid()){
        QSqlRecord rec=model->record(index);
        int fieldIndex=rec.indexOf(CFG(IDMainFieldName));
        QString idwell=rec.value(fieldIndex).toString();
        editWell(idwell);
    }
}

void QWMMain::on_actionNew_triggered()
{

    //    QItemSelectionModel * selection=ui->tbvWells->selectionModel();

    //    int role=ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt();
    switch(ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt()){

    case QWMApplication::ALL:
        QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel*>(ui->tbvWells->model());
        //        PA(proxyModel,ui->tbvWells->model());
        //        SA(tableModel,ui->tbvWells->model());
        QSqlRecord record=model->record();
        WELL->initRecord(record);
        DPRING_RECORD(record,"A");
        bool success=model->insertRecordDirect(0,record);
        if(!success)
            QMessageBox::information(this,"插入井数据错误",model->lastError().text());

        QString idWell=record.value(CFG(IDWell)).toString();

        DPRING_RECORD(record,"B");
        success=model->submitAll();
        if(!success)
            QMessageBox::information(this,"保存井数据错误",model->lastError().text());

        if(model->lastError().isValid()){
            qDebug()<<" Submit all:"<<model->lastError().text();
        }
        DPRING_RECORD(record,"C");
        if(!idWell.isNull()&&!idWell.isEmpty()){
            editWell(idWell);
        }
        break;
    }

}

void QWMMain::init_record_on_prime_insert(int /*row*/, QSqlRecord &/*record*/){
    //    QUuid uuid=QUuid::createUuid();
    //    QString idwell=UUIDToString(uuid);
    //    QString id=record.value("IDWell").toString();
    //    record.setValue("IDWell",idwell);
    //    record.setGenerated("IDWell",true);
}

void QWMMain::on_actionDelete_triggered()
{
    switch(ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt()){

    case QWMApplication::ALL:
    case QWMApplication::RECENT:
    case QWMApplication::FAVORITE:
        //        PA(proxyModel,ui->tbvWells->model());
        SA(sourceModel,ui->tbvWells->model());
        QItemSelectionModel* selection=ui->tbvWells->selectionModel();
        foreach(QModelIndex index,selection->selectedRows())
        {
            QString idWell=index.data(PK_ROLE).toString();
            int effectRows=WELL->deleteItem(idWell,idWell,CFG(KeyTblMain));
        }
        sourceModel->select();
        //        proxyModel->submitAll();
        break;
    }
}


void QWMMain::on_actionAbout_triggered()
{
    QWMAbout * aboutBox=new QWMAbout(this);
    aboutBox->setModal(true);
    aboutBox->showNormal();
}

void QWMMain::on_actionRefresh_triggered()
{
    APP->refresh();
    QMessageBox::information(this,tr("提示"),tr("成功加载数据库!"));
}
void QWMMain::before_insert(QSqlRecord &record){
    DPRING_RECORD(record,"X");
}

void QWMMain::on_actionDuplicate_triggered()
{
    switch(ui->trvCatalogs->currentIndex().data(CAT_ROLE).toInt()){

    case QWMApplication::ALL:
    case QWMApplication::RECENT:
    case QWMApplication::FAVORITE:
        //        PA(proxyModel,ui->tbvWells->model());
        SA(sourceModel,ui->tbvWells->model());
        QItemSelectionModel* selection=ui->tbvWells->selectionModel();
        foreach(QModelIndex index,selection->selectedRows())
        {
            QString idWell=index.data(PK_ROLE).toString();
            WELL->duplicateWell(idWell,this);
//            int effectRows=WELL->deleteItem(idWell,idWell,CFG(KeyTblMain));
        }
        sourceModel->select();
        //        proxyModel->submitAll();
        break;
    }

}
