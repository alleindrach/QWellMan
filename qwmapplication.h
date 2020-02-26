#ifndef QWMAPPLICATION_H
#define QWMAPPLICATION_H

#include <QApplication>
#include <QSqlDatabase>
#include <QStandardItem>
#include "qwmmain.h"
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
    enum Catalog{ RECENT,ALL,FAVORITE,QUERY,QUICK_QUERY,GROUP,TABLE};
    Q_ENUM(Catalog)

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

};

#endif // QWMAPPLICATION_H
