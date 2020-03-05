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
QWMAbstractEditor::QWMAbstractEditor( QWidget *parent) : QDialog(parent)
{
    //    this->on_comboBox_currentTextChanged(ui->comboBox->currentText());

}

bool QWMAbstractEditor::eventFilter(QObject *watched, QEvent *event)
{


    //    QList<QWidget *> ws=taborders();
    //    if(IS_KEY_EVENT(event)){
    //        if(IS_KEY(event,Qt::Key_Tab)||IS_KEY(event,Qt::Key_Backtab)){
    //            int index=ws.indexOf((QWidget*)watched);
    //            int next=(index+1)%ws.size();
    //            int prev=(index+ws.size()-1)%ws.size();
    //            if(index>=0){
    //                if(IS_KEY(event,Qt::Key_Tab))
    //                    ws[next]->setFocus();
    //                else
    //                    ws[prev]->setFocus();
    //            }
    //            event->accept();
    //            return true;
    //        }
    //        if(IS_KEY(event,Qt::Key_Return)||IS_KEY(event,Qt::Key_Enter)){
    //            emit this->accepted(this);
    //            event->accept();
    //            return true;
    //        }
    //        if (KEY_MATCHED(event,QKeySequence::Cancel)) {
    //            // don't commit data
    //            emit this->rejected(this);
    //            event->accept();
    //            return true;
    //        }
    //    }

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
    QSettings settings;
    QString objectName=this->metaObject()->className();
    QString entry=QString("%1.%2").arg(EDITOR_SIZE_ENTRY,objectName);
    QSize size= settings.value(entry,QSize(350,350)).value<QSize>();
    return  size;
}

void QWMAbstractEditor::resizeEvent(QResizeEvent *)
{
    QSettings settings;
    QString objectName=this->metaObject()->className();
    QString entry=QString("%1.%2").arg(EDITOR_SIZE_ENTRY,objectName);
    QSize size=this->size();
    settings.setValue(entry,size);
}


