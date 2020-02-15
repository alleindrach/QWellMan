#include "record.h"
#include "QVariant"
#include "QMetaType"
#include "QMetaObject"
#include "QMetaProperty"
#include "QGenericArgument"
Record::Record(QObject *parent) : QObject(parent)
{

}

Record::Record(const Record &another)
{
    this->setParent(another.parent());
}
//int Record::typeId = qRegisterMetaType<Record>();
//int Record::ptypeId=qRegisterMetaType<Record*>();
//int Record::pltypeid=qRegisterMetaType<QList<Record*>>();
//template<typename  T>
//T *  Record::fromSqlRecord(QString clazz,QSqlRecord record,QObject * parent)
//{
//    const QMetaObject *metaObject = QMetaType::metaObjectForType(
//        QMetaType::type(clazz.toUtf8().data()));
//    auto o = metaObject->newInstance(QGenericArgument(nullptr, parent));

//    for(int i = o->metaObject()->propertyOffset(); i < o->metaObject()->propertyCount(); i++)
//        o->setProperty(o->metaObject()->property(i).name(), record.value(record.indexOf(o->metaObject()->property(i).name()))) ;
//    return static_cast<T*>(o);
//}
//template<typename  T>
//QList<T *  > Record::fromSqlQuery(QString clazz,QSqlQuery q,QObject * parent)
//{

//    QList<T *> result;
//    while(q.next()){
//        QSqlRecord record=q.record();
//        auto o=fromSqlRecord<T>(clazz,record,parent);
//        result.append(o);
//    }
//     return result;
//}
