#include "qwmfieldeditcommand.h"
#include "QUndoCommand"
#include <QDebug>
QWMFieldEditCommand::QWMFieldEditCommand(QWMTableModel * model,QList<Modifier> changelist,Type typ,QUndoCommand *parent)
    :QUndoCommand(parent),_model(model),_changlist(changelist),_type(typ)
{

}

QWMFieldEditCommand::~QWMFieldEditCommand()
{
    qDebug()<<"Destory QWMFieldEditCommand";
}

void QWMFieldEditCommand::undo()
{
    if(_type==SingleField){
        foreach(Modifier modify,_changlist){
            _model->setData(_model->index(modify.row,modify.col),modify.oldValue);
            emit  _model->dataChanged(_model->index(modify.row,modify.col),_model->index(modify.row,modify.col));
        }
    }else if(_type==LinkedFields){
        foreach(Modifier modify,_changlist){
            QList<QPair<QString,QVariant>> oldV=modify.oldValue.value<QList<QPair<QString,QVariant>>>();
            QList<QPair<QString,QVariant>> newV=modify.newValue.value<QList<QPair<QString,QVariant>>>();
            if(oldV.length()==newV.length()){
                for(int i=0;i<oldV.length();i++){
                    if(oldV[i].second!=newV[i].second){
                        int col=_model->record().indexOf(oldV[i].first);
                        _model->setData(_model->index(modify.row,col),oldV[i].second,BASE_UNIT_VALUE);
                        emit  _model->dataChanged(_model->index(modify.row,col),_model->index(modify.row,col));
                    }
                }
            }
        }
    }
}

void QWMFieldEditCommand::redo()
{
    if(_type==SingleField){
        foreach(Modifier modify,_changlist){
            _model->setData(_model->index(modify.row,modify.col),modify.newValue);
        }
    }else if(_type==LinkedFields){
        foreach(Modifier modify,_changlist){
            QList<QPair<QString,QVariant>> oldV=modify.oldValue.value<QList<QPair<QString,QVariant>>>();
            QList<QPair<QString,QVariant>> newV=modify.newValue.value<QList<QPair<QString,QVariant>>>();
            if(oldV.length()==newV.length()){
                for(int i=0;i<oldV.length();i++){
                    if(oldV[i].second!=newV[i].second){
                        int col=_model->record().indexOf(oldV[i].first);
                        _model->setData(_model->index(modify.row,col),newV[i].second,BASE_UNIT_VALUE);
                        emit  _model->dataChanged(_model->index(modify.row,col),_model->index(modify.row,col));
                    }
                }
            }
        }
    }
}

QWMTableModel *QWMFieldEditCommand::model()
{
    return _model;
}
