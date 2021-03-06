#ifndef QWMGEOWELLBOREVERTICALSECTION_H
#define QWMGEOWELLBOREVERTICALSECTION_H
#include <QGraphicsItem>
#include "qwmgeotrackcontent.h"
#include "qwmgeoformationinfo.h"
#include "qwmgeosimpleformation.h"
#include "qwmgeowellboresection.h"
class QWMGeoWellboreVerticalSection : public QWMGeoTrackContent
{
    Q_OBJECT
public:
    QWMGeoWellboreVerticalSection(QString idWell,QString idBore,
                                  QVector<QWMGeoFormationInfo>  formations,
                                  QVector<QWMGeoWellboreSizeInfo>  sizeInfos,
                                  QRectF ticks,QGraphicsItem * parent=nullptr);
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event) override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual QString dataAtPos(QPointF pos) override;
public slots:
    void on_hover_data(QPointF pos,QString des);
private:
    QString _idWell;
    QString _idBore;

    QVector<QWMGeoFormationInfo>  _formations;
//    QVector<QWMGeoSimpleFormation*> _formationWidgets;
    QVector<QVector<QGraphicsWidget*>> _layers;
};

#endif // QWMGEOWELLBOREVERTICALSECTION_H
