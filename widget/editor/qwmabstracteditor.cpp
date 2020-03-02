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



