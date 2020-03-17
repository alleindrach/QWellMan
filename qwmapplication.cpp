#include "qwmapplication.h"
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QIcon>
#include "common.h"
#include "mdldao.h"
#include "udldao.h"
#include "libdao.h"
#include "welldao.h"
#include "edldao.h"
#include "record.h"
#include "mdltable.h"
#include "mdlfieldlookup.h"
#include "udltableproperty.h"
#include "udllibgroup.h"
#include "udllibtab.h"
#include "udllibtabfield.h"
#include "qwmtablemodel.h"
#include <QDir>
#include "qwmmain.h"
QWMApplication::QWMApplication(int &argc, char **argv):QApplication(argc,argv)
{
    REGISTER_ALL(Record);
    REGISTER_ALL(MDLTable);
    REGISTER_ALL(MDLField);
    REGISTER_ALL(MDLFieldVisible);
    REGISTER_ALL(MDLUnitSet);
    REGISTER_ALL(MDLUnitType);
    REGISTER_ALL(MDLUnitTypeSet);
    REGISTER_ALL(MDLUnitConversion);
    REGISTER_ALL(MDLFieldLookup);
    REGISTER_ALL(UDLTableProperty);
    REGISTER_ALL(UDLLibGroup);
    REGISTER_ALL(UDLLibTab);
    REGISTER_ALL(UDLLibTabField);
    REGISTER(QWMRotatableProxyModel*);
    REGISTER(QWMTableModel*);
    //    qRegisterMetaType<Record>("Record");
    //    qRegisterMetaType<MDLTable>("MDLTable");
    connect(this,&QWMApplication::shutdown,this,&QCoreApplication::exit,Qt::QueuedConnection);
    _iconMap[QStringLiteral("lookup")]=QIcon(QStringLiteral(":/images/icons/dictionary.svg"));
    _iconMap[QStringLiteral("files@1x")]=QIcon(QStringLiteral(":/images/icons/files@1x.svg"));
    _iconMap[QStringLiteral("files@2x")]=QIcon(QStringLiteral(":/images/icons/files@2x.svg"));
    _iconMap[QStringLiteral("files@4x")]=QIcon(QStringLiteral(":/images/icons/files@4x.svg"));
    _iconMap[QStringLiteral("file@1x")]=QIcon(QStringLiteral(":/images/icons/file@1x.svg"));
    _iconMap[QStringLiteral("file@2x")]=QIcon(QStringLiteral(":/images/icons/file@2x.svg"));
    _iconMap[QStringLiteral("file@4x")]=QIcon(QStringLiteral(":/images/icons/file@4x.svg"));
    _iconMap[QStringLiteral("folder")]=QIcon(QStringLiteral(":/images/icons/folder.svg"));
    _iconMap[QStringLiteral("folder@1x")]=QIcon(QStringLiteral(":/images/icons/folder@1x.png"));
    _iconMap[QStringLiteral("folder@2x")]=QIcon(QStringLiteral(":/images/icons/folder@2x.png"));
    _iconMap[QStringLiteral("folder@4x")]=QIcon(QStringLiteral(":/images/icons/folder@4x.png"));
    _iconMap[QStringLiteral("folder-open@1x")]=QIcon(QStringLiteral(":/images/icons/folder-open@1x.png"));
    _iconMap[QStringLiteral("folder-open@2x")]=QIcon(QStringLiteral(":/images/icons/folder-open@2x.png"));
    _iconMap[QStringLiteral("folder-open@4x")]=QIcon(QStringLiteral(":/images/icons/folder-open@4x.png"));
    _iconMap[QStringLiteral("data@1x")]=QIcon(QStringLiteral(":/images/icons/data@1x.png"));
    _iconMap[QStringLiteral("data@2x")]=QIcon(QStringLiteral(":/images/icons/data@2x.png"));
    _iconMap[QStringLiteral("data@4x")]=QIcon(QStringLiteral(":/images/icons/data@4x.png"));
    _iconMap[QStringLiteral("query@1x")]=QIcon(QStringLiteral(":/images/icons/query@1x.png"));
    _iconMap[QStringLiteral("query")]=QIcon(QStringLiteral(":/images/icons/query.svg"));
    _iconMap[QStringLiteral("query@2x")]=QIcon(QStringLiteral(":/images/icons/query@2x.png"));
    _iconMap[QStringLiteral("query@4x")]=QIcon(QStringLiteral(":/images/icons/query@4x.png"));
    _iconMap[QStringLiteral("folder-open")]=QIcon(QStringLiteral(":/images/icons/openfolder@1x.png"));
    _iconMap[QStringLiteral("well-close")]=QIcon(QStringLiteral(":/images/icons/close-folder@1x.png"));
    _iconMap[QStringLiteral("file")]=QIcon(QStringLiteral(":/images/icons/file@1x.png"));
    _iconMap[QStringLiteral("well")]=QIcon(QStringLiteral(":/images/icons/well@1x.png"));
    _iconMap[QStringLiteral("well2@1x")]=QIcon(QStringLiteral(":/images/icons/well2@1x.png"));
    _iconMap[QStringLiteral("wellplatform")]=QIcon(QStringLiteral(":/images/icons/wellplatform@1x.png"));
    _iconMap[QStringLiteral("openrecent")]=QIcon(QStringLiteral(":/images/icons/recent.png"));
    _iconMap[QStringLiteral("recentedit")]=QIcon(QStringLiteral(":/images/icons/recent-edit.png"));
    this->setStyleSheet(_style);
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
}

QWMApplication::~QWMApplication()
{
    CLOSEDB(_mdlDB);
    CLOSEDB(_udlDB);
    CLOSEDB(_wellDB);
    CLOSEDB(_libDB);
    CLOSEDB(_edlDB);
}

bool QWMApplication::selectDB(QSqlDatabase  &db,QString username,QString password ,QString & error,QString connectionName)
{

    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle=tr("选择数据库位置"); //对话框标题
    QString filter=tr("sqlite(*.sqlite);;all files(*.*)"); //文件过滤器
    QString strFileName=QFileDialog::getOpenFileName(nullptr,dlgTitle,curPath,filter);
    if (!strFileName.isEmpty()){
        CLOSEDB(db);
        db=this->openDB(strFileName,username,password,error,connectionName);
        if(error.isEmpty()){
            return true;
        }else
        {
            return false;
        }
    }
}

bool QWMApplication::selectWellDB(QString& error,QWidget * parent)
{
    SETTINGS;
    if(selectDB(_wellDB,_mdlUserName,_mdlPassword,error, WELLDB_CONNECTION_NAME)){
        settings.setValue(WELLDB_PATH_SETTING_ENTRY,_wellDB.databaseName());
    }else
    {
        QMessageBox::warning(parent,tr("错误"),tr("数据库打开失败")+" "+error,"退出");
    }
}

QSqlDatabase QWMApplication::openDB( QString path,QString username,QString password ,QString & error,QString connectionName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

    db.setDatabaseName(path);
    db.setUserName(username);
    db.setPassword(password);
    //    db.setHostName(HOST);
    if( !QFile::exists(path)) {
        //Force to execute update database
        error=tr("Database Not Exists");
    }else{
        bool ok =db.open();
        if(!ok){
            error=tr("Database Open failed");
        }
    }
    return db;
}
bool QWMApplication::initMDLDB()
{
    SETTINGS;
    QString error;
    //    qDebug()<<" Setting file:"<<settings.fileName();
    QString strMdlDbPath=settings.value(MDLDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strMdlDbPath)){
        this->mainWindow()->hideSplash();
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到MDL数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_mdlDB,_mdlUserName,_mdlPassword, error,MDLDB_CONNECTION_NAME)){
                settings.setValue(MDLDB_PATH_SETTING_ENTRY,_mdlDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("MDL数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
                return false;
            }
        }else
        {
            emit this->shutdown(0);
            return false;
        }
    }else{
        _mdlDB=openDB(strMdlDbPath,_mdlUserName,_mdlPassword,error,MDLDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("MDL数据库打开失败")+" "+error,"退出");
            settings.setValue(MDLDB_PATH_SETTING_ENTRY,"");
            emit this->shutdown(0);
            return false;
        }
    }
    //    _mdlDB.transaction();
    //    QSqlQuery q=_mdlDB.exec("select * from sqlite_master where type='table'");
    //    if(q.lastError().isValid()){
    //        qDebug()<<"open mdl error:"<<q.lastError().text();
    //    }else{
    //        while(q.next()){
    //            qDebug()<<"Mdl  table:"<<q.value("name").toString();
    //        }
    //    }
    //    _mdlDB.commit();
    return true;
}

bool QWMApplication::initUDLDB()
{
    SETTINGS;
    QString error;
    //    qDebug()<<" Setting file:"<<settings.fileName();
    QString strUdlDbPath=settings.value(UDLDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strUdlDbPath)){
        this->mainWindow()->hideSplash();
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到UDL数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_udlDB,_udlUserName,_udlPassword, error,UDLDB_CONNECTION_NAME)){
                settings.setValue(UDLDB_PATH_SETTING_ENTRY,_udlDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("UDL数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
                return false;
            }
        }else
        {
            emit this->shutdown(0);
            return false;
        }
    }else{
        _udlDB=openDB(strUdlDbPath,_udlUserName,_udlPassword,error,UDLDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("UDL数据库打开失败")+" "+error,"退出");
            settings.setValue(UDLDB_PATH_SETTING_ENTRY,"");
            emit this->shutdown(0);
            return false;
        }
    }
    //    _udlDB.transaction();
    //    QSqlQuery q=_udlDB.exec("select * from sqlite_master where type='table'");
    //    if(q.lastError().isValid()){
    //        qDebug()<<"open udl error:"<<q.lastError().text();
    //    }else{
    //        while(q.next()){
    //            qDebug()<<"Udl  table:"<<q.value("name").toString();
    //        }
    //    }
    //    _udlDB.commit();
    return true;
}

bool QWMApplication::initLIBDB()
{
    SETTINGS;
    QString error;
    QString strLibDbPath=settings.value(LIBDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strLibDbPath)){
        this->mainWindow()->hideSplash();
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到LIB数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_libDB,_mdlUserName,_mdlPassword, error,LIBDB_CONNECTION_NAME)){
                settings.setValue(LIBDB_PATH_SETTING_ENTRY,_libDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("LIB数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
                return false;
            }

        }else
        {
            emit this->shutdown(0);
            return false;
        }
    }else{
        _libDB=openDB(strLibDbPath,_mdlUserName,_mdlPassword,error,LIBDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("LIB数据库打开失败")+" "+error,"退出");
            settings.setValue(LIBDB_PATH_SETTING_ENTRY,"");
            emit this->shutdown(0);
            return false;
        }
    }
    return true;
}

bool QWMApplication::initEDLDB()
{
    SETTINGS;
    QString error;
    QString strEdlDbPath=settings.value(EDLDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strEdlDbPath)){
        this->mainWindow()->hideSplash();
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到计算列配置数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_edlDB,_edlUserName,_edlPassword, error,EDLDB_CONNECTION_NAME)){
                settings.setValue(EDLDB_PATH_SETTING_ENTRY,_edlDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("计算列配置数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
                return false;
            }

        }else
        {
            emit this->shutdown(0);
            return false;
        }
    }else{
        _edlDB=openDB(strEdlDbPath,_edlUserName,_edlPassword,error,EDLDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("计算列配置数据库打开失败")+" "+error,"退出");
            settings.setValue(EDLDB_PATH_SETTING_ENTRY,"");
            emit this->shutdown(0);
            return false;
        }
    }
    return true;
}

bool QWMApplication::initWellDB()
{
    SETTINGS;
    QString error;
    QString strWellDbPath=settings.value(WELLDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strWellDbPath)){
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到井数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_wellDB,_mdlUserName,_mdlPassword, error,WELLDB_CONNECTION_NAME)){
                settings.setValue(WELLDB_PATH_SETTING_ENTRY,_wellDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("井数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
                return false;
            }

        }else
        {
            emit this->shutdown(0);
            return false;
        }
    }else{
        _wellDB=openDB(strWellDbPath,_mdlUserName,_mdlPassword,error,WELLDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("井数据库打开失败")+" "+error,"退出");
            settings.setValue(WELLDB_PATH_SETTING_ENTRY,"");
            emit this->shutdown(0);
            return false;
        }
    }
    return true;
}

QSqlDatabase &QWMApplication::mdl()
{
    return _mdlDB;
}

QSqlDatabase &QWMApplication::lib()
{
    return _libDB;
}

QSqlDatabase &QWMApplication::udl()
{
    return _udlDB;
}

QSqlDatabase &QWMApplication::well()
{
    return _wellDB;
}

QSqlDatabase &QWMApplication::edl()
{
    return _edlDB;
}

QHash<QString, QString> &QWMApplication::config()
{
    return _config;
}


QMap<QString, QIcon> &QWMApplication::icons()
{
    return _iconMap;
}

QIcon QWMApplication::icon(QString key)  const
{
    return _iconMap[key];
}

QString QWMApplication::referenceDatumName(QString value){
    if(value=="elvcasflange") return tr("Casing Flange Elevation(CF)");
    if(value=="elvorigkb")    return tr("Original KB Elevation(KB)");
    if(value=="elvground") return tr("Ground Elevation(GRD)");
    if(value=="elvtubhead")    return tr("Tubing Head Elevation(TH)");
    if(value=="elvmudline") return tr("Mud Line Elevation(ML)");
    if(value=="msl")    return tr("Mean Sea Level(MSL)");
    if(value=="elvother")    return tr("Other Elevation(Well specfic)");
    return QString();
}
QString QWMApplication::referenceDatumValue(QString name){
    if(name==tr("Casing Flange Elevation(CF)")) return "elvcasflange";
    if(name== tr("Original KB Elevation(KB)"))    return  "elvorigkb";
    if(name==tr("Ground Elevation(GRD)")) return "elvground";
    if(name==tr("Tubing Head Elevation(TH)"))    return "elvtubhead" ;
    if(name==tr("Mud Line Elevation(ML)")) return "elvmudline";
    if(name== tr("Mean Sea Level(MSL)"))    return "msl";
    if(name==tr("Other Elevation(Well specfic)"))    return "elvother";
    return QString();
}

void QWMApplication::loadPreference()
{
    SETTINGS;
    _unit=settings.value(UNIT_SETTING_ENTRY,UNIT_SETTING_DEFAULT).toString();
    _profile=settings.value(PROFILE_SETTING_ENTRY,PROFILE_SETTING_DEFAULT).toString();
    _datumPreference =settings.value(REFERENCE_DATUM_ENTRY,REFERENCE_DATUM_DEFAULT).toString();
    _wellDisplayFields=settings.value(WELL_DISPLAY_FIELDS_ENTRY,MDL->tableMainHeadersVisible()).toStringList();

}

QString QWMApplication::profile()
{
    return _profile;
}

void QWMApplication::setProfile(QString v)
{
    _profile=v;
    SETTINGS;
    settings.setValue(PROFILE_SETTING_ENTRY,v);
}

QString QWMApplication::unit()
{
    return _unit;
}

void QWMApplication::setUnit(QString v)
{
    _unit=v;
    SETTINGS;
    settings.setValue(UNIT_SETTING_ENTRY,v);
}

QString QWMApplication::datumPreference()
{
    return _datumPreference;
}

void QWMApplication::setDatumPref(QString v)
{
    _datumPreference=v;
    SETTINGS;
    settings.setValue(REFERENCE_DATUM_ENTRY,v);
}

QStringList QWMApplication::wellDisplayList(){
    if(_wellDisplayFields.isEmpty())
        _wellDisplayFields=MDL->tableMainHeadersVisible();
    _wellDisplayFields.removeAll("");
    _wellDisplayFields.removeDuplicates();
    return _wellDisplayFields;
}
void QWMApplication::setWellDisplayList(QStringList list){
    _wellDisplayFields=list;
    SETTINGS;
    settings.setValue(WELL_DISPLAY_FIELDS_ENTRY,_wellDisplayFields);
}

QWMMain *QWMApplication::mainWindow()
{
    return _mainWnd;
}

void QWMApplication::setMainWindow(QWMMain *v)
{
    _mainWnd=v;
}

void QWMApplication::refresh()
{
    CLOSEDB(_mdlDB);
    CLOSEDB(_udlDB);
    CLOSEDB(_wellDB);
    CLOSEDB(_libDB);
    CLOSEDB(_edlDB);
    initMDLDB();
    initUDLDB();
    initLIBDB();
    initWellDB();
    initEDLDB();
    MDLDao::resetCache();
    UDLDao::resetCache();
    LIBDao::resetCache();
    WellDao::resetCache();
    EDLDao::resetCache();
}
