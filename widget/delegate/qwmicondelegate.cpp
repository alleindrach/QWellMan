#include "qwmicondelegate.h"
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include "common.h"
#include "qwmiconselector.h"
#include "qwmdatatableview.h"
#include "qwmabstracteditor.h"
#include <QPainter>
#include <QBitmap>
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

void QWMIconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString iconName=index.data().toString();
    if(iconName.isNull()||iconName.isEmpty())
        return QWMAbstractDelegate::paint(painter,option,index);

    QFileInfo fi=QWMIconSelector::findIcon(iconName);
    if(!fi.isFile()){
        return QWMAbstractDelegate::paint(painter,option,index);
    }
    //! [4]

    //! [5]
    painter->save();
    //! [5] //! [6]
    painter->setRenderHint(QPainter::Antialiasing, true);
    //! [6] //! [7]
    painter->setPen(Qt::NoPen);
    //! [7] //! [8]
    if (option.state & QStyle::State_Selected)
        //! [8] //! [9]
        painter->setBrush(option.palette.highlightedText());
    else
        //! [2]
        painter->setBrush(option.palette.text());
    //! [9]

    //! [10]
    //!
    QPixmap pixmap(fi.absoluteFilePath());
    pixmap=pixmap.scaled(option.rect.width(),option.rect.height(),Qt::KeepAspectRatio);

    QSize sp=pixmap.size();
    int x=option.rect.left()+(option.rect.width()-sp.width())/2;
    int y=option.rect.top()+(option.rect.height()-sp.height())/2;
    QRect r(x,y,sp.width(),sp.height());

    painter->drawPixmap(r,pixmap);

    painter->restore();
}

