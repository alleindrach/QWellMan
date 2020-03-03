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
    virtual QList<QWidget *> taborders() override;
    inline QWMLibSelector *selector(){
        return _selector;
    }
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
};


#endif // QWMLIBSINGLESELECTOR_H
