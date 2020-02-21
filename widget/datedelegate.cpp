#include "datedelegate.h"
#include "datedelegate.h"
#include <QDateTimeEdit>
#include "QMetaEnum"

DateDelegate::DateDelegate(int mode,QString  format,QObject *parent) :
    QItemDelegate(parent),m_mode(MODE(mode)),m_format(format)
{
}

QWidget *DateDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &/*option*/,
                                    const QModelIndex &/*index*/) const
{
    switch(m_mode){
    case DATE:
    {
        QDateEdit *editor = new QDateEdit(parent);
        editor->setDisplayFormat(m_format);
        editor->setCalendarPopup(true);
        editor->installEventFilter(const_cast<DateDelegate*>(this));
        return editor;
    }
        break;
    case TIME:
    {
        QTimeEdit *editor = new QTimeEdit(parent);
        editor->setDisplayFormat(m_format);
        editor->setCalendarPopup(true);
        editor->installEventFilter(const_cast<DateDelegate*>(this));
        return editor;
    }
        break;
    case DATETIME:
    {
        QDateTimeEdit *editor = new QDateTimeEdit(parent);
        editor->setDisplayFormat(m_format);
        editor->setCalendarPopup(true);
        editor->installEventFilter(const_cast<DateDelegate*>(this));
        return editor;
    }
        break;

    }
}


void DateDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    QString dateStr= index.model()->data(index).toString();

    switch(m_mode){
    case DATE:
    {
        QDate date = QDate::fromString(dateStr,m_format);
        QDateEdit *edit=static_cast<QDateEdit*>(editor);
        edit->setDate(date);
        break;
    }
    case TIME:
    {
        QTime date = QTime::fromString(dateStr,m_format);
        QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
        edit->setTime(date);
        break;
    }
    case DATETIME:
    {
        QDateTime date = QDateTime::fromString(dateStr,m_format);
        QDateTimeEdit *edit=static_cast<QDateTimeEdit*>(editor);
        edit->setDateTime(date);
        break;
    }
    }

}


void DateDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
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
        QDateTimeEdit *edit=static_cast<QDateTimeEdit*>(editor);
        QDateTime date = edit->dateTime();
        QString v=date.toString(m_format);
        QString ov=index.data().toDateTime().toString(m_format);
        if(v!=ov)
            model->setData(index,QVariant(date.toString(m_format)));
        break;
    }
    }
}


void DateDelegate::updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
