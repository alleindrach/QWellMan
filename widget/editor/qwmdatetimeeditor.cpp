#include "qwmdatetimeeditor.h"
#include "ui_qwmdatetimeeditor.h"
#include "QDialogButtonBox"
#include "qwmdatedelegate.h"
#include "common.h"
#include "QKeyEvent"
QWMDateTimeEditor::QWMDateTimeEditor(QWidget *parent) : QWMAbstractEditor(parent),ui(new Ui::QWMDateTimeEditor)
{
    ui->setupUi(this);
    connect(ui->btnOK,&QPushButton::clicked,this,&QWMDateTimeEditor::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMDateTimeEditor::on_btn_clicked);
    connect(ui->btnReset,&QPushButton::clicked,this,&QWMDateTimeEditor::on_btn_clicked);
    ui->calendarWidget->installEventFilter(this);
    ui->btnOK->setDefault(true);
    init();
}

bool QWMDateTimeEditor::eventFilter(QObject *watched, QEvent *event)
{
    QList<QWidget *> ws=taborders();
    if(IS_KEY_EVENT(event)){
        if(IS_KEY(event,Qt::Key_Tab)||IS_KEY(event,Qt::Key_Backtab)){
            int index=ws.indexOf((QWidget*)watched);
            int next=(index+1)%ws.size();
            int prev=(index+ws.size()-1)%ws.size();
            if(index>=0){
                if(IS_KEY(event,Qt::Key_Tab))
                    ws[next]->setFocus();
                else
                    ws[prev]->setFocus();
            }
            event->accept();
            return true;
        }
        if(IS_KEY(event,Qt::Key_Return)||IS_KEY(event,Qt::Key_Enter)){
            emit this->accepted(this);
            event->accept();
            return true;
        }
        if (KEY_MATCHED(event,QKeySequence::Cancel)) {
            // don't commit data
            emit this->rejected(this);
            event->accept();
            return true;
        }
    }
    return false;
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

void QWMDateTimeEditor::on_btn_clicked()
{
    QPushButton * button=qobject_cast<QPushButton*>(sender());
    if(button->objectName()=="btnReset")
    {
        resetToNow();
    }else if(button->objectName()=="btnOK"){
        emit this->accepted(this);
    }else if(button->objectName()=="btnCancel"){
        emit this->rejected(this);
    }
}
QList<QWidget *> QWMDateTimeEditor::taborders()
{
    QList<QWidget*> results;
    results<<ui->calendarWidget<<ui->timeEdit<<ui->btnReset<<ui->btnCancel<<ui->btnOK;
    return results;
}

void QWMDateTimeEditor::setValue(QVariant v)
{
    QDateTime date=v.toDateTime();
    ui->calendarWidget->setSelectedDate(date.date());
    ui->timeEdit->setDateTime(date);
}

QVariant QWMDateTimeEditor::value()
{
    return this->dateTime();
}

void QWMDateTimeEditor::init()
{
}

