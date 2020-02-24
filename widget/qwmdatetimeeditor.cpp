#include "qwmdatetimeeditor.h"
#include "ui_qwmdatetimeeditor.h"
#include "QDialogButtonBox"
#include "qwmdatedelegate.h"
QWMDateTimeEditor::QWMDateTimeEditor(QDateTime date,QWidget *parent) : QWidget(parent),ui(new Ui::QWMDateTimeEditor)
{
    ui->setupUi(this);
    ui->calendarWidget->setSelectedDate(date.date());
    ui->timeEdit->setDateTime(date);

    connect(ui->buttonBox,&QDialogButtonBox::clicked,this,&QWMDateTimeEditor::on_btn_clicked);
//    ui->
}
void  QWMDateTimeEditor::resetToNow(){
    QDateTime date=QDateTime::currentDateTime();
    ui->timeEdit->setDateTime(date);
    ui->calendarWidget->setSelectedDate(date.date());

}
void QWMDateTimeEditor::setDateTime(QDateTime value){
    ui->timeEdit->setDateTime(value);
    ui->calendarWidget->setSelectedDate(value.date());
}

QDateTime QWMDateTimeEditor::dateTime()
{
    QDate date=ui->calendarWidget->selectedDate();
    QTime time=ui->timeEdit->time();
    QDateTime result=QDateTime(date,time);
    return result;
}

void QWMDateTimeEditor::focusInEvent(QFocusEvent *event)
{
    return QWidget::focusInEvent(event);
}

void QWMDateTimeEditor::focusOutEvent(QFocusEvent *event)
{
    return QWidget::focusOutEvent(event);
}
void QWMDateTimeEditor::on_btn_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::ResetRole)
    {
        resetToNow();
    }else if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::AcceptRole){
        emit this->accepted(this);
    }else if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::RejectRole){
        emit this->rejected(this);
    }
}
