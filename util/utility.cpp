#include "utility.h"
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QtMath>
#include "common.h"
Utility::Utility()
{


}
QString Utility::format(QString fmtstr,QVariant v){
    QRegExp pat("([#,]*)([0,]*)(\.([0,]*)([#,]*))*");
    int pos=0;
    pos=pat.indexIn(fmtstr,0);
    if(pos>-1)
    {
        //        qDebug()<<pat.capturedTexts();
        int precisionAfterDot=pat.capturedTexts()[4].length()+pat.capturedTexts()[5].length();
        int precisionBeforeDot=pat.capturedTexts()[2].replace(",","").length()+(precisionAfterDot>0?1:0);
        int maxDigit=precisionAfterDot+precisionBeforeDot;
        QString padding=pat.capturedTexts()[2].replace(",","").length()>0?"0":"";
        QString format;
        if(precisionAfterDot>0){
            format="%"+QString("%1%2.%3%4").arg(padding).arg(maxDigit).arg(precisionAfterDot).arg("f");
            QString result=QString::asprintf(format.toStdString().c_str(),v.toDouble());
            return result;
        }
        else{

            format="%"+QString("%1%2d").arg(padding).arg(maxDigit);
            QString result=QString::asprintf(format.toStdString().c_str(),v.toInt());
            return result;
        }
    }else if(fmtstr.compare("latitude")==0)
    {
        return v.toString();
    }else if(fmtstr.compare("longitude")==0)
    {
        return v.toString();
    }else //ddddd h:Nn ampm
    {
        return v.toString();
    }
}

bool Utility::isNumber(QVariant v)
{
    bool isNumber=false;
    if(v.isNull())
        return false;
    if(v.toString().isNull()||v.toString().trimmed().isEmpty()){
        return false;
    }
    QRegExp re("[-+]?[0-9]*(\\.)?[0-9]*");  // a digit (\d), zero or more times (*)
    if (re.exactMatch(v.toString()))
        isNumber=true;
    return isNumber;
}
//0 equal ,1 left great then  right,-1 left  lessthen  right
int Utility::compare(QVariant left, QVariant right)
{
    if (left.userType() == QVariant::Invalid)
        return -1;
    if (right.userType() == QVariant::Invalid)
        return 1;
    switch (left.userType()) {
    case QVariant::Int:
        return left.toInt() < right.toInt()?-1:(left.toInt() > right.toInt()?1:0);
    case QVariant::UInt:
        return left.toUInt() < right.toUInt()?-1:(left.toUInt() > right.toUInt()?1:0);
    case QVariant::LongLong:
        return left.toLongLong() < right.toLongLong()?-1:(left.toLongLong() > right.toLongLong()?1:0);
    case QVariant::ULongLong:
        return left.toULongLong() < right.toULongLong()?-1:(left.toULongLong() > right.toULongLong()?1:0);
    case QMetaType::Float:
        return left.toFloat() < right.toFloat()?-1:(left.toFloat() > right.toFloat()?1:0);
    case QVariant::Double:
        return left.toDouble() < right.toDouble()?-1:(left.toDouble() > right.toDouble()?1:0);
    case QVariant::Char:
        return left.toChar() < right.toChar()?-1:(left.toChar() > right.toChar()?1:0);
    case QVariant::Date:
        return left.toDate() < right.toDate()?-1:(left.toDate() > right.toDate()?1:0);
    case QVariant::Time:
        return left.toTime() < right.toTime()?-1:(left.toTime() > right.toTime()?1:0);
    case QVariant::DateTime:
        return left.toDateTime() < right.toDateTime()?-1:(left.toDateTime() > right.toDateTime()?1:0);
    case QVariant::String:
    default:
        return left.toString().compare(right.toString()) ;
    }
}

double Utility::dogLeg(double incl,double azim,double md,double preIncl,double preAzim)
{

    double result=0;
    result=(IS_ZERO(md)?0:( qAcos(qCos(M_PI / 180*(incl - preIncl)) - qSin(M_PI / 180 * incl) * qSin(M_PI / 180 * preIncl) * (1- qCos( M_PI / 180 * (azim - preAzim))))));
    return result;

}
double Utility::rf(double dogleg,double md){
    return (IS_ZERO(md)?0:( 1+1 / 12 * qPow(dogleg,2) + 1/120 * qPow(dogleg,4) + 17/ 20160*qPow(dogleg,6)));
}

double Utility::crsLgt(double md,double preMd){
    return md-preMd;
}

double Utility::tvd(double incl,double azim, double md,double preIncl,double preAzim, double preMd,double preTvd)
{
    double _dogleg=dogLeg(preIncl,incl,preAzim,azim,md);
    double _rf=rf(_dogleg,md);
    double _crsLgt=crsLgt(preMd,md);
    //IF($C12=0," ",$F12÷2×$T12×(COS(PI()÷180×$D12)+COS(PI()÷180×$D11))+$G11)
    double result=(IS_ZERO(md)?0:(_crsLgt  / 2 * _rf * ( qCos( M_PI / 180 * incl ) + qCos( M_PI / 180 * preIncl )) + preTvd));
    return result;
}

double Utility::ns(double incl, double azim, double md, double preIncl, double preAzim, double preMd, double preTvd, double preNs)
{
    //    IF($C12=0," ",(SIN(PI()÷180×$D12)×COS(PI()÷180×$E12)+SIN(PI()÷180×$D11)×COS(PI()÷180×$E11))×$F12÷2×$T12+$I11)
    double _dogleg=dogLeg(preIncl,incl,preAzim,azim,md);
    double _rf=rf(_dogleg,md);
    double _crsLgt=crsLgt(preMd,md);
    double result=
            (IS_ZERO(md)?0:(qSin(M_PI /180 * incl ) * qCos( M_PI / 180 * azim ) + qSin( M_PI / 180 * preIncl ) * qCos(M_PI / 180 * preAzim )) * _crsLgt / 2 * _rf +preNs);
    return result;

}

double Utility::ew(double incl, double azim, double md, double preIncl, double preAzim, double preMd, double preTvd, double preEw)
{
    double _dogleg=dogLeg(preIncl,incl,preAzim,azim,md);
    double _rf=rf(_dogleg,md);
    double _crsLgt=crsLgt(preMd,md);
    double result=(IS_ZERO(md)?0:((qSin(M_PI / 180 * incl ) * qSin( M_PI / 180 * azim) + qSin(M_PI  / 180 * preIncl) * qSin(M_PI / 180 * preAzim)) * _crsLgt / 2 * _rf+preEw));
    return result;
}

double Utility::dls(double incl, double azim, double md, double preIncl, double preAzim, double preMd, double preTvd, double preEw,double doglegInt)
{
    double _dogleg=dogLeg(preIncl,incl,preAzim,azim,md);
    //    double _rf=rf(_dogleg,md);
    double _crsLgt=crsLgt(preMd,md);
    double result=(IS_ZERO(md)?0:_dogleg*18000/(_crsLgt * M_PI) * doglegInt/100);
    return result;
}

double Utility::bur(double incl, double azim, double md, double preIncl, double preAzim, double preMd, double preTvd, double preEw, double doglegInt)
{
    double _dogleg=dogLeg(preIncl,incl,preAzim,azim,md);
    double _rf=rf(_dogleg,md);
    double _crsLgt=crsLgt(preMd,md);
    double result=(IS_ZERO(md)?0:(incl-preIncl)*100/_crsLgt * doglegInt/100);
    return result;
}

double Utility::depart(double md,double ns,double ew,double whNs,double whEw)
{

    double result=(IS_ZERO(md)?0:qSqrt(qPow((ns - whNs),2) + qPow((ew - whEw),2)));
    return result;
}

double Utility::closureAzim(double md, double ns, double ew, double preNs, double whNs, double whEw)
{
    return IS_ZERO(md)?0:(qAtan2(ns - whNs,ew - whEw)<0?360 + 180/M_PI * qAtan2(ns - whNs,ew - whEw):180 / M_PI * qAtan2(ns - preNs,ew - whEw));
}

double Utility::vs(double md, double ns, double ew, double whNs,double whEw, double planeVertSec)
{
//IF($C12=0," ",($I12−$I$7)×COS(PI()÷180×$I$6)+($J12−$J$7)×SIN(PI()÷180×$I$6))
   return IS_ZERO(md)?0:(ns - whNs) * qCos(M_PI / 180 * planeVertSec) + (ew - whEw) * qSin(M_PI / 180 * planeVertSec);
}
//IF($C12=0," ",(SIN(PI()÷180×$D12)×SIN(PI()÷180×$E12)+SIN(PI()÷180×$D11)×SIN(PI()÷180×$E11))×$F12÷2×$T12+$J11)
QJsonObject Utility::QStringToJson(QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
    if(jsonDocument.isNull())
    {
        qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

QString Utility::JsonToQString(QJsonObject jsonObject)
{
    return QString(QJsonDocument(jsonObject).toJson());
}
