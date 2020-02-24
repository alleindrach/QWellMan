#ifndef QWMDATETIMEDELEGATE_H
#define QWMDATETIMEDELEGATE_H
#include <QDateTime>
#include <QWidget>
#include <QAbstractButton>
namespace Ui {
class QWMDateTimeEditor;
}

class QWMDateTimeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit QWMDateTimeEditor(QDateTime date=QDateTime::currentDateTime(), QWidget *parent = nullptr);
    void setDateTime(QDateTime value);
    QDateTime dateTime();
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
signals:
    void  accepted(QWMDateTimeEditor * );
    void  rejected(QWMDateTimeEditor * );
private:
     Ui::QWMDateTimeEditor *ui;
//     QDateTime _date;
private slots:
    void resetToNow();
    void on_btn_clicked(QAbstractButton *button);

};

#endif // QWMDATETIMEDELEGATE_H
