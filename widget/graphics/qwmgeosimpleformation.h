#ifndef QWMFORMATION_H
#define QWMFORMATION_H
#include "QGraphicsWidget"
#include "qwmgeotrackcontent.h"
//单井的地层，矩形，多井的，就是不规则的线了
class QWMGeoSimpleFormation: public QWMGeoTrackContent
{
    Q_OBJECT
public:
    QWMGeoSimpleFormation(QString idrec, QString title ,float depthScale,float topDepth,float bottomDepth,QString icon, QGraphicsItem  * parent);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    void setDepthScale(float depthScale);
    virtual QString dataAtPos(QPointF pos);
private :
    QString _idrec;
    QString _title;
    QString _icon;
    float _topDepth;
    float _bottomDepth;
    float _depthScale;

};

#endif // QWMFORMATIONGRAPHICSITEM_H
