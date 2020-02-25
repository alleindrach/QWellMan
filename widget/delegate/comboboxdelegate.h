#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H
#include <QStyledItemDelegate>
#include <QVector>
#include <QPair>
class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QList<QPair<QString,QVariant>> options, QObject * parent=nullptr);
    ~ComboBoxDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    QList<QPair<QString,QVariant>> m_options;
};

#endif // COMBOBOXDELEGATE_H
