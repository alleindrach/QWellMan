#ifndef QFIELDEDITCOMMAND_H
#define QFIELDEDITCOMMAND_H
#include <QUndoCommand>

class QFieldEditCommand : public QUndoCommand
{
public:
    explicit QFieldEditCommand(QUndoCommand *parent = nullptr);
signals:

public slots:
};

#endif // QFIELDEDITCOMMAND_H
