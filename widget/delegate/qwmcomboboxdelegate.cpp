#include "qwmcomboboxdelegate.h"
#include <QComboBox>
#include <QtDebug>
#include <QApplication>
#include <QStyle>
#include "common.h"
QWMComboBoxDelegate::QWMComboBoxDelegate(QList<QPair<QString,QVariant>> options,bool editable,QObject *parent)
    :QStyledItemDelegate(parent),m_options(options),_editable(editable)
{
    qDebug()<<"delegate constructor";
}

QWMComboBoxDelegate::~QWMComboBoxDelegate()
{
    qDebug()<<"delegate destructor";
}

QWidget *QWMComboBoxDelegate::createEditor(QWidget *parent , const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);

    for(int i=0;i<m_options.size();i++){
        QPair<QString,QVariant> opt=m_options[i];
        editor->addItem(opt.first,opt.second);
    }
    editor->setEditable(_editable);
    editor->setCurrentIndex(0);
    return editor;
}

void QWMComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = static_cast<QComboBox*>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QVariant data = index.data(DATA_ROLE);

//    qDebug()<<"text="<<value<<",data="<<data;

    int idx = cb->findData(data);
    if (idx > -1 ) {
        cb->setCurrentIndex(idx);
    }
}

void QWMComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = static_cast<QComboBox*>(editor);
    QString displayData = cb->currentText();
    QVariant value = cb->currentData();
    QVariant oldValue=index.data(DATA_ROLE);
    if(oldValue!=value){
        model->setData(index, value);
    }
}

void QWMComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
