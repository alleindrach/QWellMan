#ifndef QWMDATAEDITOR_H
#define QWMDATAEDITOR_H

#include <QDialog>

namespace Ui {
class QWMDataEditor;
}

class QWMDataEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QWMDataEditor(QString idWell,QWidget *parent = nullptr);
    ~QWMDataEditor();

private:
    Ui::QWMDataEditor *ui;
    QString _idWell;
};

#endif // QWMDATAEDITOR_H
