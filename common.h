#ifndef COMMON_H
#define COMMON_H

#define CLOSEDB(db)   \
    if(db.isValid() && db.isOpen() && db.isOpenError()){ \
    db.close(); \
    }
#define APP ((QWMApplication *) (QCoreApplication::instance()))
#define MDLDB_PATH_SETTING_ENTRY "mdl_path"
#define LIBDB_PATH_SETTING_ENTRY "lib_path"
#define UDLDB_PATH_SETTING_ENTRY "udl_path"
#define WELLDB_PATH_SETTING_ENTRY "well_path"
#define MDLDB_CONNECTION_NAME "mdl"
#define LIBDB_CONNECTION_NAME "lib"
#define UDLDB_CONNECTION_NAME "udl"
#define WELLDB_CONNECTION_NAME "well"
#define UNIT_SETTING_ENTRY "unit_setting"
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

#define DECL_SQL(name,sql) _sql.insert(#name,sql);
#define SQL(name) (_sql[#name])
#define PRINT_ERROR(q) \
if(q.lastError().isValid()) qDebug()<<" Query["<<q.lastQuery()<<"] Error["<<q.lastError().text()<<"],bind["<< q.boundValues()<<"]"<<endl<<flush;\
    else qDebug()<<" Query["<<q.lastQuery()<<"]"<<",bind["<< q.boundValues()<<"]"<<endl<<flush;

#define CFG(x) APP->config()[#x]
#define MDL (MDLDao::instance())
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

#endif // COMMON_H
