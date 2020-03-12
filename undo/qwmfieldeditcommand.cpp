#include "qwmfieldeditcommand.h"
#include "QUndoCommand"
#include <QDebug>
#include <QItemSelectionRange>
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
    QModelIndexList changedList;
    if(_type==SingleField){
        foreach(Modifier modify,_changlist){
            _model->setData(_model->index(modify.row,modify.col),modify.oldValue);
            changedList<<_model->index(modify.row,modify.col);
//            emit  _model->dataChanged(_model->index(modify.row,modify.col),_model->index(modify.row,modify.col));
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
                        changedList<<_model->index(modify.row,modify.col);
//                        emit  _model->dataChanged(_model->index(modify.row,col),_model->index(modify.row,col));
                    }
                }
            }
        }
    }
    QRect r(0,0,0,0);
    foreach(QModelIndex ind,changedList){
        if(ind.row()<r.top()){
            r.setTop(ind.row());
        }
        if(ind.row()>r.bottom()){
            r.setBottom(ind.row());
        }
        if(ind.column()<r.left()){
            r.setLeft(ind.column());
        }
        if(ind.column()>r.right()){
            r.setRight(ind.column());
        }

    }
    emit _model->dataChanged(_model->index(r.top(),r.left()),_model->index(r.bottom(),r.right()));

//    emit this->commandCommitted(_model->tableName());
}

void QWMFieldEditCommand::redo()
{
    QModelIndexList changedList;
    if(_type==SingleField){
        foreach(Modifier modify,_changlist){
            _model->setData(_model->index(modify.row,modify.col),modify.newValue);
            changedList<<_model->index(modify.row,modify.col);
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
                        changedList<<_model->index(modify.row,modify.col);
//                        emit  _model->dataChanged(_model->index(modify.row,col),_model->index(modify.row,col));
                    }
                }
            }
        }
    }
    QRect r(0,0,0,0);
    foreach(QModelIndex ind,changedList){
        if(ind.row()<r.top()){
            r.setTop(ind.row());
        }
        if(ind.row()>r.bottom()){
            r.setBottom(ind.row());
        }
        if(ind.column()<r.left()){
            r.setLeft(ind.column());
        }
        if(ind.column()>r.right()){
            r.setRight(ind.column());
        }
    }
    emit _model->dataChanged(_model->index(r.top(),r.left()),_model->index(r.bottom(),r.right()));
//    emit this->commandCommitted(_model->tableName());
}

QWMTableModel *QWMFieldEditCommand::model()
{
    return _model;
}
