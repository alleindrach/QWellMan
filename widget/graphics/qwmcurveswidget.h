#ifndef QWMCURVEWIDGET_H
#define QWMCURVEWIDGET_H
#include "qwmgeotrackcontent.h"
#include "qwmgeocurveinfo.h"
class QWMCurvesWidget : public QWMGeoTrackContent
{
    Q_OBJECT
public:
    QWMCurvesWidget(QList<QPair<QWMGeoCurveInfo, QPointF*>> curves,QRectF ticks,QGraphicsItem * parent = nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    ~QWMCurvesWidget(){
        for(int i=0;i<_curves.size();i++){
            QPair<QWMGeoCurveInfo, QPointF *> curve=_curves[i];
            if(curve.second!=nullptr){
                delete[] curve.second;
                curve.second=nullptr;
            }
        }
    }
    virtual QString dataAtPos(QPointF pos);
private:
    QList<QPair<QWMGeoCurveInfo, QPointF*>> _curves;
    void drawCurve(QPainter *painter,QPair<QWMGeoCurveInfo, QPointF*> c);
};

#endif // QWMCURVEWIDGET_H
