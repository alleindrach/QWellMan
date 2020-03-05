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
    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
    explicit QWMAbstractEditor( QWidget *parent = nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QList<QWidget *> taborders()=0;
    void installEventFilters();
    void focusNext();
    void focusPrev();
    virtual void on_btn_clicked();
    virtual QSize sizeHint();
    void resizeEvent(QResizeEvent *) override;
    inline void setKey(QString value){
        _key=value;
    }
    inline QString key(){
        return _key;
    }
signals:
    void  accepted(QWidget * );
    void  rejected(QWidget * );
private slots:

protected:
//    virtual void showEvent(QShowEvent *event);

private:
    QString _key{QString()};
    QWMAbstractDelegate * _delegate{nullptr};
    friend class QWMAbstractDelegate;
};

#endif // QWMICONSELECTOR_H
