#include "qwmdatatableview.h"
#include "QDebug"
QWMDataTableView::QWMDataTableView(QWidget *parent):QTableView(parent)
{

}

void QWMDataTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
}

void QWMDataTableView::bindDelegate()
{

}

QRect QWMDataTableView::visualRect(const QModelIndex &index) const{
    qDebug()<<"VisualRect:"<<index.row()<<","<<index.column();
    return QTableView::visualRect(index);
}

QModelIndex QWMDataTableView::indexAt(const QPoint &pos) const
{

    return QTableView::indexAt(pos);
}
