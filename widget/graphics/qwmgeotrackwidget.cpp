#include "qwmgeotrackwidget.h"
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
QWMGeoTrackWidget::QWMGeoTrackWidget(QString title,QGraphicsScene *scene,QGraphicsItem *parent):QGraphicsWidget(parent),_title(title)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

    QLabel * titleLabel=new QLabel();

    titleLabel->setMinimumHeight(24);
    titleLabel->setMaximumHeight(24);
    titleLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    titleLabel->setStyleSheet("background-color:rgb(155, 0, 2);color:white; border: 1px solid rgb(155, 0, 2) ");

    titleLabel->setText(_title);
    titleLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    _titleWidget = scene->addWidget(titleLabel);

    QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout;
    topLayout->setOrientation(Qt::Vertical);
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setItemSpacing(0,0);
    topLayout->addItem(_titleWidget);
    this->setLayout(topLayout);
}

void QWMGeoTrackWidget::setTitle(QString s)
{
    _title=s;
    ((QLabel *)_titleWidget->widget())->setText(s);
}

void QWMGeoTrackWidget::setContent(QGraphicsWidget *widget)
{
    ((QGraphicsLinearLayout*)this->layout())->addItem(widget);
    widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}
