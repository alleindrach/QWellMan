#ifndef COMMON_H
#define COMMON_H
#include <QDebug>
#include <QTime>
#include <QDir>
#define CLOSEDB(db)   \
    if(db.isValid() && db.isOpen() && db.isOpenError()){ \
    db.close(); \
    }
#define APP ((QWMApplication *) (QCoreApplication::instance()))

#define DOC (((QWMMain *)(((QWMApplication *) (QCoreApplication::instance()))->mainWindow()))->currentEditor())

#define MDLDB_PATH_SETTING_ENTRY "mdl_path"
#define LIBDB_PATH_SETTING_ENTRY "lib_path"
#define EDLDB_PATH_SETTING_ENTRY "edl_path"
#define UDLDB_PATH_SETTING_ENTRY "udl_path"
#define WELLDB_PATH_SETTING_ENTRY "well_path"
#define MDLDB_CONNECTION_NAME "mdl"
#define LIBDB_CONNECTION_NAME "lib"
#define EDLDB_CONNECTION_NAME "edl"
#define UDLDB_CONNECTION_NAME "udl"
#define WELLDB_CONNECTION_NAME "well"
#define UNIT_SETTING_ENTRY "unit_setting"
#define EDITOR_TABLE_ENTRY_PREFIX "editor_table_setting.%1"
#define UNIT_SETTING_DEFAULT "Metric"
#define PROFILE_SETTING_ENTRY "profile"
#define PROFILE_SETTING_DEFAULT "All Data"
#define REFERENCE_DATUM_ENTRY "reference_datum"
#define REFERENCE_DATUM_DEFAULT "elvorigkb"
#define WELL_DISPLAY_FIELDS_ENTRY "well_display_fields"
#define EDITOR_SIZE_ENTRY "editor_size"
#define EDITOR_POS_ENTRY "editor_pos"

#define MDL_TABLE(x) "pce"#x
#define LIB_TABLE(x)  "lib"#x
#define CAT_ROLE (Qt::UserRole+100)
#define VISIBLE_ROLE (Qt::UserRole+102)
#define DATA_ROLE (Qt::UserRole+104)
#define TEXT_ROLE (Qt::UserRole+106)
#define RECORD_DES_ROLE (Qt::UserRole+108)
#define PK_ROLE (Qt::UserRole+110)
#define FIELD_ROLE (Qt::UserRole+120)
#define TABLE_NAME_ROLE (Qt::UserRole+130)
#define PK_VALUE_ROLE (Qt::UserRole+150)
#define MODEL_ROLE  (Qt::UserRole+160)
#define SELECT_ROLE (Qt::UserRole+170)
#define CHILD_TABLE_NAME_ROLE (Qt::UserRole+180)
#define LINKED_FIELDS (Qt::UserRole+190)
#define BASE_UNIT_VALUE (Qt::UserRole+210)
#define SORT_ROLE (Qt::UserRole+220)
#define BEGIN_SQL_DECLARATION(typ) \
    QHash<QString,QString> typ::_sql={

#define END_SQL_DECLARATION \
};

#define DECL_SQL(name,sql) {#name,sql},
#define SQL(name) (_sql[#name])
#define PRINT_ERROR(q) \
if(q.lastError().isValid()) qDebug()<<" Query["<<q.lastQuery()<<"] Error["<<q.lastError().text()<<"],bind["<< q.boundValues()<<"]"<<endl<<flush;\
//    else qDebug()<<" Query["<<q.lastQuery()<<"]"<<",bind["<< q.boundValues()<<"]"<<endl<<flush;

#define CFG(x) APP->config()[#x]
#define MDL (MDLDao::instance())
#define LIB (LIBDao::instance())
#define UDL (UDLDao::instance())
#define WELL (WellDao::instance())
#define EDL (EDLDao::instance())
//#define SYS_DEL_REC "wvSysRecDel"
//#define SYS_RECENT_WELL"wvSys01"
//#define SYS_FAVORITE_WELL "wvSys02"

#define StringToUUID(x) QUuid::fromString(x.replace(QRegExp("((?:\\w){8})((?:\\w){4})((?:\\w){4})((?:\\w){4})((?:\\w){12})"),"\\1-\\2-\\3-\\4-\\5"))
#define UUIDToString(x) x.toString(QUuid::Id128).toUpper()
#define DEFAULT_PROFILE "All Data"
#define QS(x,f) (x.value(x.record().indexOf(#f)).toString())
#define QI(x,f) (x.value(x.record().indexOf(#f)).toInt())
#define QB(x,f) (x.value(x.record().indexOf(#f)).toBool())
#define QD(x,f) (x.value(x.record().indexOf(#f)).toDouble())
#define RS(x,f) (x.value(x.indexOf(#f)).toString())
#define RI(x,f) (x.value(x.indexOf(#f)).toInt())
#define RB(x,f) (x.value(x.indexOf(#f)).toBool())
#define RD(x,f) (x.value(x.indexOf(#f)).toDouble())
#define REGISTER_ALL(x) \
    REGISTER(x) \
    REGISTER(x*) \
    REGISTER(QList<x*>)

#define REGISTER(x) qDebug() << (#x) << "type id:" << qMetaTypeId<x>();
#define CS(key,typ) \
    QVariant  cachedValue =_cache[key]; \
    if(cachedValue.isValid()) \
        return cachedValue.value<typ>(); \

#define CS_LIST(key,typ) \
    QVariant  cachedValue =_cache[key]; \
    if(cachedValue.isValid()) \
        return  cachedValue.value<QList<typ*>>(); \

#define CI(key,v) \
    _cache.insert(key,QVariant::fromValue (v));\
    return v;

#define R(r,typ) \
    Record::fromSqlRecord<typ>(typ::staticMetaObject.className(), r,this);
#define Q(q,typ) \
    Record::fromSqlQuery<typ>(typ::staticMetaObject.className(),q,this);

#define S(model) \
    QWMTableModel * model=static_cast<QWMTableModel *>(static_cast<QWMSortFilterProxyModel *>(this->sourceModel())->sourceModel());
#define P(model) \
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel *>(this->sourceModel());

#define SX(model,proxy) \
    QWMTableModel * model=static_cast<QWMTableModel *>(static_cast<QWMSortFilterProxyModel *>(static_cast<QWMRotatableProxyModel *>(proxy)->sourceModel())->sourceModel());
#define PX(model,proxy) \
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel *>(static_cast<QWMRotatableProxyModel *>(proxy)->sourceModel());

#define SA(model,proxy) \
    QWMTableModel * model=static_cast<QWMTableModel *>(static_cast<QWMSortFilterProxyModel *>(proxy)->sourceModel());
#define PA(model,proxy) \
    QWMSortFilterProxyModel * model=static_cast<QWMSortFilterProxyModel *>(proxy);

#define INITFLD(record,fld,value) \
    if(!value.isNull()){ \
        int index=record.indexOf(fld); \
        if(index>=0){ \
            record.setValue(index,value); \
            record.setGenerated(index,true);\
        }\
    }

#define PK_VALUE(pk,record) \
    QString pk; \
    if(!record.isEmpty()) {\
    if(record.indexOf(CFG(ID))>=0){ \
        pk=record.value(CFG(ID)).toString(); \
    }else \
    { \
        pk=record.value(CFG(IDWell)).toString(); \
    }  \
    }

#define PK_INDEX(pk,record) \
    int pk=-1; \
    if(!record.isEmpty()) {\
        if(record.indexOf(CFG(ID))>=0){ \
            pk=record.indexOf(CFG(ID)); \
        }else \
        { \
            pk=record.indexOf(CFG(IDWell)); \
        }  \
    }


#define PK_FLD(pkf,record) \
    QString pkf; \
    if(record.indexOf(CFG(ID))>=0){\
        pkf=CFG(ID); \
    }     \
    else { \
        pkf=CFG(IDWell); \
    }

#define PARENT_ID_FLD(pkf,record) \
    QString pkf; \
    if(record.indexOf(CFG(ParentID))>=0){\
        pkf=CFG(ParentID); \
    }     \
    else { \
        pkf=CFG(IDWell); \
    }

#define TP(t,tp,tpx) \
QObject * x=t;\
     tp *  tpx=nullptr;\
while(x->metaObject()->className()!=tp::staticMetaObject.className() && x->parent()!=nullptr){\
    x=x->parent(); \
}\
if(x!=nullptr && x->metaObject()->className()==tp::staticMetaObject.className()){\
   tpx=(tp*) x; \
}
#define IS_KEY(event,keycode) \
(((QKeyEvent*) event)->key()==(keycode))
#define IS_EVENT(event ,typ) \
(event->type()==typ)

#define IS_KEY_EVENT(event ) \
(event->type()==QEvent::KeyPress)

#define IS_MOUSE_EVENT(event ) \
(event->type()==QEvent::MouseButtonPress)

#define KEY_MATCHED(event ,seq)\
    (((QKeyEvent*) event)->matches(seq))

#define SET_EDITOR(T,D) \
    disconnect(editor,0,0,0); \
    connect(editor,&T::rejected,this,&D::closeEditorAndRevert); \
    connect(editor,&T::accepted,this,&D::commitAndCloseEditor); \
    editor->setParent(parent);\
    editor->setModal(true);\
    editor->setWindowFlag(Qt::Dialog);\
    QWMRotatableProxyModel  *  model=(QWMRotatableProxyModel*)index.model();\
    QString title=  model->fieldTitle(index);\
    editor->setWindowTitle(title);\
    editor->setObjectName("WMEditor");\
    editor->setProperty("TYPE",T::staticMetaObject.className());

#define DISABLE_EDITOR \
     disconnect(editor,0,0,0);

#define SET_PRIMARY_EDITOR(T) \
    editor->setObjectName("WMEditorPM");\
    editor->setProperty("TYPE",T::staticMetaObject.className());

#define IS_EDITOR(W) \
(W->objectName().startsWith("WMEditor"))

#define IS_PRIMARY_EDITOR(W) \
(W->objectName().startsWith("WMEditorPM"))


#define IS_EDITOR_OF(W,T) \
 (W->property("TYPE").toString().compare(T::staticMetaObject.className())==0)


#define EDITOR_TYPE(W,V) \
    QString V=editor->property("TYPE").toString();

//如果一个lookuptyp=8的字段，没有main.pceMDLTableFieldLookupList对应的记录，且有Tblkey***的字段，则判定为引用全部的idrec
#define IS_SPEC_REF_FIELD(fieldInfo)\
UDL->isLookupAllField(fieldInfo)

#define SPEC_REF_TABLE_FLD(fieldInfo) \
    UDL->lookupTableNameField(fieldInfo)

#define USER_PROPERTY "UserProperty"
#define TIMESTAMP(tag)
//\
//    qDebug()<<"==="<< QTime::currentTime().toString("HH:mm:ss.zzz")<<"==="<<__FILE__<<"|"<<__LINE__<< "|"<<#tag<<"  ";

#define ICON_SUFFIX ".svg"
#define ICON_ROOT "pceicons"


#define EC(key,T,editor) \
    T *editor=nullptr; \
    if(APP->editorCached(key)){ \
        editor=(T*)APP->getCachedEditor(key); \
    }

#define EI(key,editor) \
    editor->setKey(key); \
    APP->cachEditor(key,editor)
#define DPRING_RECORD(record,tag)\
    qDebug()<<"******** record start ********"<<tag;\
    for(int i=0;i<record.count();i++){\
        qDebug()<<"**"<<"\t"<<record.fieldName(i)<<","<<record.isGenerated(i)<<"="<<record.value(i);\
    }\
     qDebug()<<"******** record end ********"<<tag;

#define SETTINGS\
 QSettings settings(APP->applicationDirPath()+QDir::separator()+"settings.ini",QSettings::IniFormat);

#define ADD_DUP_ERROR(success,model,errors,table) \
if(!success){\
    if(model->lastError().isValid()){\
        errors<<QString("%1  %2").arg(table).arg(model->lastError().text()); \
    }else{ \
        errors<<QString(tr("%1 复制错误")).arg(table);\
    }\
}

#define ATTACHMENT_TABLE "wvAttachment"
#endif // COMMON_H
