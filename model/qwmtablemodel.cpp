#include "qwmtablemodel.h"
#include <QSqlRelationalTableModel>
#include "QSqlRecord"
#include "mdldao.h"
#include "udldao.h"
#include "welldao.h"
#include "QSqlQuery"
#include "QSqlError"
#include "QDebug"
QWMTableModel::QWMTableModel(QObject *parent,QSqlDatabase db) : QSqlRelationalTableModel(parent,db)
{

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
                if(!unitType.isEmpty()){
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

    }
    return QSqlRelationalTableModel::headerData(section,orientation,role);
}
QVariant QWMTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount() || index.row() < 0)
        return QVariant("-");

    QSqlQuery q=this->query();
    bool seekSuccess=q.seek(index.row());
    if(!seekSuccess ){
        PRINT_ERROR(q);
    }
    QSqlRecord record=q.record();
    QVariant v=record.value(index.column());
    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);

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
                    v=MDL->unitBase2User(baseUnit,userUnit,v);
                }
            }
        }
        return v;
    }else if(role== PK_ROLE){
        QString idField=MDL->idField(this->tableName());
        v=record.value(record.indexOf(idField));
        return v;
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
            bool b=v.toBool();
            return  b ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QSqlRelationalTableModel::data(index,role);
}

bool QWMTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;


    QSqlQuery q=this->query();
    bool seekSuccess=q.seek(index.row());
    if(!seekSuccess ){
        PRINT_ERROR(q);
    }
    QSqlRecord record=q.record();

    QVariant v=value;
    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);

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
    return QSqlRelationalTableModel::setData(index, v,  role);
}


Qt::ItemFlags QWMTableModel::flags( const QModelIndex &index ) const
{
    if(!index.isValid())
        return 0;
    if(_readonly)
        return Qt::ItemIsEnabled|Qt::ItemIsSelectable;

    QSqlQuery q=this->query();
    bool seekSuccess=q.seek(index.row());
    if(!seekSuccess ){
        PRINT_ERROR(q);
    }
    QSqlRecord record=q.record();
    QVariant v=record.value(index.column());
    QString fieldName=this->record().fieldName(index.column());
    QString tableName=this->tableName();
    MDLField *  fieldInfo=MDL->fieldInfo(tableName,fieldName);

    if(fieldInfo->PhysicalType()==11)//booelan
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }else{
        return Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

bool QWMTableModel::readonly()
{
    return _readonly;
}

void QWMTableModel::setReadonly(bool v)
{
    _readonly=v;
}
