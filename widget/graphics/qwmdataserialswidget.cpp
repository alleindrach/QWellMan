#include "qwmdataserialswidget.h"
#include <QPainter>
#include <QPair>
#include <QTextItem>
#include <QGraphicsScene>
#include <QLabel>
#include "common.h"
#include <QGraphicsLinearLayout>
QWMDataSerialsWidget::QWMDataSerialsWidget( QVector<QPair<float, QString> > *data, QRectF ticks,QString title,int width, QGraphicsItem *parent):
    QGraphicsWidget(parent),_data(data),_ticks(ticks),_width(width),_trackTitle(title)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

}

void QWMDataSerialsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
    float xpos=boundingRect.left();
    painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine));
    font.setPixelSize(8);
    font.setBold(true);
    painter->setFont(font);


    for(QVector<QPair<float, QString>>::const_iterator i=_data->constBegin();i!=_data->constEnd(); i++){
        float ypos=(i->first-_ticks.top())*yscale;
        painter->setPen(QPen(QBrush(LIGHT_RED),1,Qt::SolidLine));
        painter->drawLine(0,ypos,20,ypos);
        painter->drawLine(boundingRect.width()-20,ypos,boundingRect.width(),ypos);
        painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
        QRect txtRect(20,ypos-6,boundingRect.width()-40,12);
        font.setPixelSize(8);
        font.setBold(false);
        painter->setFont(font);
        painter->drawText(txtRect, Qt::AlignCenter,i->second);
    }
    painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
    painter->drawRect(boundingRect);
    painter->restore();
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

void QWMDataSerialsWidget::keyPressEvent(QKeyEvent *event)
{

}

void QWMDataSerialsWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void QWMDataSerialsWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    this->setSelected(true);
    this->update();
}

void QWMDataSerialsWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->setSelected(false);
    this->update();
}

void QWMDataSerialsWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

}

void QWMDataSerialsWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}
