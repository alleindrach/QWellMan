#include "qwmicondelegate.h"
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include "common.h"
#include "qwmiconselector.h"
#include "qwmdatatableview.h"
#include "qwmabstracteditor.h"
QWMIconDelegate::QWMIconDelegate(QObject * parent):QWMAbstractDelegate(parent)
{

}

QWMIconDelegate::~QWMIconDelegate()
{
    qDebug()<<"delegate destructor";
}

QWidget *QWMIconDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWMIconSelector *editor = new QWMIconSelector(parent);
    editor->setModal(true);
    connect(editor,&QWMIconSelector::rejected,this,&QWMIconDelegate::closeEditorAndRevert);
    connect(editor,&QWMIconSelector::accepted,this,&QWMIconDelegate::commitAndCloseEditor);
    QWMRotatableProxyModel  *  model=(QWMRotatableProxyModel*)index.model();
    QString title=  model->fieldTitle(index);
    editor->setWindowTitle(title);
    return editor;
}

void QWMIconDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QWMIconSelector *iconSelector = static_cast<QWMIconSelector*>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    QVariant data = index.data(DATA_ROLE);
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

bool QWMIconDelegate::isEditor(QObject *widget)
{
    if(instanceof<QWMIconSelector>(widget))
        return true;
    else
        return false;
}

