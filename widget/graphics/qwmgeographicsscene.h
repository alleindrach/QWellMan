#ifndef QWMGEOGRAPHICSSCENE_H
#define QWMGEOGRAPHICSSCENE_H

#include "QGraphicsScene"
#include <QLabel>
#include <QGraphicsLinearLayout>
class QWMGeoGraphicsScene : public  QGraphicsScene
{
public:
    QWMGeoGraphicsScene(QString idWell, QObject *parent = nullptr);
    void  resize(QSizeF s);
    void  addTrack(QGraphicsWidget * gw,int pos=0,int stretchFactor=0);
    void  reset();
    void  setTitle(QString);
private:
    QString _idWell;
    QGraphicsProxyWidget * _wellTitle;
    QGraphicsWidget  * _form;
    QGraphicsLinearLayout * _tracksLayout;
    QVector<QGraphicsWidget *> _tracks;

};

#endif // QWMGEOGRAPHICSSCENE_H
