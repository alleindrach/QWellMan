#ifndef QWMPROGRESSDIALOG_H
#define QWMPROGRESSDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
namespace Ui {
class QWMProgressDialog;
}
class QWMProgressDialog : public QDialog
{
    Q_OBJECT
public:
    Q_INVOKABLE QWMProgressDialog(QWidget * parent=nullptr);
public slots:
    void on_progress(QString message,int progress);
private:
     Ui::QWMProgressDialog *ui;
};

#endif // QWMPROGRESSDIALOG_H
