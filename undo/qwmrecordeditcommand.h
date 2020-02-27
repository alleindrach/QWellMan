#ifndef QWMRECORDEDITCOMMAND_H
#define QWMRECORDEDITCOMMAND_H
#include <QUndoCommand>
#include <QSqlRecord>
#include "qwmtablemodel.h"
class QWMRecordEditCommand : public QUndoCommand
{
public:
    enum Type{insert,remove};
    explicit QWMRecordEditCommand(QWMTableModel * model,QSqlRecord record,Type typ, QUndoCommand *parent = nullptr);
    ~QWMRecordEditCommand();
    virtual void undo() override;
    virtual void redo() override;
    bool insertRecord();
    bool removeRecord();
    QWMTableModel *  model();
signals:

public slots:
    void init_record_on_prime_insert(int row, QSqlRecord &record);
private:
    QWMTableModel * _model;
    QSqlRecord _record;
    Type _type;

};

#endif // QWMRECORDEDITCOMMAND_H
