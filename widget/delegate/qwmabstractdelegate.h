#ifndef QWMABSTRACTDELEGATE_H
#define QWMABSTRACTDELEGATE_H
#include <QStyledItemDelegate>
#include <QCoreApplication>
//#include "qwmiconselector.h"
//template<typename Base, typename T>
//inline bool instanceof(const T *ptr) {
//    return dynamic_cast<const Base*>(ptr) != nullptr;
//}

class QWMAbstractDelegate : public QStyledItemDelegate
{
public:
    QWMAbstractDelegate( QObject * parent=nullptr);
    ~QWMAbstractDelegate();

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override=0;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override =0;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override =0;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual bool editorEvent(QEvent *event,
                             QAbstractItemModel *model,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) override;
    virtual bool isEditor(QObject  * widget )=0;
public slots:
    virtual void commitAndCloseEditor(QWidget * editor);
    virtual void closeEditorAndRevert(QWidget * editor);
signals:
    void accept();
    void reject();
};
#endif // QWMABSTRACTDELEGATE_H
