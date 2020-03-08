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
    QWMAbstractDelegate(parent),m_mode(MODE(mode)),m_format(format)
{
}

QWidget *QWMDateDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem & /*option*/,
                                       const QModelIndex & index) const
{
    switch(m_mode){
    case DATE:
    {
        QDateEdit *editor = new QDateEdit(parent);
        editor->setDisplayFormat(m_format);
        editor->setCalendarPopup(true);
        editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
        return editor;
    }
        break;
    case TIME:
    {
        QTimeEdit *editor = new QTimeEdit(parent);
        editor->setDisplayFormat(m_format);
        editor->setCalendarPopup(true);
        editor->installEventFilter(const_cast<QWMDateDelegate*>(this));
        return editor;
    }
        break;
    case DATETIME:
    {
        QString key="QWMDateTimeEditor";
        EC(key,QWMDateTimeEditor,editor);
        if(editor==nullptr){
            editor= new QWMDateTimeEditor( APP->mainWindow());
            EI(key,editor);
        }
        editor->setModal(true);
        connect(editor,&QWMDateTimeEditor::rejected,this,&QWMDateDelegate::closeEditorAndRevert);
        connect(editor,&QWMDateTimeEditor::accepted,this,&QWMDateDelegate::commitAndCloseEditor);
        {SET_EDITOR(QWMDateTimeEditor,QWMDateDelegate)};
        return editor;
    }
        break;

    }
}


void QWMDateDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value= index.model()->data(index);


    switch(m_mode){
    case DATE:
    {
        QDate date = value.toDate();
        QDateEdit *edit=static_cast<QDateEdit*>(editor);
        edit->setDate(date);
        break;
    }
    case TIME:
    {
        QTime date =  value.toTime();
        QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
        edit->setTime(date);
        break;
    }
    case DATETIME:
    {
        QDateTime date =  value.toDateTime();
        QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
        edit->setValue(date);
        break;
    }
    }

}


void QWMDateDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
                                   const QModelIndex &index) const
{



    switch(m_mode){
    case DATE:
    {
        QDateEdit *edit=static_cast<QDateEdit*>(editor);
        QDate date = edit->date();
        QString v=date.toString(m_format);
        QString ov=index.data().toDate().toString(m_format);
        if(v!=ov)
            model->setData(index,QVariant(date.toString(m_format)));
        break;
    }
    case TIME:
    {
        QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
        QTime date = edit->time();
        QString v=date.toString(m_format);
        QString ov=index.data().toTime().toString(m_format);
        if(v!=ov)
            model->setData(index,QVariant(date.toString(m_format)));
        break;
    }
    case DATETIME:
    {
        //            QDateTimeEdit *edit=static_cast<QDateTimeEdit*>(editor);
        //            QDateTime date = edit->dateTime();
        //            QString v=date.toString(m_format);
        //            QString ov=index.data().toDateTime().toString(m_format);
        //            if(v!=ov)
        //                model->setData(index,QVariant(date.toString(m_format)));
        QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
        QDateTime date = edit->value().toDateTime();
        QString v=date.toString(m_format);
        QString ov=index.data().toDateTime().toString(m_format);
        if(v!=ov)
            model->setData(index,QVariant(date.toString(m_format)));

        break;
    }
    }
}


void QWMDateDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{

    if(m_mode==DATETIME){
        return QWMAbstractDelegate::updateEditorGeometry(editor,option,index);
    }else{
        editor->setGeometry(option.rect);
    }
}

bool QWMDateDelegate::isEditor(QObject *widget)
{
    if(m_mode==DATE){
        if(instanceof<QDateEdit>(widget))
            return true;
        else
            return false;
    }else if(m_mode==TIME){
        if(instanceof<QTimeEdit>(widget))
            return true;
        else
            return false;
    }else if(m_mode==DATETIME){
        if(instanceof<QWMDateTimeEditor>(widget))
            return true;
        else
            return false;
    }
    return false;
}
