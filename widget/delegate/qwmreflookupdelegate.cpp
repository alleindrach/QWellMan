#include "qwmreflookupdelegate.h"
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
#include "udldao.h"
#include "qwmlibsingleselector.h"
#include "qwmlibtabselector.h"
#include "qwmabstracteditor.h"
#include "qwmrecordsinglestepselector.h"
#include "qwmrecordtwostepselector.h"
QWMRefLookupDelegate::QWMRefLookupDelegate(QObject *parent):
    QWMAbstractDelegate(parent){

}

void QWMRefLookupDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const {
    if(instanceof<QWMAbstractEditor>(editor)){
        QWMAbstractEditor * selector=dynamic_cast<QWMAbstractEditor*>(editor);
        if(APP->editorCached(selector->key()))
        {
            return ;
        }
    }
    QAbstractItemDelegate::destroyEditor(editor,index);
}
QWidget *QWMRefLookupDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &/*option*/,
                                            const QModelIndex &index) const{

    QWMRotatableProxyModel * model=(QWMRotatableProxyModel *)index.model();
    SX(sourceModel,model);
    PX(filterModel,model);
    QString table=sourceModel->tableName();
    QString field=model->fieldName(index);
    MDLField * fieldInfo=MDL->fieldInfo(table,field);
    QString lib=fieldInfo->LookupTableName();
    QString group=UDL->lookupGroup(APP->profile(),table);
    if(!group.isNull()&&!group.isEmpty() && fieldInfo->LookupTyp()==MDLDao::LibEdit){//BiTable
        QString key=QString("%1.%2.%3").arg(QWMLibTabSelector::staticMetaObject.className(), table,lib);
        qDebug()<<"===="<<key;
        EC(key,QWMLibTabSelector,editor);
        if(editor!=nullptr){
            //            editor->setParent(parent);
            editor->setValue(index.data().toString());
        }else{
            editor = new QWMLibTabSelector(table,lib);
            EI(key,editor);
        }
        disconnect(editor,&QWMLibTabSelector::rejected,0,0);
        connect(editor,&QWMLibTabSelector::rejected,this,&QWMRefLookupDelegate::closeEditorAndRevert);
        disconnect(editor,&QWMLibTabSelector::accepted,0,0);
        connect(editor,&QWMLibTabSelector::accepted,this,&QWMRefLookupDelegate::commitAndCloseEditor);
        editor->setTitle(fieldInfo->caption());
        editor->setModal(true);
        TIMESTAMP(QWMRefLookupDelegateE);
        {EDITOR_TITLE};
        return editor;

    }
    else if(fieldInfo->LookupTyp()==MDLDao::LibEdit||fieldInfo->LookupTyp()==MDLDao::LibOnly){
        if(LIB->libLookupAsStruct(lib)->record().indexOf(CFG(LibTab))>=0){//Tab
            QString key=QString("%1.%2.%3").arg(QWMLibTabSelector::staticMetaObject.className(), table,lib);
            EC(key,QWMLibTabSelector,editor);
            if(editor==nullptr){
                editor=new QWMLibTabSelector(table,lib);
                EI(key,editor);
            }
            //            editor->setLookupFld(fieldInfo->LookupFieldName());
            editor->setEditable(fieldInfo->LookupTyp()==MDLDao::LibEdit);
            editor->init();
            connect(editor,&QWMLibSingleSelector::rejected,this,&QWMRefLookupDelegate::closeEditorAndRevert);
            connect(editor,&QWMLibSingleSelector::accepted,this,&QWMRefLookupDelegate::commitAndCloseEditor);
            editor->setModal(true);
            {EDITOR_TITLE};
            return editor;
        }else{//无Tab
            QString key=QString("%1.%2.%3").arg(QWMLibSingleSelector::staticMetaObject.className(), table,lib);
            EC(key,QWMLibSingleSelector,editor);
            if(editor==nullptr){
                editor= new QWMLibSingleSelector(table,lib);
                EI(key,editor);
            }
            editor->setEditable(fieldInfo->LookupTyp()==MDLDao::LibEdit);
            editor->init();
            connect(editor,&QWMLibSingleSelector::rejected,this,&QWMRefLookupDelegate::closeEditorAndRevert);
            connect(editor,&QWMLibSingleSelector::accepted,this,&QWMRefLookupDelegate::commitAndCloseEditor);
            editor->setModal(true);
            {EDITOR_TITLE};
            return editor;
        }
    }else if(fieldInfo->LookupTyp()==MDLDao::Foreign){
        QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
        //                QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
        QStringList tables;
        foreach(MDLFieldLookup * l,fls){
            if(l->TableKey()){
                QString tableName=l->LookupItem();
                tables<<tableName;
            }
        }
        if(IS_SPEC_REF_FIELD(fieldInfo)){//two step

            QWMRecordTwoStepSelector * editor=new  QWMRecordTwoStepSelector(DOC->idWell());
            connect(editor,&QWMRecordTwoStepSelector::rejected,this,&QWMRefLookupDelegate::closeEditorAndRevert);
            connect(editor,&QWMRecordTwoStepSelector::accepted,this,&QWMRefLookupDelegate::commitAndCloseEditor);
            editor->setModal(true);
            {EDITOR_TITLE};
            return editor;
        }else{
            QWMRecordSingleStepSelector *editor = new QWMRecordSingleStepSelector(tables,DOC->idWell());
            connect(editor,&QWMRecordSingleStepSelector::rejected,this,&QWMRefLookupDelegate::closeEditorAndRevert);
            connect(editor,&QWMRecordSingleStepSelector::accepted,this,&QWMRefLookupDelegate::commitAndCloseEditor);
            editor->setModal(true);
            {EDITOR_TITLE};
            return editor;
        }
    }

    return nullptr;
}

bool QWMRefLookupDelegate::eventFilter(QObject *watched, QEvent *event) {
    return QWMAbstractDelegate::eventFilter(watched,event);
}

bool QWMRefLookupDelegate::isEditor(QObject *widget){
    if(instanceof<QWMAbstractEditor>(widget)){
        QWMAbstractEditor * editor=dynamic_cast<QWMAbstractEditor*>(widget);
        QWMAbstractEditor::Type type=editor->type();
        if(type==QWMAbstractEditor::Simple){
            if(instanceof<QWMLibSingleSelector>(widget))
                return true;
            else
                return false;
        }else if(type==QWMAbstractEditor::Tab||type==QWMAbstractEditor::BiTab)
        {
            if(instanceof<QWMLibTabSelector>(widget))
                return true;
            else
                return false;
        }else if(type==QWMAbstractEditor::Tree)
        {
            if(instanceof<QWMRecordSingleStepSelector>(widget))
                return true;
            else
                return false;
        }else if(type==QWMAbstractEditor::BiTree)
        {
            if(instanceof<QWMRecordTwoStepSelector>(widget))
                return true;
            else
                return false;
        }
    }
    return false;
}


void QWMRefLookupDelegate::setEditorData(QWidget *editor,
                                         const QModelIndex &index) const{
    QVariant value=index.data(Qt::EditRole);
    QWMRotatableProxyModel*  model=(QWMRotatableProxyModel*) index.model();
    SX(sourceModel,model);
    //    PX(filterModel,model);
    QString table=sourceModel->tableName();
    QString field=model->fieldName(index);
    MDLField * fieldInfo=MDL->fieldInfo(table,field);
    QString lookupFld=fieldInfo->LookupFieldName();
    QString tableLookup=fieldInfo->lookupTable();
    if(instanceof<QWMAbstractEditor>(editor)){
        QWMAbstractEditor * absEditor=dynamic_cast<QWMAbstractEditor*>(editor);
        QWMAbstractEditor::Type type=absEditor->type();

        if(type==QWMAbstractEditor::Simple){
            QWMLibSingleSelector *selector=static_cast<QWMLibSingleSelector*>(editor);
            selector->setLookupFld(lookupFld);
            selector->setValue(value.toString());
        }else if(type==QWMAbstractEditor::Tab||type==QWMAbstractEditor::BiTab){
            QWMLibTabSelector *selector=static_cast<QWMLibTabSelector*>(editor);
            selector->setLookupFld(lookupFld);
            selector->setValue(value.toString());
        }else if(type==QWMAbstractEditor::Tree){
            //        QSqlRecord rec=qobject_cast<QWMRotatableProxyModel*> (index.model())->record(index);
            QWMRecordSingleStepSelector *selector=static_cast<QWMRecordSingleStepSelector*>(editor);
            QString strValue=value.toString();
            if(!tableLookup.isNull()&& !tableLookup.isEmpty()&&!strValue.isNull()&&!strValue.isEmpty())
                selector->setValue(QString("%1.%2").arg(tableLookup,value.toString()));
            else
                selector->setValue(strValue);
        }else if(type==QWMAbstractEditor::BiTree){
            QSqlRecord rec=model->record(index);
            MDLField *  fieldInfo=MDL->fieldInfo(model->tableName(),model->fieldName(index));
            //对于需要从多个表引用IDREC的字段，需要找出此id对应的表名，
            //            有两种情况，
            //            1 如果是IDRecParent，则为同一行的TblKeyParent的值
            //            2 如果是其他字段，则依据main.pceMDLTableFieldLookupList.lookupItem
            tableLookup=fieldInfo->lookupTable(rec);
            QString strValue=value.toString();
            QWMRecordTwoStepSelector *selector=static_cast<QWMRecordTwoStepSelector*>(editor);
            if(!tableLookup.isNull()&& !tableLookup.isEmpty()&&!strValue.isNull()&&!strValue.isEmpty())
                selector->setValue(QString("%1.%2").arg(tableLookup,value.toString()));
            else
                selector->setValue(strValue);
        }
    }
}

void QWMRefLookupDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
                                        const QModelIndex &index) const{

    QWMRotatableProxyModel * rmodel=(QWMRotatableProxyModel*) model;
    if(instanceof<QWMAbstractEditor>(editor)){
        QWMAbstractEditor * absEditor=dynamic_cast<QWMAbstractEditor*>(editor);
        QWMAbstractEditor::Type type=absEditor->type();

        if(type==QWMAbstractEditor::Simple||type==QWMAbstractEditor::Tab||type==QWMAbstractEditor::BiTab){
            QWMLibSelector *selector=nullptr;
            if(type==QWMAbstractEditor::Simple){
                selector=static_cast<QWMLibSingleSelector*>(editor)->selector();
            }else {
                selector=static_cast<QWMLibTabSelector*>(editor)->currentWidget();
            }
            QSqlRecord selectRecord=selector->selectedRecord();
            if(selectRecord.isEmpty()){
                if(selector->editable()){
                    QString value= selector->text();
                    model->setData(index,value);
                }else{
                    return ;
                }
            }else{
                QList<QPair<QString,QVariant>> spv;
                //                SX(sourceModel,rmodel);
                //                PX(filterModel,rmodel);
                QString table=rmodel->tableName();
                QString field=rmodel->fieldName(index);
                MDLField * fieldInfo=MDL->fieldInfo(table,field);
                for(int i=0;i<selectRecord.count();i++){
                    QString namedLibTable=fieldInfo->LookupTableName();
                    //                    QString libTable=selector->libName();
                    QString libField=selectRecord.fieldName(i);
                    MDLField * baseFieldInfo=MDL->fieldByLookup(table, namedLibTable,libField);
                    if(baseFieldInfo!=nullptr){
                        //                        QModelIndex aIndex=rmodel->indexOfSameRecord(index,baseFieldInfo->KeyFld());
                        spv.append(QPair<QString,QVariant>(baseFieldInfo->KeyFld(),selectRecord.value(i)));
                    }
                }
                rmodel->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
            }

        }else if(type==QWMAbstractEditor::Tree){
            QWMRecordSingleStepSelector * selector=static_cast<QWMRecordSingleStepSelector*>(editor);
            QString v=selector->value().toString();
            QStringList parts=v.split(".",Qt::SkipEmptyParts);
            if(parts.size()==2){
                QString ov=index.data().toString();
                v=parts[1];
                if(v!=ov && !v.isNull()) {
                    model->setData(index,v);
                }
            }
        }else if(type==QWMAbstractEditor::BiTree){
            QWMRecordTwoStepSelector * selector=static_cast<QWMRecordTwoStepSelector*>(editor);
            QString v=selector->value().toString();
            QStringList parts=v.split(".",Qt::SkipEmptyParts);
            QWMRotatableProxyModel * rModel=(QWMRotatableProxyModel*) model;
            if(parts.size()==2){
                QString tableNameFld=SPEC_REF_TABLE_FLD;
                QString ov=index.data().toString();
                v=parts[1];
                if(v!=ov && !v.isNull()) {
                    QList<QPair<QString,QVariant>> spv;
                    spv.append(QPair<QString,QVariant>(rModel->fieldName(index),v));
                    QModelIndex aIndex=rModel->indexOfSameRecord(index,tableNameFld);
                    spv.append(QPair<QString,QVariant>(rModel->fieldName(aIndex),parts[0]));
                    model->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
                }
            }
        }
    }
}
