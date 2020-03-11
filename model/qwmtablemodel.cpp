#include "qwmtablemodel.h"
#include <QSqlTableModel>
#include "QSqlRecord"
#include "mdldao.h"
#include "udldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QDebug"
#include <QScriptEngine>
#include <QScriptValue>
#include "QUuid"
#include "QDateTime"
#include "utility.h"
#include "edldao.h"
#include "qwmcalculator.h"
#include "QSqlField"
QWMTableModel::QWMTableModel(QString idWell,QObject *parent,QSqlDatabase db)
    : QSqlTableModel(parent,db),_idWell(idWell)
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
            MDLField *  fieldInfo=UDL->fieldInfo(table,field);

            if(fieldInfo!=nullptr){
                QString cap=fieldInfo->CaptionLong();
                if(fieldInfo->Calculated())
                {

                }else if(fieldInfo->LookupTyp()==MDLDao::Foreign){
                    cap=fieldInfo->caption();//lookup type=8,<capl>
                }
                QString unitType=fieldInfo->KeyUnit();
                if(!unitType.isNull() && !unitType.isEmpty()){
                    MDLUnitType * baseUnitInfo=MDL->baseUnitKey(unitType);
                    MDLUnitTypeSet* userUnitInfo=MDL->userUnitKey(APP->unit(),unitType);
                    if(userUnitInfo!=nullptr){
                        QString userUnit=userUnitInfo->UserUnits();
                        cap=cap+"["+userUnit+"]";
                    }else{
                        if(baseUnitInfo->BaseUnits()!=nullptr){
                            cap=cap+"["+baseUnitInfo->BaseUnits()+"]";
                        }
                    }

                }
                return cap;
            }
            return QSqlTableModel::headerData(section,orientation,role);
        }
    }
    if(orientation==Qt::Orientation::Horizontal && section>=record.count()){
        //计算列的题头
        return QVariant();
    }else{
        return QSqlTableModel::headerData(section,orientation,role);
    }
}
void QWMTableModel::setTable(const QString &tableName){

    QSqlTableModel::setTable(tableName);
    this->initFields(tableName);
}

bool QWMTableModel::select()
{
    return QSqlTableModel::select();
}

void QWMTableModel::connectSignals()
{
    disconnect(this,&QWMTableModel::primeInsert,this,&QWMTableModel::init_record_on_prime_insert);
    if(!this->isSignalConnected(QMetaMethod::fromSignal(&QWMTableModel::primeInsert))){
        connect(this,&QWMTableModel::primeInsert,this,&QWMTableModel::init_record_on_prime_insert);
    }
    disconnect(this,&QWMTableModel::beforeUpdate,this,&QWMTableModel::before_update_record);
    if(!this->isSignalConnected(QMetaMethod::fromSignal(&QWMTableModel::beforeUpdate))){
        connect(this,&QWMTableModel::beforeUpdate,this,&QWMTableModel::before_update_record);
    }
    disconnect(this,&QWMTableModel::beforeInsert,this,&QWMTableModel::before_insert);
    if(!this->isSignalConnected(QMetaMethod::fromSignal(&QWMTableModel::beforeInsert))){
        connect(this,&QWMTableModel::beforeInsert,this,&QWMTableModel::before_insert);
    }
}
QVariant QWMTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount() || index.row() < 0)
        return QVariant("-");


    QSqlQuery q=this->query();

    QSqlRecord record=q.record();


    QString table=this->tableName();
    QString field=this->fieldNameEx(index.column());
    MDLField *  fieldInfo=UDL->fieldInfo(table,field);

    if(fieldInfo!=nullptr && fieldInfo->Calculated()==true)
    {
        if(role==SORT_ROLE){
            return QVariant();
        }else if(role == Qt::BackgroundColorRole ){
            return QColor(218, 170, 170);
        }else if(role==Qt::TextColorRole){
            return QColor(0,0, 0);
        }
    }

    //   所有列

    if(role== PK_ROLE){
        QString idField=MDL->idField(this->tableName());
        int col=record.indexOf(idField);
        QVariant value=QSqlTableModel::data(this->index(index.row(),col));
        return value;
    }

    if(role==SORT_ROLE){
        if(fieldInfo->Calculated()==true){
            if(_calcData.contains(index.row())){
                return _calcData[index.row()].value(index.column()-_natureFieldCount);
            }
            return  QVariant();
        }
        return QSqlTableModel::data(index,Qt::EditRole);
    }else if(role==Qt::EditRole || role==Qt::DisplayRole||role==DATA_ROLE){
        QVariant value;
        if(fieldInfo!=nullptr && fieldInfo->Calculated()==true){
            if(_calcData.contains(index.row())){
                value= _calcData[index.row()].value(index.column()-_natureFieldCount);
            }else
            {
                value=QVariant();
            }
        }else{
            value= QSqlTableModel::data(index,Qt::EditRole);
        }
        if(fieldInfo!=nullptr){
            if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
            {
                if(role==Qt::EditRole||role==DATA_ROLE)
                {
                    bool b=value.toBool();
                    return b;
                }else
                {
                    return QVariant();
                }
            }else if(fieldInfo->PhysicalType()==MDLDao::DateTime){
                if(role==Qt::DisplayRole){
                    if(fieldInfo->LookupTyp()==MDLDao::Date){
                        QDate date=value.toDate();
                        return date.toString("yyyy-MM-dd");
                    }else if(fieldInfo->LookupTyp()==MDLDao::Time){
                        QTime time=value.toTime();
                        return time.toString("hh-mm-ss");;
                    }else{
                        return value.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    }
                }else
                {
                    if(fieldInfo->LookupTyp()==MDLDao::Date){
                        QDate date=value.toDate();
                        return date;
                    }else if(fieldInfo->LookupTyp()==MDLDao::Time){
                        QTime time=value.toTime();
                        return time;
                    }else{
                        return value.toDateTime();
                    }
                }
            }
            else{
                if(fieldInfo->LookupTyp()==MDLDao::Foreign||fieldInfo->LookupTyp()==MDLDao::List||fieldInfo->LookupTyp()==MDLDao::TabList){
                    if(role==Qt::DisplayRole){
                        value=fieldInfo->refValue(value.toString(),this->record(index.row()));
                    }
                }
                if(Utility::isNumber(value) && (role==Qt::DisplayRole||role==DATA_ROLE) )
                    value=fieldInfo->displayValue(value);
            }
            return value;
        }
        return value;
    }else{
        if(fieldInfo!=nullptr){
            if (role == Qt::CheckStateRole)
            {
                if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
                {
                    if(!fieldInfo->Calculated()){
                        QVariant value= QSqlTableModel::data(index,Qt::EditRole);
                        bool b=value.toBool();
                        QString  v=value.toString();
                        //                    qDebug()<<"V:"<<v<<",B:"<<b;
                        return  b ? Qt::Checked : Qt::Unchecked;
                    }
                }
            }
        }
    }
    return QSqlTableModel::data(index,role);

}

bool QWMTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;
    QString fieldName=fieldNameEx(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=UDL->fieldInfo(tableName,fieldName);

    QVariant v=value;
    if(role==Qt::CheckStateRole && fieldInfo!=nullptr && fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
    {
        v=QVariant::fromValue(Qt::Checked==value);
        role=Qt::EditRole;//转换为EditRole，否则无效
    }
    else if(fieldInfo!=nullptr ){
        if(role==Qt::EditRole && Utility::isNumber(v)){
            v=fieldInfo->baseValue(v);
        }
        else if(role==BASE_UNIT_VALUE){
            role=Qt::EditRole;
        }
    }
    bool success=true;;
    if(fieldInfo->Calculated()){
        if(!_calcData.contains(index.row())){
            QSqlRecord record(_calcRecord);
            _calcData.insert(index.row(),record);
        }
        if(role==Qt::EditRole){
            _calcData[index.row()].setValue(index.column()-_natureFieldCount,v);
            emit dataChanged(index,index);
        }
    }else{
        success= QSqlTableModel::setData(index, v,  role);
        if(!success){
            qDebug()<<"ERROR:"<<this->lastError().text();
        }
    }
    return success;
    //    }
}


Qt::ItemFlags QWMTableModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid())
        return 0;
    Qt::ItemFlags flags=Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    if(_readonly)
        return flags;

    QVariant v=QSqlTableModel::data(index);
    QString fieldName=this->fieldNameEx(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=UDL->fieldInfo(tableName,fieldName);
    if(fieldInfo!=nullptr){
        if(fieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
        {
            return flags|Qt::ItemIsUserCheckable|Qt::ItemIsEditable;
        }else if(fieldInfo->Calculated()){
            return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
        }else{
            return flags|Qt::ItemIsEditable;
        }
    }
    return flags;
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
    QSqlRecord rec=record();
    //    if(visibleFieldsList.contains(CFG(ID)) && rec.indexOf(CFG(ID))>=0)
    //        visibleFieldsList.append(CFG(ID));
    //******************初始化成物理字段在前，计算字段在后的形式********************
    _fieldsInNatureOrder.clear();
    _fieldsNameToNatureOrderMap.clear();
    _fieldsInDisplayOrder.clear();
    _fieldsNameToDisplayOrderMap.clear();
    _calFields.clear();
    for(int i=0;i<rec.count();i++){
        _fieldsInNatureOrder.append(rec.fieldName(i));
        _fieldsNameToNatureOrderMap.insert(rec.fieldName(i).toLower(),i);
        _fieldsInDisplayOrder.append(rec.fieldName(i));
        _fieldsNameToDisplayOrderMap.insert(rec.fieldName(i).toLower(),i);
    }
    //    ********下面开始初始化 计算字段*************
    _natureFieldCount=rec.count();
    for(int i=0;i<visibleFieldsList.count();i++){
        QString field=visibleFieldsList[i];
        if(!_fieldsNameToNatureOrderMap.contains(field.toLower())){
            QString table=tableName;
            MDLField * fieldInfo=UDL->fieldInfo(table,field);
            if(fieldInfo!=nullptr&& fieldInfo->Calculated()==true){//非物理存在的，只有计算字段才是正常的，其他多出的字段不计入内
                _fieldsInNatureOrder.append(field);
                _fieldsNameToNatureOrderMap.insert(field.toLower(),_natureFieldCount+_calFields.size());
                _fieldsInDisplayOrder.append(field);
                _fieldsNameToDisplayOrderMap.insert(field.toLower(),_natureFieldCount+_calFields.size());
                _calFields.append(field.toLower());
                //                bool success=this->insertColumns(this->columnCount(),1);
                //                qDebug()<<"REC Count:"<<this->record().count()<<","<<this->columnCount();
            }
        }
    }
    foreach(QString calcField,_calFields){
        QSqlField field;
        field.setName(calcField);
        field.setGenerated(false);
        field.setReadOnly(false);
        _calcRecord.insert(_calcRecord.count(),field);
    }
    setVisibleFields(visibleFieldsList);
}

void QWMTableModel::setVisibleFields( QStringList visibleFieldsList)
{

    //**********************调整显示顺序***********************

    _fieldsInDisplayOrder.clear();
    _fieldsNameToDisplayOrderMap.clear();
    for(int  i=0;i<visibleFieldsList.size();i++){
        QString vf=visibleFieldsList[i];
        if(_fieldsNameToNatureOrderMap.contains(vf.toLower())){
            _fieldsInDisplayOrder.append(vf);
            _fieldsNameToDisplayOrderMap.insert(vf.toLower(),i);
        }
    }
    _visibleFields=_fieldsInDisplayOrder.length();
    //****************只有前_visibleFields个字段才是可见的******
    foreach(QString f,_fieldsInNatureOrder){
        if(!_fieldsNameToDisplayOrderMap.contains(f.toLower())){
            _fieldsInDisplayOrder.append(f);
            _fieldsNameToDisplayOrderMap.insert(f.toLower(),_fieldsNameToDisplayOrderMap.size());
        }
    }
}
//-1 说明不可见，其余是显示顺序
int QWMTableModel::fieldVisibleOrder(const QString &field)
{
    if(_fieldsNameToDisplayOrderMap.contains(field.toLower())){
        int pos=_fieldsNameToDisplayOrderMap[field.toLower()];
        if(pos>=_visibleFields)
            return -1;
        return pos;
    }
    else
        return -1;
}

int QWMTableModel::visibleFieldsCount()
{
    return _visibleFields;
}

int QWMTableModel::fieldsCount()
{
    return _fieldsInNatureOrder.length();
}

int QWMTableModel::columnCount(const QModelIndex &parent) const
{
    //    return _visibleFields;
    return QSqlTableModel::columnCount(parent)+_calFields.size();
}

//显示顺序
const QString QWMTableModel::fieldInVisibleOrder(int pos)
{
    //    QSqlRecord rec=this->record();
    if(pos<_fieldsInDisplayOrder.size()&&pos>=0)
        return _fieldsInDisplayOrder[pos];
    else
        return QString();
}

bool QWMTableModel::isFieldVisible(const QString &field)
{
    if(_fieldsNameToDisplayOrderMap.contains(field.toLower())){
        int pos=_fieldsNameToDisplayOrderMap[field.toLower()];
        if(pos<_visibleFields)
            return true;
        else
            return false;
    }
    return false;
}
//nature order
int QWMTableModel::fieldIndexEx(const QString &fieldName)
{
    if(_fieldsNameToNatureOrderMap.contains(fieldName.toLower()))
    {
        return _fieldsNameToNatureOrderMap[fieldName.toLower()];
    }

    return -1;
}

QString QWMTableModel::fieldNameEx(const int index) const
{

    return _fieldsInNatureOrder[index];
}

QModelIndex QWMTableModel::createIndex(int row, int col)
{
    return QSqlTableModel::createIndex(row,col);

}

void QWMTableModel::calc(int curRow,int preRow)
{
    QSqlRecord rec=record(curRow);
    QSqlRecord preRec=record(preRow);
    foreach(QString calField,_calFields){
        QString express=EDL->equation(tableName(),calField);
//        MDLField * fieldInfo=UDL->fieldInfo(tableName(),calField);
        int pos=fieldIndexEx(calField);
        QVariant value;
        if(!express.isNull()&& !express.isEmpty()){
            QScriptEngine engin;
            //            auto r=engin.evaluate("2-1");
            auto r=QWMCalculator::evaluate(calField,rec,idWell(),tableName(),express);
            setData(createIndex(curRow,pos),r);

        }
    }
}

//bool QWMTableModel::updateRowInTable(int row, const QSqlRecord &values)
//{
//    return QSqlRelationalTableModel::updateRowInTable(row,values);
//}

void QWMTableModel::init_record_on_prime_insert(int /*row*/, QSqlRecord &record)
{
    if(record.indexOf(CFG(ParentID))>=0 && !_parentID.isNull()&& !_parentID.isEmpty()){
        record.setValue(record.indexOf(CFG(ParentID)),_parentID);
    }
    if(record.indexOf(CFG(IDWell))>=0 && !_idWell.isNull()&& !_idWell.isEmpty()){
        record.setValue(record.indexOf((CFG(IDWell))),_idWell);
    }

}

void QWMTableModel::before_update_record(int /*row*/, QSqlRecord &record)
{
    int sysMDIndex=record.indexOf(CFG(SysMD));
    if(sysMDIndex>=0){
        record.setValue(sysMDIndex,QDateTime::currentDateTime());
        record.setGenerated(sysMDIndex,true);
    }
    //    计算字段不插入
    foreach(QString f,_calFields){
        int pos=this->fieldIndexEx(f);
        if(pos>=0){
            record.setGenerated(pos,false);
        }
    }

}

void QWMTableModel::before_insert(QSqlRecord &record)
{
    //    计算字段不插入
    foreach(QString f,_calFields){
        int pos=this->fieldIndexEx(f);
        if(pos>=0){
            record.setGenerated(pos,false);
        }
    }

}
