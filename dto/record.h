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
            qFatal(QString("["+clazz+"*] 未注册！").toStdString().c_str());
        }
        const QMetaObject *metaObject = QMetaType::metaObjectForType(
                    typeID);
        auto o = metaObject->newInstance(Q_ARG(QObject *, parent));

        for(int i = o->metaObject()->propertyOffset(); i < o->metaObject()->propertyCount(); i++)
            o->setProperty(o->metaObject()->property(i).name(), record.value(record.indexOf(o->metaObject()->property(i).name()))) ;
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
