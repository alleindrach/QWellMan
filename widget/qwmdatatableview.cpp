#include "qwmdatatableview.h"

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
