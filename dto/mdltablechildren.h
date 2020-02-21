#ifndef MDLTABLECHILDREN_H
#define MDLTABLECHILDREN_H

#include <QObject>
#include "record.h"
#include "dto.h"
class MDLTableChildren : public Record
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE MDLTableChildren(QObject * parent=nullptr);

    AUTO_PROPERTY(QString, KeyTbl)
    AUTO_PROPERTY(QString, KeyTblChild)
    AUTO_PROPERTY(int, DisplayOrder)
private :

};
DECLARE(MDLTableChildren)
#endif // MDLTABLECHILDREN_H
