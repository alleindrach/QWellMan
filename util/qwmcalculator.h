#ifndef QWMCALCULATOR_H
#define QWMCALCULATOR_H

#include <QObject>
#include <QSqlRecord>
#include <QScriptValue>
class QWMCalculator : public QObject
{
    Q_OBJECT
public:
    explicit QWMCalculator(QString  idWell,QString table,QSqlRecord rec, QObject *parent = nullptr);
    static QVariant evaluate(QString field, QSqlRecord rec,QString  idWell,QString table,QString  equ);
public slots:
    QString PBTDAll();
    QString TDAll();
    double TD();
    double TVDAll();
signals:
private:
    QString _idWell;
    QString _table;
    QSqlRecord _curRecord;
};

#endif // QWMCALCULATOR_H
