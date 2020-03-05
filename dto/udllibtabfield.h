#ifndef UDLLIBTABFIELD_H
#define UDLLIBTABFIELD_H

#include <QObject>
#include "record.h"
#include "dto.h"
class UDLLibTabField : public Record
{

Q_OBJECT
public:
explicit Q_INVOKABLE  UDLLibTabField(QObject *parent = nullptr);

AUTO_PROPERTY(QString, KeySet)
AUTO_PROPERTY(QString, KeyTbl)
AUTO_PROPERTY(QString, KeyTab)
AUTO_PROPERTY(QString, KeyFld)
AUTO_PROPERTY(int, DisplayOrder)

};
DECLARE(UDLLibTabField)
#endif // UDLLIBTABFIELD_H
