#include <QFile>
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include "qwmmain.h"
#include "ui_qwmmain.h"
#include "common.h"
#include "qwmapplication.h"
QWMMain::QWMMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QWMMain)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,4);
    APP->initMDLDB();
    APP->initLIBDB();
}

QWMMain::~QWMMain()
{
    delete ui;


}


void QWMMain::on_actionChangeDB_triggered()
{
    QString error;
    APP->selectWellDB(error,this);
}
