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
    enum Type{SingleField,LinkedFields};
    explicit QWMFieldEditCommand(QWMTableModel * model,QList<Modifier> changelist,Type typ=SingleField, QUndoCommand *parent = nullptr);
    ~QWMFieldEditCommand();
    virtual void undo() override;
    virtual void redo() override;
    QWMTableModel *  model();
signals:

public slots:
private:
    QWMTableModel * _model;
    QList<Modifier> _changlist;
    Type _type{SingleField};

};

#endif // QFIELDEDITCOMMAND_H
