#ifndef QWMLIBLOOKUPDELEGATE_H
#define QWMLIBLOOKUPDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <qwmlibselector.h>
class QWMLibLookupDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum TYPE{ Plain,Tab};
    Q_ENUM(TYPE);
    Q_INVOKABLE   QWMLibLookupDelegate(QString lib,QString disp,QString title,bool editable=false,QObject * parent=nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void handleNeighbourField(QWidget *editor,QAbstractItemModel *model,
                              const QModelIndex &index,QModelIndex aIndex) const ;
public slots:
    void commitAndCloseEditor(QWMLibSelector * editor);
    void justCloseEditor(QWMLibSelector * editor);
signals:
    void accept();
    void reject();
private :
    QString _lib;
    QString _disp;
    QString _title;
    TYPE _type{Plain};
    bool _editable{false};
};

#endif // QWMLIBLOOKUPDELEGATE_H
