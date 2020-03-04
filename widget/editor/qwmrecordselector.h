#ifndef QWMRECORDSELECTOR_H
#define QWMRECORDSELECTOR_H
#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSqlQueryModel>
#include "qwmabstracteditor.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
namespace Ui {
class QWMRecordSelector;
}
class QWMRecordSelector : public QWidget
{
    Q_OBJECT
public:
    explicit QWMRecordSelector(QStringList tables,QString idwell,QString title,QWidget *parent = nullptr);
    void  setText(QString text);
    QString text();
    QItemSelectionModel * selectionModel();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    QList<QWidget *> taborders()  ;
private:
    void init();
    Ui::QWMRecordSelector *ui;
    QString _selectedValue;
    QString _title;
    QStringList _tables;
    QString _idWell;
    void  loadTable(QString table,QStandardItemModel * tvModel);
    void  loadChildTable(QStandardItem* parent,QStack<QString> trace);
signals:
    void  accepted(QWidget * );
    void  rejected(QWidget * );
private slots:
    void on_item_doubleclick(const QModelIndex &index);
};


#endif // QWMRECORDSELECTOR_H
