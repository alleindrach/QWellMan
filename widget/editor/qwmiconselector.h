#ifndef QWMICONSELECTOR_H
#define QWMICONSELECTOR_H
#include <QCoreApplication>
#include <QWidget>
#include <QList>
#include <QFileInfo>
#include <QListWidgetItem>
namespace Ui {
class QWMIconSelector;
}

class QWMIconSelector : public QWidget
{
    Q_OBJECT
public:
    explicit QWMIconSelector(QWidget *parent = nullptr);

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    void selectFile(QString  filename);
    QString file();
    QList<QFileInfo> folders();
    QList<QFileInfo> files(QString subFolder);
signals:
    void  accepted(QWMIconSelector * );
    void  rejected(QWMIconSelector * );
private slots:
    //    void on_btn_clicked(QAbstractButton *button);
    void on_btn_clicked();
    void on_return_pressed();
    void on_item_doubleclick(const QModelIndex &index);
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_item_click(const QModelIndex &);

    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_listWidget_itemSelectionChanged();

private:
    Ui::QWMIconSelector *ui;
    QList<QFileInfo> _files;
    QString  _defaultDir{QCoreApplication::applicationDirPath()+"/pceicons"};
    void showIcon(QFileInfo );
};

#endif // QWMICONSELECTOR_H
