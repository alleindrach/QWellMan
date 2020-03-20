#include "qwmcurveswidget.h"
#include "QPainter"
#include "common.h"
#include "qgraphicsscene.h"
#include  <QGraphicsScene>
#include <QGraphicsView>
#include <QPolygonF>
#include "utility.h"
QWMCurvesWidget::QWMCurvesWidget( QList<QPair<QWMGeoCurveInfo, QPointF *>> curves, QRectF ticks, QGraphicsItem *parent):
    QWMGeoTrackContent(ticks,parent),_curves(curves)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

void QWMCurvesWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->save();
    drawGrid(painter);
    for(int i=0;i<_curves.size();i++){
        QPair<QWMGeoCurveInfo, QPointF *> curve=_curves[i];
        drawCurve(painter,curve);
    }
    painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
    QRectF boundingRect=this->boundingRect();
    painter->drawRect(boundingRect);
    painter->restore();
}

QString QWMCurvesWidget::dataAtPos(QPointF pos)
{
    QRectF boundingRect=this->boundingRect();
    QString result;
    QStringList r;
    if(_curves.size()>0){
        for(int i=0;i<_curves.size();i++){
            float yscale = _curves[i].first.ticks.height()/boundingRect.height();
            float depth = pos.y()*yscale;
            int pos=Utility::binarySearch(_curves[i].second,0,_curves[i].first.points-1,depth);
            QString des=QString("%1-%2").arg(_curves[i].first.title).arg(_curves[i].second[pos].x());
            r<<des;
        }
    }
    return r.join(",");
}

void QWMCurvesWidget::drawCurve(QPainter * painter, QPair<QWMGeoCurveInfo, QPointF *> c)
{

    QTransform oriTransform=painter->transform();
    QRectF boundingRect=this->boundingRect();
    //    QPointF scrollDiff=view->mapFromScene(QPointF(0,0));
    //    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    float xscale = boundingRect.width()/c.first.ticks.width();
    float yscale = boundingRect.height()/c.first.ticks.height();

    painter->setPen(QPen(QBrush(c.first.color),c.first.lineWidth,c.first.lineStyle));

    QTransform  transform;
    transform.translate(0-c.first.ticks.left()*xscale,0-c.first.ticks.top()*yscale);
    transform.scale(xscale,yscale);

    painter->setWorldTransform(transform,true);
    painter->setPen(QPen(QBrush(c.first.color),c.first.lineWidth,c.first.lineStyle));
    int points=c.first.points;
    painter->drawPolyline(c.second,points);
    painter->resetTransform();
    painter->setTransform(oriTransform);
}


