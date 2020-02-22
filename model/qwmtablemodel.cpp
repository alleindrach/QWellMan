#include "qwmtablemodel.h"
#include <QSqlRelationalTableModel>
#include "QSqlRecord"
#include "mdldao.h"
#include "udldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QDebug"
#include "QUuid"
#include "utility.h"
QWMTableModel::QWMTableModel(QObject *parent,QSqlDatabase db) : QSqlRelationalTableModel(parent,db)
{
    //    connect(this ,&QSqlTableModel::primeInsert,this,&QWMTableModel::init_record_on_prime_insert);
}

QVariant QWMTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role == Qt::DisplayRole){
        if( orientation==Qt::Orientation::Horizontal)
        {
            QString fieldName=this->record().fieldName(section);
            QString tableName=this->tableName();
            MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);
            if(fieldInfo!=nullptr){
                QString cap=fieldInfo->CaptionLong();

                QString unitType=fieldInfo->KeyUnit();
                if(!unitType.isNull() && !unitType.isEmpty()){
                    MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
                    MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
                    if(userUnitInfo!=nullptr){
                        QString userUnit=userUnitInfo->UserUnits();
                        cap=cap+"-"+userUnit;
                    }

                }
                return cap;
            }
            return QSqlRelationalTableModel::headerData(section,orientation,role);
        }
        //        else
        //        {
        //            return QString::number(section+1, 3);
        //        }

        //    }else if(role=FIELD_ROLE && orientation==Qt::Horizontal){
        //         QString fieldName=this->record().fieldName(section);
        //         return fieldName;
    }
//    if(orientation==Qt::Horizontal && role==VISIBLE_ROLE){
//        QVariant   v=QSqlRelationalTableModel::headerData(section,orientation,role);
//        qDebug()<<"V["<<section<<"]="<<v.toString();
//    }else if(orientation==Qt::Horizontal && role==Qt::SizeHintRole){
//        qDebug()<<"y";
//    }
    return QSqlRelationalTableModel::headerData(section,orientation,role);
}
void QWMTableModel::setTable(const QString &tableName){

    QSqlRelationalTableModel::setTable(tableName);
    this->initFields(tableName);
}
QVariant QWMTableModel::data(const QModelIndex &index, int role) const
{


    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount() || index.row() < 0)
        return QVariant("-");

    QVariant value= QSqlRelationalTableModel::data(index,role);
    QSqlQuery q=this->query();

    QSqlRecord record=q.record();

    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    //    if(fieldName=="WellName" && index.row()==0 && role==Qt::DisplayRole){
    //        qDebug()<<"["<<fieldName<<"]="<<value<<",["<<index.row()<<","<<index.column()<<"]";
    //    }
    if(role== PK_ROLE){
        QString idField=MDL->idField(this->tableName());
        int col=record.indexOf(idField);
        value=QSqlTableModel::data(this->index(index.row(),col));
        return value;
    }
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);
    if(fieldInfo!=nullptr){
        if(role== Qt::DisplayRole || role==Qt::EditRole){
            //单位转换

            QString unitType=fieldInfo->KeyUnit();
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
                    }
                }
            }
            return value;
        }

        if(role == Qt::BackgroundColorRole ){
            if(fieldInfo->Calculated()){
                return QColor(253, 91, 100);
            }
        }
        if(role==Qt::TextColorRole){
            if(fieldInfo->Calculated()){
                return QColor(0,0, 0);
            }
        }
        if (role == Qt::CheckStateRole)
        {
            if(fieldInfo->PhysicalType()==11)//booelan
            {
                bool b=value.toBool();
                return  b ? Qt::Checked : Qt::Unchecked;
            }
        }
    }
    return value;

}

bool QWMTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;


    QSqlQuery q=this->query();

    QSqlRecord record=q.record();


    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);

    QVariant v=value;
    if(fieldInfo!=nullptr && Utility::isNumber(v)){
        QString unitType=fieldInfo->KeyUnit();
        if(!unitType.isEmpty()){
            MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
            if(baseUnitInfo!=nullptr)
            {
                QString baseUnit=baseUnitInfo->BaseUnits();
                QString baseUnitFormat=baseUnitInfo->BaseFormat();
                MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
                if(userUnitInfo!=nullptr){
                    QString userUnit=userUnitInfo->UserUnits();
                    QString userUnitFormat=userUnitInfo->UserFormat();
                    v=MDL->unitUser2Base(baseUnit,userUnit,v);
                }
            }
        }
    }
    bool success= QSqlRelationalTableModel::setData(index, v,  role);
    return success;

}


Qt::ItemFlags QWMTableModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid())
        return 0;
    if(_readonly)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;


    QSqlRecord record=this->record();
    QVariant v=QSqlRelationalTableModel::data(index);
    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);
    if(fieldInfo!=nullptr){
        if(fieldInfo->PhysicalType()==11)//booelan
        {
            return  Qt::ItemIsEditable|Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        }else{
            return Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
        }
    }
    return Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

bool QWMTableModel::readonly()
{
    return _readonly;
}

void QWMTableModel::setReadonly(bool v)
{
    _readonly=v;
}

void QWMTableModel::initFields(const QString &tableName)
{
    QStringList visibleFieldsList=UDL->fieldsVisibleInOrder(APP->profile(),tableName);
    setVisibleFields(visibleFieldsList);
    QSqlRecord rec=record();
    for(int i=0;i<rec.count();i++){
        QString fieldName=rec.fieldName(i);
        setHeaderData(i,Qt::Horizontal,fieldName,FIELD_ROLE);
    }
}

void QWMTableModel::setVisibleFields(const QStringList visibleFieldsList)
{
    _fieldsInOrder.clear();
    _fieldsInOrderVice.clear();
    for(int i=0;i<visibleFieldsList.length();i++){
        _fieldsInOrder.append(visibleFieldsList[i]);
        _fieldsInOrderVice.insert(visibleFieldsList[i],i+1);
    }
    _visibleFields=visibleFieldsList.length();
    QSqlRecord rec=this->record();
    int recn=rec.count();
    for(int i=0;i<recn;i++){
        QString fn=rec.fieldName(i);
        if(!_fieldsInOrderVice.contains(fn)){
            _fieldsInOrderVice.insert(fn,_fieldsInOrder.length());
            _fieldsInOrder.append(fn);
        }
    }
}

void QWMTableModel::mergeVisibleFields( QStringList lst)
{
    for(int i=0;i<_visibleFields;i++){
        lst<<_fieldsInOrder[i];
    }
    lst.removeDuplicates();
    lst.sort(Qt::CaseInsensitive);
    setVisibleFields(lst);
}

int QWMTableModel::fieldPosByOrder(const QString &field)
{
    if(_fieldsInOrderVice.contains(field))
        return _fieldsInOrderVice[field];
    else
        return -1;
}

int QWMTableModel::visibleFieldsCount()
{
    return _visibleFields;
}

const QString QWMTableModel::fieldInPosByOrder(int pos)
{
    QSqlRecord rec=this->record();
    int recn=rec.count();
    if(pos<_fieldsInOrder.size()&&pos>=0)
        return _fieldsInOrder[pos];
    else
        return QString();
}

bool QWMTableModel::isFieldVisible(const QString &field)
{
   if( _fieldsInOrderVice[field]<_visibleFields)
       return true;
   else
       return false;
}

void QWMTableModel::init_record_on_prime_insert(int row, QSqlRecord &record)
{

}
