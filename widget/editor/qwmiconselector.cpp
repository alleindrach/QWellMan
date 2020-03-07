#include "qwmiconselector.h"
#include "ui_qwmiconselector.h"
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDebug>
#include "common.h"
#include "libdao.h"
#include "QSqlQueryModel"
#include <QFileSystemModel>
#include "qwmapplication.h"
#include "qcoreapplication.h"


QWMIconSelector::QWMIconSelector( QWidget *parent) : QWMAbstractEditor(parent),ui(new Ui::QWMIconSelector)
{
    ui->setupUi(this);
    init();
}
void QWMIconSelector::init()
{
    connect(ui->btnOK,&QPushButton::clicked,this,&QWMIconSelector::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMIconSelector::on_btn_clicked);
    connect(ui->listWidget,&QListWidget::doubleClicked,this,&QWMIconSelector::on_item_doubleclick);
    connect(ui->listWidget,&QListWidget::clicked,this,&QWMIconSelector::on_item_click);

    QList<QFileInfo> subFolders=folders();
    ui->comboBox->clear();
    foreach(QFileInfo fileInfo,subFolders){
        ui->comboBox->addItem(fileInfo.completeBaseName(),QVariant::fromValue(fileInfo));
    }
    emit ui->comboBox->currentTextChanged(ui->comboBox->currentText());
}

bool QWMIconSelector::eventFilter(QObject *watched, QEvent *event)
{
//    if(IS_KEY_EVENT(event)){
//        if(IS_KEY(event,Qt::Key_Return)||IS_KEY(event,Qt::Key_Enter)){
//            emit this->accepted(this);
//            event->accept();
//            return true;
//        }
//    }
//    bool result = QWMAbstractEditor::eventFilter(watched,event);
//    return result;
    return false;
}

void QWMIconSelector::showEvent(QShowEvent *event)
{
    if(ui->listWidget->selectionModel()->hasSelection()&& ui->listWidget->selectionModel()->currentIndex().isValid()){
        QModelIndex index=ui->listWidget->selectionModel()->currentIndex();
        QFileInfo file=index.data(DATA_ROLE).value<QFileInfo>();
        showIcon(file);
    }
}

void QWMIconSelector::setValue(QVariant v)
{
    QString filename=v.toString();
    this->selectFile(filename);
}

QVariant QWMIconSelector::value()
{
    return this->file();
}


void QWMIconSelector::on_btn_clicked()
{
    if(sender()->objectName()=="btnOK"){
        emit this->accepted(this);
    }
    if(sender()->objectName()=="btnCancel"){
        emit this->rejected(this);
    }
}


void QWMIconSelector::on_item_doubleclick(const QModelIndex &)
{
    emit this->accepted(this);
}
void QWMIconSelector::on_item_click(const QModelIndex &)
{
    if(ui->listWidget->selectionModel()->hasSelection()&& ui->listWidget->selectionModel()->currentIndex().isValid()){
        QModelIndex index=ui->listWidget->selectionModel()->currentIndex();
        QFileInfo file=index.data(DATA_ROLE).value<QFileInfo>();
        showIcon(file);
    }

}

void QWMIconSelector::selectFile(QString filename)
{
    QString path = QDir::cleanPath(defaultDir());
    QDir    currentDir = QDir(path);
    QStringList filter;
    if(filename.isNull()|| filename.isEmpty())
        return ;
    else
        filter<<filename+".svg";

    QList<QFileInfo> subFolders=folders();
    foreach(QFileInfo folder,subFolders){
        QDirIterator it(folder.absoluteFilePath(),filter, QDir::NoFilter, QDirIterator::NoIteratorFlags);
        if(it.hasNext()){
            it.next();
            QFileInfo file=it.fileInfo();
            int index=ui->comboBox->findData(folder.completeBaseName(),Qt::DisplayRole);
            ui->comboBox->setCurrentIndex(index);
            QList<QListWidgetItem*>  items=ui->listWidget->findItems(file.completeBaseName(),Qt::MatchExactly);
            if(!items.isEmpty()){
                items.first()->setSelected(true);
            }
        }
    }
}

QString QWMIconSelector::file()
{
    if(ui->listWidget->selectionModel()->hasSelection()){
        QFileInfo file=ui->listWidget->currentIndex().data(DATA_ROLE).value<QFileInfo>();
        return file.completeBaseName();
    }
    return QString();
}

QList<QFileInfo> QWMIconSelector::folders()
{
    QDirIterator it(defaultDir(), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    QList<QFileInfo> list;
    while(it.hasNext()){
        it.next();
        list<<it.fileInfo();
    }
    return list;
}

QList<QFileInfo> QWMIconSelector::files(QString subFolder)
{
    QString subPath=QString("%1%2%3").arg(defaultDir()).arg(QDir::separator()).arg(subFolder);
    QDirIterator it(subPath, QStringList() << "*.svg", QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    QList<QFileInfo> list;
    while(it.hasNext()){
        it.next();
        list<<it.fileInfo();
    }
    return list;
}

QList<QWidget *> QWMIconSelector::taborders()
{
    QList<QWidget*> results;
    results<<ui->comboBox<<ui->listWidget;
    return results;
}


void QWMIconSelector::on_comboBox_currentTextChanged(const QString &text)
{
    ui->listWidget->clear();
    if(ui->comboBox->currentIndex()>=0){
        QFileInfo folderInfo=ui->comboBox->currentData().value<QFileInfo>();
        QList<QFileInfo> files=this->files(folderInfo.fileName());
        foreach(QFileInfo fileInfo,files){
            QString base = fileInfo.completeBaseName();
            QListWidgetItem * item=new QListWidgetItem(base);
            item->setData(DATA_ROLE,QVariant::fromValue(fileInfo));
            ui->listWidget->addItem(item);
        }
        if(ui->listWidget->count()>0){
            ui->listWidget->setCurrentRow(0);
            emit ui->listWidget->clicked(ui->listWidget->model()->index(0,0));
        }
        ui->listWidget->setFocus();
    }
}


void QWMIconSelector::showIcon(QFileInfo file)
{
    QGraphicsScene *scene = new QGraphicsScene;
    QPixmap pixmap(file.absoluteFilePath());
    QSize sp=pixmap.size();
    QSize sv=ui->graphicsView->size();
    float ratio=qMin(float(sv.width())/float(sp.width()),float(sv.height())/float(sp.height()));
    ratio*=0.95;
    QSize scaledSize=QSize(sp.width()*ratio,sp.height()*ratio);
    QGraphicsPixmapItem* item=scene->addPixmap(pixmap.scaled(scaledSize, Qt::KeepAspectRatio) );
    ui->graphicsView->setScene(scene);
}

void QWMIconSelector::on_listWidget_itemSelectionChanged()
{
    QModelIndex index=ui->listWidget->currentIndex();
    QFileInfo file=index.data(DATA_ROLE).value<QFileInfo>();
    showIcon(file);
}
