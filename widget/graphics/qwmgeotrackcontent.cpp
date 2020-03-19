#include "qwmgeotrackcontent.h"
#include <QPainter>
#include <QPair>
#include <QTextItem>
#include <QGraphicsScene>
#include <QLabel>
#include "common.h"
#include <QGraphicsLinearLayout>
QWMGeoTrackContent::QWMGeoTrackContent(QRectF ticks, QGraphicsItem *parent):
    QGraphicsWidget(parent),_ticks(ticks)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

}


//QRectF QWMDataSerialsGraphicsItem::boundingRect() const
//{
//    if(this->parentItem()==nullptr){
//        QGraphicsScene *scene=this->scene();
//        int h=scene->height();
//        //        QRectF sceneBounding=scene->sceneRect();
//        return QRectF(0,0,_width,h);
//    }else{
//        QRectF sceneBounding=this->parentItem()->boundingRect();
//        return QRectF(0,0,_width,sceneBounding.height());
//    }
//}

void QWMGeoTrackContent::keyPressEvent(QKeyEvent *event)
{

}

void QWMGeoTrackContent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void QWMGeoTrackContent::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    this->setSelected(true);
    this->update();
}

void QWMGeoTrackContent::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->setSelected(false);
    this->update();
}

void QWMGeoTrackContent::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

    QString dataDes=dataAtPos(event->pos());
    emit hoverData(event->pos(),dataDes);

}

void QWMGeoTrackContent::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

}

void QWMGeoTrackContent::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void QWMGeoTrackContent::drawGrid(QPainter *painter)
{

    QRectF boundingRect=this->boundingRect();
    QFont font=painter->font();

    font.setPixelSize(8);
    painter->setFont(font);

    //    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    if(this->isSelected()){
        painter->fillRect(boundingRect,MUCH_LIGHT_RED);
    }else{
        painter->fillRect(boundingRect,Qt::white);
    }

    if(_showGrid){
        painter->setPen(QPen(QBrush(LIGHT_GRAY),1,Qt::DashDotLine));

        for(int ypos=boundingRect.top();ypos<boundingRect.bottom();ypos+=_gridSize.height()){
            painter->drawLine(0,ypos,boundingRect.width(),ypos);
        }

        for(int xpos=boundingRect.left();xpos<boundingRect.right();xpos+=_gridSize.width()){
            painter->drawLine(xpos,0,xpos,boundingRect.height());
        }
    }
    drawGround(painter);

}

void QWMGeoTrackContent::drawGround(QPainter *painter)
{

    QRectF boundingRect=this->boundingRect();
    //    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    float yscale = boundingRect.height()/ticks().height();
    float ypos=(0-ticks().top())*yscale;
    QRectF box{0,ypos-5,boundingRect.width(),5};
    painter->fillRect(box,Qt::BDiagPattern);
    painter->setPen(QPen(QBrush(Qt::black),2,Qt::SolidLine));
    painter->drawLine(0,ypos,boundingRect.width(),ypos);

}
