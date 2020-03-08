#include "qwmrecordeditcommand.h"
#include <QDebug>
#include "qwmtablemodel.h"
#include "welldao.h"

QWMRecordEditCommand::QWMRecordEditCommand(QWMTableModel *model,QString idWell, QSqlRecord record,Type typ, QUndoCommand *parent)
    :QUndoCommand(parent),_idWell(idWell),_model(model),_record(record),_type(typ)
{
    PK_VALUE(id,_record);
    _id=id;
}
QWMRecordEditCommand::~QWMRecordEditCommand(){
    qDebug()<<"Destory QWMRecordEditCommand";
}
void QWMRecordEditCommand::undo()
{
    if(_type==insert){
        unInsertRecord();
    }else
    {
        unDeleteRecord();
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
    }else{
         WELL->undeleteItem(_idWell,_id);
    };
    return success;
}

bool QWMRecordEditCommand::removeRecord()
{
    bool success=false;
//    PK_VALUE(id,_record);
    WELL->deleteItem(_idWell,_id,_model->tableName());
    emit  _model->rowsChanged();
    return success;
}

QWMTableModel *QWMRecordEditCommand::model()
{
    return _model;
}

void QWMRecordEditCommand::init_record_on_prime_insert(int /*row*/, QSqlRecord &/*record*/)
{

}

void QWMRecordEditCommand::unInsertRecord()
{
//    if(_submitted){
        removeRecord();
//    }else{
//        PK_VALUE(id,_record);
//        PK_INDEX(idIndex,_record);
//        QModelIndexList matchedIndexs= _model->match(_model->index(0,idIndex),Qt::EditRole,id,1,Qt::MatchCaseSensitive|Qt::MatchRecursive);
//        if(matchedIndexs.size()>0){
//            QModelIndex index=matchedIndexs.first();
//            _model->revertRow(index.row());
//        }
//    }

}

void QWMRecordEditCommand::unDeleteRecord()
{
//    PK_VALUE(id,_record);
    WELL->undeleteItem(_idWell,_id);
    emit  _model->rowsChanged();
}
