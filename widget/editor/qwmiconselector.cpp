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
QWMIconSelector::QWMIconSelector(QWidget *parent) : QWidget(parent),ui(new Ui::QWMIconSelector)
{
    ui->setupUi(this);

    connect(ui->btnOK,&QPushButton::clicked,this,&QWMIconSelector::on_btn_clicked);
    connect(ui->btnCancel,&QPushButton::clicked,this,&QWMIconSelector::on_btn_clicked);
    //    connect(ui->lineEdit,&QLineEdit::returnPressed,this,&QWMIconSelector::on_return_pressed);
    connect(ui->listWidget,&QListWidget::doubleClicked,this,&QWMIconSelector::on_item_doubleclick);
    connect(ui->listWidget,&QListWidget::clicked,this,&QWMIconSelector::on_item_click);
    ui->comboBox->installEventFilter(this);;
    ui->listWidget->installEventFilter(this);;

    QList<QFileInfo> subFolders=folders();
    ui->comboBox->clear();
    foreach(QFileInfo fileInfo,subFolders){
        ui->comboBox->addItem(fileInfo.completeBaseName(),QVariant::fromValue(fileInfo));
    }
    emit ui->comboBox->currentTextChanged(ui->comboBox->currentText());
    //    this->on_comboBox_currentTextChanged(ui->comboBox->currentText());
}


bool QWMIconSelector::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::KeyPress){
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Escape){
            emit this->rejected(this);
            return true;
        }
    }
    if(watched==ui->comboBox && event->type()==QEvent::KeyPress){
        //        qDebug()<<"LineEdit:"<<event->type();
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Tab){
            if(ui->comboBox->nextInFocusChain()!=nullptr)
                ui->comboBox->nextInFocusChain()->setFocus();
            return true;
        }
    }
    if(watched==ui->listWidget && event->type()==QEvent::KeyPress ){
        //        qDebug()<<"TableView:"<<event->type();
        QKeyEvent * keyEvent=(QKeyEvent*) event;
        if(keyEvent->key()==Qt::Key_Return){
            emit this->accepted(this);
            return true;
        }
        if(keyEvent->key()==Qt::Key_Tab){
            if(ui->listWidget->nextInFocusChain()!=nullptr){
                ui->listWidget->nextInFocusChain()->setFocus();
                return true;
            }
        }
    }
    return false;
    //    return true;
}

void QWMIconSelector::focusInEvent(QFocusEvent *event)
{
    ui->comboBox->setFocus();
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



void QWMIconSelector::on_return_pressed()
{
    //    QFileSystemModel  * model=qobject_cast<QFileSystemModel*>(ui->listView->model());
    //    int qc=model->rowCount();
    ////    qDebug()<<"RowCount:"<<proxyModel->rowCount();
    //    if(model->rowCount()>0){
    //        QModelIndex index=model->index(0,0);
    //        QItemSelectionModel * selectModel= ui->tableView->selectionModel();
    //        selectModel->select(index,QItemSelectionModel::SelectCurrent);
    //        ui->tableView->setFocus();
    //        //        emit this->accepted(this);
    //    }

    //    emit this->accepted(this);

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
        QGraphicsScene *scene = new QGraphicsScene;
        QPixmap pixmap(file.absoluteFilePath());
        QSize sp=pixmap.size();
        QSize sv=ui->graphicsView->size();
        float ratio=qMin((sv.width()-20)/sp.width(),(sv.height()-20)/sp.height());

        QGraphicsPixmapItem* item=scene->addPixmap(pixmap.scaledToWidth(sv.width()) );
        ui->graphicsView->setScene(scene);
//        ui->graphicsView->scale(ratio,ratio);
//        QSize size=ui->graphicsView->size();
//        scene->setSceneRect(0,0,size.width(),size.height());
//        item->setOffset(0,0);
//        item->

//        ui->graphicsView->scale()
    }

}

void QWMIconSelector::selectFile(QString filename)
{
    QString path = QDir::cleanPath(_defaultDir);
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
    QDirIterator it(_defaultDir, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    QList<QFileInfo> list;
    while(it.hasNext()){
        it.next();
        list<<it.fileInfo();
    }
    return list;
}

QList<QFileInfo> QWMIconSelector::files(QString subFolder)
{
    QString subPath=QString("%1%2%3").arg(_defaultDir).arg(QDir::separator()).arg(subFolder);
    QDirIterator it(subPath, QStringList() << "*.svg", QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    QList<QFileInfo> list;
    while(it.hasNext()){
        it.next();
        list<<it.fileInfo();
    }
    return list;
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
    }
}
