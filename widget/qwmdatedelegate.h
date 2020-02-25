#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include "QItemDelegate"
#include "qwmdatetimeeditor.h"
class QWMDateDelegate : public QItemDelegate
{
   Q_OBJECT
public:
    QWMDateDelegate(int mode=DATE,QString format="yyyy-MM-dd", QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    enum MODE{ DATE,TIME,DATETIME} ;
    MODE m_mode;
    QString m_format;
public slots:
    void commitAndCloseEditor(QWMDateTimeEditor * editor);
    void justCloseEditor(QWMDateTimeEditor * editor);
signals:
    void accept();
    void reject();
private:
    QWMDateTimeEditor * _editor;

};
#endif // DATEDELEGATE_H
