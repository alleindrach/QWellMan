#ifndef MDLFIELD_H
#define MDLFIELD_H

#include <QObject>
#include "record.h"
#include "dto.h"
class MDLField : public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE  MDLField(QObject *parent = nullptr);

    AUTO_PROPERTY(QString, KeyTbl)
    AUTO_PROPERTY(QString, KeyFld)
    AUTO_PROPERTY(QString, GroupName)
    AUTO_PROPERTY(QString, CaptionLong)
    AUTO_PROPERTY(QString, CaptionShort)
    AUTO_PROPERTY(int, DisplayOrder)
    AUTO_PROPERTY(bool, Calculated)
    AUTO_PROPERTY(int, LookupTyp)
    AUTO_PROPERTY(int, AvgSize)
    AUTO_PROPERTY(QString, KeyUnit)
    AUTO_PROPERTY(int, HAlign)
    AUTO_PROPERTY(bool, CarryFwdWithParent)
    AUTO_PROPERTY(bool, CarryFwd)
    AUTO_PROPERTY(QString, CarryFwdFromFld)
    AUTO_PROPERTY(double, CarryFwdIncrement)
    AUTO_PROPERTY(QString, InheritFirstFromParentFld)
    AUTO_PROPERTY(QString, LookupTableName)
    AUTO_PROPERTY(QString, LookupFieldName)
    AUTO_PROPERTY(int, PhysicalSize)
    AUTO_PROPERTY(int, PhysicalType)
    AUTO_PROPERTY(QString, Help)
    AUTO_PROPERTY(QString, HelpKey)
    AUTO_PROPERTY(QString, NoteKeyFld)
    AUTO_PROPERTY(QString, Temp_FieldStatus)
    AUTO_PROPERTY(QString, Temp_CalcPriority)
    AUTO_PROPERTY(QString, Temp_CalcDifficulty)

    public :
    QString caption();
    QString refValue(QString refId);
    QString lookupTable(QSqlRecord  rec=QSqlRecord());
    signals:

};

    class MDLFieldVisible : public MDLField
    {
        Q_OBJECT
    public:
        explicit Q_INVOKABLE  MDLFieldVisible(QObject *parent = nullptr);
        AUTO_PROPERTY(int, Visible)
    };
    DECLARE(MDLField)
    DECLARE(MDLFieldVisible)
#endif // MDLFIELD_H
