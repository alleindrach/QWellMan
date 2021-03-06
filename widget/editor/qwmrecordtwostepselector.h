#ifndef QWMRECORDTWOSTEPSELECTOR_H
#define QWMRECORDTWOSTEPSELECTOR_H
#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSqlQueryModel>
#include "qwmabstracteditor.h"
#include "qwmrecordselector.h"
#include "qwmrecordselector.h"
namespace Ui {
class QWMRecordTwoStepSelector;
}
class QWMRecordTwoStepSelector : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMRecordTwoStepSelector(QString idwell,QWidget *parent = nullptr);
    ~QWMRecordTwoStepSelector();
    virtual void setValue(QVariant v) override;
    virtual QVariant value() override;

    const QItemSelectionModel* selectionModel();
    virtual QList<QWidget *> taborders() override;
    inline QWMRecordSelector *selector(){
        return _selector;
    }
    virtual QSize sizeHint() override;
    virtual Type type() override;
    virtual void init() override;;
    virtual void on_btn_clicked() override;
private:
    Ui::QWMRecordTwoStepSelector *ui;
    QString _selectedValue;
    QString _title;
    QString _idWell;
    QWMRecordSelector * _selector;
    void  nav2RecordSelector();
    void  nav2TableSelector();
    void  setText(QString text);
    QString text();
private slots:

    void on_tab_accepted(QWidget *);
    void on_tab_recjected(QWidget *);
//    void on_btn_clicked();

};


#endif // QWMRECORDTWOSTEPSELECTOR_H
