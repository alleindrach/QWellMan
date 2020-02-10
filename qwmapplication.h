#ifndef QWMAPPLICATION_H
#define QWMAPPLICATION_H

#include  <QApplication>
#include <QSqlDatabase>
class QWMApplication : public QApplication
{
    Q_OBJECT
public:
    QWMApplication(int &argc, char **argv);
    ~QWMApplication();
    QSqlDatabase openDB( QString path,QString username,QString password ,QString & error,QString connectionName);
    bool selectDB( QSqlDatabase  &db,QString username,QString password ,QString & error,QString connectionName);
    bool selectWellDB(QString &error,QWidget * parent);
    void initMDLDB();
    void initLIBDB();
    void openWellDB(QString path);
signals:
    void shutdown(int  code);
public slots:

private :

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
};

#endif // QWMAPPLICATION_H
