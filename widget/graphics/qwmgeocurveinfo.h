#ifndef QWMGEOTRACKINFO_H
#define QWMGEOTRACKINFO_H
#include "QRectF"
#include <QString>
#include <QColor>
struct QWMGeoCurveInfo
{
    QRectF ticks;
    QString title;
    Qt::PenStyle lineStyle;
    QColor color;
    int lineWidth;
    int points;
};

#endif // QWMGEOTRACKINFO_H
