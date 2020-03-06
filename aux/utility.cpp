#include "utility.h"
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QDateTime>
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
