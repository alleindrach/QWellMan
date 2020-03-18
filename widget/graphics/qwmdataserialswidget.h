#ifndef QWMDATASERIALSWIDGET_H
#define QWMDATASERIALSWIDGET_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>
#include <QSqlQueryModel>
#include <QVector>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include "qwmtrackcontent.h"
//用于显示数据系的GraphicsItem
//data ：float 为y轴坐标，QString分量为标注
class QWMDataSerialsWidget : public QWMTrackContent
{
public:
    QWMDataSerialsWidget( QVector<QPair<float,QString>> * data,QRectF ticks,QString title,int width=100,QGraphicsItem * parent = nullptr);
    ~QWMDataSerialsWidget(){
        if(_data!=nullptr)
        {
            _data->clear();
            delete _data;
            _data=nullptr;
        }
    }
//    virtual QRectF boundingRect() const  override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    int width() const{
      return _width;
    };

protected:
    QString title(){return _trackTitle;};
    QGraphicsProxyWidget * titleWidget(){return _titleWidget;};
    QVector<QPair<float,QString>>  * data(){ return _data;};

private:
    //    QRectF  _boundingRect;
    QVector<QPair<float,QString>> * _data{nullptr};
    int _width;
    QString _trackTitle;
    QGraphicsProxyWidget * _titleWidget;
//    bool _selected{false};
};

#endif // QWMDATASERIALSGRAPHICITEM_H
