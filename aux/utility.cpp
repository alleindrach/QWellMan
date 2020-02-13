#include "utility.h"
#include <QDebug>
Utility::Utility()
{


}
QString Utility::format(QString fmtstr,QVariant v){
    QRegExp pat("([#,]*)([0,]*)(\.([0,]*)([#,]*))*");
    int pos=0;
    pos=pat.indexIn(fmtstr,0);
    if(pos>-1)
    {
        qDebug()<<pat.capturedTexts();
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
