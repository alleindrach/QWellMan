#ifndef UTILITY_H
#define UTILITY_H
#include <QString>
#include <QVariant>
#include <QPointF>
class Utility
{
public:
    Utility();
    static QString format(QString fmtstr,QVariant v);
    static bool isNumber(QVariant v);
    static int compare(QVariant left,QVariant right);
    //计算狗腿值
//    参数 :
    static double dogLeg(double incl,double azim,double md,double preIncl=0,double preAzim=0);
    static double rf(double dogleg,double md);
    static double crsLgt(double md,double preMd=0);
//    IF($C12=0," ",$F12÷2×$T12×(COS(PI()÷180×$D12)+COS(PI()÷180×$D11))+$G11)
    static double tvd(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0);
    static double ns(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preNs=0);
    static double ew(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0);
    static double dls(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0,double doglegInt=30);
    static double bur(double incl,double azim, double md,double preIncl=0,double preAzim=0, double preMd=0,double preTvd=0,double preEw=0,double doglegInt=30);
    //Horz Displ

    static double depart(double md,double ns,double ew,double whNs=0,double whEw=0);

};

#endif // UTILITY_H