#ifndef QWMMAIN_H
#define QWMMAIN_H

#include <QMainWindow>
#include <QSqlDatabase>
QT_BEGIN_NAMESPACE
namespace Ui { class QWMMain; }
QT_END_NAMESPACE

class QWMMain : public QMainWindow
{
    Q_OBJECT

public:
    QWMMain(QWidget *parent = nullptr);
    ~QWMMain();

private slots:
    void on_actionChangeDB_triggered();

private:
    Ui::QWMMain *ui;
};
#endif // QWMMAIN_H
