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
    explicit QWMLibSingleSelector(QString table,QString lib,QWidget *parent = nullptr);

    const QItemSelectionModel* selectionModel();
    virtual QSqlRecord selectedRecord();
    virtual QList<QWidget *> taborders() override;
    inline QWMLibSelector *selector(){
        return _selector;
    }
    inline void setEditable(bool v){
        _editable=v;
    }
    inline void setLookupFld(QString fld){
        _selector->setLookupFld(fld);
    }
    virtual void setValue(QVariant v) override;
    virtual QVariant value() override;
    virtual void init() override;
    virtual QWMAbstractEditor::Type type() override;
private:
    void  setText(QString text);
    QString text();
    Ui::QWMLibSingleSelector *ui;
    QString _selectedValue;
    bool _editable{false};
    QString _table;
    int _col;

    QWMLibSelector * _selector;
private slots:
    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);
};


#endif // QWMLIBSINGLESELECTOR_H
