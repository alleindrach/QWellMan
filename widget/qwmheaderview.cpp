#include "qwmheaderview.h"
#include <QDebug>
#include <QWidget>
QWMHeaderView::QWMHeaderView(Qt::Orientation orientation, QWidget *parent ):QHeaderView(orientation,parent)
{

}

QWMHeaderView::~QWMHeaderView()
{

}

void QWMHeaderView::setModel(QAbstractItemModel *model)
{
    QHeaderView::setModel(model);
    if(this->orientation()==Qt::Horizontal){
        QObject::disconnect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),0,0);
        QObject::connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),
                                     this, SLOT(sectionsInserted(QModelIndex,int,int)));
    }

}

void QWMHeaderView::sectionsInserted(const QModelIndex &parent,
                                   int logicalFirst, int logicalLast)
{
    qDebug()<<"sectionsInserted:"<<logicalFirst<<","<<logicalLast;
    return QHeaderView::sectionsInserted(parent,logicalFirst,logicalLast);
}
