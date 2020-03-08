#ifndef QWMLIBLOOKUPDELEGATE_H
#define QWMLIBLOOKUPDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <qwmlibselector.h>
#include <qwmabstractdelegate.h>
class QWMRefLookupDelegate : public QWMAbstractDelegate
{
    Q_OBJECT
public:
    Q_INVOKABLE   QWMRefLookupDelegate(QObject * parent=nullptr);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual bool isEditor(QObject  * widget ) override;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
private :

};

#endif // QWMLIBLOOKUPDELEGATE_H
