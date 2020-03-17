#include "qwmcalculator.h"
#include <QScriptEngine>
#include <QSortFilterProxyModel>
#include <common.h>
#include <mdldao.h>
#include <udldao.h>
#include <welldao.h>
QWMCalculator::QWMCalculator(QString idWell,QString table,QSqlRecord rec,QObject *parent)
    : QObject(parent),_idWell(idWell),_table(table),_curRecord(rec)
{

}

QVariant QWMCalculator::evaluate(QString field ,QSqlRecord record,QString  idWell,QString table, QString equation)
{
    QScriptEngine engine;
    //    step 1 为当前记录的每一个字段添加全局变量
    for(int i=0;i<record.count();i++){
        QString field=record.fieldName(i);
        QVariant value=record.value(i);
        MDLField * fieldInfo=UDL->fieldInfo(table,field);
        if(fieldInfo!=nullptr){
            if(fieldInfo->PhysicalType()==MDLDao::Integer||fieldInfo->PhysicalType()==MDLDao::Long){
                engine.globalObject().setProperty(record.fieldName(i), value.toInt());
            }else if(fieldInfo->PhysicalType()==MDLDao::Single||fieldInfo->PhysicalType()==MDLDao::Double||fieldInfo->PhysicalType()==MDLDao::Currency){
                engine.globalObject().setProperty(record.fieldName(i), value.toDouble());
            }else if(fieldInfo->PhysicalType()==MDLDao::DateTime){
                if(fieldInfo->LookupTyp()==MDLDao::Date){
                    engine.globalObject().setProperty(record.fieldName(i), value.toDate().toString("yyyy-MM-dd"));
                }else if(fieldInfo->LookupTyp()==MDLDao::Time){
                    engine.globalObject().setProperty(record.fieldName(i), value.toTime().toString("HH:mm:ss"));
                }else if(fieldInfo->LookupTyp()==MDLDao::DateTime){
                    engine.globalObject().setProperty(record.fieldName(i), value.toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
                }
            }else if(fieldInfo->PhysicalType()==MDLDao::Boolean){
                engine.globalObject().setProperty(record.fieldName(i), value.toBool());
            }else{
                engine.globalObject().setProperty(record.fieldName(i), value.toString());
            }
        }else{
            if( value.isValid() && !value.isNull() && value.canConvert<QDateTime>()){
                engine.globalObject().setProperty(record.fieldName(i), value.toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            }else if( value.isValid() && !value.isNull() && value.canConvert<bool>()){
                engine.globalObject().setProperty(record.fieldName(i), value.toBool());
            }else {
                engine.globalObject().setProperty(record.fieldName(i), value.toString());
            }
        }

    }
    engine.globalObject().setProperty("idwell",idWell);
    engine.globalObject().setProperty("tablename",table);
    //    step 2 增加一个全局计算函数变量
    QObject *wmObject =new QWMCalculator(idWell,table,record) ;
    QScriptValue calObject = engine.newQObject(wmObject,QScriptEngine::ScriptOwnership);
    engine.globalObject().setProperty("wm", calObject);

    QScriptValue  r= engine.evaluate(equation);
    MDLField * fieldInfo=UDL->fieldInfo(table,field);

    QVariant value;
    if(!equation.isNull()&& !equation.isEmpty()){
        QScriptEngine engin;
        switch(fieldInfo->PhysicalType()){
        //            Integer=2,Long=3,Single=4,Double=5,Currency=6,DateTime=7,Boolean=11,Text=200,Memo=201,Bolob=205
        case MDLDao::Integer:
        case MDLDao::Long:
            value=r.toInt32();
            break;
        case MDLDao::Single:
        case MDLDao::Double:
        case MDLDao::Currency:
            value=r.toNumber();
            break;
        case MDLDao::DateTime:
            value=r.toDateTime();
            break;
        case MDLDao::Boolean:
            value=r.toBool();
            break;
        case MDLDao::Text:
        case MDLDao::Memo:
        case MDLDao::Bolob:
            value=r.toString();
            break;
        }
        return value;
    }
    return QVariant();
}

QString QWMCalculator::PBTDAll()
{
    return WELL->PBTDAll(_idWell);
}

QString QWMCalculator::TDAll()
{
    return WELL->TDAll(_idWell);
}

void QWMCalculator::calTVDwithMCM(QWMTableModel * model)
{
    QSqlRecord refRecord=model->record();
    model->setSort(refRecord.indexOf("MD"),Qt::AscendingOrder);
    QSqlRecord wellBore=WELL->wellbore4SurvyData(model->parentID());
//    double whNs=wellBore
//    for(int i=0;i<model->rowCount();i++){

//    }
}

double QWMCalculator::TD()
{
    return WELL->TD(_idWell);
}

double QWMCalculator::TVDAll()
{
    return  0;
}
