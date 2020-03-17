#ifndef UTILITY_H
#define UTILITY_H
#include <QString>
#include <QVariant>
#include <QPointF>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

class Utility
{
public:
    Utility();
    static QString format(QString fmtstr,QVariant v);
    static bool isNumber(QVariant v);
    static int compare(QVariant left,QVariant right);
//    MINIMUM CURVATURE METHOD
//    https://spec2000.net/19-dip13.htm
    //计算狗腿值
//    参数 :
    static double dogLeg(double incl,double azim,double md,double preIncl=0,double preAzim=0);
    static double rf(double dogleg,double md);
    static double crsLgt(double md,double preMd=0);
    static double tvd(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0);
    static double ns(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preNs=0);
    static double ew(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0);
    static double dls(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0,double doglegInt=30);
    static double bur(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0,double doglegInt=30);
    //Horz Displ

//  whNs/whEw  Well Head Coords
    static double depart(double md,double ns,double ew,double whNs=0,double whEw=0);
    static double closureAzim(double md,double ns,double ew, double preNs=0,double whNs=0,double whEw=0);
    static double vs(double md,double ns,double ew,double whNs=0,double whEw=0,double planeVertSec=0);
    static QJsonObject QStringToJson(QString jsonString);
    static QString JsonToQString(QJsonObject jsonObject);
};

#endif // UTILITY_H
