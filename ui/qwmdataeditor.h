#ifndef QWMDATAEDITOR_H
#define QWMDATAEDITOR_H
#include <QUndoCommand>
#include <QUndoStack>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStandardItem>
#include <QLabel>
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
    void loadChildTable(QStandardItem *);

    void showStatus(QString status);
    void showProfile(QString profile);
    void showUnitSetting(QString unit);
    void showReferenceDatum(QString datum);

protected:
     virtual void closeEvent(QCloseEvent *event);
private slots:
    void on_actionSaveExit_triggered();



private:
    Ui::QWMDataEditor *ui;
    QString _idWell;
    QString _wellName;
    QUndoStack _undoStack;

    QLabel * _lblStatus ;
    QLabel * _lblProfile ;
    QLabel * _lblUnit;
    QLabel * _lblReferenceDatum ;
    QLabel * _lblMessage;
};

#endif // QWMDATAEDITOR_H
