#ifndef QWMGEOFORMATIONINFO_H
#define QWMGEOFORMATIONINFO_H
#include "QRectF"
#include <QString>
#include <QColor>
struct QWMGeoFormationInfo{
    QString title;
    QString id;
    QString desc;
    float top;
    float bottom;
    QString icon;
};
struct QWMGeoWellboreSizeInfo{
    QString id;
    QString desc;
    float top;
    float bottom;
    float radius;
    QColor borderColor;
    QColor hatchColor;
    Qt::BrushStyle hatchStyle;
};

#endif // QWMGEOFORMATIONINFO_H
