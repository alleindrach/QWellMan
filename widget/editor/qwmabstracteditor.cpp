#include "qwmabstracteditor.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDebug>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include "qwmapplication.h"
#include <QSettings>
#include <QPoint>
QWMAbstractEditor::QWMAbstractEditor( QWidget *parent) : QDialog(parent)
{
    //    this->on_comboBox_currentTextChanged(ui->comboBox->currentText());
}

bool QWMAbstractEditor::eventFilter(QObject */*watched*/, QEvent */*event*/)
{
    return false;
}

void QWMAbstractEditor::focusInEvent(QFocusEvent *event)
{
    if(event->reason()!=Qt::NoFocusReason){
        if(taborders().size()>0)
            this->taborders()[0]->setFocus();
    }
}

void QWMAbstractEditor::installEventFilters()
{
    QList<QWidget *> ws=taborders();
    foreach(QWidget * w,ws){
        w->removeEventFilter(this);
        w->installEventFilter(this);
    }
}

void QWMAbstractEditor::focusNext()
{

}

void QWMAbstractEditor::focusPrev()
{

}

void QWMAbstractEditor::on_btn_clicked()
{
    if(sender()->objectName()=="btnOK"){
        emit this->accepted(this);
    }
    if(sender()->objectName()=="btnCancel"){
        emit this->rejected(this);
    }
}

QSize QWMAbstractEditor::sizeHint()
{
    //    return QSize(350,350);
    SETTINGS;
    QString objectName=this->metaObject()->className();
    QString entry=QString("%1.%2").arg(EDITOR_SIZE_ENTRY,objectName);
    QSize size= settings.value(entry,QSize(350,350)).value<QSize>();
    return  size;
}

void QWMAbstractEditor::resizeEvent(QResizeEvent *)
{
    SETTINGS;
    QString objectName=this->metaObject()->className();
    QString entry=QString("%1.%2").arg(EDITOR_SIZE_ENTRY,objectName);
    QSize size=this->size();
    settings.setValue(entry,size);
}

void QWMAbstractEditor::moveEvent(QMoveEvent */*event*/)
{
    SETTINGS;
    QString objectName=this->metaObject()->className();
    QString entry=QString("%1.%2").arg(EDITOR_POS_ENTRY,objectName);
    QPoint pos=this->pos();
    settings.setValue(entry,pos);
}

void QWMAbstractEditor::setTitle(QString title)
{
    _title=title;
    this->setWindowTitle(_title);

}

bool QWMAbstractEditor::event(QEvent *event)
{
//    qDebug()<<"#####"<<event->type();
    return  QDialog::event(event);

}


