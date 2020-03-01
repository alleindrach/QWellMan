#ifndef QWMICONDELEGATE_H
#define QWMICONDELEGATE_H

#include <QStyledItemDelegate>
#include <QCoreApplication>
#include "qwmiconselector.h"
class QWMIconDelegate : public QStyledItemDelegate
{
public:
    QWMIconDelegate( QObject * parent=nullptr);
    ~QWMIconDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual bool editorEvent(QEvent *event,
                             QAbstractItemModel *model,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index);
public slots:
    void commitAndCloseEditor(QWMIconSelector * editor);
    void justCloseEditor(QWMIconSelector * editor);
signals:
    void accept();
    void reject();
};


#endif // QWMICONDELEGATE_H
