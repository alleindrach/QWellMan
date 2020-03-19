#ifndef QWMTRACKWIDGET_H
#define QWMTRACKWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include "qwmgeocurveinfo.h"
#include "qwmgeotrackcontent.h"
#include "qwmgeotracktitle.h"
class QWMGeoTrackWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    QWMGeoTrackWidget(QWMGeoTrackTitle * title,QWMGeoTrackContent * content,QGraphicsItem * parent=nullptr);
    int titleContentHight(){
        return _title->contentheight();
    }
    void setTitleHeight(int v) {
        _title->setHeight(v);
    }
signals:
    void hoverData(QPointF pos,QString comp,QString des);
public slots:
    void on_hover_data(QPointF pos,QString des);
private:
    QWMGeoTrackTitle* _title{nullptr};
    QWMGeoTrackContent * _content{nullptr};

};

#endif // QWMTRACKWIDGET_H
