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
#include "qwmtablemodel.h"
//#include "qwmtablemodel.h"
namespace Ui {
class QWMDataEditor;
}
class QWMTableModel;
class QWMDataEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit QWMDataEditor(QString idWell,QString name,QWidget *parent = nullptr);
    ~QWMDataEditor();
    void init();
    void addUndoCommand(QUndoCommand * command,QString table,QString  parentId);
    QUndoStack* undoStack();
    void loadDataTree();
    void showStatus(QString status);
    void showProfile(QString profile);
    void showUnitSetting(QString unit);
    void showReferenceDatum(QString datum);
    //    void showDataGrid(QWMRotatableProxyModel *  model);
    QList<QWMRotatableProxyModel*> dirtyTables(QModelIndex index);
    bool saveDirtTables(QModelIndex index,QStringList & errors);
    bool saveAll(QStringList & errors);
    void revertAll(QStringList & errors);
    bool isDirty();
    bool isCurrentTableDirty();
    QString inline const idWell(){return _idWell;};
    inline QWMDataTableView * dataView (){return _tbvData;}

protected:
    virtual void closeEvent(QCloseEvent *event);
    bool nodeParentInfo(const QModelIndex &index,QString& parentTable,QString& parentDes,QString& parentID ,QString & lastError);
    MDLTable * nodeTableInfo(const QModelIndex & index);
    void clearChildSelection(const QModelIndex & index);
    void editTable(const QModelIndex & inddex);
    void addRecord(const QModelIndex & index);
    void removeRecord(const QModelIndex &index);
    void breadcrumbInfo(const  QModelIndex &index);
private:
    void checkUndoStacks(QWMTableModel * commitedModel);
    void parentRecordNavigate(int step);
signals:
    void RecordCountChanged(int ,int);
public Q_SLOTS:
    void undo();
    void redo();
    void on_model_submitted(QString tablename);
    void on_model_data_modified(QString tablename,QString parentId);
private Q_SLOTS:
    void on_actionSaveExit_triggered();
    void on_trv_table_node_clicked(const QModelIndex &index);
    void on_actionRotate_triggered(bool checked);
    void on_current_record_changed(const QModelIndex &current, const QModelIndex &previous);
    void on_actionNew_triggered();
    void on_actionDelete_triggered();
    void on_data_record_changed(int , int );
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionSaveAll_triggered();
    void on_actionSort_triggered(bool checked);

    //    void init_record_on_prime_insert(int row, QSqlRecord &record);
    //    void before_update_record(int row, QSqlRecord &record);

    void on_actionCalc_triggered();

    void on_actionactNextRec_triggered();

    void on_actionactPrevRec_triggered();

private:
    Ui::QWMDataEditor *ui;
    QString _idWell;
    QString _wellName;
    //    QUndoStack _undoStack;

    QLabel * _lblStatus ;
    QLabel * _lblProfile ;
    QLabel * _lblUnit;
    QLabel * _lblReferenceDatum ;
    QLabel * _lblMessage;
    QWMDataTableView * _tbvData;
    QString _parentID;
    QSqlRecord _wellRec;
    QString _wellDes;
    QToolBar*  _tableOpToolBar;
//    QLabel * _lblParentTable{nullptr};
    QList<QAction *> _navActions;

    void showRecordNav();
    QHash<QString,QWMTableModel*> _dirtyTables;
    QHash<QString,QUndoStack *> _undoStacks;//key:idWell.tablename.parentId,为每一个编辑的可见的grid增加一个undostack
    //    QString _TableStyle="/*tablewidget 样式*/ \
    //            QTableView { \
    //            font-size:10px ;\
    //            selection-background-color:rgb(155, 0, 2);\
    //            alternate-background-color: gray; \
    //            background-color:white;/*整个表格的背景色，这里为白色*/ \
    //            border:1px solid #E0DDDC;/*边框为1像素，灰色*/ \
    //            gridline-color:lightgray;/*这个是表格的格子线的颜色，为亮灰*/ \
    //} \
    //            QTableView QTableCornerButton::section{\
    //            border:0px solid lightgray; \
    //            background-color: rgb(50,50,50); \
    //            selection-background-color: darkblue ;\
    //            color:white;\
    //}  \
    //            \
    //            QHeaderView::section { \
    //            font-size:10px ;\
    //            selection-background-color: darkblue; \
    //            padding-left: 4px; \
    //            border-right: 1px solid lightgray; \
    //            border-bottom: 1px solid lightgray; \
    //} \
    //            QHeaderView::section:checked { \
    //            color:white;\
    //            background-color:rgb(50, 50, 50); \
    //} \
    //            QHeaderView::section:unchecked { \
    //            color:black;\
    //            background-color: white; \
    //} \
    //            QTableView::indicator { \
    //            width: 17px; \
    //            height: 17px; \
    //}\
    //            QTableView::indicator:enabled:unchecked { \
    //            image: url(:/images/icons/checkbox-on.svg); \
    //}\
    //            QTableView::indicator:enabled:checked { \
    //            image: url(:/images/icons/checkbox-off.svg);\
    //}\
    //            ";
};

#endif // QWMDATAEDITOR_H
