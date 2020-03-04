#include "qwmabstractdelegate.h"
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include "common.h"
//#include "qwmiconselector.h"
#include "qwmdatatableview.h"
#include "qwmabstracteditor.h"

QWMAbstractDelegate::QWMAbstractDelegate(QObject * parent):QStyledItemDelegate(parent)
{

}

QWMAbstractDelegate::~QWMAbstractDelegate()
{
    qDebug()<<"QWMBaseDelegate delegate destructor";
}

void QWMAbstractDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWMAbstractEditor  * absEditor=qobject_cast<QWMAbstractEditor*>(editor);

    QWidget * view=editor->parentWidget();
    //    QRect visualRect=view->visualRect(index);
    QRect rect =view->geometry();
    int widgetWidth=absEditor->sizeHint().width();//option.rect.width();
    int widgetHeight=absEditor->sizeHint().height();
    int margin=0;

    int left=(rect.width()-widgetWidth)/2;
    int top=(rect.height()-widgetHeight)/2;
    if(left<0)
        left=0;
    if(top<0)
        top=0;
    QPoint topleft=view->mapToParent(QPoint(left,top));

    editor->setGeometry(QRect(topleft,QSize(widgetWidth,widgetHeight)));
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
//    if(event->type()==QEvent::FocusOut){
//        if(isEditor(watched)){
//            closeEditorAndRevert(dynamic_cast<QWidget*>( watched));

//            event->accept();
//            return true;
//        }
//    }
    //        if (KEY_MATCHED(event,QKeySequence::Cancel)) {
    //            // don't commit data
    //            emit this->rejected(this);
    //            event->accept();
    //            return true;
    //        }
    //    }


    //    qDebug()<<"eventFilter Event:W="<<watched->metaObject()->className()<<"/"<<watched->objectName()<<",E="<<event->type();
    //    if(event->type()==QEvent::KeyPress||event->type()==QEvent::KeyRelease){
    //        QKeyEvent *ke=static_cast<QKeyEvent*>(event);

    //        if(ke->key()==Qt::Key_Tab){
    //            qDebug()<<"\t KeyPressed:"<<ke->key();
    //            event->accept();
    //            return true;
    //        }
    //    }
    //    if(watched->metaObject()->className()==QWMIconSelector::staticMetaObject.className()){

    //        if(event->type()==QEvent::KeyPress){
    //            QKeyEvent *  keyEvent=(QKeyEvent*) event;
    //            qDebug()<<"Key:"<<keyEvent->key();
    //        }
    //    }
    //    if(event->type()==QEvent::FocusIn){
    //        qDebug()<<"FocusIN!";
    //    }
    //    if(event->type()==QEvent::FocusOut){
    //        qDebug()<<"FocusOut!"<<watched->metaObject()->className()<<","<<watched->objectName();
    //        event->ignore();
    //    }
    return QStyledItemDelegate::eventFilter(watched,event);
}

bool QWMAbstractDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug()<<"editorEvent:"<<event->type()<<",index["<<index.row()<<","<<index.column()<<"]";
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}


