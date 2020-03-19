#include "qwmgeosimpleformation.h"
#include "common.h"
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QPixmap>
#include <QBitmap>
#include "qwmiconselector.h"
QWMGeoSimpleFormation::QWMGeoSimpleFormation(QString idrec, QString title ,float depthScale,float topDepth,float bottomDepth,QString icon, QGraphicsItem  * parent)
    :QWMGeoTrackContent(QRectF(0,topDepth,100,bottomDepth),parent),_idrec(idrec),_title(title),_depthScale(depthScale),_topDepth(topDepth),_bottomDepth(bottomDepth),_icon(icon)
{

}

void QWMGeoSimpleFormation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    QRectF bounder= boundingRect() ;
    QBrush brush ;
    if(_icon.isNull()){

    }else{
        QFileInfo f=QWMIconSelector::findIcon(_icon);
        if(f.isFile()){


            QPixmap pixs=QPixmap(f.absoluteFilePath());
            pixs=pixs.copy(pixs.width()/5,pixs.height()/5,pixs.width()*0.6,pixs.height()*0.6);
//            pixs=pixs.scaled(pixs.width()/2,pixs.height()/2);
            brush=QBrush(pixs);
            painter->fillRect(bounder,brush);
        }else{
            brush= QBrush(Qt::BDiagPattern) ;
            painter->fillRect(bounder,brush);
        }
        if(this->isSelected()){
            painter->setPen(QPen(QBrush(DARK_RED),2,Qt::DotLine));
            painter->drawRect(boundingRect());
        }
    }


    painter->restore();
}

QSizeF QWMGeoSimpleFormation::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    if(which==Qt::PreferredSize  || which==Qt::MaximumSize||which==Qt::MinimumSize){
        int height=(_bottomDepth-_topDepth)*_depthScale;
        qDebug()<<which<<",Formation:"<<this->_title<<",top:"<<_topDepth<<",bottom:"<<_bottomDepth<<",scale:"<<_depthScale<<",sh:"<<height;
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

void QWMGeoSimpleFormation::setDepthScale(float depthScale)
{
    this->_depthScale=depthScale;
    this->updateGeometry();
    //    if(this->parentItem()!=nullptr)
    //        this->parentItem()->update();
}

QString QWMGeoSimpleFormation::dataAtPos(QPointF pos)
{
    QString des=QString("%3 %1~%2").arg(_topDepth).arg(_bottomDepth).arg(_title);
    return des;
}
