#ifndef QWMCURVEWIDGET_H
#define QWMCURVEWIDGET_H
#include "qwmtrackcontent.h"

class QWMCurveWidget : public QWMTrackContent
{
public:
    QWMCurveWidget( QPointF * data,int size,QRectF ticks,QGraphicsItem * parent = nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    ~QWMCurveWidget(){
        if(_data!=nullptr){
            delete[] _data;
            _data=nullptr;
        }
    }
private:
    QPointF *  _data{nullptr};
    int _size{0};
};

#endif // QWMCURVEWIDGET_H
