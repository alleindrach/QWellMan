#ifndef QWMLIBTABSELECTOR_H
#define QWMLIBTABSELECTOR_H
#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <qwmlibselector.h>
#include <QDebug>
#include "qwmabstracteditor.h"
#include "common.h"
#include "qwmapplication.h"
namespace Ui {
class QWMLibTabSelector;
}

class QWMLibTabSelector : public QWMAbstractEditor
{
    Q_OBJECT

public:
    explicit QWMLibTabSelector(QString table,QString lib,QWidget *parent = nullptr);
    virtual void setValue(QVariant v) override;
    virtual QVariant value() override;
    QWMLibSelector * currentWidget();
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QSqlRecord selectedRecord();
    virtual QList<QWidget *> taborders() override;
    virtual QSize sizeHint() override;
    virtual Type type() override;
    virtual void init();
    inline void setEditable(bool v){
        _editable=v;
    }
    void setLookupFld(QString fld);
private:
    void  setText(QString text);
    Ui::QWMLibTabSelector *ui;
    QString _selectedValue;
    bool _editable{false};
    QString _title;
    QString _table;
    Type _type;
private slots:
    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);

};

#endif // QWMLIBTABSELECTOR_H
