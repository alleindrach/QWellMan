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
    explicit QWMLibSelector(QString table,QString lib,QSqlQueryModel * model=nullptr,QWidget *parent = nullptr);
    void setEditable(bool v);
    bool editable();
    void init() ;
    void  setText(QString text);
    QString text();
    inline void setLookupFld(QString  fld){
        _lookupFld=fld;
    }
    void setTitle(QString v);
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
    QString _lookupFld{QString()};
    int _col;
    QString _refLibName;
    QString _table;
    QString  _title;
    QAbstractItemModel * _model;
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
