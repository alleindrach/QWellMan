#ifndef QWMLIBSELECTOR_H
#define QWMLIBSELECTOR_H

#include <QWidget>
#include <QAbstractButton>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QSqlQueryModel>
namespace Ui {
class QWMLibSelector;
}

class QWMLibSelector : public QWidget
{
    Q_OBJECT
public:
    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
    explicit QWMLibSelector(QString table,QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    explicit QWMLibSelector(QString table,QString lib,QString lookupFld,QString title,QAbstractItemModel * model,QStringList visibleFields=QStringList(),bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    void  setText(QString text);
    QString text();
    void init(QAbstractItemModel * model);
    const QItemSelectionModel* selectionModel();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void showEvent(QShowEvent *event)  override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual QSqlRecord selectedRecord();
    inline QString libName(){
        return _refLibName;
    }
    QList<QWidget *> taborders() ;
private:
     Ui::QWMLibSelector *ui;
     QString _selectedValue;
     bool _editable{false};
     QString _title;
     QString _lookupFld;
     QStringList _visibleFlds;
     int _col;
     QString _refLibName;
     QString _table;
signals:
    void  accepted(QWMLibSelector * );
    void  rejected(QWMLibSelector * );
private slots:
//    void on_btn_clicked(QAbstractButton *button);
    void on_btn_clicked();
    void on_text_changed(const QString &);
    void on_return_pressed();
    void on_item_doubleclick(const QModelIndex &index);
};

#endif // QWMLIBSELECTOR_H
