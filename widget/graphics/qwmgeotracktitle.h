#ifndef QWMGEOTRACKTITLE_H
#define QWMGEOTRACKTITLE_H

#include "QGraphicsWidget"
#include "qwmgeocurveinfo.h"
#include "common.h"
class QWMGeoTrackTitle : public QGraphicsWidget
{
    Q_OBJECT
public:
    QWMGeoTrackTitle(QString idWell,QString title,QList<QWMGeoCurveInfo> items, QGraphicsItem * parent);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;
    int contentheight() const{
        return (_items.size()+1)*GEO_TITLE_ITEM_HEIGHT;
    }
    void setHeight(int v){ _height=v;}
    QString title(){
        return _title;
    }
private :
    QString _idWell;
    QString _title;
    QList<QWMGeoCurveInfo> _items;
    int _height{0};
};

#endif // QWMGEOTITLEITEM_H
