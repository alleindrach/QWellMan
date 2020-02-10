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
#endif // COMMON_H
