#include "mdltable.h"
#include "udldao.h"
#include "udltableproperty.h"
#include "common.h"
MDLTable::MDLTable(QObject * parent):Record(parent)
{

}

QString MDLTable::CaptionLongP()
{
    UDLTableProperty  *prop=UDL->tableProperty(this->KeyTbl());
    if(prop==nullptr){
        return m_CaptionLongP;
    }else {
        return prop->CaptionLongP();
    }
}

QString MDLTable::CaptionLongS()
{
    UDLTableProperty  *prop=UDL->tableProperty(this->KeyTbl());
    if(prop==nullptr){
        return m_CaptionLongS;
    }else {
        return prop->CaptionLongS();
    }
}

QString MDLTable::CaptionShortP()
{
    UDLTableProperty  *prop=UDL->tableProperty(this->KeyTbl());
    if(prop==nullptr){
        return m_CaptionShortP;
    }else {
        return prop->CaptionShortP();
    }
}
//int MDLTable::typeId = qRegisterMetaType<Record>();
//int MDLTable::ptypeId=qRegisterMetaType<Record*>();
//int MDLTable::pltypeid=qRegisterMetaType<QList<Record*>>();

QString MDLTable::CaptionShortS()
{
    UDLTableProperty  *prop=UDL->tableProperty(this->KeyTbl());
    if(prop==nullptr){
        return m_CaptionShortS;
    }else {
        return prop->CaptionShortS();
    }
}
