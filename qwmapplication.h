#ifndef QWMAPPLICATION_H
#define QWMAPPLICATION_H

#include <QApplication>
#include <QSqlDatabase>
#include <QStandardItem>
#include "qwmmain.h"
class QWMMain;
class QWMApplication : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QWMMain* mainWindow READ mainWindow WRITE setMainWindow)
public:
    QWMApplication(int &argc, char **argv);
    ~QWMApplication();
    QSqlDatabase openDB( QString path,QString username,QString password ,QString & error,QString connectionName);
    bool selectDB( QSqlDatabase  &db,QString username,QString password ,QString & error,QString connectionName);
    bool selectWellDB(QString &error,QWidget * parent);
    bool initMDLDB();
    bool initUDLDB();
    bool initLIBDB();
    bool initWellDB();
    bool openWellDB(QString path);
    QSqlDatabase & mdl();
    QSqlDatabase & lib();
    QSqlDatabase & udl();
    QSqlDatabase & well();
    QHash<QString,QString>& config();
    QString referenceDatumName(QString value);
    QString referenceDatumValue(QString name);
    QMap<QString,QIcon> & icons();
    QIcon  icon(QString) const;
    void loadPreference();
    QString profile();
    void setProfile(QString v);
    QString unit();
    void setUnit(QString v);
    QString datumPreference();
    void setDatumPref(QString v);
    QStringList wellDisplayList();
    void setWellDisplayList(QStringList list);
    QWMMain * mainWindow();
    void setMainWindow(QWMMain * v);
    enum Catalog{ RECENT,ALL,FAVORITE,QUERY,QUICK_QUERY,GROUP,TABLE,RECORD,NA};
    Q_ENUM(Catalog)
    inline QString style(){return _style;};

    void refresh();

signals:
    void shutdown(int  code);
public slots:

private :
    QMap<QString,QIcon> _iconMap;
    QSqlDatabase _mdlDB;
    QSqlDatabase _udlDB;
    QSqlDatabase _libDB;
    QSqlDatabase _wellDB;
    QString _mdlUserName{QString()};
    QString _mdlPassword{QString()};
    QString _udlUserName{QString()};
    QString _udlPassword{QString()};
    QString _libUserName{QString()};
    QString _libPassword{QString()};
    QString _wellUserName{QString()};
    QString _wellPassword{QString()};
    QHash<QString,QString> _config;


    QString _profile;
    QString _unit;
    QString _datumPreference;
    QStringList  _wellDisplayFields;
    QWMMain * _mainWnd;
    QHash<QString ,QWidget *> _editor_cache;
public :

    QWidget * getCachedEditor(QString key){
        if(_editor_cache.contains(key))
            return _editor_cache[key];
        else
            return nullptr;
    }
    bool editorCached(QString key){
        if(_editor_cache.contains(key))
        {
            return true;
        }
        return false;
    }
    bool cachEditor(QString key,QWidget * widget){
        if(!_editor_cache.contains(key))
        {
            _editor_cache.insert(key,widget);
            return true;
        }
        return false;
    }
    bool removeCachedEditor(QString key){
        if(_editor_cache.contains(key))
        {
            _editor_cache.remove(key);
            return true;
        }
        return false;
    }

private:
    QString _style="/*tablewidget 样式*/ \
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
            image: url(:/images/icons/checkbox-off.svg); \
}\
            QTableView::indicator:enabled:checked { \
            image: url(:/images/icons/checkbox-on.svg);\
}\
            QTreeView::item:selected:active{\
            background: rgb(155, 0, 2);\
            color:white; \
}\
            QTreeView::item:selected:!active{\
            background: rgb(155, 0, 2);\
            color:white;\
}\
            QTabBar::tab:selected, QTabBar::tab:hover { \
                                                    background: rgb(155, 0, 2);\
}\
";

};

#endif // QWMAPPLICATION_H
