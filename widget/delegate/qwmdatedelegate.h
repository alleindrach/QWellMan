#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H
#include <QObject>
#include "QItemDelegate"
#include "qwmdatetimeeditor.h"
#include "qwmabstractdelegate.h"
class QWMDateDelegate : public QWMAbstractDelegate
{
    Q_OBJECT
public:
    QWMDateDelegate(int mode=DATE,QString format="yyyy-MM-dd", QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QList<QWidget *> taborders();
    enum MODE{ DATE,TIME,DATETIME} ;
    MODE m_mode;
    QString m_format;
    virtual bool isEditor(QObject  * widget ) override;
private:
    QWMDateTimeEditor * _editor;
};
#endif // DATEDELEGATE_H
