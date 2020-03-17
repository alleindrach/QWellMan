#ifndef QWMTRACKWIDGET_H
#define QWMTRACKWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
class QWMGeoTrackWidget : public QGraphicsWidget
{
public:
    QWMGeoTrackWidget(QString title,QGraphicsScene *scene,QGraphicsItem * parent=nullptr);
    void setTitle(QString  s);
    void setContent(QGraphicsWidget * widget);
private:
    QString _title;
    QGraphicsProxyWidget * _titleWidget{nullptr};
};

#endif // QWMTRACKWIDGET_H
