#ifndef QWMLIBTABSELECTOR_H
#define QWMLIBTABSELECTOR_H
#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <qwmlibselector.h>
namespace Ui {
class QWMLibTabSelector;
}

class QWMLibTabSelector : public QWidget
{
    Q_OBJECT
public:
    explicit QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    void  setText(QString text);
    QWMLibSelector * currentWidget();
private:
     Ui::QWMLibTabSelector *ui;
     QString _selectedValue;
     bool _editable{false};
     QString _title;

//     int _col;
signals:
    void  accepted(QWMLibSelector * );
    void  rejected(QWMLibSelector * );
private slots:
    void on_tab_accepted(QWMLibSelector *);
    void on_tab_recjected(QWMLibSelector *);
    void on_tabWidget_tabBarClicked(int index);

};

#endif // QWMLIBTABSELECTOR_H
