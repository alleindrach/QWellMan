#ifndef UDLLIBTAB_H
#define UDLLIBTAB_H

#include <QObject>
#include "record.h"
#include "dto.h"
class UDLLibTab : public Record
{

Q_OBJECT
public:
explicit Q_INVOKABLE  UDLLibTab(QObject *parent = nullptr);

AUTO_PROPERTY(QString, KeySet)
AUTO_PROPERTY(QString, KeyTbl)
AUTO_PROPERTY(QString, KeyTab)
AUTO_PROPERTY(int, DisplayOrder)

};
DECLARE(UDLLibTab)

#endif // UDLLIBTAB_H
