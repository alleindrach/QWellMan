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
    enum Type{S,B};
    //    explicit QWMLibTabSelector(QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    explicit QWMLibTabSelector(QString table,QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    explicit QWMLibTabSelector(QStringList lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
//    ~QWMLibTabSelector(){
//        qDebug()<<"destory QWMLibTabSelector";
//    }
    void  setText(QString text);
    QWMLibSelector * currentWidget();
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QSqlRecord selectedRecord();
    virtual QList<QWidget *> taborders() override;
    virtual QSize sizeHint();
    inline Type type(){return _type;};
private:
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
