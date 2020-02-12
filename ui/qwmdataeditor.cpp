#include "qwmdataeditor.h"
#include "ui_qwmdataeditor.h"

QWMDataEditor::QWMDataEditor(QString idWell,QWidget *parent) :
    QDialog(parent),_idWell(idWell),
    ui(new Ui::QWMDataEditor)
{
    ui->setupUi(this);
}

QWMDataEditor::~QWMDataEditor()
{
    delete ui;
}
