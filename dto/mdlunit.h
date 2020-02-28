#ifndef MDLUNIT_H
#define MDLUNIT_H
#include <QObject>
#include "record.h"
#include "dto.h"
class MDLUnitSet : public Record
{
    Q_OBJECT

public:
    explicit Q_INVOKABLE MDLUnitSet(QObject *parent = nullptr);
    AUTO_PROPERTY(QString, KeySet)
    AUTO_PROPERTY(int, DisplayOrder)
    AUTO_PROPERTY(QString, KeySetInherit)
    AUTO_PROPERTY(QString, Comment)
signals:

};

DECLARE(MDLUnitSet)

class MDLUnitType : public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE MDLUnitType(QObject *parent = nullptr);
    AUTO_PROPERTY(QString, KeyType)
    AUTO_PROPERTY(QString, BaseUnits)
    AUTO_PROPERTY(QString, BaseFormat)
    AUTO_PROPERTY(bool, BaseFormatSpecial)
signals:

};

DECLARE(MDLUnitType)

class MDLUnitTypeSet : public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE MDLUnitTypeSet(QObject *parent = nullptr);
    AUTO_PROPERTY(QString, KeyType)
    AUTO_PROPERTY(QString, KeySet)
    AUTO_PROPERTY(QString, UserUnits)
    AUTO_PROPERTY(QString, UserFormat)
    AUTO_PROPERTY(bool, UserFormatSpecial)

signals:
};

DECLARE(MDLUnitTypeSet)

class MDLUnitConversion: public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE MDLUnitConversion(QObject *parent = nullptr);


    AUTO_PROPERTY(QString, BaseUnits)
    AUTO_PROPERTY(QString, UserUnits)
    AUTO_PROPERTY(float, Factor)
    AUTO_PROPERTY(int, Exponent)
    AUTO_PROPERTY(float, Offset)
    AUTO_PROPERTY(QString, Comments)
    signals:
};

DECLARE(MDLUnitConversion)
#endif // MDLUNIT_H
