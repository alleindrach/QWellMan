#ifndef UDLLIBGROUP_H
#define UDLLIBGROUP_H
#include <QObject>
#include "record.h"
#include "dto.h"
class UDLLibGroup : public Record
{

Q_OBJECT
public:
explicit Q_INVOKABLE  UDLLibGroup(QObject *parent = nullptr);

AUTO_PROPERTY(QString, KeySet)
AUTO_PROPERTY(QString, KeyGroup)
AUTO_PROPERTY(QString, KeyTbl)
AUTO_PROPERTY(QString, TblApp)
AUTO_PROPERTY(QString, Caption)
AUTO_PROPERTY(int, DisplayOrder)

};
DECLARE(UDLLibGroup)

#endif // UDLLIBGROUP_H
