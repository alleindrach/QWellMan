#ifndef QWMGEOWELLBORESECTION_H
#define QWMGEOWELLBORESECTION_H
#include "qwmgeotrackcontent.h"
#include <QGraphicsWidget>
#include <QGraphicsItem>
#include "qwmgeoformationinfo.h"
class QWMGeoWellboreSection : public QWMGeoTrackContent
{
    Q_OBJECT
public:
    QWMGeoWellboreSection(QWMGeoWellboreSizeInfo  s,QRectF ticks,float depthScale, QGraphicsItem * parent =nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    void setDepthScale(float depthScale);
    virtual QString dataAtPos(QPointF pos);
private:
    QWMGeoWellboreSizeInfo _sizeInfo;
    float _depthScale;
};

#endif // QWMGEOWELLBORESECTION_H
