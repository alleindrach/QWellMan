#ifndef MDLFIELDLOOKUP_H
#define MDLFIELDLOOKUP_H

#include "record.h"
#include "dto.h"
class MDLFieldLookup : public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE  MDLFieldLookup(QObject *parent = nullptr);

    AUTO_PROPERTY(QString, KeyTbl)
    AUTO_PROPERTY(QString, KeyFld)
    AUTO_PROPERTY(QString, LookupItem)
    AUTO_PROPERTY(bool, TableKey)
    AUTO_PROPERTY(int, DisplayOrder)
};
DECLARE(MDLFieldLookup)
#endif // MDLFIELDLOOKUP_H
