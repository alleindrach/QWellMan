#ifndef RECORD_H
#define RECORD_H

#include <QObject>
#include <QSqlRecord>
#include <QSqlQuery>
#include "dto.h"
#include "QVariant"
#include "QMetaType"
#include "QMetaObject"
#include "QMetaProperty"
#include <QDebug>
#include "QGenericArgument"
class MDLTable;
class Record : public QObject
{
    Q_OBJECT
public:
    explicit Q_INVOKABLE Record(QObject *parent = nullptr);
    explicit Q_INVOKABLE Record(const  Record & another);

    template<typename T>
    static T * fromSqlRecord(QString clazz,QSqlRecord record,QObject * parent=nullptr){

        int typeID=QMetaType::type((clazz+"*").toUtf8().data());
        if(typeID==0)
        {
            qFatal(QString(tr("[%1*] 未注册！")).arg(clazz).toStdString().c_str());
        }
        const QMetaObject *metaObject = QMetaType::metaObjectForType(
                    typeID);
        auto o = metaObject->newInstance(Q_ARG(QObject *, parent));

        for(int i = o->metaObject()->propertyOffset(); i < o->metaObject()->propertyCount(); i++)
        {

            QString name=QString(o->metaObject()->property(i).name());
            QVariant v=record.value(record.indexOf(o->metaObject()->property(i).name()));
            QVariant::Type type=o->metaObject()->property(i).type();
            if(v.isNull()||!v.isValid()){
                if(type==QVariant::Int||type==QVariant::UInt||type==QVariant::LongLong||type==QVariant::ULongLong||type==QVariant::Double||type==QMetaType::Float){
                    v=0;
                }
            }
            bool success=o->setProperty(name.toStdString().c_str(),v);
            if(!success){
                qDebug()<<" set Property Error:"<<name<<",v="<<v;
            }
            //            QVariant::Type type=o->metaObject()->property(i).type();
            //            if(name.compare("Calculated")==0
            //                    ){
            //                if(v.isNull()||!v.isValid()){
            //                    //                   if(type==QVariant::Int||type==QVariant::UInt||type==QVariant::LongLong||type==QVariant::ULongLong||type==QVariant::Double||type==QMetaType::Float){
            //                    //                       v=0;
            //                    //                   }
            //                    if(type==QVariant::Bool){
            //                        v=false;
            //                    }else if(type==QVariant::Int||type==QVariant::UInt||type==QVariant::LongLong||type==QVariant::ULongLong||type==QVariant::Double||type==QMetaType::Float){
            //                        v=0;
            //                    }
            //                }else{
            //                    v=v.toBool();
            //                }
            //                bool success=o->setProperty(name.toStdString().c_str(),v) ;
            //                bool result=o->property(name.toStdString().c_str()).toBool();
            //                qDebug()<<"name:"<<name<<"="<< v<<","<<success<<",va="<<result;
            //            }
        }
        return static_cast<T*>(o);
    }

    template<typename T>
    static QList<T *> fromSqlQuery(QString clazz,QSqlQuery q,QObject * parent=nullptr){
        QList<T *> result;
        while(q.next()){
            QSqlRecord record=q.record();
            auto o=fromSqlRecord<T>(clazz,record,parent);
            result.append(o);
        }
        return result;
    }

signals:
private :
    //    static int typeId;
    //    static int ptypeId;
    //    static int pltypeid;
};
DECLARE(Record)
#endif // RECORD_H
