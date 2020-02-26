#ifndef QFIELDEDITCOMMAND_H
#define QFIELDEDITCOMMAND_H
#include <QUndoCommand>
#include "qwmrotatableproxymodel.h"
#include "qwmsortfilterproxymodel.h"
#include "qwmtablemodel.h"
struct Modifier{
    int row;
    int col;
    QVariant oldValue;
    QVariant newValue;
};

class QWMFieldEditCommand : public QUndoCommand
{
public:
    explicit QWMFieldEditCommand(QWMTableModel * model,QList<Modifier> changelist, QUndoCommand *parent = nullptr);
    virtual void undo() override;
    virtual void redo() override;
signals:

public slots:
private:
    QWMTableModel * _model;
    QList<Modifier> _changlist;

};

#endif // QFIELDEDITCOMMAND_H
