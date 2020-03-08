#include "qwmabstractdelegate.h"
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include "common.h"
//#include "qwmiconselector.h"
#include "qwmdatatableview.h"
#include "qwmabstracteditor.h"
#include <QSettings>
QWMAbstractDelegate::QWMAbstractDelegate(QObject * parent):QStyledItemDelegate(parent)
{

}

QWMAbstractDelegate::~QWMAbstractDelegate()
{
    qDebug()<<"QWMBaseDelegate delegate destructor";
}

void QWMAbstractDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(IS_EDITOR(editor)&&!IS_PRIMARY_EDITOR(editor)){
        QWMAbstractEditor  * absEditor=qobject_cast<QWMAbstractEditor*>(editor);
        //    QWidget * view=DOC->dataView();//editor->parentWidget();//
        QWidget * view=editor->parentWidget();//
        //    QRect visualRect=view->visualRect(index);
        QRect rect =view->geometry();
        int widgetWidth=absEditor->sizeHint().width();//option.rect.width();
        int widgetHeight=absEditor->sizeHint().height();
        int left=(rect.width()-widgetWidth)/2;
        int top=(rect.height()-widgetHeight)/2;
        if(left<0)        left=0;
        if(top<0)        top=0;
        QPoint topleft=view->mapToParent(QPoint(left,top));
        QSettings settings;
        QString objectName=editor->metaObject()->className();
        QString entry=QString("%1.%2").arg(EDITOR_POS_ENTRY,objectName);
        topleft=settings.value(entry,topleft).value<QPoint>();
        editor->setGeometry(QRect(topleft,QSize(widgetWidth,widgetHeight)));
    }else{
        QStyledItemDelegate::updateEditorGeometry(editor,option,index);
    }
}

void QWMAbstractDelegate::commitAndCloseEditor(QWidget * editor)
{
    editor->setFocus(Qt::NoFocusReason);//必须，否则tab位置不对
    emit commitData(editor);
    emit closeEditor(editor,QAbstractItemDelegate::NoHint);
}

void QWMAbstractDelegate::closeEditorAndRevert(QWidget *editor)
{
    editor->setFocus(Qt::NoFocusReason);//必须，否则tab位置不对
    emit closeEditor(editor,QAbstractItemDelegate::RevertModelCache);
}

bool QWMAbstractDelegate::eventFilter(QObject *watched, QEvent *event) {

    if(isEditor(watched)  && !IS_PRIMARY_EDITOR(watched)){
        if(IS_KEY_EVENT(event)){
            if(IS_KEY(event,Qt::Key_Return)||IS_KEY(event,Qt::Key_Enter)){
                if(isEditor(watched)){
                    commitAndCloseEditor(dynamic_cast<QWidget*>( watched));
                    event->accept();
                    return true;
                }
                //
            }
            if (KEY_MATCHED(event,QKeySequence::Cancel)) {
                if(isEditor(watched)){
                    closeEditorAndRevert(dynamic_cast<QWidget*>( watched));
                    event->accept();
                    return true;
                }
            }
            if (IS_KEY(event,Qt::Key_Left)||IS_KEY(event,Qt::Key_Right)) {
                if(isEditor(watched)){
                    event->accept();
                    return true;
                }
            }
            if (IS_KEY(event,Qt::Key_Tab)) {
                if(isEditor(watched)){
                    dynamic_cast<QWMAbstractEditor*>( watched)->focusNextChild();
                    event->accept();
                    return true;
                }
            }
        }
        if(event->type()==QEvent::Close)
        {
            event->accept();
            return true;
        }
        if(event->type()==QEvent::Hide){
            closeEditorAndRevert(dynamic_cast<QWidget*>( watched));
            event->accept();
            return true;
        }
    }

    //    if(event->type()==QEvent::FocusOut){
    ////        closeEditorAndRevert(dynamic_cast<QWidget*>( watched));
    //        event->accept();
    //        return true;
    //    }
    //    qDebug()<<"*****Event:"<<event->type();
    return QStyledItemDelegate::eventFilter(watched,event);
}

bool QWMAbstractDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug()<<"editorEvent:"<<event->type()<<",index["<<index.row()<<","<<index.column()<<"]";
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}

void QWMAbstractDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if(instanceof<QWMAbstractEditor>(editor)){
        QWMAbstractEditor * selector=dynamic_cast<QWMAbstractEditor*>(editor);
        if(!selector->key().isNull()&&!selector->key().isEmpty()){
            if(APP->editorCached(selector->key()))
            {
                //                {DISABLE_EDITOR;}
                return ;
            }
        }
    }
    QAbstractItemDelegate::destroyEditor(editor,index);
}


