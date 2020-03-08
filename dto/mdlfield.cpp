#include "mdlfield.h"
#include <QRegExp>
#include "common.h"
#include "mdldao.h"
#include "welldao.h"
#include "utility.h"
MDLField::MDLField(QObject *parent) : Record(parent)
{

}
//如果当前字段查找方式=8，值转换方式
//  如果TableKey=true,则在lookupItem对应的表中，查找 对应的记录（idrec=当前记录值），显示对应表的RecordDes
//如果当前字段查找方式=11，则直接从main.pceMDLTableFieldLookupList读取LookupItem作为可选项，不需要翻译
//如果当前字段查找方式=12,则首先从lookupItem对应的表中，查找 对应的记录（idrec=当前记录值），显示对应表的RecordDes，如果 找不到 ，则按照11方式
//参数： refID，该字段的一个值，指向参考表的一条记录
//      rec，本表含有该字段的一条记录
QString MDLField::refValue(QString refId,QSqlRecord rec){
    if(this->LookupTyp()==MDLDao::Foreign&& IS_SPEC_REF_FIELD(this) && !rec.isEmpty()){
        QString refTableName=rec.value(SPEC_REF_TABLE_FLD).toString();
        QSqlRecord rec2=WELL->refRecord(refTableName,refId);
        if(!rec2.isEmpty()){
            QString des=WELL->recordDes(refTableName,rec2);
            return des;
        }
    }
    else if(this->LookupTyp()==MDLDao::Foreign||this->LookupTyp()==MDLDao::TabList){//12+8
        QList<MDLFieldLookup *> fl=MDL->fieldLookupinfo(this->KeyTbl(),this->KeyFld());
        foreach(MDLFieldLookup * fli,fl){
            if(fli->TableKey()){
                QSqlRecord rec2=WELL->refRecord(fli->LookupItem(),refId);
                if(!rec2.isEmpty()){
                    QString des=WELL->recordDes(fli->LookupItem(),rec2);
                    return des;
                }
            }
        }
    }
    return refId;
}
//将数据库中的基本单位数据转换为当前单位体系的用户单位数据
//参数：v 数据库中的基本单位数据值
QVariant MDLField::displayValue(QVariant v)
{
    QVariant value=v;
    QString unitType=KeyUnit();
    if(!unitType.isEmpty()){
        MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
        if(baseUnitInfo!=nullptr)
        {
            QString baseUnit=baseUnitInfo->BaseUnits();
            QString baseUnitFormat=baseUnitInfo->BaseFormat();
            MDLUnitTypeSet * userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
            if(userUnitInfo!=nullptr){
                QString userUnit=userUnitInfo->UserUnits();
                QString userUnitFormat=userUnitInfo->UserFormat();
                value=MDL->unitBase2User(baseUnit,userUnit,value);
                value=Utility::format(baseUnitFormat,value);
            }
        }
    }
    return value;
}
//将【当前单位体系的用户单位数据】 转换为 【数据库中的基本单位数据】
//参数：dispValue用户单位体系数据
QVariant MDLField::baseValue(QVariant dispValue)
{
    QVariant value=dispValue;
    QString unitType=KeyUnit();
    if(!unitType.isEmpty()){
        MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
        if(baseUnitInfo!=nullptr)
        {
            QString baseUnit=baseUnitInfo->BaseUnits();
            QString baseUnitFormat=baseUnitInfo->BaseFormat();
            MDLUnitTypeSet * userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
            if(userUnitInfo!=nullptr){
                QString userUnit=userUnitInfo->UserUnits();
                QString userUnitFormat=userUnitInfo->UserFormat();
                value=MDL->unitUser2Base(baseUnit,userUnit,value);
                value=Utility::format(userUnitFormat,value);
            }
        }
    }
    return value;
}

QString MDLField::lookupTable(QSqlRecord  rec)
{
    if(this->LookupTyp()==MDLDao::Foreign){//12+8
        if(IS_SPEC_REF_FIELD(this)){
            //由实际记录的TblKeyParent指定
            if(!rec.isEmpty()){
                QString fn=SPEC_REF_TABLE_FLD;
                if( !rec.value(SPEC_REF_TABLE_FLD).isValid() && !rec.value(SPEC_REF_TABLE_FLD).isNull() )
                    return rec.value(SPEC_REF_TABLE_FLD).toString();
            }
        }else{
            QList<MDLFieldLookup *> fl=MDL->fieldLookupinfo(this->KeyTbl(),this->KeyFld());
            foreach(MDLFieldLookup * fli,fl){
                if(fli->TableKey()){
                    //如果需要查找的表中，有这个记录，则返回，否则继续超找下一个表
                    if(WELL->hasRecord(fli->LookupItem(),rec.value(this->KeyFld()).toString()))
                        return fli->LookupItem();
                }
            }
        }
    }
    return QString();
}
QString MDLField::caption()
{
    QString rd=CaptionLong();

    QString rdResult=rd;
    QHash<QString,QVariant> cachedValue;
    if(!rd.isNull()&&!rd.isEmpty()){
        QRegExp pat("<([\\w\.]*)>");
        int count = 0;
        int pos = 0;
        while ((pos = pat.indexIn(rd, pos)) != -1) {
            ++count;
            QString var=pat.cap(1);
            if(!cachedValue.contains(var)){
                if(var.compare("capl",Qt::CaseInsensitive)==0){
                    QList<MDLFieldLookup *> fl=MDL->fieldLookupinfo(this->KeyTbl(),this->KeyFld());
                    if(!fl.isEmpty()){
                        MDLFieldLookup *fli=fl.first();
                        MDLTable * refTable=UDL->tableInfo(fli->LookupItem());
                        if(refTable!=nullptr){
                            QString value=refTable->CaptionLongS();
                            cachedValue.insert(var,value);
                            //                              cachedTransferedValue.insert(var,value);
                        }
                    }
                }
            }
            pos += pat.matchedLength();
        }
        foreach(QString key,cachedValue.keys()){
            QVariant value=cachedValue[key];
            rdResult.replace("<"+key+">",value.toString());
        }
    }
    QString unitType=KeyUnit();
    if(!unitType.isNull() && !unitType.isEmpty()){
        MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
        MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
        if(userUnitInfo!=nullptr){
            QString userUnit=userUnitInfo->UserUnits();
            rdResult=rdResult+"["+userUnit+"]";
        }else{
            if(baseUnitInfo->BaseUnits()!=nullptr){
                rdResult=rdResult+"["+baseUnitInfo->BaseUnits()+"]";
            }
        }

    }
    return rdResult;
}



MDLFieldVisible::MDLFieldVisible(QObject *parent) : MDLField(parent)
{

}

