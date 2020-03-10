#include "qwmcalculator.h"
#include <QScriptEngine>
#include <common.h>
#include <mdldao.h>
#include <udldao.h>
QWMCalculator::QWMCalculator(QString idWell,QString table,QSqlRecord rec,QObject *parent)
    : QObject(parent),_idWell(idWell),_table(table),_curRecord(rec)
{

}

QScriptValue QWMCalculator::evaluate(QSqlRecord record,QString  idWell,QString table, QString equation)
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
//    step 2 增加一个全局计算函数变量
    QObject *wmObject =new QWMCalculator(idWell,table,record) ;
    QScriptValue calObject = engine.newQObject(wmObject,QScriptEngine::ScriptOwnership);
    engine.globalObject().setProperty("wm", calObject);
    return engine.evaluate(equation);
}
