#include "qwmdataserialswidget.h"
#include <QPainter>
#include <QPair>
#include <QTextItem>
#include <QGraphicsScene>
#include <QLabel>
#include "common.h"
#include <QGraphicsLinearLayout>
#include "qwmgeotrackcontent.h"
#include "utility.h"
QWMDataSerialsWidget::QWMDataSerialsWidget( QVector<QPair<float, QString> > *data, QRectF ticks,QString title,int width, QGraphicsItem *parent):
    QWMGeoTrackContent(ticks, parent),_data(data),_width(width),_trackTitle(title)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

}

void QWMDataSerialsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->save();
    QRectF boundingRect=this->boundingRect();
    QFont font=painter->font();

    font.setPixelSize(8);
    painter->setFont(font);

    //    ticks的 top对齐到y=0，ticks的bottom对齐到boundingRect的底部
    float yscale = boundingRect.height()/ticks().height();

    if(showGrid()){
        drawGrid(painter);
    }
    painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine));
    font.setPixelSize(8);
    font.setBold(true);
    painter->setFont(font);

    for(QVector<QPair<float, QString>>::const_iterator i=_data->constBegin();i!=_data->constEnd(); i++){
        float ypos=(i->first-ticks().top())*yscale;
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

QString QWMDataSerialsWidget::dataAtPos(QPointF pos)
{
    QString r;
    QRectF boundingRect=this->boundingRect();

    float yscale = ticks().height()/boundingRect.height();
    float depth = pos.y()*yscale;
    int dp=Utility::binarySearch(_data,0,_data->size()-1,depth);
    QString des=QString("%1").arg(_data->at(dp).second);
    return des;
}

