#include "qwmstyleditemdelegate.h"
#include "common.h"
#include <QMetaObject>
#include <QMetaProperty>
QWMStyledItemDelegate::QWMStyledItemDelegate(QObject* parent):QStyledItemDelegate(parent)
{

}

void QWMStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant v = index.data(DATA_ROLE);
    QByteArray n = editor->metaObject()->userProperty().name();

    if (!n.isEmpty()) {
        if (!v.isValid())
            v = QVariant(editor->property(n).userType(), (const void *)0);
        editor->setProperty(n, v);
    }
}
