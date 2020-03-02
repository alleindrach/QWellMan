#ifndef QWMABSTRACTEDITOR_H
#define QWMABSTRACTEDITOR_H
#include <QCoreApplication>
#include <QDialog>
#include <QList>
#include <QFileInfo>
#include <QListWidgetItem>
#include "qwmabstractdelegate.h"

class QWMAbstractEditor : public QDialog
{
    Q_OBJECT
public:
    explicit QWMAbstractEditor( QWidget *parent = nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QList<QWidget *> taborders()=0;
    void installEventFilters();
    void focusNext();
    void focusPrev();
signals:
    void  accepted(QWidget * );
    void  rejected(QWidget * );
private slots:

protected:
//    virtual void showEvent(QShowEvent *event);
private:

    QWMAbstractDelegate * _delegate{nullptr};
    friend class QWMAbstractDelegate;
};

#endif // QWMICONSELECTOR_H
