#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H
#include "QStyledItemDelegate"
#include "QItemDelegate"
#include "tablemodel.h"
#include "defines.h"
#include "table.h"
#include "qwelltablemodel.h"

class  Ref : public QObjectUserData {
    int _row;  // ID
    QString _value;  //
public:
    Ref(int row,QString value):_row(row),_value(value){

    };
    int row(){
        return  _row;
    }
    QString value(){
        return _value;
    }

};

class LibraryDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    LibraryDelegate(QWellTableModel * model,LibraryModel * refModel,QString keyField, QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWellTableModel * m_libmodel;
    LibraryModel * m_refModel;
    QString m_keyField;
    QString m_refKeyField;
private slots:
    void commitAndCloseEditor(const QModelIndex &index);
};

#endif // COMBODELEGATE_H
