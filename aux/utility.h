#ifndef UTILITY_H
#define UTILITY_H
#include <QString>
#include <QVariant>

class Utility
{
public:
    Utility();
    static QString format(QString fmtstr,QVariant v);
    static bool isNumber(QVariant v);
    static int compare(QVariant left,QVariant right);
};

#endif // UTILITY_H
