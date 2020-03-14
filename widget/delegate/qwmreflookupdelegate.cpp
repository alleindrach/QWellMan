#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QKeyEvent>
#include <QDebug>
#include <QDateEdit>
#include <QComboBox>
#include <QTimeEdit>
#include "qwmreflookupdelegate.h"
#include "qwmlibselector.h"
#include "qwmdatatableview.h"
#include "common.h"
#include "mdldao.h"
#include "mdlfield.h"
#include "qwmtablemodel.h"
#include "qwmsortfilterproxymodel.h"
#include "qwmrotatableproxymodel.h"
#include "libdao.h"
#include "udldao.h"
#include "qwmlibsingleselector.h"
#include "qwmlibtabselector.h"
#include "qwmabstracteditor.h"
#include "qwmrecordsinglestepselector.h"
#include "qwmrecordtwostepselector.h"
#include "qwmicondelegate.h"
#include "qwmiconselector.h"
#include "qpainter.h"
#include "qwmdatetimeeditor.h"
#include "welldao.h"
#include "qwmmain.h"

QWMRefLookupDelegate::QWMRefLookupDelegate(QObject *parent):
    QWMAbstractDelegate(parent){
}

void QWMRefLookupDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const {
    if(IS_EDITOR(editor)&&!IS_PRIMARY_EDITOR(editor)){
        QWMAbstractEditor * selector=dynamic_cast<QWMAbstractEditor*>(editor);
        if(!selector->key().isNull()&&!selector->key().isEmpty()){
            if(APP->editorCached(selector->key()))
            {
                return ;
            }
        }
    }
    QAbstractItemDelegate::destroyEditor(editor,index);
}
QWidget *QWMRefLookupDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const{

    QWMRotatableProxyModel * model=(QWMRotatableProxyModel *)index.model();
    QString table=model->tableName();
    QString field=model->fieldName(index);
    MDLField * fieldInfo=UDL->fieldInfo(table,field);
    QString lib=fieldInfo->LookupTableName();
    QString group=UDL->lookupGroup(APP->profile(),table);
    if(fieldInfo->LookupTyp()==MDLDao::DBDistinctValues){
        QComboBox *editor = new QComboBox(parent);
        QFont font=editor->font();
        font.setPixelSize(10);
        editor->setFont(font);
        editor->setEditable(true);
        QStringList values=WELL->distinctValue(table,field);

        for(int i=0;i<values.size();i++){
            editor->addItem(values[i],values[i]);
        }
        {SET_PRIMARY_EDITOR(QComboBox);}
        return editor;
    }else if( fieldInfo->PhysicalType()==MDLDao::DateTime ){
        if(fieldInfo->LookupTyp()==MDLDao::DateAndTime){
            QString key=QWMDateTimeEditor::staticMetaObject.className();
            EC(key,QWMDateTimeEditor,editor);
            if(editor==nullptr){
                editor= new QWMDateTimeEditor();
                EI(key,editor);
            }
            editor->setModal(true);
            {SET_EDITOR(QWMDateTimeEditor,QWMRefLookupDelegate);}
            return editor;
        }else if(fieldInfo->LookupTyp()==MDLDao::Date){
            QDateEdit *editor = new QDateEdit(parent);
            editor->setDisplayFormat("yyyy-MM-dd");
            editor->setCalendarPopup(true);
            //            editor->installEventFilter(this));
            {SET_PRIMARY_EDITOR(QDateEdit);}
            return editor;
        }else if(fieldInfo->LookupTyp()==MDLDao::Time){
            QTimeEdit *editor = new QTimeEdit(parent);
            editor->setDisplayFormat("HH:mm:ss");
            editor->setCalendarPopup(true);
            {SET_PRIMARY_EDITOR(QTimeEdit);}
            return editor;
        }
    }else if(fieldInfo->LookupTyp()==MDLDao::Icon){
        QString key=QWMIconSelector::staticMetaObject.className();
        EC(key,QWMIconSelector,editor);
        if(editor==nullptr){
            editor= new QWMIconSelector();
            EI(key,editor);
        }
        {SET_EDITOR(QWMIconSelector,QWMRefLookupDelegate)};
        return editor;
    }else if(!group.isNull()&&!group.isEmpty() && fieldInfo->LookupTyp()==MDLDao::LibEdit){//BiTable
        QString key=QString("%1.%2.%3").arg(QWMLibTabSelector::staticMetaObject.className(), table,lib);
        //        qDebug()<<"===="<<key;
        EC(key,QWMLibTabSelector,editor);
        if(editor!=nullptr){
            //            editor->setParent(parent);
            editor->setValue(index.data().toString());
        }else{
            editor = new QWMLibTabSelector(table,lib);
            EI(key,editor);
        }
        {SET_EDITOR(QWMLibTabSelector,QWMRefLookupDelegate)};
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
            editor->setEditable(fieldInfo->LookupTyp()==MDLDao::LibEdit);
            editor->init();
            {SET_EDITOR(QWMLibTabSelector,QWMRefLookupDelegate)};

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
            {SET_EDITOR(QWMLibSingleSelector,QWMRefLookupDelegate)};
            return editor;
        }
    }else if(fieldInfo->LookupTyp()==MDLDao::Foreign){
        QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
        QStringList tables;
        foreach(MDLFieldLookup * l,fls){
            if(l->TableKey()){
                QString tableName=l->LookupItem();
                tables<<tableName;
            }
        }
        if(IS_SPEC_REF_FIELD(fieldInfo)){//two step

            QWMRecordTwoStepSelector * editor=new  QWMRecordTwoStepSelector(DOC->idWell());
            {SET_EDITOR(QWMRecordTwoStepSelector,QWMRefLookupDelegate)};
            return editor;
        }else{
            QWMRecordSingleStepSelector *editor = new QWMRecordSingleStepSelector(tables,DOC->idWell());
            {SET_EDITOR(QWMRecordSingleStepSelector,QWMRefLookupDelegate)};
            return editor;
        }
    }else if(fieldInfo->LookupTyp()==MDLDao::List){
        QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
        QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
        foreach(MDLFieldLookup * l,fls){
            if(!l->TableKey())
            {
                QPair<QString,QVariant> p(l->LookupItem(),l->LookupItem());
                options<<p;
            }
        }
        QComboBox *editor = new QComboBox(parent);
        QFont font=editor->font();
        font.setPixelSize(10);
        editor->setFont(font);
        for(int i=0;i<options.size();i++){
            QPair<QString,QVariant> opt=options[i];
            editor->addItem(opt.first,opt.second);
        }
        editor->setEditable(true);
        {SET_PRIMARY_EDITOR(QComboBox);}
        return editor;
    }else if(fieldInfo->LookupTyp()==MDLDao::TabList){
        QList<MDLFieldLookup *> fls=MDL->fieldLookupinfo(fieldInfo->KeyTbl(),fieldInfo->KeyFld());
        QList<QPair<QString,QVariant>> options{{tr("<空白>"),""}};
        foreach(MDLFieldLookup * l,fls){
            if(!l->TableKey())
            {
                QPair<QString,QVariant> p(l->LookupItem(),l->LookupItem());
                options<<p;
            }else {
                QString tableName=l->LookupItem();
                QString caption=UDL->tableInfo(tableName)->CaptionLongP();
                QPair<QString,QVariant> p(caption,tableName);
                options<<p;
            }
        }
        QComboBox *editor = new QComboBox(parent);
        QFont font=editor->font();
        font.setPixelSize(10);
        editor->setFont(font);
        for(int i=0;i<options.size();i++){
            QPair<QString,QVariant> opt=options[i];
            editor->addItem(opt.first,opt.second);
        }
        editor->setEditable(true);
        {SET_PRIMARY_EDITOR(QComboBox);}
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent,option,index);
}

bool QWMRefLookupDelegate::eventFilter(QObject *watched, QEvent *event) {
    return QWMAbstractDelegate::eventFilter(watched,event);
}

bool QWMRefLookupDelegate::isEditor(QObject *widget){
    return IS_EDITOR(widget);
    //    if(instanceof<QWMAbstractEditor>(widget)){
    //        QWMAbstractEditor * editor=dynamic_cast<QWMAbstractEditor*>(widget);
    //        QWMAbstractEditor::Type type=editor->type();
    //        if(type==QWMAbstractEditor::Simple){
    //            if(instanceof<QWMLibSingleSelector>(widget))
    //                return true;
    //            else
    //                return false;
    //        }else if(type==QWMAbstractEditor::Tab||type==QWMAbstractEditor::BiTab)
    //        {
    //            if(instanceof<QWMLibTabSelector>(widget))
    //                return true;
    //            else
    //                return false;
    //        }else if(type==QWMAbstractEditor::Tree)
    //        {
    //            if(instanceof<QWMRecordSingleStepSelector>(widget))
    //                return true;
    //            else
    //                return false;
    //        }else if(type==QWMAbstractEditor::BiTree)
    //        {
    //            if(instanceof<QWMRecordTwoStepSelector>(widget))
    //                return true;
    //            else
    //                return false;
    //        }else if(type==QWMAbstractEditor::Icon){
    //            if(instanceof<QWMIconSelector>(widget))
    //                return true;
    //            else
    //                return false;
    //        }else if(type==QWMAbstractEditor::DateTime){
    //            if(instanceof<QWMDateTimeEditor>(widget))
    //                return true;
    //            else
    //                return false;
    //        }
    //    }else if(instanceof<QDateEdit>(widget)){

    //    }
    //    else if(instanceof<QTimeEdit>(widget)){

    //        }
    //    return false;
}


void QWMRefLookupDelegate::setEditorData(QWidget *editor,
                                         const QModelIndex &index) const{
    if(IS_EDITOR(editor)){
        QVariant value=index.data(Qt::EditRole);
        QWMRotatableProxyModel*  model=(QWMRotatableProxyModel*) index.model();
        SX(sourceModel,model);
        QString table=sourceModel->tableName();
        QString field=model->fieldName(index);
        MDLField * fieldInfo=UDL->fieldInfo(table,field);
        QString lookupFld=fieldInfo->LookupFieldName();
        QString tableLookup=fieldInfo->lookupTable();

        if(IS_EDITOR_OF(editor,QWMLibSingleSelector)){
            QWMLibSingleSelector *selector=static_cast<QWMLibSingleSelector*>(editor);
            selector->setLookupFld(lookupFld);
            selector->setValue(value.toString());
        }else if(IS_EDITOR_OF(editor,QWMLibTabSelector)){
            QWMLibTabSelector *selector=static_cast<QWMLibTabSelector*>(editor);
            selector->setLookupFld(lookupFld);
            selector->setValue(value.toString());
        }else if(IS_EDITOR_OF(editor,QWMRecordSingleStepSelector)){
            QWMRecordSingleStepSelector *selector=static_cast<QWMRecordSingleStepSelector*>(editor);
            QString strValue=value.toString();
            if(!tableLookup.isNull()&& !tableLookup.isEmpty()&&!strValue.isNull()&&!strValue.isEmpty())
                selector->setValue(QString("%1.%2").arg(tableLookup,value.toString()));
            else
                selector->setValue(strValue);
        }else if(IS_EDITOR_OF(editor,QWMRecordTwoStepSelector)){
            QSqlRecord rec=model->record(index);
            MDLField *  fieldInfo=UDL->fieldInfo(model->tableName(),model->fieldName(index));
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
        }else if(IS_EDITOR_OF(editor,QWMIconSelector)){
            QWMIconSelector *iconSelector = static_cast<QWMIconSelector*>(editor);
            QString value = index.model()->data(index, Qt::DisplayRole).toString();
            QVariant data = index.data(DATA_ROLE);
            iconSelector->setValue(value);
        }else if(IS_EDITOR_OF(editor,QWMDateTimeEditor)){
            QDateTime date =  value.toDateTime();
            QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
            edit->setValue(date);

        }else if(IS_EDITOR_OF(editor,QDateEdit)){
            QDate date = value.toDate();
            QDateEdit *edit=static_cast<QDateEdit*>(editor);
            edit->setDate(date);
        }else if(IS_EDITOR_OF(editor,QTimeEdit)){
            QTime date =  value.toTime();
            QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
            edit->setTime(date);
        }else if(IS_EDITOR_OF(editor,QComboBox)){
            QComboBox *cb = static_cast<QComboBox*>(editor);
            QString value = index.model()->data(index, Qt::EditRole).toString();
            int idx = cb->findData(value);
            if (idx > -1 ) {
                cb->setCurrentIndex(idx);
                return;
            }
            cb->setEditText(value);
        }
    }else
    {
        QVariant v = index.data(DATA_ROLE);
        QByteArray n = editor->metaObject()->userProperty().name();

        if (!n.isEmpty()) {
            if (!v.isValid())
                v = QVariant(editor->property(n).userType(), (const void *)0);
            editor->setProperty(n, v);
        }
        //        return QStyledItemDelegate::setEditorData(editor,index);
    }
}

void QWMRefLookupDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,
                                        const QModelIndex &index) const{

    QWMRotatableProxyModel * rmodel=(QWMRotatableProxyModel*) model;
    if(IS_EDITOR(editor)){
        if(IS_EDITOR_OF(editor,QWMLibSingleSelector)||IS_EDITOR_OF(editor,QWMLibTabSelector)){
            QWMLibSelector *selector=nullptr;
            if(IS_EDITOR_OF(editor,QWMLibSingleSelector)){
                selector=static_cast<QWMLibSingleSelector*>(editor)->selector();
            }else{
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
                MDLField * fieldInfo=UDL->fieldInfo(table,field);
                for(int i=0;i<selectRecord.count();i++){
                    QString namedLibTable=fieldInfo->LookupTableName();
                    //                    QString libTable=selector->libName();
                    QString libField=selectRecord.fieldName(i);
                    MDLField * baseFieldInfo=UDL->fieldByLookup(table, namedLibTable,libField);
                    if(baseFieldInfo!=nullptr){
                        //                        QModelIndex aIndex=rmodel->indexOfSameRecord(index,baseFieldInfo->KeyFld());
                        spv.append(QPair<QString,QVariant>(baseFieldInfo->KeyFld(),selectRecord.value(i)));
                    }
                }
                rmodel->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
            }
        }else if(IS_EDITOR_OF(editor,QWMRecordSingleStepSelector)){
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
        }else if(IS_EDITOR_OF(editor,QWMRecordTwoStepSelector)){
            QWMRecordTwoStepSelector * selector=static_cast<QWMRecordTwoStepSelector*>(editor);
            QString v=selector->value().toString();
            QStringList parts=v.split(".",Qt::SkipEmptyParts);
            QString fieldName=rmodel->fieldName(index);
            QString tableName=rmodel->tableName();
            MDLField * fieldInfo=UDL->fieldInfo(tableName,fieldName);
            if(parts.size()==2){
                QString tableNameFld=SPEC_REF_TABLE_FLD(fieldInfo);
                QString ov=index.data().toString();
                v=parts[1];
                if(v!=ov && !v.isNull()) {
                    QList<QPair<QString,QVariant>> spv;
                    spv.append(QPair<QString,QVariant>(rmodel->fieldName(index),v));
                    spv.append(QPair<QString,QVariant>(tableNameFld,parts[0]));
                    model->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
                }
            }
        }else if(IS_EDITOR_OF(editor,QWMIconSelector)){
            QWMIconSelector *cb = static_cast<QWMIconSelector*>(editor);
            QString newValue = cb->value().toString();

            QVariant oldValue=index.data(Qt::DisplayRole);
            if(oldValue!=newValue){
                model->setData(index, newValue);
            }
        }else if(IS_EDITOR_OF(editor,QWMDateTimeEditor)){
            QWMDateTimeEditor *edit=static_cast<QWMDateTimeEditor*>(editor);
            QDateTime date = edit->value().toDateTime();
            QString format="yyyy-MM-dd HH:mm:ss";
            QString v=date.toString(format);
            QString ov=index.data().toDateTime().toString(format);
            if(v!=ov)
                model->setData(index,QVariant(date.toString(format)));
        }else if(IS_EDITOR_OF(editor,QDateEdit)){
            QDateEdit *edit=static_cast<QDateEdit*>(editor);
            QDate date = edit->date();
            QString format="yyyy-MM-dd";
            QString v=date.toString(format);
            QString ov=index.data().toDate().toString(format);
            if(v!=ov)
                model->setData(index,QVariant(date.toString(format)));
        }else if(IS_EDITOR_OF(editor,QTimeEdit)){
            QTimeEdit *edit=static_cast<QTimeEdit*>(editor);
            QTime date = edit->time();
            QString format="HH-mm-ss";
            QString v=date.toString(format);
            QString ov=index.data().toTime().toString(format);
            if(v!=ov)
                model->setData(index,QVariant(date.toString(format)));
        }else if(IS_EDITOR_OF(editor,QComboBox)){
            QComboBox *cb = static_cast<QComboBox*>(editor);
            QString displayData = cb->currentText();
            QString oldValue=index.data(Qt::EditRole).toString();
            if(oldValue!=displayData){
                model->setData(index, displayData);
            }
        }
    }else
    {
        QStyledItemDelegate::setModelData(editor,model,index);
    }


    //    if(instanceof<QWMAbstractEditor>(editor)){
    //        QWMAbstractEditor * absEditor=dynamic_cast<QWMAbstractEditor*>(editor);
    //        QWMAbstractEditor::Type type=absEditor->type();

    //        if(type==QWMAbstractEditor::Simple||type==QWMAbstractEditor::Tab||type==QWMAbstractEditor::BiTab){
    //            QWMLibSelector *selector=nullptr;
    //            if(type==QWMAbstractEditor::Simple){
    //                selector=static_cast<QWMLibSingleSelector*>(editor)->selector();
    //            }else {
    //                selector=static_cast<QWMLibTabSelector*>(editor)->currentWidget();
    //            }
    //            QSqlRecord selectRecord=selector->selectedRecord();
    //            if(selectRecord.isEmpty()){
    //                if(selector->editable()){
    //                    QString value= selector->text();
    //                    model->setData(index,value);
    //                }else{
    //                    return ;
    //                }
    //            }else{
    //                QList<QPair<QString,QVariant>> spv;
    //                //                SX(sourceModel,rmodel);
    //                //                PX(filterModel,rmodel);
    //                QString table=rmodel->tableName();
    //                QString field=rmodel->fieldName(index);
    //                MDLField * fieldInfo=UDL->fieldInfo(table,field);
    //                for(int i=0;i<selectRecord.count();i++){
    //                    QString namedLibTable=fieldInfo->LookupTableName();
    //                    //                    QString libTable=selector->libName();
    //                    QString libField=selectRecord.fieldName(i);
    //                    MDLField * baseFieldInfo=UDL->fieldByLookup(table, namedLibTable,libField);
    //                    if(baseFieldInfo!=nullptr){
    //                        //                        QModelIndex aIndex=rmodel->indexOfSameRecord(index,baseFieldInfo->KeyFld());
    //                        spv.append(QPair<QString,QVariant>(baseFieldInfo->KeyFld(),selectRecord.value(i)));
    //                    }
    //                }
    //                rmodel->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
    //            }

    //        }else if(type==QWMAbstractEditor::Tree){
    //            QWMRecordSingleStepSelector * selector=static_cast<QWMRecordSingleStepSelector*>(editor);
    //            QString v=selector->value().toString();
    //            QStringList parts=v.split(".",Qt::SkipEmptyParts);
    //            if(parts.size()==2){
    //                QString ov=index.data().toString();
    //                v=parts[1];
    //                if(v!=ov && !v.isNull()) {
    //                    model->setData(index,v);
    //                }
    //            }
    //        }else if(type==QWMAbstractEditor::BiTree){
    //            QWMRecordTwoStepSelector * selector=static_cast<QWMRecordTwoStepSelector*>(editor);
    //            QString v=selector->value().toString();
    //            QStringList parts=v.split(".",Qt::SkipEmptyParts);
    //            QWMRotatableProxyModel * rModel=(QWMRotatableProxyModel*) model;
    //            if(parts.size()==2){
    //                QString tableNameFld=SPEC_REF_TABLE_FLD;
    //                QString ov=index.data().toString();
    //                v=parts[1];
    //                if(v!=ov && !v.isNull()) {
    //                    QList<QPair<QString,QVariant>> spv;
    //                    spv.append(QPair<QString,QVariant>(rModel->fieldName(index),v));
    //                    QModelIndex aIndex=rModel->indexOfSameRecord(index,tableNameFld);
    //                    spv.append(QPair<QString,QVariant>(rModel->fieldName(aIndex),parts[0]));
    //                    model->setData(index,QVariant::fromValue(spv),LINKED_FIELDS);
    //                }
    //            }
    //        }else if(type==QWMAbstractEditor::Icon){
    //            QWMIconSelector *cb = static_cast<QWMIconSelector*>(editor);
    //            QString newValue = cb->value().toString();

    //            QVariant oldValue=index.data(Qt::DisplayRole);
    //            if(oldValue!=newValue){
    //                model->setData(index, newValue);
    //            }
    //        }
    //    }
}

void QWMRefLookupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWMRotatableProxyModel * model=(QWMRotatableProxyModel *)index.model();
    QString table=model->tableName();
    QString field=model->fieldName(index);
    MDLField * fieldInfo=UDL->fieldInfo(table,field);
    if(fieldInfo!=nullptr){
        QString lib=fieldInfo->LookupTableName();
        if(fieldInfo->LookupTyp()==MDLDao::Icon){
            QString iconName=index.data().toString();
            if(iconName.isNull()||iconName.isEmpty())
                return QWMAbstractDelegate::paint(painter,option,index);

            QFileInfo fi=QWMIconSelector::findIcon(iconName);
            if(!fi.isFile()){
                return QWMAbstractDelegate::paint(painter,option,index);
            }
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setPen(Qt::NoPen);
            if (option.state & QStyle::State_Selected)
                painter->setBrush(option.palette.highlightedText());
            else
                painter->setBrush(option.palette.text());
            QPixmap pixmap(fi.absoluteFilePath());
            pixmap=pixmap.scaled(option.rect.width(),option.rect.height(),Qt::KeepAspectRatio);

            QSize sp=pixmap.size();
            int x=option.rect.left()+(option.rect.width()-sp.width())/2;
            int y=option.rect.top()+(option.rect.height()-sp.height())/2;
            QRect r(x,y,sp.width(),sp.height());

            painter->drawPixmap(r,pixmap);

            painter->restore();
            return ;
        }
    }
    QStyledItemDelegate::paint(painter,option,index);

}
