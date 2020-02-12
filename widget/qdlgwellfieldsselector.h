#ifndef QDLGWELLFIELDSSELECTOR_H
#define QDLGWELLFIELDSSELECTOR_H

#include <QDialog>

namespace Ui {
class QDlgWellFieldsSelector;
}

class QDlgWellFieldsSelector : public QDialog
{
    Q_OBJECT

public:
    explicit QDlgWellFieldsSelector(QStringList selected, QWidget *parent = nullptr);
    ~QDlgWellFieldsSelector();
    QStringList selectedFields();
    void setSelectedFields(QStringList v);
private slots:
    void on_addBtn_clicked();

    void on_delBtn_clicked();

private:
    Ui::QDlgWellFieldsSelector *ui;
    QStringList _selectedFields;

};

#endif // QDLGWELLFIELDSSELECTOR_H
