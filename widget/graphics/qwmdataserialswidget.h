#ifndef QWMDATASERIALSWIDGET_H
#define QWMDATASERIALSWIDGET_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>
#include <QSqlQueryModel>
#include <QVector>
#include <QGraphicsWidget>
//用于显示数据系的GraphicsItem
//data ：float 为y轴坐标，QString分量为标注
class QWMDataSerialsWidget : public QGraphicsWidget
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
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent * event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
private:
    //    QRectF  _boundingRect;
    QVector<QPair<float,QString>> * _data{nullptr};
    bool _showGrid{true};
    QSize _gridSize{50,50};
    int _width;
    QRectF _ticks;//xy轴的刻度范围
    QString _trackTitle;
//    bool _selected{false};
};

#endif // QWMDATASERIALSGRAPHICITEM_H
