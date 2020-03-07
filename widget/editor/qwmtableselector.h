#ifndef QWMTABLESELECTOR_H
#define QWMTABLESELECTOR_H
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
class QWMTableSelector;
}
class QWMTableSelector : public QWidget
{
    Q_OBJECT
public:
    explicit QWMTableSelector(QWidget *parent = nullptr);
    void  setText(QString text);
    QString text();
    QItemSelectionModel * selectionModel();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    QList<QWidget *> taborders()  ;
private:
    void init();
    Ui::QWMTableSelector *ui;
    QString _selectedValue;
signals:
    void  accepted(QWidget * );
    void  rejected(QWidget * );
private slots:
    void on_item_doubleclick(const QModelIndex &index);
};
#endif // QWMTABLESELECTOR_H
