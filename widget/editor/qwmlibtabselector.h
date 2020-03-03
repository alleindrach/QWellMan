#ifndef QWMLIBTABSELECTOR_H
#define QWMLIBTABSELECTOR_H
#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <qwmlibselector.h>
#include "qwmabstracteditor.h"
namespace Ui {
class QWMLibTabSelector;
}

class QWMLibTabSelector : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    explicit QWMLibTabSelector(QStringList lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);

    void  setText(QString text);
    QWMLibSelector * currentWidget();
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QList<QWidget *> taborders() override;
private:
     Ui::QWMLibTabSelector *ui;
     QString _selectedValue;
     bool _editable{false};
     QString _title;
private slots:
    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);

};

#endif // QWMLIBTABSELECTOR_H
