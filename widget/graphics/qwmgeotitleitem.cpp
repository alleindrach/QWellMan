#include "qwmgeotitleitem.h"

#include "QGraphicsScene"

QWMGeoTitleItem::QWMGeoTitleItem(QString idWell,int height,QGraphicsItem *parent):QGraphicsItem(parent),_idWell(idWell),_height(height)
{

}

QRectF QWMGeoTitleItem::boundingRect() const
{
    if(this->parentItem()==nullptr){
        QGraphicsScene *scene=this->scene();
        int w=scene->width();
        //        QRectF sceneBounding=scene->sceneRect();
        return QRectF(0,0,w,_height);
    }else{
        QRectF sceneBounding=this->parentItem()->boundingRect();
        return QRectF(0,0,sceneBounding.width(),_height);
    }
}

void QWMGeoTitleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}
