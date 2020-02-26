#include "qwmdistinctvaluedelegate.h"
#include "comboboxdelegate.h"
#include <QComboBox>
#include <QtDebug>
#include <QApplication>
#include <QStyle>
#include "common.h"
#include "welldao.h"

QWMDistinctValueDelegate::QWMDistinctValueDelegate(QString table,QString field,QObject *parent)
    :QStyledItemDelegate(parent),_table(table),_field(field)
{
    qDebug()<<"delegate constructor";
}

QWMDistinctValueDelegate::~QWMDistinctValueDelegate()
{
    qDebug()<<"delegate destructor";
}

QWidget *QWMDistinctValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(true);
    QStringList values=WELL->distinctValue(_table,_field);

    for(int i=0;i<values.size();i++){
        editor->addItem(values[i],values[i]);
    }
//    editor->setCurrentIndex(0);
    return editor;
}

void QWMDistinctValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = static_cast<QComboBox*>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QVariant data = index.data(DATA_ROLE);

    qDebug()<<"text="<<value<<",data="<<data;

    int idx = cb->findData(data);
    if (idx > -1 ) {
        cb->setCurrentIndex(idx);
    }
    cb->setEditText(value);
}

void QWMDistinctValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = static_cast<QComboBox*>(editor);
    QString displayData = cb->currentText();
    QVariant value = cb->currentIndex()>=0?cb->currentData():cb->currentText();
    QVariant oldValue=index.data(DATA_ROLE);
    if(oldValue!=value){
        model->setData(index, value);
    }
}

void QWMDistinctValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
