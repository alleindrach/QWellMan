#ifndef QWMICONDELEGATE_H
#define QWMICONDELEGATE_H

#include <QStyledItemDelegate>
#include <QCoreApplication>
#include "qwmiconselector.h"
#include "qwmabstractdelegate.h"
class QWMIconDelegate : public QWMAbstractDelegate
{
    Q_OBJECT
public:
    QWMIconDelegate( QObject * parent=nullptr);
    ~QWMIconDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual bool isEditor(QObject  * widget ) override;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
public slots:
};
#endif // QWMICONDELEGATE_H
