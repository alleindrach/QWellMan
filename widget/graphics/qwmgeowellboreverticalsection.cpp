#include "qwmgeowellboreverticalsection.h"
#include "QGraphicsAnchorLayout"
#include "qwmgeosimpleformation.h"
#include "QPainter"
#include <QRectF>
#include "common.h"
QWMGeoWellboreVerticalSection::QWMGeoWellboreVerticalSection(QString idWell, QString idBore,
                                                             QVector<QWMGeoFormationInfo> formations,
                                                             QVector<QWMGeoWellboreSizeInfo>  sizeInfos,
                                                             QRectF ticks, QGraphicsItem *parent):
    QWMGeoTrackContent(ticks,parent)
{
    QGraphicsAnchorLayout * layout=new QGraphicsAnchorLayout(this);
    QRectF  boundingRect=this->boundingRect();
    float depthScale=boundingRect.height()/ticks.height();
    QWMGeoSimpleFormation  * preFormation=nullptr;
    QVector<QGraphicsWidget *> formationWidgets;
    for(QVector<QWMGeoFormationInfo>::iterator i=formations.begin() ;i!=formations.end(); i++){

        QWMGeoSimpleFormation * formationWidget=new QWMGeoSimpleFormation(i->id,i->title,depthScale,i->top,i->bottom,i->icon,this);
        if(i==formations.begin()){
            layout->addAnchor(formationWidget, Qt::AnchorTop, layout, Qt::AnchorTop);
            layout->addAnchor(formationWidget, Qt::AnchorHorizontalCenter, layout, Qt::AnchorHorizontalCenter);

        }else{
            layout->addAnchor(formationWidget,Qt::AnchorTop,preFormation,Qt::AnchorBottom);
            layout->addAnchor(formationWidget, Qt::AnchorHorizontalCenter, preFormation, Qt::AnchorHorizontalCenter);
        }
        preFormation=formationWidget;
        formationWidgets.append(formationWidget);
        formationWidget->setOpacity(0.6);
        connect(formationWidget,&QWMGeoTrackContent::hoverData,this,&QWMGeoWellboreVerticalSection::on_hover_data);
    }
    _layers.append(formationWidgets);

    QWMGeoWellboreSection  * preSizeWidget=nullptr;
    QVector<QGraphicsWidget *> wellboreSectionWidgets;
    for(QVector<QWMGeoWellboreSizeInfo>::iterator i=sizeInfos.begin() ;i!=sizeInfos.end(); i++){

        QWMGeoWellboreSection * wbSectionWidget=new QWMGeoWellboreSection(*i,QRectF(0,i->top,ticks.width(),i->bottom),depthScale,this);
        if(i==sizeInfos.begin()){
            layout->addAnchor(wbSectionWidget, Qt::AnchorTop, layout, Qt::AnchorTop);
            layout->addAnchor(wbSectionWidget, Qt::AnchorHorizontalCenter, layout, Qt::AnchorHorizontalCenter);

        }else{
            layout->addAnchor(wbSectionWidget,Qt::AnchorTop,preSizeWidget,Qt::AnchorBottom);
            layout->addAnchor(wbSectionWidget, Qt::AnchorHorizontalCenter, preSizeWidget, Qt::AnchorHorizontalCenter);
        }
        preSizeWidget=wbSectionWidget;
        wellboreSectionWidgets.append(wbSectionWidget);

        connect(wbSectionWidget,&QWMGeoTrackContent::hoverData,this,&QWMGeoWellboreVerticalSection::on_hover_data);
    }

    _layers.append(wellboreSectionWidgets);

    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    this->setLayout(layout);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
}

void QWMGeoWellboreVerticalSection::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    QRectF  boundingRect=this->boundingRect();
    qDebug()<<"WS,H:"<<boundingRect.height()<<",W:"<<boundingRect.width();
    float depthScale=boundingRect.height()/ticks().height();
    for(QVector<QVector<QGraphicsWidget*>>::iterator v=_layers.begin();v!=_layers.end();v++){
        for(QVector<QGraphicsWidget*>::iterator i=v->begin() ;i!=v->end(); i++){
            if(instanceof<QWMGeoSimpleFormation>(*i)){
                ((QWMGeoSimpleFormation*)(*i))->setDepthScale(depthScale);
            }else if(instanceof<QWMGeoWellboreSection>(*i)){
                ((QWMGeoWellboreSection*)(*i))->setDepthScale(depthScale);
            }
            //        (*i)->setPreferredSize(boundingRect.width(),(*i)->sizeHint(Qt::PreferredSize).height());
        }
    }
    QGraphicsAnchorLayout * layout=(QGraphicsAnchorLayout*)this->layout();
    layout->invalidate();
    layout->activate();
    return;
}

void QWMGeoWellboreVerticalSection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
    QRectF boundingRect=this->boundingRect();
    painter->drawRect(boundingRect);
    float middlex=boundingRect.width()/2;
    //    painter->drawRect(0,boundingRect.bottom()-200,200,boundingRect.bottom());
    drawGround(painter);
    painter->restore();
}

QString QWMGeoWellboreVerticalSection::dataAtPos(QPointF pos)
{
    return "";
}
void QWMGeoWellboreVerticalSection::on_hover_data(QPointF pos,QString des){
    emit this->hoverData(pos,des);
}
