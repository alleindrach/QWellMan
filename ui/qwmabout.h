#ifndef QWMABOUT_H
#define QWMABOUT_H

#include <QWidget>
#include <QDialog>
namespace Ui {
class QWMAbout;
}
class QWMAbout : public QDialog
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE QWMAbout(QWidget * parent);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Ui::QWMAbout *ui;
};

#endif // QWMABOUT_H
