#ifndef QWMLIBSINGLESELECTOR_H
#define QWMLIBSINGLESELECTOR_H

#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSqlQueryModel>
#include "qwmabstracteditor.h"
#include "qwmlibselector.h"
namespace Ui {
class QWMLibSingleSelector;
}
class QWMLibSingleSelector : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMLibSingleSelector(QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    void  setText(QString text);
    QString text();
    const QItemSelectionModel* selectionModel();
//    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual QList<QWidget *> taborders() override;
    inline QWMLibSelector *selector(){
        return _selector;
    }
//    virtual void showEvent(QShowEvent *event)  override;
//    virtual void focusInEvent(QFocusEvent *event) override;
private:
    Ui::QWMLibSingleSelector *ui;
     QString _selectedValue;
     bool _editable{false};
     QString _title;
     QString _lookupFld;
     int _col;

     QWMLibSelector * _selector;
private slots:
    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);
//    void on_btn_clicked();
//    void on_text_changed(const QString &);
//    void on_return_pressed();
//    void on_item_doubleclick(const QModelIndex &index);
};


#endif // QWMLIBSINGLESELECTOR_H
