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
#include "QDateTime"
#include "utility.h"
QWMTableModel::QWMTableModel(QObject *parent,QSqlDatabase db) : QSqlRelationalTableModel(parent,db)
{
    //    connect(this ,&QSqlTableModel::primeInsert,this,&QWMTableModel::init_record_on_prime_insert);
}

QVariant QWMTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QSqlRecord record=this->record();

    if( role == Qt::DisplayRole){
        if( orientation==Qt::Orientation::Horizontal)
        {
            QString table=this->tableName();
            QString field=this->fieldNameEx(section);
            MDLField *  fieldInfo=MDL->fieldInfo(table,field);

            if(fieldInfo!=nullptr){
                QString cap=fieldInfo->CaptionLong();
                if(fieldInfo->Calculated())
                {

                }else{

                }
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
    }
    if(orientation==Qt::Orientation::Horizontal && section>=record.count()){
        //计算列的题头
        return QVariant();
    }else{
        return QSqlRelationalTableModel::headerData(section,orientation,role);
    }
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


    QSqlQuery q=this->query();

    QSqlRecord record=q.record();


    if(index.column()>=record.count())
    {
         //计算列
        QString table=this->tableName();
        QString field=_fieldsCalcInOrder[index.column()-record.count()];
        MDLField *  fieldInfo=MDL->fieldInfo(table,field);
        if(role==Qt::EditRole||role==Qt::DisplayRole){
            return QVariant();
        }else if(role == Qt::BackgroundColorRole ){
            if(fieldInfo->Calculated()){
                return QColor(253, 91, 100);
            }
        }else if(role==Qt::TextColorRole){
            if(fieldInfo->Calculated()){
                return QColor(0,0, 0);
            }
        }
    }else{
        //其他列
        QString fieldName=this->record().fieldName(index.column());
        QString tableName=this->tableName();
        //    if(fieldName=="WellName" && index.row()==0 && role==Qt::DisplayRole){
        //        qDebug()<<"["<<fieldName<<"]="<<value<<",["<<index.row()<<","<<index.column()<<"]";
        //    }
        if(role== PK_ROLE){
            QString idField=MDL->idField(this->tableName());
            int col=record.indexOf(idField);
            QVariant value=QSqlTableModel::data(this->index(index.row(),col));
            return value;
        }
        MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);
        if(role==Qt::EditRole || role==Qt::DisplayRole){
            QVariant value= QSqlRelationalTableModel::data(index,role);
            if(fieldInfo!=nullptr){
                //单位转换
                if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
                {
                    bool b=value.toBool();
                    return QVariant();
                }else if(fieldInfo->PhysicalType()==MDLDao::DateTime){
//                    QDateTime  dateValue=value.toDateTime();
                    return value;
                }
                else{
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
                }
                return value;
            }
            return value;
        }else{
            if (role == Qt::CheckStateRole)
            {
                if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
                {
                    QVariant value= QSqlRelationalTableModel::data(index,Qt::DisplayRole);
                    bool b=value.toBool();
                    QString  v=value.toString();
//                    qDebug()<<"V:"<<v<<",B:"<<b;
                    return  b ? Qt::Checked : Qt::Unchecked;
                }
            }
        }
        return QSqlRelationalTableModel::data(index,role);
    }
    return QVariant();

}

bool QWMTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;


    QSqlQuery q=this->query();

    QSqlRecord record=q.record();

    if(index.column()>=record.count()){
        //计算列
        return true;
    }else{
        QString fieldName=this->record().fieldName(index.column());
        QString tableName=this->tableName();
        MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);

        QVariant v=value;
        if(role==Qt::CheckStateRole && fieldInfo!=nullptr && fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
        {
            v=QVariant::fromValue(Qt::Checked==value);
            role=Qt::EditRole;//转换为EditRole，否则无效
        }
        else if(fieldInfo!=nullptr && Utility::isNumber(v)){

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
}


Qt::ItemFlags QWMTableModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid())
        return 0;
    Qt::ItemFlags flags=Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    if(_readonly)
        return flags;

    QSqlRecord record=this->record();
    if(index.column()>=record.count()){
        //计算列
        QString  fieldName=_fieldsCalcInOrder[index.column()-record.count()];
        return  flags;
    }else{
        QVariant v=QSqlRelationalTableModel::data(index);
        QString fieldName=this->record().fieldName(index.column());
        QString tableName=this->tableName();
        MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);
        if(fieldInfo!=nullptr){
            if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
            {
                return flags|Qt::ItemIsUserCheckable;
            }else{
                return flags|Qt::ItemIsEditable;
            }
        }
        return Qt::ItemIsEditable|flags;
    }
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

int QWMTableModel::fieldsCount()
{
    return _fieldsInOrder.length();
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

int QWMTableModel::fieldIndexEx(const QString &fieldName)
{
    int result=fieldIndex(fieldName);
    if(result<0){
        if(!_fieldsCalcMap.contains(fieldName)){
            MDLField * fieldInfo=MDL->fieldInfo(this->tableName(),fieldName);
            if(fieldInfo!=nullptr&& fieldInfo->Calculated()==true){
                _fieldsCalcMap.insert(fieldName,this->record().count()+_fieldsCalcMap.size());
                _fieldsCalcInOrder.append(fieldName);
            }
        }
        result=_fieldsCalcMap[fieldName];
    }
    return result;
}

QString QWMTableModel::fieldNameEx(const int index) const
{

    QString result;
    QSqlRecord  record=this->record();
    int fieldCount=record.count();
    if(index<fieldCount)
    {
        result=this->record().fieldName(index);

    }else{
        result=_fieldsCalcInOrder[index-fieldCount];
    }
    return result;
}

QModelIndex QWMTableModel::createIndex(int row, int col)
{
    return QSqlRelationalTableModel::createIndex(row,col);

}

void QWMTableModel::init_record_on_prime_insert(int row, QSqlRecord &record)
{

}
