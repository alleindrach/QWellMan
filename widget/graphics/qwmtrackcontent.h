#ifndef QWMTRACKCONTENT_H
#define QWMTRACKCONTENT_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneEvent>
#include <QSqlQueryModel>
#include <QVector>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
//用于显示数据系的GraphicsItem
//data ：float 为y轴坐标，QString分量为标注
class QWMTrackContent : public QGraphicsWidget
{
public:
    QWMTrackContent(QRectF ticks,QGraphicsItem * parent = nullptr);
    ~QWMTrackContent(){

    }
//    virtual QRectF boundingRect() const  override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent * event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void drawGrid(QPainter *painter);
    bool showGrid(){return _showGrid;}
    void setShowGrid(bool v){ _showGrid=v;};
    QRectF& ticks(){return _ticks;}
    QSize gridSize(){return _gridSize;};
private:
    //    QRectF  _boundingRect;
    bool _showGrid{true};
    QSize _gridSize{50,50};
    QRectF _ticks;//xy轴的刻度范围
};

#endif // QWMTRACKCONTENT_H
