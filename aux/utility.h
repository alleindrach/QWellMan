#ifndef UTILITY_H
#define UTILITY_H
#include <QString>
#include <QVariant>

class Utility
{
public:
    Utility();
    static QString format(QString fmtstr,QVariant v);
};

#endif // UTILITY_H
