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
    enum TYPE{ Plain,Tab,BiTab,SigleStepRecord,TwoStepRecord};
    Q_ENUM(TYPE);
    Q_INVOKABLE   QWMRefLookupDelegate(QString table,QString lib,QString disp,QString title,bool editable=false,QObject * parent=nullptr);
    Q_INVOKABLE   QWMRefLookupDelegate(QStringList tables,QString title,QString  idwell,TYPE typ=SigleStepRecord, QObject * parent=nullptr);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual bool isEditor(QObject  * widget ) override;
private :
    QString _lib;
    QString _disp;
    QString _title;
    QStringList _tables;
    QString _idwell;
    QString _table;
    TYPE _type{Plain};
    bool _editable{false};
    void handleNeighbourField(QWidget *editor,QAbstractItemModel *model,
                              const QModelIndex &index,QModelIndex aIndex, QList<QPair<QString,QVariant>>&) const ;

};

#endif // QWMLIBLOOKUPDELEGATE_H
