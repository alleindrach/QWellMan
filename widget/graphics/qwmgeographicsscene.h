#ifndef QWMGEOGRAPHICSSCENE_H
#define QWMGEOGRAPHICSSCENE_H

#include "QGraphicsScene"
#include <QLabel>
#include <QGraphicsLinearLayout>
#include "qwmgeotrackwidget.h"
class QWMGeoGraphicsScene : public  QGraphicsScene
{
    Q_OBJECT
public:
    QWMGeoGraphicsScene(QString idWell, QObject *parent = nullptr);
    void  resize(QSizeF s);
    void  addTrack(QWMGeoTrackWidget * track,int pos=0,int stretchFactor=0);
    void  reset();
    void  setTitle(QString);
    QGraphicsWidget *  topWidget(){
        return _form;
    }
private:
    QString _idWell;
    QGraphicsProxyWidget * _wellTitle;
    QGraphicsWidget  * _form;
    QGraphicsLinearLayout * _tracksLayout;
    QVector<QWMGeoTrackWidget *> _tracks;
    int _titleHeight{0};
};

#endif // QWMGEOGRAPHICSSCENE_H
