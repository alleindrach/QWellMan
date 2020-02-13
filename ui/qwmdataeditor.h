#ifndef QWMDATAEDITOR_H
#define QWMDATAEDITOR_H
#include <QUndoCommand>
#include <QUndoStack>
#include <QMainWindow>
#include <QTreeWidgetItem>
namespace Ui {
class QWMDataEditor;
}

class QWMDataEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit QWMDataEditor(QString idWell,QString name,QWidget *parent = nullptr);
    ~QWMDataEditor();
    void undo();
    void redo(QUndoCommand * command);
    void loadDataTree();
    void loadChildTable(QTreeWidgetItem*);
protected:
     virtual void closeEvent(QCloseEvent *event);
private slots:
    void on_actionSaveExit_triggered();



private:
    Ui::QWMDataEditor *ui;
    QString _idWell;
    QString _wellName;
    QUndoStack _undoStack;

};

#endif // QWMDATAEDITOR_H
