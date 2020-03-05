#ifndef QWMDATETIMEDELEGATE_H
#define QWMDATETIMEDELEGATE_H
#include <QDateTime>
#include <QDialog>
#include <QAbstractButton>
#include <qwmabstracteditor.h>
namespace Ui {
class QWMDateTimeEditor;
}

class QWMDateTimeEditor : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMDateTimeEditor(QDateTime date=QDateTime::currentDateTime(), QWidget *parent = nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void setDateTime(QDateTime value);
    QDateTime dateTime();
    virtual QList<QWidget *> taborders() override;
signals:
    void  accepted(QWMDateTimeEditor * );
    void  rejected(QWMDateTimeEditor * );
private:
    Ui::QWMDateTimeEditor *ui;
    //     QDateTime _date;
private slots:
    void resetToNow();
    virtual void on_btn_clicked() override;
};

#endif // QWMDATETIMEDELEGATE_H
