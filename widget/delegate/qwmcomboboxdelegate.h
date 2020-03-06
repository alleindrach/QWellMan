#ifndef QWMCOMBOBOXDELEGATE_H
#define QWMCOMBOBOXDELEGATE_H
#include <QObject>
#include <QStyledItemDelegate>
#include <QVector>
#include <QPair>
class QWMComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QWMComboBoxDelegate(QList<QPair<QString,QVariant>> options,bool editable=false, QObject * parent=nullptr);
    ~QWMComboBoxDelegate();

    QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    QList<QPair<QString,QVariant>> m_options;
    bool _editable{false};
};

#endif // QWMCOMBOBOXDELEGATE_H
