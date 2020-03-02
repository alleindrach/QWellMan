#ifndef QWMDATAEDITOR_H
#define QWMDATAEDITOR_H
#include <QUndoCommand>
#include <QUndoStack>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStandardItem>
#include <QLabel>
#include "qwmdatatableview.h"
#include "qwmrotatableproxymodel.h"
#include "mdltable.h"
namespace Ui {
class QWMDataEditor;
}

class QWMDataEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit QWMDataEditor(QString idWell,QString name,QWidget *parent = nullptr);
    ~QWMDataEditor();

    void addUndoCommand(QUndoCommand * command);
    QUndoStack& undoStack();
    void loadDataTree();
    void loadChildTable(QStandardItem *);

    void showStatus(QString status);
    void showProfile(QString profile);
    void showUnitSetting(QString unit);
    void showReferenceDatum(QString datum);
//    void showDataGrid(QWMRotatableProxyModel *  model);
    QList<QWMRotatableProxyModel*> dirtyTables(QModelIndex index);
    bool saveDirtTables(QModelIndex index,QStringList & errors);
    bool saveAll(QStringList & errors);
    bool isDirty();
    bool isCurrentTableDirty();
protected:
    virtual void closeEvent(QCloseEvent *event);
    QString nodeParentID(const QModelIndex &index,QString & lastError);
    MDLTable * nodeTableInfo(const QModelIndex & index);
    void clearChildSelection(const QModelIndex & index);
    void editTable(const QModelIndex & inddex);
    void addRecord(const QModelIndex & index);
    void removeRecord(const QModelIndex &index);
public Q_SLOTS:
    void undo();
    void redo();
private Q_SLOTS:
    void on_actionSaveExit_triggered();
    void on_trv_table_node_clicked(const QModelIndex &index);
    void on_actionRotate_triggered(bool checked);
    void on_current_record_changed(const QModelIndex &current, const QModelIndex &previous);
    void on_actionNew_triggered();
    void on_actionDelete_triggered();
    void init_record_on_prime_insert(int row, QSqlRecord &record);
    void on_data_record_changed(int , int );
    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void on_actionSaveAll_triggered();

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
    QWMDataTableView * _tbvData;
    QString _parentID;
    QString _TableStyle="/*tablewidget 样式*/ \
            QTableView { \
            font-size:10px ;\
            selection-background-color:rgb(155, 0, 2);\
            alternate-background-color: gray; \
            background-color:white;/*整个表格的背景色，这里为白色*/ \
            border:1px solid #E0DDDC;/*边框为1像素，灰色*/ \
            gridline-color:lightgray;/*这个是表格的格子线的颜色，为亮灰*/ \
} \
            QTableView QTableCornerButton::section{\
            border:0px solid lightgray; \
            background-color: rgb(50,50,50); \
            selection-background-color: darkblue ;\
            color:white;\
}  \
            \
            QHeaderView::section { \
            font-size:10px ;\
            selection-background-color: darkblue; \
            padding-left: 4px; \
            border-right: 1px solid lightgray; \
            border-bottom: 1px solid lightgray; \
} \
            QHeaderView::section:checked { \
            color:white;\
            background-color:rgb(50, 50, 50); \
} \
            QHeaderView::section:unchecked { \
            color:black;\
            background-color: white; \
} \
            QTableView::indicator { \
            width: 17px; \
            height: 17px; \
} \
            QTableView::indicator:enabled:unchecked { \
            image: url(:/images/icons/uncheck.png); \
}\
            QTableView::indicator:enabled:checked { \
            image: url(:/images/icons/checked.png);\
}\
            ";
};

#endif // QWMDATAEDITOR_H
