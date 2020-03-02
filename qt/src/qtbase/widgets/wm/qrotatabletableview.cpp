#include "qrotatabletableview.h"
#include "QDebug"
#include <QHeaderView>
#include <QSettings>
#include <QTableView>
#include "qrotatabletableview_p.h"
#define CLEAR_DELEGATES(w) \
    for(int i=0;i<w->model()->columnCount();i++){ \
    w->setItemDelegateForColumn(i,nullptr);\
    }\
    for(int i=0;i<w->model()->rowCount();i++){\
    w->setItemDelegateForRow(i,nullptr);\
    }
#define MODE(mode) \
    QAbstractItemModel * model=this->model();\
int mode=Qt::Horizontal; \
if(!model->property("mode").isNull()&& model->property("mode").isValid()){ \
     mode = model->property("mode").toInt(); \
}
#define VISIBLE_FIELDS(model) \
int visibleFieldsCount=model->columnCount();\
if(!model->property("visibleFieldsCount").isNull()&& model->property("visibleFieldsCount").isValid()){ \
     visibleFieldsCount = model->property("visibleFieldsCount").toInt(); \
}

QRotatableTableView::QRotatableTableView(QRotatableTableViewPrivate &dd, QWidget *parent)
    : QTableView(dd, parent)
{
//    Q_D(QRotatableTableView);
//    d->init();
}


QRotatableTableView::QRotatableTableView(QWidget *parent):QTableView(*new  QRotatableTableViewPrivate ,parent)
{
//    Q_D(QRotatableTableView);
//    d->init();
    //    QWMHeaderView * vHeaderView= new QWMHeaderView(Qt::Vertical, this);
    //    setVerticalHeader(vHeaderView);
    //    disconnect(vHeaderView, SIGNAL(sectionCountChanged(int,int)),0,0);
    //    connect(vHeaderView, SIGNAL(sectionCountChanged(int,int)),
    //            this, SLOT(rowCountChanged(int,int)));

    //    QWMHeaderView * hHeaderView= new QWMHeaderView(Qt::Horizontal, this);
    //    setHorizontalHeader(hHeaderView);
    //    disconnect(hHeaderView, SIGNAL(sectionCountChanged(int,int)),0,0);
    //    connect(hHeaderView, SIGNAL(sectionCountChanged(int,int)),
    //            this, SLOT(columnCountChanged(int,int)));
}

void QRotatableTableView::setHorizontalHeader(QHeaderView *header)
{
    //same as QTableview
    Q_D(QRotatableTableView);

    if (!header || header == d->horizontalHeader)
        return;
    if (d->horizontalHeader && d->horizontalHeader->parent() == this)
        delete d->horizontalHeader;
    d->horizontalHeader = header;
    d->horizontalHeader->setParent(this);
    d->horizontalHeader->setFirstSectionMovable(true);
    if (!d->horizontalHeader->model()) {
        d->horizontalHeader->setModel(d->model);
        if (d->selectionModel)
            d->horizontalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->horizontalHeader,SIGNAL(sectionResized(int,int,int)),
            this, SLOT(columnResized(int,int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(columnMoved(int,int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(columnCountChanged(int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectColumn(int)));
    connect(d->horizontalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectColumn(int)));
    connect(d->horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeColumnToContents(int)));
    connect(d->horizontalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));

    //update the sorting enabled states on the new header
    setSortingEnabled(d->sortingEnabled);
}

void QRotatableTableView::setVerticalHeader(QHeaderView *header)
{
    Q_D(QRotatableTableView);

    if (!header || header == d->verticalHeader)
        return;
    if (d->verticalHeader && d->verticalHeader->parent() == this)
        delete d->verticalHeader;
    d->verticalHeader = header;
    d->verticalHeader->setParent(this);
    d->verticalHeader->setFirstSectionMovable(true);
    if (!d->verticalHeader->model()) {
        d->verticalHeader->setModel(d->model);
        if (d->selectionModel)
            d->verticalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->verticalHeader, SIGNAL(sectionResized(int,int,int)),
            this, SLOT(rowResized(int,int,int)));
    connect(d->verticalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(rowMoved(int,int,int)));
    connect(d->verticalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(rowCountChanged(int,int)));
    connect(d->verticalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectRow(int)));
    connect(d->verticalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectRow(int)));
    connect(d->verticalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeRowToContents(int)));
    connect(d->verticalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));
}

void QRotatableTableView::setModel(QAbstractItemModel *model)
{
    Q_D(QRotatableTableView);
    //    if (model == d->model)
    //           return;
    QTableView::setModel(model);

//    QWMRotatableProxyModel * rotateModel=(QWMRotatableProxyModel*)model;
    disconnect(model,SIGNAL(modeChanged()),0,0);
    connect(model,SIGNAL(modeChanged()),this,SLOT(onModeChange()));
    connect(model,SIGNAL(modelReset()),this,SLOT(onModeChange()));
}


void QRotatableTableView::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    Q_D(QRotatableTableView);

    MODE(mode);
    // Close the editor
    if (editor) {
        bool isPersistent = d->persistent.contains(editor);
        bool hadFocus = editor->hasFocus();
        QModelIndex index = d->indexForEditor(editor);
        if (!index.isValid())
            return; // the editor was not registered

        if (!isPersistent) {
            setState(NoState);
            QModelIndex index = d->indexForEditor(editor);
            editor->removeEventFilter(d->delegateForIndex(index));
            d->removeEditor(editor);
        }
        if (hadFocus) {
            if (focusPolicy() != Qt::NoFocus)
                setFocus(); // this will send a focusLost event to the editor
            else
                editor->clearFocus();
        } else {
            d->checkPersistentEditorFocus();
        }

        QPointer<QWidget> ed = editor;
        QCoreApplication::sendPostedEvents(editor, 0);
        editor = ed;

        if (!isPersistent && editor)
            d->releaseEditor(editor, index);
    }

    // The EndEditHint part
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::NoUpdate;
    if (d->selectionMode != NoSelection)
        flags = QItemSelectionModel::ClearAndSelect | d->selectionBehaviorFlags();
    switch (hint) {
    case QAbstractItemDelegate::EditNextItem: {
        QModelIndex index = (mode==Qt::Horizontal? moveCursor(MoveNext, Qt::NoModifier): moveCursor(MoveDown, Qt::NoModifier));
        if (index.isValid()) {
            QPersistentModelIndex persistent(index);
            d->selectionModel->setCurrentIndex(persistent, flags);
            // currentChanged signal would have already started editing
            if (index.flags() & Qt::ItemIsEditable
                    && (!(editTriggers() & QAbstractItemView::CurrentChanged)))
                QAbstractItemView::edit(persistent);
        } break; }
    case QAbstractItemDelegate::EditPreviousItem: {
        QModelIndex index =(mode==Qt::Horizontal? moveCursor(MovePrevious, Qt::NoModifier):moveCursor(MoveUp, Qt::NoModifier));
        if (index.isValid()) {
            QPersistentModelIndex persistent(index);
            d->selectionModel->setCurrentIndex(persistent, flags);
            // currentChanged signal would have already started editing
            if (index.flags() & Qt::ItemIsEditable
                    && (!(editTriggers() & QAbstractItemView::CurrentChanged)))
                QAbstractItemView::edit(persistent);
        } break; }
    case QAbstractItemDelegate::SubmitModelCache:
        d->model->submit();
        break;
    case QAbstractItemDelegate::RevertModelCache:
        d->model->revert();
        break;
    default:
        break;
    }

}


void QRotatableTableView::rowCountChanged(int oldCount, int newCount)
{
    QAbstractItemModel  * model=this->verticalHeader()->model();
    if(model!=nullptr){
        QVariant vMode=model->property("mode");
        if(!vMode.isNull()&& vMode.isValid()){
            int mode = vMode.toInt();
            if(Qt::Horizontal == mode){
                emit this->RecordCountChanged(oldCount,newCount);
            }
        }
    }
    QTableView::rowCountChanged(oldCount,newCount);
}

void QRotatableTableView::columnCountChanged(int oldCount, int newCount)
{
    QAbstractItemModel * model=this->horizontalHeader()->model();
    if(model!=nullptr){
        QVariant vMode=model->property("mode");
        if(!vMode.isNull()&& vMode.isValid()){
            int mode = vMode.toInt();
            if(Qt::Vertical == mode){
                emit this->RecordCountChanged(oldCount,newCount);
            }
        }
    }
    QTableView::columnCountChanged(oldCount,newCount);
}
