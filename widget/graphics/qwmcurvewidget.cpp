#include "qwmcurvewidget.h"
#include "QPainter"
#include "common.h"
#include "qgraphicsscene.h"
#include  <QGraphicsScene>
#include <QGraphicsView>
#include <QPolygonF>
QWMCurveWidget::QWMCurveWidget( QPointF* data,int dataSize, QRectF ticks, QGraphicsItem *parent):
    QWMTrackContent(ticks,parent),_data(data),_size(dataSize)
{

}

void QWMCurveWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QGraphicsScene* scene=this->scene();
//    QGraphicsView * view= (QGraphicsView *)scene->parent();
//    QList<QGraphicsView*> vs=scene->views();
    QRectF boundingRect=this->boundingRect();
    QFont font=painter->font();
    QPointF globalOriginalPoint=this->mapToScene(QPointF(0,0));

    font.setPixelSize(8);
    painter->setFont(font);

    QMatrix oriMatrix=painter->matrix();

//    QPointF scrollDiff=view->mapFromScene(QPointF(0,0));
    //    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    float xscale = boundingRect.width()/ticks().width();
    float yscale = boundingRect.height()/ticks().height();
    float yoffset =boundingRect.top()-ticks().top()*yscale;

    drawGrid(painter);

    float xpos=boundingRect.left();
    painter->setPen(QPen(QBrush(Qt::darkRed),1,Qt::SolidLine));

    QTransform  transform;
    transform.translate(0-ticks().left()*xscale,0-ticks().top()*yscale);
//    transform.translate(globalOriginalPoint.x()-ticks().left()*xscale+scrollDiff.x()
//                        ,globalOriginalPoint.y()-ticks().top()*yscale+scrollDiff.y());
    transform.scale(xscale,yscale);

    //    transform.translate(ticks().left(),ticks().top());
    painter->setWorldTransform(transform,true);
    painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
    //    for(int i=0;i<_size;i++){
    //        if(i>0){
    //            float xpos1=(_data[i-1].x()-ticks().left())*xscale;
    //            float ypos1=(_data[i-1].y()-ticks().top())*yscale;
    //            float xpos2=(_data[i].x()-ticks().left())*xscale;
    //            float ypos2=(_data[i].y()-ticks().top())*yscale;
    //            painter->drawLine(xpos1,ypos1,xpos2,ypos2);
    //        }
    //    }

    painter->drawPolyline(_data,_size);
    painter->resetTransform();
    painter->setMatrix(oriMatrix);
    painter->drawRect(boundingRect);
    painter->restore();

}
