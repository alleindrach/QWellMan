#ifndef QWMABSTRACTEDITOR_H
#define QWMABSTRACTEDITOR_H
#include <QCoreApplication>
#include <QDialog>
#include <QList>
#include <QFileInfo>
#include <QListWidgetItem>
#include "qwmabstractdelegate.h"
#include "common.h"
#include "qwmapplication.h"
class QWMAbstractEditor : public QDialog
{
    Q_OBJECT
public:
    enum Type{Simple,Tab,BiTab,Tree,BiTree,Icon,DateTime};
    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
    explicit QWMAbstractEditor( QWidget *parent = nullptr);
    ~QWMAbstractEditor(){
        APP->removeCachedEditor(this->key());
    }
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QList<QWidget *> taborders()=0;
    void installEventFilters();
    void focusNext();
    void focusPrev();
    virtual void on_btn_clicked();
    virtual QSize sizeHint()  ;
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void moveEvent(QMoveEvent *event) override;
    inline void setKey(QString value){
        _key=value;
    }
    inline QString key(){
        return _key;
    }
    virtual void setTitle(QString title);
    virtual QVariant value()=0;
    virtual void  setValue(QVariant v)=0;
    virtual void init()=0;
    virtual Type type()=0;
signals:
    void  accepted(QWidget * );
    void  rejected(QWidget * );
private slots:

protected:
    bool event(QEvent *event) override;
    //    virtual void showEvent(QShowEvent *event);
    QString _title;
private:
    QString _key{QString()};
    QWMAbstractDelegate * _delegate{nullptr};
    friend class QWMAbstractDelegate;
};

#endif // QWMICONSELECTOR_H
