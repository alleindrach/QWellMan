#include "qwmfieldeditcommand.h"
#include "QUndoCommand"
#include <QDebug>
QWMFieldEditCommand::QWMFieldEditCommand(QWMTableModel * model,QList<Modifier> changelist,QUndoCommand *parent):QUndoCommand(parent),_model(model),_changlist(changelist)
{

}

QWMFieldEditCommand::~QWMFieldEditCommand()
{
    qDebug()<<"Destory QWMFieldEditCommand";
}

void QWMFieldEditCommand::undo()
{
    foreach(Modifier modify,_changlist){
        _model->setData(_model->index(modify.row,modify.col),modify.oldValue);
        emit  _model->dataChanged(_model->index(modify.row,modify.col),_model->index(modify.row,modify.col));
    }
}

void QWMFieldEditCommand::redo()
{
    foreach(Modifier modify,_changlist){
        _model->setData(_model->index(modify.row,modify.col),modify.newValue);
    }
}

QWMTableModel *QWMFieldEditCommand::model()
{
    return _model;
}
