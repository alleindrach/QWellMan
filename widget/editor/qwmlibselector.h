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
    explicit QWMLibSelector(QString lib,QString lookupFld,QString title,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    explicit QWMLibSelector(QString lib,QString lookupFld,QString title,QSqlQueryModel * model,bool editable=false,QString v=QString(), QWidget *parent = nullptr);
    void  setText(QString text);
    QString text();
    void init(QSqlQueryModel * model);
    const QItemSelectionModel* selectionModel();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void showEvent(QShowEvent *event)  override;
    virtual void focusInEvent(QFocusEvent *event) override;
private:
     Ui::QWMLibSelector *ui;
     QString _selectedValue;
     bool _editable{false};
     QString _title;
     QString _lookupFld;
     int _col;
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
