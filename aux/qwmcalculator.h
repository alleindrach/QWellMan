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
    static QScriptValue evaluate( QSqlRecord rec,QString  idWell,QString table,QString  equ);
signals:
private:
    QString _idWell;
    QString _table;
    QSqlRecord _curRecord;
};

#endif // QWMCALCULATOR_H
