#ifndef QWMGEOTITLEITEM_H
#define QWMGEOTITLEITEM_H

#include "QGraphicsItem"
class QWMGeoTitleItem : public QGraphicsItem
{
public:
    QWMGeoTitleItem(QString idWell,int height, QGraphicsItem * parent);
    virtual QRectF boundingRect() const  override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

private :
    QString _idWell;
    int _height{20};
};

#endif // QWMGEOTITLEITEM_H
