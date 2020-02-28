#include "qwmheaderview.h"
#include <QDebug>
#include <QWidget>
#include "qwmrotatableproxymodel.h"
QWMHeaderView::QWMHeaderView(Qt::Orientation orientation, QWidget *parent ):QHeaderView(orientation,parent)
{

}

QWMHeaderView::~QWMHeaderView()
{

}

void QWMHeaderView::setModel(QAbstractItemModel *model)
{
    QHeaderView::setModel(model);
    QWMRotatableProxyModel  * rmodel=qobject_cast<QWMRotatableProxyModel*>(model);
    if(this->orientation()==Qt::Horizontal){

//        QObject::disconnect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),0,0);
//        QObject::connect(model,SIGNAL(columnsInserted(QModelIndex,int,int)),rmodel, SLOT(columnsInsertedAdaptor(QModelIndex,int,int)));
//        QObject::connect(rmodel, SIGNAL(columnsInserted(QModelIndex,int,int)),
//                                     this, SLOT(sectionsInserted(QModelIndex,int,int)));

//        QObject::disconnect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),0,0);
//        QObject::connect(model,SIGNAL(columnsRemoved(QModelIndex,int,int)),rmodel, SLOT(columnsRemovedAdaptor(QModelIndex,int,int)));
//        QObject::connect(rmodel, &QWMRotatableProxyModel::columnsRemoved,
//                                     this, &QHeaderView::section);

    }

}

void QWMHeaderView::sectionsInserted(const QModelIndex &parent,
                                   int logicalFirst, int logicalLast)
{
    qDebug()<<"sectionsInserted:"<<logicalFirst<<","<<logicalLast;
    return QHeaderView::sectionsInserted(parent,logicalFirst,logicalLast);
}
