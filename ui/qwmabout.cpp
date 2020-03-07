#include "qwmabout.h"

#include "ui_about.h"
#include "QPixmap"
#include "common.h"
#include <QKeyEvent>
#include <QKeySequence>
QWMAbout::QWMAbout(QWidget *parent):QDialog(parent),ui(new Ui::QWMAbout)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::SplashScreen,true);
    QPixmap pixmap(":/images/About.png");
    ui->label->setPixmap(pixmap);
    ui->label->installEventFilter(this);
    this->resize(pixmap.size());
}

bool QWMAbout::eventFilter(QObject *watched, QEvent *event)
{
    if(IS_KEY_EVENT(event)){

        if(IS_KEY(event,Qt::Key_Return)||IS_KEY(event,Qt::Key_Enter)){
            this->accept();
            return true;
        }
        if (KEY_MATCHED(event,QKeySequence::Cancel)) {
            // don't commit data
            this->accept();
            return true;
        }
    }else if(IS_MOUSE_EVENT(event)){
        this->accept();
        return true;
    }
    return false;
}
