#ifndef QWMRECORDSINGLESTEPSELECTOR_H
#define QWMRECORDSINGLESTEPSELECTOR_H

#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSqlQueryModel>
#include "qwmabstracteditor.h"
#include "qwmrecordselector.h"
#include "qwmrecordselector.h"
namespace Ui {
class QWMRecordSingleStepSelector;
}
class QWMRecordSingleStepSelector : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMRecordSingleStepSelector(QStringList tables,QString idwell,QWidget *parent = nullptr);

    const QItemSelectionModel* selectionModel();
    virtual QList<QWidget *> taborders() override;
    inline QWMRecordSelector *selector(){
        return _selector;
    }
    virtual QSize sizeHint() override;
    virtual Type type() override;
    virtual void init() override;;
    virtual QVariant value()  override;
    virtual void  setValue(QVariant v) override;
private:
    Ui::QWMRecordSingleStepSelector *ui;
    QString _selectedValue;
    QString _title;
    QString _idWell;
    QStringList _tables;
    QWMRecordSelector * _selector;
    void  setText(QString text);
    QString text();
private slots:
    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);
//    void on_btn_clicked();

};


#endif // QWMRECORDSINGLESTEPSELECTOR_H
