#include "spinboxdelegate.h"
#include "QSpinBox"
#include "qwmrotatableproxymodel.h"
#include "mdlfield.h"
#include "common.h"
#include "mdldao.h"
#include "qwmtablemodel.h"
#include "qwmsortfilterproxymodel.h"
SpinBoxDelegate::SpinBoxDelegate(int decimals,QObject *parent)
    : QItemDelegate(parent),m_decimals(decimals)
{
}
QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem & /*option*/ ,
                                       const QModelIndex & index ) const
{

    QWMRotatableProxyModel * model= ( QWMRotatableProxyModel *)index.model();
//    SX(sourceModel,model);
//    MDLField *  fieldInfo=UDL->fieldInfo(model->tableName(),model->fieldName(index));

    QWidget *widget = nullptr;
    if(m_decimals>0)
    {
        QDoubleSpinBox * editor=new QDoubleSpinBox(parent);
//        editor->setMinimum(field->inputRangeMin);
//        editor->setMaximum(field->inputRangeMax);
        editor->setDecimals(m_decimals);
        widget=editor;
    }else
    {
        QSpinBox * editor=new QSpinBox(parent);
//        editor->setMinimum(field->inputRangeMin);
//        editor->setMaximum(field->inputRangeMax);
        widget=editor;
    }

    return widget;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    if(m_decimals==0){
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }else{
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    }
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    if(m_decimals==0){
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        int oldValue=index.data().toInt();
        if(value!=oldValue)
            model->setData(index, value, Qt::EditRole);
    }else{
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        double oldValue=index.data().toDouble();
        if(value!=oldValue)
            model->setData(index, value, Qt::EditRole);
    }
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
