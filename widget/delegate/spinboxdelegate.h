#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QObject>
#include <QWidget>
#include "QItemDelegate"
class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    SpinBoxDelegate(int decimals=0,QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
private :
    int m_decimals;
};

#endif // SPINBOXDELEGATE_H
