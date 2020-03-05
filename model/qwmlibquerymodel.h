#ifndef QWMLIBQUERYMODEL_H
#define QWMLIBQUERYMODEL_H

#include <QSqlQueryModel>
#include <QObject>
#include "common.h"
#include "qwmapplication.h"
#include "mdldao.h"
#include "mdlfield.h"
#include "mdltable.h"
class QWMLibQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit QWMLibQueryModel(QString baseTable  /*使用此lib做录入的基本表名*/,QString libTable/*lib表表名*/, QObject *parent = nullptr);
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
private:
    QString _baseTable;
    QString _libTable;
};

#endif // QWMLIBQUERYMODEL_H
