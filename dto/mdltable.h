#ifndef MDLTABLE_H
#define MDLTABLE_H

#include "record.h"
#include "dto.h"
class MDLTable:public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE MDLTable(QObject * parent=nullptr);

    AUTO_PROPERTY(QString, KeyTbl)
    AUTO_PROPERTY(QString, CaptionLongP)
    AUTO_PROPERTY(QString, CaptionLongS)
    AUTO_PROPERTY(QString, CaptionShortP)
    AUTO_PROPERTY(QString, CaptionShortS)
    AUTO_PROPERTY(bool, OneToOne)
    AUTO_PROPERTY(QString, Help)
    AUTO_PROPERTY(bool, Calculated)
    AUTO_PROPERTY(QString, RecordDes)
    AUTO_PROPERTY(QString, HelpKey)
    AUTO_PROPERTY(QString, KeyFldCopyFlag)
    AUTO_PROPERTY(QString, SQLOrderBy)
    AUTO_PROPERTY(bool, Sequenced)
    AUTO_PROPERTY(bool, AllowInsertTop)
    AUTO_PROPERTY(bool, AllowSeqInvert)
    AUTO_PROPERTY(bool, CarryFwdFromPrevParent)
 private :
//    static int typeId;
//    static int ptypeId;
//    static int pltypeid;
};
DECLARE(MDLTable)
#endif // MDLTABLE_H
