#include "qwmreportor.h"
#include "ui_qwmreportor.h"
#include "welldao.h"
#include "common.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QGraphicsScene>
#include "udldao.h"
#include "qwmdataserialswidget.h"
#include "qwmrotatableproxymodel.h"
#include "qwmsortfilterproxymodel.h"
#include <qwmmain.h>
#include <QCloseEvent>
#include "qwmgeographicsscene.h"
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QSizePolicy>
#include "qwmgeotrackwidget.h"
#include "qwmcurvewidget.h"
QWMReportor::QWMReportor(QString idWell,QString title,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_title(title) ,
    ui(new Ui::QWMReportor)
{
    ui->setupUi(this);
    //    if(ui->graphicsView->scene()==nullptr){
    //        QWMGeoGraphicsScene * scene=new QWMGeoGraphicsScene(this);
    //        scene->setSceneRect(0,0,ui->graphicsView->width(),ui->graphicsView->width());
    //        ui->graphicsView->setScene(scene);
    //    }
    this->setWindowTitle(title);

    QWMGeoGraphicsScene * scene=new QWMGeoGraphicsScene(idWell,ui->graphicsView);
    //    scene->setBackgroundBrush(QBrush(Qt::green));
    ui->graphicsView->setContentsMargins(0,0,0,0);
    ui->graphicsView->setScene(scene);

}
void QWMReportor::init(){
    QSqlQuery wellsQuery=WELL->records(WELLBORE_TABLE,_idWell,QString());
    ui->comboWellbore->clear();
    while(wellsQuery.next()){
        QSqlRecord rec=wellsQuery.record();
        QString wellbore=WELL->recordDes(WELLBORE_TABLE,rec);
        PK_VALUE(wellboreId,rec);
        ui->comboWellbore->addItem(wellbore,wellboreId);
    }
    ui->comboJob->clear();
    QSqlQuery  jobsQuery=WELL->records(JOB_TABLE,_idWell,QString());
    while(jobsQuery.next()){
        QSqlRecord rec=jobsQuery.record();
        QString job=WELL->recordDes(JOB_TABLE,rec);
        PK_VALUE(jobId,rec);
        ui->comboJob->addItem(job,jobId);
    }
    ui->graphicsView->installEventFilter(this);
}

void QWMReportor::resizeEvent(QResizeEvent *event)
{
    float w=ui->graphicsView->width();
    float h=ui->graphicsView->height();
    QRectF f=ui->graphicsView->sceneRect();
    QWMGeoGraphicsScene* scene=( QWMGeoGraphicsScene*)ui->graphicsView->scene();
    scene->resize(QSizeF(w,h));
    QMainWindow::resizeEvent(event);
}

bool QWMReportor::eventFilter(QObject *watched, QEvent *event)
{
    if(watched->objectName()=="graphicsView"){
        if(event->type()==QEvent::Resize){
            int h=ui->graphicsView->height();
            int w=ui->graphicsView->width();
            qDebug()<<"H:"<<h<<",W:"<<w;
            on_resize_graphicsview();
        }
    }
    if(event->type()==QEvent::Wheel){
        QPoint m=ui->graphicsView->mapFromScene(QPoint(0,0));
        qDebug()<<"scroll:"<<m;
    }


}

QGraphicsItem* QWMReportor::survyDataSerial(QString survyId, QRectF ticks,QString dataField)
{
    if(ticks.height()>0){
        long top=ticks.top();
        long bottom=ticks.bottom();
        QSizeF sizeOfGraphic=ui->graphicsView->size();
        double hScale=sizeOfGraphic.height()/double(bottom-top);

        QWMRotatableProxyModel * model=WELL->tableForEdit(SURVY_DATA_TABLE,_idWell,survyId);
        PX(proxyModel,model);
        proxyModel->sort(1);
        QString title=UDL->fieldInfo(SURVY_DATA_TABLE,dataField)->caption();
        if(proxyModel->rowCount()>0){
            QVector<QPair<float, QString> > *data=new QVector<QPair<float, QString> >();
            float max=INT_MIN;
            float min=INT_MAX;
            for(int i=0;i<proxyModel->rowCount();i++){
                float v=proxyModel->data(i,dataField).toFloat();
                if(v>max)
                    max=v;
                if(v<min)
                    min=v;
                QPair<float,QString> sd(proxyModel->data(i,"MD").toFloat(),QString("").sprintf("%6.2f",v));
                if(!proxyModel->data(i,"DontUse",Qt::EditRole).toBool()){
                    data->append(sd);
                }
            }
            QWMDataSerialsWidget * item=new QWMDataSerialsWidget(data,QRectF(min,top,max,bottom),title);
            QWMGeoTrackWidget * track=new QWMGeoTrackWidget(title,ui->graphicsView->scene());
            track->setContent(item);
            //            QRectF f=ui->graphicsView->scene()->sceneRect();
            item->setData(0,dataField);
            return track;
        }
    }
    return nullptr;
}

QGraphicsItem *QWMReportor::survyDataCurve(QString survyId, QRectF ticks, QString dataField)
{

    if(ticks.height()>0){
        long top=ticks.top();
        long bottom=ticks.bottom();
        QSizeF sizeOfGraphic=ui->graphicsView->size();
        double hScale=sizeOfGraphic.height()/double(bottom-top);

        QWMRotatableProxyModel * model=WELL->tableForEdit(SURVY_DATA_TABLE,_idWell,survyId);
        PX(proxyModel,model);
        proxyModel->sort(1);
        QString title=UDL->fieldInfo(SURVY_DATA_TABLE,dataField)->caption();

        if(proxyModel->rowCount()>0){
            QPointF * points=new QPointF[proxyModel->rowCount()];
            float max=INT_MIN;
            float min=INT_MAX;
            int j=0;
            for(int i=0;i<proxyModel->rowCount();i++){
                float v=proxyModel->data(i,dataField).toFloat();
                if(v>max)
                    max=v;
                if(v<min)
                    min=v;

                if(!proxyModel->data(i,"DontUse",Qt::EditRole).toBool()){
                    points[j].setY(proxyModel->data(i,"MD").toFloat());
                    points[j].setX(v);
                    j++;
                }
            }
            if(min==max){
                max=min+10;
            }
            int w=int((max-min) /10);
            if(w<1)
                w=1;
            min-=w;
            max+=w;
            min=qRound(min-0.5);
            max=qRound(max+0.5);

            QWMCurveWidget * item=new QWMCurveWidget(points,j,QRectF(min,top,max,bottom));
            QWMGeoTrackWidget * track=new QWMGeoTrackWidget(title,ui->graphicsView->scene());
            track->setContent(item);
            //            QRectF f=ui->graphicsView->scene()->sceneRect();
            item->setData(0,dataField);
            return track;
        }
    }
    return nullptr;
}

void QWMReportor::closeEvent(QCloseEvent *event)
{
    //    bool canceled=false;

    QWMMain * main= static_cast<QWMMain *>(this->parent());
    main->show();
    main->setCurrentReportor(nullptr);
    event->accept();
}
void QWMReportor::on_resize_graphicsview(){

}
void QWMReportor::on_comboWellbore_currentIndexChanged(int index)
{
    QWMGeoGraphicsScene * geoScene= ((QWMGeoGraphicsScene*)ui->graphicsView->scene());
    geoScene->reset();
    QString wellboreId=ui->comboWellbore->currentData().toString();

    if(!wellboreId.isNull()&&!wellboreId.isEmpty()){

        QPointF  depths=WELL->wellboreDepth(wellboreId,QDateTime::currentDateTime());
        if(depths.y()-depths.x()>1){
            long top=int(depths.x()/BASE_TICK-1)*BASE_TICK;
            long bottom=int(depths.y()/BASE_TICK+2)*BASE_TICK;
            QString survyId=WELL->wellboreActiveSurvyId(wellboreId);
            QWMDataSerialsWidget * item;
            item=(QWMDataSerialsWidget*)this->survyDataSerial(survyId,QRectF(0,top,200,bottom-top),"MD");
            if(item!=nullptr){
                item->setMaximumWidth(100);
                item->setMinimumWidth(100);
                item->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
                geoScene->addTrack(item);
            }
            item =(QWMDataSerialsWidget*)this->survyDataSerial(survyId,QRectF(0,top,200,bottom-top),"Inclination");
            if(item!=nullptr){
                item->setMaximumWidth(100);
                item->setMinimumWidth(100);
                item->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
                geoScene->addTrack(item);
                geoScene->addTrack(item);
            }

            QWMCurveWidget * curve =(QWMCurveWidget*)this->survyDataCurve(survyId,QRectF(0,top,200,bottom-top),"Inclination");
            if(curve!=nullptr){
                curve->setMaximumWidth(100);
                curve->setMinimumWidth(100);
                curve->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
                geoScene->addTrack(curve);
            }

        }
    }
}
