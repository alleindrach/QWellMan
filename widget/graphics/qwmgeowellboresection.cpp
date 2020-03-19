#include "qwmgeowellboresection.h"
#include "common.h"
#include <QPainter>
QWMGeoWellboreSection::QWMGeoWellboreSection(QWMGeoWellboreSizeInfo s, QRectF ticks, float depthScale, QGraphicsItem *parent)
    :QWMGeoTrackContent(ticks,parent),_sizeInfo(s),_depthScale(depthScale)
{

}

void QWMGeoWellboreSection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if(_sizeInfo.radius>0){
        QRectF bounder= boundingRect() ;
        QBrush brush ;
        brush= QBrush(_sizeInfo.hatchColor, _sizeInfo.hatchStyle) ;
        float middleX=bounder.width()/2;
        float semiWidth=bounder.width()*_sizeInfo.radius/ticks().width()/2;
        QRectF sectionRect(middleX-semiWidth,0,semiWidth*2,bounder.height());
        painter->fillRect(sectionRect,brush);
        painter->setPen(QPen(QBrush(_sizeInfo.borderColor),1,Qt::SolidLine));
        painter->drawRect(sectionRect);
        if(this->isSelected()){
            painter->setPen(QPen(QBrush(DARK_RED),2,Qt::DotLine));
            painter->drawRect(sectionRect);
        }
    }
    painter->restore();
}

QSizeF QWMGeoWellboreSection::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if(which==Qt::PreferredSize  || which==Qt::MaximumSize||which==Qt::MinimumSize){
        int height=(_sizeInfo.bottom-_sizeInfo.top)*_depthScale;
        int width=GEO_TRACK_MIN_WIDTH;
        if(this->parentItem()==nullptr){

        }else{
            QRectF parentBoundingRect=this->parentItem()->boundingRect();
            if(parentBoundingRect.width()>GEO_TRACK_MIN_WIDTH){
                width=parentBoundingRect.width();
            }
        }
        return QSizeF(width,height);
    }else{
        return QGraphicsWidget::sizeHint(which,constraint);
    }
}

void QWMGeoWellboreSection::setDepthScale(float depthScale)
{
    this->_depthScale=depthScale;
    this->updateGeometry();
}

QString QWMGeoWellboreSection::dataAtPos(QPointF pos)
{
    QString des=QString("%3 %1~%2").arg(_sizeInfo.top).arg(_sizeInfo.bottom).arg(_sizeInfo.radius);
    return des;
}
