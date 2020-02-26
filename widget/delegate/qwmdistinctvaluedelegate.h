#ifndef QWMDISTINCTVALUEDELEGATE_H
#define QWMDISTINCTVALUEDELEGATE_H
#include <QStyledItemDelegate>
#include <QVector>
#include <QPair>
#include <QObject>

class QWMDistinctValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QWMDistinctValueDelegate(QString table,QString  field, QObject * parent=nullptr);
    ~QWMDistinctValueDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    QString _table;
    QString _field;
};

#endif // QWMDISTINCTVALUEDELEGATE_H
