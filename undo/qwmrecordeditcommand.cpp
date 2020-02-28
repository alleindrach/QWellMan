#include "qwmrecordeditcommand.h"
#include <QDebug>
#include "qwmtablemodel.h"

QWMRecordEditCommand::QWMRecordEditCommand(QWMTableModel *model, QSqlRecord record,Type typ, QUndoCommand *parent)
    :QUndoCommand(parent),_model(model),_record(record),_type(typ)
{

}
QWMRecordEditCommand::~QWMRecordEditCommand(){
    qDebug()<<"Destory QWMRecordEditCommand";
}
void QWMRecordEditCommand::undo()
{
    if(_type==insert){
        removeRecord();
    }else
    {
        insertRecord();
    }
}

void QWMRecordEditCommand::redo()
{
    if(_type==insert){
        insertRecord();
    }else
    {
        removeRecord();
    }
}
bool QWMRecordEditCommand::insertRecord()
{


    bool success=_model->insertRecord(-1,_record);
    if(!success){
        if(_model->lastError().isValid()){
            qDebug()<<"Insert Record Error:["<<_model->tableName()<<"],record["<<(_record.indexOf(CFG(ID))>=0?_record.value(CFG(ID)):_record.value(CFG(IDWell)))<<"]";
        }
    };
    return success;
}

bool QWMRecordEditCommand::removeRecord()
{
    bool success=false;
    QString idFld=_record.indexOf(CFG(ID))>=0?CFG(ID):CFG(IDWell);
    int idIndex=_record.indexOf(idFld);
    QString id=_record.value(idFld).toString();
    QModelIndexList matchedIndexs= _model->match(_model->index(0,idIndex),Qt::EditRole,id,1,Qt::MatchCaseSensitive|Qt::MatchRecursive);
    if(matchedIndexs.size()>0){
        QModelIndex index=matchedIndexs.first();
        success=_model->removeRow(index.row());
        if(!success){
            if(_model->lastError().isValid()){
                qDebug()<<"Insert Record Error:["<<_model->tableName()<<"],record["<<(_record.indexOf(CFG(ID))>=0?_record.value(CFG(ID)):_record.value(CFG(IDWell)))<<"]";
            }
        };
    }
    return success;
}

QWMTableModel *QWMRecordEditCommand::model()
{
    return _model;
}

void QWMRecordEditCommand::init_record_on_prime_insert(int row, QSqlRecord &record)
{

}
