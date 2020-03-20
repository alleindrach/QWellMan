#include "qwmgeotracktitle.h"

#include "QGraphicsScene"
#include <QSizeF>
#include <QPainter>
#include "common.h"
QWMGeoTrackTitle::QWMGeoTrackTitle(QString idWell,QString title, QList<QWMGeoCurveInfo> items, QGraphicsItem *parent)
    :QGraphicsWidget(parent),_idWell(idWell),_items(items),_title(title)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

void QWMGeoTrackTitle::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->save();
    QRectF boundingRect=this->boundingRect();
    QRectF titleBox(0,0,boundingRect.width(),GEO_TITLE_ITEM_HEIGHT);
    QFont font=painter->font();
    font.setPixelSize(8);
    //    font.setBold(true);
    painter->setFont(font);

    painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine));
    painter->fillRect(titleBox,QBrush(DARK_RED));
    painter->drawText(titleBox,Qt::AlignCenter,_title);
    for(int i=0;i<_items.size();i++){
        int boxbottom=(i+1)*GEO_TITLE_ITEM_HEIGHT;
        QRectF lineLegendBox(0,boxbottom,boundingRect.width(),boxbottom-4);
        painter->setPen(_items[i].color);
        painter->drawText(lineLegendBox,Qt::AlignLeft|Qt::AlignBottom,QString().asprintf("%6.2f",_items[i].ticks.left()));
        painter->drawText(lineLegendBox,Qt::AlignRight|Qt::AlignBottom,QString().asprintf("%6.2f",_items[i].ticks.right()));
        painter->setPen(QPen(QBrush(_items[i].color),_items[i].lineWidth,_items[i].lineStyle));
        painter->drawLine(lineLegendBox.bottomLeft(),lineLegendBox.bottomRight());
        painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
        QRectF lineHeaderBox(0,boxbottom,boundingRect.width(),boxbottom);
        painter->drawRect(lineHeaderBox);
    }
    painter->setPen(QPen(QBrush(DARK_RED),1,Qt::SolidLine));
    painter->drawRect(boundingRect);
    painter->restore();
}

QSizeF QWMGeoTrackTitle::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if(which==Qt::PreferredSize){
        int height=_height;
        int width=GEO_TRACK_MIN_WIDTH;
        if(height<=0){
            height=GEO_TITLE_ITEM_HEIGHT;
        }
        if(this->parentItem()==nullptr){
            return QSizeF(GEO_TRACK_MIN_WIDTH,height);
        }else{
            QRectF parentBoundingRect=this->parentItem()->boundingRect();
            if(parentBoundingRect.width()>GEO_TRACK_MIN_WIDTH){
                width=parentBoundingRect.width();
            }
            return QSizeF(width,height);
        }
    }else {
        return QGraphicsWidget::sizeHint(which,constraint);
    }
}
