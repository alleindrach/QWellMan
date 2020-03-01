#include "qwmicondelegate.h"
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include "common.h"
#include "qwmiconselector.h"
#include "qwmdatatableview.h"
QWMIconDelegate::QWMIconDelegate(QObject * parent):QStyledItemDelegate(parent)
{

}

QWMIconDelegate::~QWMIconDelegate()
{
    qDebug()<<"delegate destructor";
}

QWidget *QWMIconDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWMIconSelector *editor = new QWMIconSelector(parent);
    connect(editor,&QWMIconSelector::rejected,this,&QWMIconDelegate::justCloseEditor);
    connect(editor,&QWMIconSelector::accepted,this,&QWMIconDelegate::commitAndCloseEditor);
//    editor->installEventFilter(this);
    return editor;
}

void QWMIconDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QWMIconSelector *iconSelector = static_cast<QWMIconSelector*>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QVariant data = index.data(DATA_ROLE);

//    qDebug()<<"text="<<value<<",data="<<data;

    iconSelector->selectFile(value);

}

void QWMIconDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QWMIconSelector *cb = static_cast<QWMIconSelector*>(editor);
    QString newValue = cb->file();

    QVariant oldValue=index.data(Qt::DisplayRole);
    if(oldValue!=newValue){
        model->setData(index, newValue);
    }
}

void QWMIconDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QWMDataTableView * view=(QWMDataTableView *)this->parent();
    QRect rect =view->geometry();
//    qDebug()<<"view:"<<rect;
    int widgetWidth=350;//option.rect.width();
    int widgetHeight=350;
    int margin=0;
    int left;
    int top;

    if(option.rect.left()+widgetWidth+margin<  rect.width()){
        left=option.rect.left();
    }else{
        left=rect.width()-widgetWidth-margin;
    }
    if(option.rect.top()+widgetHeight+margin<rect.height()){
        top=option.rect.top()+margin;
    }else
    {
        top=option.rect.top()-widgetHeight-margin;
        if(top<0)
            top=0;
    }
    QPoint topleft(left, top);
    editor->setGeometry(QRect(topleft,QSize(widgetWidth,widgetHeight)));
}
void QWMIconDelegate::commitAndCloseEditor(QWMIconSelector * editor)
{
    editor->setFocus(Qt::NoFocusReason);
    emit commitData(editor);
    emit closeEditor(editor,QAbstractItemDelegate::EditNextItem);
}

void QWMIconDelegate::justCloseEditor(QWMIconSelector * editor)
{
    editor->setFocus(Qt::NoFocusReason);
    emit closeEditor(editor,QAbstractItemDelegate::RevertModelCache);
}
bool QWMIconDelegate::eventFilter(QObject *watched, QEvent *event) {
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

bool QWMIconDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug()<<"editorEvent:"<<event->type()<<",index["<<index.row()<<","<<index.column()<<"]";
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}


