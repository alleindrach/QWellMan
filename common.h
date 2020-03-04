#ifndef COMMON_H
#define COMMON_H

#define CLOSEDB(db)   \
    if(db.isValid() && db.isOpen() && db.isOpenError()){ \
    db.close(); \
    }
#define APP ((QWMApplication *) (QCoreApplication::instance()))
#define DOC (((QWMMain *)(((QWMApplication *) (QCoreApplication::instance()))->mainWindow()))->currentEditor())

#define MDLDB_PATH_SETTING_ENTRY "mdl_path"
#define LIBDB_PATH_SETTING_ENTRY "lib_path"
#define UDLDB_PATH_SETTING_ENTRY "udl_path"
#define WELLDB_PATH_SETTING_ENTRY "well_path"
#define MDLDB_CONNECTION_NAME "mdl"
#define LIBDB_CONNECTION_NAME "lib"
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
#define BEGIN_SQL_DECLARATION(typ) \
    QHash<QString,QString> typ::_sql={

#define END_SQL_DECLARATION \
};

#define DECL_SQL(name,sql) {#name,sql},
#define SQL(name) (_sql[#name])
#define PRINT_ERROR(q) \
if(q.lastError().isValid()) qDebug()<<" Query["<<q.lastQuery()<<"] Error["<<q.lastError().text()<<"],bind["<< q.boundValues()<<"]"<<endl<<flush;\
    else qDebug()<<" Query["<<q.lastQuery()<<"]"<<",bind["<< q.boundValues()<<"]"<<endl<<flush;

#define CFG(x) APP->config()[#x]
#define MDL (MDLDao::instance())
#define LIB (LIBDao::instance())
#define UDL (UDLDao::instance())
#define WELL (WellDao::instance())
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
    if(record.indexOf(CFG(ID))>0){ \
        pk=record.value(CFG(ID)).toString(); \
    }else \
    { \
        pk=record.value(CFG(IDWell)).toString(); \
    }

#define PK_FLD(pkf,record) \
    if(record.indexOf(CFG(ID))>=0){\
        pkf=CFG(ID); \
    }     \
    else { \
        pkf=CFG(IDWell); \
    }

#define PARENT_ID_FLD(pkf,record) \
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

#define KEY_MATCHED(event ,seq)\
    (((QKeyEvent*) event)->matches(seq))

#define EDITOR_TITLE \
    QWMRotatableProxyModel  *  model=(QWMRotatableProxyModel*)index.model();\
    QString title=  model->fieldTitle(index);\
    editor->setWindowTitle(title);

#define IS_SPEC_REF_FIELD(fieldInfo)\
    (fieldInfo->KeyFld().compare(CFG(ParentID),Qt::CaseInsensitive)==0)

#define SPEC_REF_TABLE_FLD CFG(TblKeyParent)
#endif // COMMON_H
