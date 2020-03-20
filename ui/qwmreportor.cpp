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
#include "qwmcurveswidget.h"
#include "qwmgeocurveinfo.h"
#include "qwmgeoformationinfo.h"
#include "qwmgeowellboreverticalsection.h"
QWMReportor::QWMReportor(QString idWell,QString title,QWidget *parent) :
    QMainWindow(parent),_idWell(idWell),_title(title) ,
    ui(new Ui::QWMReportor)
{
    ui->setupUi(this);



    _statusComponenet = new QLabel(this);
    QFont font=_statusComponenet->font();
    font.setPixelSize(10);
    _statusComponenet->setFont(font);
    _statusComponenet->setMinimumWidth(160);
    _statusData= new QLabel(this);

    _statusData->setFont(font);
    _statusData->setMinimumWidth(160);

    _statusDepth=new QLabel(this);
    _statusDepth->setFont(font);
    _statusDepth->setMinimumWidth(100);

    ui->statusbar->addWidget(_statusComponenet,0);
    ui->statusbar->addWidget(_statusDepth,0);
    ui->statusbar->addWidget(_statusData,0);

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
    QMainWindow::resizeEvent(event);
    float w=ui->graphicsView->width()-10;
    float h=ui->graphicsView->height()-10;
    QRectF f=ui->graphicsView->sceneRect();
    qDebug()<<"H:"<<h<<",W:"<<w<<",SceneH:"<<f.height()<<",SceneW:"<<f.width();
    QWMGeoGraphicsScene* scene=( QWMGeoGraphicsScene*)ui->graphicsView->scene();
    scene->resize(QSizeF(w,h));
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
        QString fieldTitle=UDL->fieldInfo(SURVY_DATA_TABLE,dataField)->caption();
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
            QWMDataSerialsWidget * content=new QWMDataSerialsWidget(data,QRectF(min,top,max-min,ticks.height()),fieldTitle);

            QWMGeoTrackTitle * title=new QWMGeoTrackTitle(_idWell,fieldTitle,
                                                          QList<QWMGeoCurveInfo>(),
                                                          ((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());
            QWMGeoTrackWidget * track=new QWMGeoTrackWidget(title,content,((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());

            track->setData(0,dataField);
            connect(track,&QWMGeoTrackWidget::hoverData,this,&QWMReportor::on_hover_data);
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
        QString fieldTitle=UDL->fieldInfo(SURVY_DATA_TABLE,dataField)->caption();

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
            //QList<QPair<QWMGeoCurveInfo, QPointF *>> curves,QRectF ticks,
            QWMGeoCurveInfo info;
            info.color=qRgb(3,123,78);
            info.ticks=QRectF(min,ticks.top(),max-min,ticks.height());
            info.lineStyle=Qt::DotLine;
            info.lineWidth=1;
            info.points=j;
            info.title=fieldTitle;
            QList<QPair<QWMGeoCurveInfo  ,QPointF *>> data;
            data.append(QPair<QWMGeoCurveInfo  ,QPointF *>(info,points));
            QWMCurvesWidget * content=new QWMCurvesWidget(data,info.ticks);

            QWMGeoTrackTitle * title=new QWMGeoTrackTitle(_idWell,fieldTitle,
                                                          QList<QWMGeoCurveInfo>{info},
                                                          ((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());
            QWMGeoTrackWidget * track=new QWMGeoTrackWidget(title,content,((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());
            track->setData(0,dataField);
            connect(track,&QWMGeoTrackWidget::hoverData,this,&QWMReportor::on_hover_data);
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

void QWMReportor::on_hover_data(QPointF pos,QString comp, QString des)
{
    _statusComponenet->setText(comp);
    _statusData->setText(des);
    _statusDepth->setText(QString().asprintf(("MD:%6.2f"),pos.y()));
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
            QWMGeoTrackWidget * track;
            track=(QWMGeoTrackWidget*)this->survyDataSerial(survyId,QRectF(0,top,200,bottom-top),"MD");
            if(track!=nullptr){
                track->setMaximumWidth(100);
                track->setMinimumWidth(100);
                track->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
                geoScene->addTrack(track);
            }
            track =(QWMGeoTrackWidget*)this->survyDataSerial(survyId,QRectF(0,top,200,bottom-top),"Inclination");
            if(track!=nullptr){
                track->setMaximumWidth(100);
                track->setMinimumWidth(100);
                track->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
                geoScene->addTrack(track);
                geoScene->addTrack(track);
            }

            QWMGeoTrackWidget * curve =(QWMGeoTrackWidget*)this->survyDataCurve(survyId,QRectF(0,top,200,bottom-top),"Inclination");
            if(curve!=nullptr){
                curve->setMaximumWidth(100);
                curve->setMinimumWidth(100);
                curve->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
//                curve->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
                geoScene->addTrack(curve);
            }

            //井眼地层图
            QWMRotatableProxyModel * model=WELL->tableForEdit(WELLBORE_FORMATION_TABLE,_idWell,wellboreId);
            PX(proxyModel,model);

            proxyModel->sortByField("DepthDrillingTop");

            QVector<QWMGeoFormationInfo> formations;
            float preBottom=top;
            for(int i=0;i<proxyModel->rowCount();i++){

                QWMGeoFormationInfo formation;
                formation.id=proxyModel->data(i,"IDRec",Qt::EditRole).toString();
                formation.top=proxyModel->data(i,"DepthDrillingTop").toFloat();
                formation.bottom=proxyModel->data(i,"DepthDrillingBtm").toFloat();
                formation.icon=proxyModel->data(i,"IconName").toString();
                formation.desc=WELL->recordDes(WELLBORE_FORMATION_TABLE,proxyModel->record(i));

                if(formation.top>preBottom){
                    QWMGeoFormationInfo formationPadding;
                    formationPadding.id=tr("padding");
                    formationPadding.top=preBottom;
                    formationPadding.bottom=formation.top;
                    formationPadding.icon=QString();
                    formationPadding.desc=tr("填充层");
                    formations.append(formationPadding);
                }

                formations.append(formation);
                preBottom=formation.bottom;
            }



            //井眼尺寸图

            QWMRotatableProxyModel * wbsmodel=WELL->tableForEdit(WELLBORE_SIZE_TABLE,_idWell,wellboreId);
            PX(proxyWbsModel,wbsmodel);
            proxyWbsModel->sortByField("DepthTopActual");

            QVector<QWMGeoWellboreSizeInfo> sizes;
            preBottom=top;
            float max=0;
            for(int i=0;i<proxyWbsModel->rowCount();i++){

                QWMGeoWellboreSizeInfo sizeInfo;
                sizeInfo.id=proxyWbsModel->data(i,"IDRec",Qt::EditRole).toString();
                sizeInfo.top=proxyWbsModel->data(i,"DepthTopActual").toFloat();
                sizeInfo.bottom=proxyWbsModel->data(i,"DepthBtmActual").toFloat();
                sizeInfo.radius=proxyWbsModel->data(i,"Sz").toFloat();
                sizeInfo.desc=WELL->recordDes(WELLBORE_SIZE_TABLE,proxyWbsModel->record(i));
                sizeInfo.hatchColor=qRgb(239,243,139);
                sizeInfo.hatchStyle=Qt::SolidPattern;
                sizeInfo.borderColor=Qt::black;

                if(sizeInfo.top>preBottom){
                    QWMGeoWellboreSizeInfo sizePadding;
                    sizePadding.id=tr("padding");
                    sizePadding.radius=0;
                    sizePadding.top=preBottom;
                    sizePadding.bottom=sizeInfo.top;
                    sizePadding.desc=tr("填充层");
                    sizes.append(sizePadding);
                }
                if(sizeInfo.radius>max){
                    max=sizeInfo.radius;
                }
                sizes.append(sizeInfo);
                preBottom=sizeInfo.bottom;
            }
            max=int(max*2);
            QWMGeoWellboreVerticalSection * content=new QWMGeoWellboreVerticalSection(_idWell,wellboreId,formations,sizes,QRectF(0,top,max,bottom-top),geoScene->topWidget());
            content->setMinimumWidth(100);
            content->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            QWMGeoTrackTitle * title=new QWMGeoTrackTitle(_idWell,tr("井眼纵剖面"),
                                                          QList<QWMGeoCurveInfo>(),
                                                          ((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());
            QWMGeoTrackWidget * sectionTrack=new QWMGeoTrackWidget(title,content,((QWMGeoGraphicsScene*)(ui->graphicsView->scene()))->topWidget());
            sectionTrack->setData(0,"section");
            sectionTrack->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            connect(sectionTrack,&QWMGeoTrackWidget::hoverData,this,&QWMReportor::on_hover_data);
            geoScene->addTrack(sectionTrack);
        }
    }
}
