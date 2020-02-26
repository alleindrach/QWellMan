#include "qwmliblookupdelegate.h"
#include "qwmlibselector.h"
#include "qwmdatatableview.h"
#include "qdebug.h"
#include "common.h"
#include "mdldao.h"
#include "mdlfield.h"
#include "qwmtablemodel.h"
#include "qwmsortfilterproxymodel.h"
#include "qwmrotatableproxymodel.h"
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QKeyEvent>
#include "libdao.h"
#include "qwmlibtabselector.h"
QWMLibLookupDelegate::QWMLibLookupDelegate(QString lib, QString disp,QString title,bool editable, QObject *parent):QStyledItemDelegate(parent),_title(title),_disp(disp),_lib(lib),_editable(editable)
{
    if(LIB->libLookup(_lib)->record().indexOf(CFG(LibTab))>=0){
        _type=Tab;
    }
}

QWidget *QWMLibLookupDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &/*option*/,
                                            const QModelIndex &index) const
{

    if(_type==Plain){
        QWMLibSelector *editor = new QWMLibSelector(_lib,_disp,_title,_editable,index.data().toString(),parent);

        connect(editor,&QWMLibSelector::rejected,this,&QWMLibLookupDelegate::justCloseEditor);
        connect(editor,&QWMLibSelector::accepted,this,&QWMLibLookupDelegate::commitAndCloseEditor);

        return editor;
    }else{
        QWMLibTabSelector *editor = new QWMLibTabSelector(_lib,_disp,_title,_editable,index.data().toString(),parent);
        connect(editor,&QWMLibTabSelector::rejected,this,&QWMLibLookupDelegate::justCloseEditor);
        connect(editor,&QWMLibTabSelector::accepted,this,&QWMLibLookupDelegate::commitAndCloseEditor);
        return editor;
    }
}
bool QWMLibLookupDelegate::eventFilter(QObject *watched, QEvent *event) {
//    qDebug()<<"Event:W="<<watched->metaObject()->className()<<"/"<<watched->objectName()<<",E="<<event->type();
    if(watched->metaObject()->className()==QWMLibSelector::staticMetaObject.className()){
        if(event->type()==QEvent::FocusIn){

            qDebug()<<"FocusIN!";
        }
        if(event->type()==QEvent::KeyPress){
            QKeyEvent *  keyEvent=(QKeyEvent*) event;
            qDebug()<<"Key:"<<keyEvent->key();
        }
    }
    return false;
}

void QWMLibLookupDelegate::setEditorData(QWidget *editor,
                                         const QModelIndex &index) const
{
    QVariant value=index.data(Qt::EditRole);
    if(_type==Plain){
        QWMLibSelector *selector=static_cast<QWMLibSelector*>(editor);
        selector->setText(value.toString());
    }else{
        QWMLibTabSelector *selector=static_cast<QWMLibTabSelector*>(editor);
        selector->setText(value.toString());
    }
}

void QWMLibLookupDelegate::handleNeighbourField(QWidget *editor,QAbstractItemModel *model,
                                                const QModelIndex &index,QModelIndex aIndex)const {
    QWMLibSelector *selector=nullptr;
    if(_type==Plain){
        selector=static_cast<QWMLibSelector*>(editor);
    }else{
        selector=static_cast<QWMLibTabSelector*>(editor)->currentWidget();
    }

    QModelIndex selected=selector->selectionModel()->selection().indexes().first();
    QWMRotatableProxyModel * rModel=(QWMRotatableProxyModel*) model;
    QString fn=rModel->fieldName(aIndex);
    SX(sourceModel,model);
    MDLField * nfieldInfo=MDL->fieldInfo(sourceModel->tableName(),fn);
    if(nfieldInfo!=nullptr){
        QString field=rModel->fieldName(index);
        MDLField * fieldInfo=MDL->fieldInfo(sourceModel->tableName(),field);
        int row=selected.row();
        if(fieldInfo->LookupTyp() ==nfieldInfo->LookupTyp() &&
                fieldInfo->LookupTableName()==nfieldInfo->LookupTableName()){
            QSortFilterProxyModel * pm=(QSortFilterProxyModel*)selector->selectionModel()->model();
            QSqlQueryModel * sm=(QSqlQueryModel * )pm->sourceModel();
            int col=sm->record().indexOf(fn);
            QModelIndex nSelected=pm->index(row,col);
            QVariant v2=nSelected.data();
            model->setData(aIndex,v2);
        }
    }
}

void QWMLibLookupDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
                                        const QModelIndex &index) const
{

    QWMLibSelector *selector=nullptr;
    if(_type==Plain){
        selector=static_cast<QWMLibSelector*>(editor);
    }else{
        selector=static_cast<QWMLibTabSelector*>(editor)->currentWidget();
    }

    QString v=selector->text();
    QString ov=index.data().toString();
    if(v!=ov && !v.isNull()) {
        model->setData(index,v);
        if(selector->selectionModel()->hasSelection()&&!selector->selectionModel()->selection().isEmpty()){
            //如果上下各一个的有关联字段，填充之
            QWMRotatableProxyModel * rModel=(QWMRotatableProxyModel*) model;
            if(rModel->mode()==QWMRotatableProxyModel::H){
                for(int i=index.column()-1;i<=index.column()+1;i++){
                    if(i!=index.column()){
                        QModelIndex aIndex=rModel->index(index.row(),i);
                        handleNeighbourField(editor,model,index,aIndex);
                    }
                }
            }else{
                for(int i=index.row()-1;i<=index.row()+1;i++){
                    if(i!=index.row()){
                        QModelIndex aIndex=rModel->index(i,index.column());
                        handleNeighbourField(editor,model,index,aIndex);
                    }
                }
            }
        }
    }
}


void QWMLibLookupDelegate::updateEditorGeometry(QWidget *editor,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{


    QWMDataTableView * view=(QWMDataTableView *)this->parent();
    QRect rect =view->geometry();
    qDebug()<<"view:"<<rect;
    int widgetWidth=350;//option.rect.width();
    int widgetHeight=350;
    int margin=0;
    int left;
    int top;
    QWMRotatableProxyModel *  model= ( QWMRotatableProxyModel *)index.model();
    if(option.rect.left()+widgetWidth+margin<  rect.width()){
        left=option.rect.left();
    }else{
        left=rect.width()-widgetWidth-margin;
    }
    if(option.rect.top()+widgetHeight+margin<rect.height()){
        top=option.rect.top()+margin;
    }else
    {
        top=option.rect.top()-widgetHeight-margin;
        if(top<0)
            top=0;
    }
    if(model->mode()==QWMRotatableProxyModel::H){

        QPoint topleft(left, top);
        editor->setGeometry(QRect(topleft,QSize(widgetWidth,widgetHeight)));
    }else
    {
        QPoint topleft(left,top);
        editor->setGeometry(QRect(topleft,QSize(widgetWidth,widgetHeight)));
    }

}
void QWMLibLookupDelegate::commitAndCloseEditor(QWMLibSelector * editor)
{
    if(_type==Plain){
        emit commitData(editor);
        emit closeEditor(editor);
    }else{
        TP(editor,QWMLibTabSelector,tabSelector);
        if(tabSelector!=nullptr){
            emit commitData(tabSelector);
            emit closeEditor(tabSelector);
        }
    }
}

void QWMLibLookupDelegate::justCloseEditor(QWMLibSelector * editor)
{
    if(_type==Plain){
        emit closeEditor(editor);
    }else
    {
        TP(editor,QWMLibTabSelector,tabSelector);
        if(tabSelector!=nullptr){
            emit commitData(tabSelector);
            emit closeEditor(tabSelector);
        }
    }
}