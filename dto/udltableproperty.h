#ifndef UDLTABLEPROPERTY_H
#define UDLTABLEPROPERTY_H


#include <QObject>
#include "record.h"
#include "dto.h"
class UDLTableProperty : public Record
{

Q_OBJECT
public:
explicit Q_INVOKABLE  UDLTableProperty(QObject *parent = nullptr);

AUTO_PROPERTY(QString, KeyTbl)
AUTO_PROPERTY(QString, CaptionLongP)
AUTO_PROPERTY(QString, CaptionLongS)
AUTO_PROPERTY(QString, CaptionShortP)
AUTO_PROPERTY(QString, CaptionShortS)
AUTO_PROPERTY(QString, Help)
AUTO_PROPERTY(QString, RecordDes)
AUTO_PROPERTY(int, CarryFwdFromPrevParent)
AUTO_PROPERTY(QString, AdminNote)


};
DECLARE(UDLTableProperty)
#endif // UDLTABLEPROPERTY_H
