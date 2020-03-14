#include "qwmprogressdialog.h"
#include "ui_qwmprogressdialog.h"
QWMProgressDialog::QWMProgressDialog(QWidget *parent):QDialog(parent),ui(new Ui::QWMProgressDialog)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::SplashScreen,true);
}

void QWMProgressDialog::on_progress(QString message,int progress)
{
    this->ui->lblInformation->setText(message);
    this->ui->progressBar->setValue(progress);
    if(progress>=100){
        this->close();
    }
}
