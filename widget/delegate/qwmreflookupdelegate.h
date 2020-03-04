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
    enum TYPE{ Plain,Tab,SigleStepRecord,MultiStepRecord};
    Q_ENUM(TYPE);
    Q_INVOKABLE   QWMRefLookupDelegate(QString lib,QString disp,QString title,bool editable=false,QObject * parent=nullptr);
    Q_INVOKABLE   QWMRefLookupDelegate(QStringList tables,QString title,QString  idwell,TYPE typ=SigleStepRecord, QObject * parent=nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
//    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void handleNeighbourField(QWidget *editor,QAbstractItemModel *model,
                              const QModelIndex &index,QModelIndex aIndex) const ;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual bool isEditor(QObject  * widget ) override;
private :
    QString _lib;
    QString _disp;
    QString _title;
    QStringList _tables;
    QString _idwell;
    TYPE _type{Plain};
    bool _editable{false};
};

#endif // QWMLIBLOOKUPDELEGATE_H
