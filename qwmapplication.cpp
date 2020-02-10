#include "qwmapplication.h"
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include  "common.h"
QWMApplication::QWMApplication(int &argc, char **argv):QApplication(argc,argv)
{
    connect(this,&QWMApplication::shutdown,this,&QCoreApplication::exit,Qt::QueuedConnection);
}

QWMApplication::~QWMApplication()
{
    CLOSEDB(_mdlDB);
    CLOSEDB(_udlDB);
    CLOSEDB(_wellDB);
    CLOSEDB(_libDB);
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
            //            QMessageBox::information(this, tr("提示"), tr("数据库打开成功"),
            //                                     QMessageBox::Ok,QMessageBox::NoButton);
            return true;
        }else
        {

            return false;
        }
    }
}

bool QWMApplication::selectWellDB(QString& error,QWidget * parent)
{

    QSettings settings;
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
void QWMApplication::initMDLDB()
{
    QSettings settings;
    QString error;
    QString strMdlDbPath=settings.value(MDLDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strMdlDbPath)){
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到MDL数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_mdlDB,_mdlUserName,_mdlPassword, error,MDLDB_CONNECTION_NAME)){
                settings.setValue(MDLDB_PATH_SETTING_ENTRY,_mdlDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
            }

        }else
        {
            emit this->shutdown(0);
        }
    }else{
        _mdlDB=openDB(strMdlDbPath,_mdlUserName,_mdlPassword,error,MDLDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("数据库打开失败")+" "+error,"退出");
            emit this->shutdown(0);
        }
    }
}

void QWMApplication::initLIBDB()
{
    QSettings settings;
    QString error;
    QString strMdlDbPath=settings.value(LIBDB_PATH_SETTING_ENTRY).toString();
    if(!QFile::exists(strMdlDbPath)){
        int select=QMessageBox::information(nullptr,tr("提示"),tr("未找到LIB数据库，请选择数据库的位置"),"选择","退出",0);
        if(select==0){
            if(selectDB(_mdlDB,_mdlUserName,_mdlPassword, error,LIBDB_CONNECTION_NAME)){
                settings.setValue(LIBDB_PATH_SETTING_ENTRY,_mdlDB.databaseName());
            }else
            {
                QMessageBox::warning(nullptr,tr("错误"),tr("数据库打开失败")+" "+error,"退出");
                emit this->shutdown(0);
            }

        }else
        {
            emit this->shutdown(0);
        }
    }else{
        _mdlDB=openDB(strMdlDbPath,_mdlUserName,_mdlPassword,error,LIBDB_CONNECTION_NAME);
        if(!error.isEmpty()){
            QMessageBox::warning(nullptr,tr("错误"),tr("数据库打开失败")+" "+error,"退出");
            emit this->shutdown(0);
        }
    }
}
