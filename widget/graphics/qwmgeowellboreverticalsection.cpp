#include "qwmgeowellboreverticalsection.h"
#include "QGraphicsAnchorLayout"
#include "qwmgeosimpleformation.h"
#include "QPainter"
#include <QRectF>
#include "common.h"
QWMGeoWellboreVerticalSection::QWMGeoWellboreVerticalSection(QString idWell, QString idBore, QVector<QWMGeoFormationInfo> formations,QRectF ticks, QGraphicsItem *parent):
    QWMGeoTrackContent(ticks,parent)
{
    QGraphicsAnchorLayout * layout=new QGraphicsAnchorLayout(this);
    QRectF  boundingRect=this->boundingRect();
    float depthScale=boundingRect.height()/ticks.height();
    QWMGeoSimpleFormation  * preFormation=nullptr;
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
        _formationWidgets.append(formationWidget);
        formationWidget->setOpacity(0.6);
        connect(formationWidget,&QWMGeoTrackContent::hoverData,this,&QWMGeoWellboreVerticalSection::on_hover_data);
    }
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    this->setLayout(layout);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
}

void QWMGeoWellboreVerticalSection::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QRectF  boundingRect=this->boundingRect();
    float depthScale=boundingRect.height()/ticks().height();
    for(QVector<QWMGeoSimpleFormation*>::iterator i=_formationWidgets.begin() ;i!=_formationWidgets.end(); i++){
        (*i)->setDepthScale(depthScale);
//        (*i)->setPreferredSize(boundingRect.width(),(*i)->sizeHint(Qt::PreferredSize).height());
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
