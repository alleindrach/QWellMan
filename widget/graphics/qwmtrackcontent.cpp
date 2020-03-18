#include "qwmtrackcontent.h"
#include <QPainter>
#include <QPair>
#include <QTextItem>
#include <QGraphicsScene>
#include <QLabel>
#include "common.h"
#include <QGraphicsLinearLayout>
QWMTrackContent::QWMTrackContent(QRectF ticks, QGraphicsItem *parent):
    QGraphicsWidget(parent),_ticks(ticks)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

}

void QWMTrackContent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

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

void QWMTrackContent::keyPressEvent(QKeyEvent *event)
{

}

void QWMTrackContent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void QWMTrackContent::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    this->setSelected(true);
    this->update();
}

void QWMTrackContent::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->setSelected(false);
    this->update();
}

void QWMTrackContent::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

}

void QWMTrackContent::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void QWMTrackContent::drawGrid(QPainter *painter)
{
    painter->save();
    QRectF boundingRect=this->boundingRect();
    QFont font=painter->font();

    font.setPixelSize(8);
    painter->setFont(font);

//    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    float xscale = boundingRect.width()/_ticks.width();
    float yscale = boundingRect.height()/_ticks.height();
    float yoffset =boundingRect.top()-_ticks.top()*yscale;
    if(this->isSelected()){
        painter->fillRect(boundingRect,MUCH_LIGHT_RED);
    }else{
        painter->fillRect(boundingRect,Qt::white);
    }

    if(_showGrid){
        painter->setPen(QPen(QBrush(LIGHT_GRAY),1,Qt::DashDotLine));
        for(int ypos=_ticks.top();ypos<_ticks.bottom();ypos+=_gridSize.height()){
            painter->drawLine(0,ypos*yscale+yoffset,boundingRect.width(),ypos*yscale+yoffset);
        }
        float yGridSize=_gridSize.height()*yscale;
        float xGridSizeF=yGridSize/xscale;
        for(int xpos=_ticks.left();xpos<_ticks.right();xpos+=xGridSizeF){
            painter->drawLine(xpos*xscale,0,xpos*xscale,boundingRect.height());
        }
    }
    painter->restore();
}
