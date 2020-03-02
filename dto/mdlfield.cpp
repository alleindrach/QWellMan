#include "mdlfield.h"
#include <QRegExp>
#include "common.h"
#include "mdldao.h"
#include "welldao.h"
MDLField::MDLField(QObject *parent) : Record(parent)
{

}
//如果当前字段查找方式=8，值转换方式
//  如果TableKey=true,则在lookupItem对应的表中，查找 对应的记录（idrec=当前记录值），显示对应表的RecordDes
//如果当前字段查找方式=11，则直接从main.pceMDLTableFieldLookupList读取LookupItem作为可选项，不需要翻译
//如果当前字段查找方式=12,则首先从lookupItem对应的表中，查找 对应的记录（idrec=当前记录值），显示对应表的RecordDes，如果 找不到 ，则按照11方式

QString MDLField::refValue(QString refId){
    if(this->LookupTyp()==MDLDao::Foreign||this->LookupTyp()==MDLDao::TabList){//12+8
        QList<MDLFieldLookup *> fl=MDL->fieldLookupinfo(this->KeyTbl(),this->KeyFld());
        foreach(MDLFieldLookup * fli,fl){
            if(fli->TableKey()){
                QSqlRecord rec=WELL->refRecord(fli->LookupItem(),refId);
                if(!rec.isEmpty()){
                    QString des=WELL->recordDes(fli->LookupItem(),rec);
                    return des;
                }
            }
        }
    }
    return refId;
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
                          MDLTable * refTable=MDL->tableInfo(fli->LookupItem());
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
      return rdResult;
}



MDLFieldVisible::MDLFieldVisible(QObject *parent) : MDLField(parent)
{

}

