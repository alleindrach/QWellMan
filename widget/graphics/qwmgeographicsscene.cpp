#include "qwmgeographicsscene.h"
#include <QTextEdit>
#include <QSizePolicy>
#include <QGraphicsLinearLayout>
#include <QSqlRecord>
#include <QGraphicsProxyWidget>
#include "welldao.h"
QWMGeoGraphicsScene::QWMGeoGraphicsScene(QString idWell,QObject *parent)
    :QGraphicsScene(parent),_idWell(idWell)
{
    QSqlRecord wellRec=WELL->well(_idWell);
    QString wellDes=WELL->recordDes(CFG(KeyTblMain),wellRec);


    QLabel * wellTitleLabel=new QLabel();

    wellTitleLabel->setMinimumHeight(32);
    wellTitleLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    wellTitleLabel->setStyleSheet("background-color:white;color:black; border: 1px solid red ");

    wellTitleLabel->setText(wellDes);
    wellTitleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _wellTitle = this->addWidget(wellTitleLabel);


    QTextEdit * textEditWidget2=new QTextEdit();
    textEditWidget2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    textEditWidget2->setStyleSheet("background-color:black");
    textEditWidget2->setText("Helloworld");
    textEditWidget2->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QGraphicsProxyWidget *textEdit2 = this->addWidget(textEditWidget2);


    QTextEdit * textEditWidget3=new QTextEdit();
    textEditWidget3->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    textEditWidget3->setStyleSheet("background-color:gray");
    textEditWidget3->setText("Helloworld");
    textEditWidget3->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QGraphicsProxyWidget *textEdit3 = this->addWidget(textEditWidget3);


    QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout;
    topLayout->setOrientation(Qt::Vertical);
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setItemSpacing(0,0);

    topLayout->addItem(_wellTitle);

    QGraphicsWidget * tracks = new QGraphicsWidget;
    _tracksLayout = new QGraphicsLinearLayout;
    _tracksLayout->setOrientation(Qt::Horizontal);
    _tracksLayout->setContentsMargins(0,0,0,0);
    _tracksLayout->setItemSpacing(0,0);
    tracks->setLayout(_tracksLayout);
//    _tracksLayout->addItem(textEdit2);
//    _tracksLayout->addItem(textEdit3);

    topLayout->addItem(tracks);


    _form = new QGraphicsWidget;
    _form->setContentsMargins(0,0,0,0);
    _form->setLayout(topLayout);
    _form->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //    form->setGeometry(QRectF(0,0,300,600));
    _form->setPos(0,0);
    //    _form->setAutoFillBackground(true);
    this->addItem(_form);
}

void QWMGeoGraphicsScene::resize(QSizeF s)
{
    _form->resize(s);
}

void QWMGeoGraphicsScene::addTrack(QGraphicsWidget *gw, int pos, int stretchFactor)
{
    if(pos<0||pos>=_tracks.size()){
        _tracks.append(gw);
    }else{
        _tracks.insert(pos,gw);
    }
    _tracksLayout->addItem(gw);
}

void QWMGeoGraphicsScene::reset()
{
    for(int i=0;i<_tracksLayout->count();i++){
        _tracksLayout->removeAt(i);
    }

}

void QWMGeoGraphicsScene::setTitle(QString v)
{
    QLabel *lbl=(QLabel *)_wellTitle->widget();
    lbl->setText(v);
}
