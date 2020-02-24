#include "qwmdatedelegate.h"
#include <QDateTimeEdit>
#include "QMetaEnum"
#include "qwmdatetimeeditor.h"
#include "qwmrotatableproxymodel.h"
#include "qwmdataeditor.h"
#include "qwmdatatableview.h"
#include <QDialogButtonBox>
#include "qdebug.h"
QWMDateDelegate::QWMDateDelegate(int mode,QString  format,QObject *parent) :
    QItemDelegate(parent),m_mode(MODE(mode)),m_format(format)
{
}

QWidget *QWMDateDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    QWMDateTimeEditor *editor = new QWMDateTimeEditor(QDateTime::currentDateTime(), parent);

    connect(editor,&QWMDateTimeEditor::rejected,this,&QWMDateDelegate::justCloseEditor);
    connect(editor,&QWMDateTimeEditor::accepted,this,&QWMDateDelegate::commitAndCloseEditor);

//    editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
    return editor;

    //    switch(m_mode){
    //    case DATE:
    //    {
    //        QDateEdit *editor = new QDateEdit(parent);
    //        editor->setDisplayFormat(m_format);
    //        editor->setCalendarPopup(true);
    //        editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
    //        return editor;
    //    }
    //        break;
    //    case TIME:
    //    {
    //        QTimeEdit *editor = new QTimeEdit(parent);
    //        editor->setDisplayFormat(m_format);
    //        editor->setCalendarPopup(true);
    //        editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
    //        return editor;
    //    }
    //        break;
    //    case DATETIME:
    //    {
    //        QDateTimeEdit *editor = new QDateTimeEdit(parent);
    //        editor->setDisplayFormat(m_format);
    //        editor->setCalendarPopup(true);
    //        editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
    //        return editor;
    //    }
    //        break;

    //    }
}


void QWMDateDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value= index.model()->data(index);

    QDateTime date =  value.toDateTime();
    QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
    edit->setDateTime(date);
    //    switch(m_mode){
    //    case DATE:
    //    {
    //        QDate date = value.toDate();
    //        QDateEdit *edit=static_cast<QDateEdit*>(editor);
    //        edit->setDate(date);
    //        break;
    //    }
    //    case TIME:
    //    {
    //        QTime date =  value.toTime();
    //        QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
    //        edit->setTime(date);
    //        break;
    //    }
    //    case DATETIME:
    //    {
    //        QDateTime date =  value.toDateTime();
    //        QDateTimeEdit *edit=static_cast<QDateTimeEdit*>(editor);
    //        edit->setDateTime(date);
    //        break;
    //    }
    //    }

}


void QWMDateDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
                                   const QModelIndex &index) const
{

    QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
    QDateTime date = edit->dateTime();
    QString v=date.toString(m_format);
    QString ov=index.data().toDateTime().toString(m_format);
    if(v!=ov)
        model->setData(index,QVariant(date.toString(m_format)));


    //    switch(m_mode){
    //    case DATE:
    //    {
    //        QDateEdit *edit=static_cast<QDateEdit*>(editor);
    //        QDate date = edit->date();
    //        QString v=date.toString(m_format);
    //        QString ov=index.data().toDate().toString(m_format);
    //        if(v!=ov)
    //            model->setData(index,QVariant(date.toString(m_format)));
    //        break;
    //    }
    //    case TIME:
    //    {
    //        QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
    //        QTime date = edit->time();
    //        QString v=date.toString(m_format);
    //        QString ov=index.data().toTime().toString(m_format);
    //        if(v!=ov)
    //            model->setData(index,QVariant(date.toString(m_format)));
    //        break;
    //    }
    //    case DATETIME:
    //    {
    //        QDateTimeEdit *edit=static_cast<QDateTimeEdit*>(editor);
    //        QDateTime date = edit->dateTime();
    //        QString v=date.toString(m_format);
    //        QString ov=index.data().toDateTime().toString(m_format);
    //        if(v!=ov)
    //            model->setData(index,QVariant(date.toString(m_format)));
    //        break;
    //    }
    //    }
}


void QWMDateDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{

    QWMDataTableView * view=(QWMDataTableView *)this->parent();
    QRect rect =view->geometry();
    qDebug()<<"view:"<<rect;
    int widgetWidth=250;
    int widgetHeight=250;
    int margin=20;
    int left;
    int top;
    QWMRotatableProxyModel *  model= ( QWMRotatableProxyModel *)index.model();
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
    if(model->mode()==QWMRotatableProxyModel::H){

        QPoint topleft(left, top);
        editor->setGeometry(QRect(topleft,QSize(250,250)));
    }else
    {
        QPoint topleft(left,top);
        editor->setGeometry(QRect(topleft,QSize(250,250)));
    }

//    editor->setGeometry(option.rect);
}
void QWMDateDelegate::commitAndCloseEditor(QWMDateTimeEditor * editor)
{
    //    qDebug()<<"on_treeView_doubleClicked";

    emit commitData(editor);
    emit closeEditor(editor);
}

void QWMDateDelegate::justCloseEditor(QWMDateTimeEditor * editor)
{
    //    qDebug()<<"on_treeView_doubleClicked";
//    QDialogButtonBox *btn = qobject_cast<QDialogButtonBox *>(sender());
//    QWMDateTimeEditor *  editor=qobject_cast<QWMDateTimeEditor *>(btn->parent());
    //    emit commitData(editor);
    emit closeEditor(editor);
}
