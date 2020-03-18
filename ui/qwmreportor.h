#ifndef QWMREPORT_H
#define QWMREPORT_H

#include <QMainWindow>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsItem>
namespace Ui {
class QWMReportor;
}
class QWMReportor : public QMainWindow
{
    Q_OBJECT
public:
    explicit QWMReportor(QString idWell,QString title, QWidget *parent = nullptr);
    void init();

    virtual void resizeEvent(QResizeEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    QGraphicsItem*  survyDataSerial(QString survyId,QRectF ticks,QString dataField);
    QGraphicsItem*  survyDataCurve(QString survyId,QRectF ticks,QString dataField);
    virtual void closeEvent(QCloseEvent *event);
signals:
private slots:
    void on_comboWellbore_currentIndexChanged(int index);
    void on_resize_graphicsview();
private:
    Ui::QWMReportor *ui;
    QString _idWell;
    QString _title;

    QGraphicsWidget * _form;
};

#endif // QWMREPORT_H
