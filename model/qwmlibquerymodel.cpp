#include "qwmlibquerymodel.h"
#include "utility.h"

QWMLibQueryModel::QWMLibQueryModel(QString baseTable,QString libTable ,QObject *parent)
    :QSqlQueryModel(parent),_baseTable(baseTable),_libTable(libTable)
{

}

QVariant QWMLibQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= this->rowCount() || index.row() < 0)
        return QVariant("-");
    if(role==Qt::DisplayRole){
        MDLTable * tableInfo=MDL->tableInfo(_baseTable);
        QString libFieldName=this->record().fieldName(index.column());
        MDLField * baseFieldInfo=MDL->fieldByLookup(_baseTable,_libTable,libFieldName);
        QVariant value= QSqlQueryModel::data(index);
        if(baseFieldInfo!=nullptr){
            if(baseFieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
            {
                return QVariant();
            }else if(baseFieldInfo->PhysicalType()==MDLDao::DateTime){
                return value.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            }else if(baseFieldInfo->LookupTyp()==MDLDao::Foreign||baseFieldInfo->LookupTyp()==MDLDao::List||baseFieldInfo->LookupTyp()==MDLDao::TabList){
                value=baseFieldInfo->refValue(value.toString(),QSqlRecord());
            }else if(Utility::isNumber(value)){
                value=baseFieldInfo->displayValue(value);//单位转换
            }
        }
        return value;
    }else if (role == Qt::CheckStateRole){
        MDLTable * tableInfo=MDL->tableInfo(_baseTable);
        QString libFieldName=this->record().fieldName(index.column());
        MDLField * baseFieldInfo=MDL->fieldByLookup(_baseTable, _libTable,libFieldName);
        if(baseFieldInfo!=nullptr && baseFieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
        {
            QVariant value= QSqlQueryModel::data(index);
            bool b=value.toBool();
            QString  v=value.toString();
            //                    qDebug()<<"V:"<<v<<",B:"<<b;
            return  b ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QSqlQueryModel::data(index,role);
}

QVariant QWMLibQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QSqlRecord record=this->record();

    if( role == Qt::DisplayRole){
        if( orientation==Qt::Orientation::Horizontal)
        {
            MDLTable * tableInfo=MDL->tableInfo(_baseTable);
            QString libFieldName=this->record().fieldName(section);
            MDLField * baseFieldInfo=MDL->fieldByLookup(_baseTable, _libTable,libFieldName);

            if(baseFieldInfo!=nullptr){
                QString cap=baseFieldInfo->CaptionLong();

                cap=baseFieldInfo->caption();//lookup type=8,<capl>
                return cap;
            }
            return QSqlQueryModel::headerData(section,orientation,role);
        }
    }else if(role==FIELD_ROLE){
        if( orientation==Qt::Orientation::Horizontal)
        {
            return this->record().fieldName(section);
        }
    }
    return QSqlQueryModel::headerData(section,orientation,role);
}

Qt::ItemFlags QWMLibQueryModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    Qt::ItemFlags flags=Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    QSqlRecord record=this->record();
    MDLTable * tableInfo=MDL->tableInfo(_baseTable);
    QString libFieldName=this->record().fieldName(index.column());
    MDLField * baseFieldInfo=MDL->fieldByLookup(_baseTable, _libTable,libFieldName);

    if(baseFieldInfo!=nullptr){
        QVariant value=QSqlQueryModel::data(index);
        if(baseFieldInfo->PhysicalType()==MDLDao::Boolean)//booelan
        {
            return flags|Qt::ItemIsUserCheckable;
        }else{
            return flags;
        }
    }
    return flags;
}
