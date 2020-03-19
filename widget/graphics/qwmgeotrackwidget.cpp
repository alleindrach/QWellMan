#include "qwmgeotrackwidget.h"
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsLinearLayout>


QWMGeoTrackWidget::QWMGeoTrackWidget(QWMGeoTrackTitle *title, QWMGeoTrackContent *content, QGraphicsItem *parent):
    QGraphicsWidget(parent),_title(title),_content(content)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);

    QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout;
    topLayout->setOrientation(Qt::Vertical);
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->setItemSpacing(0,0);
    topLayout->addItem(title);

    topLayout->addItem(content);
    content->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setLayout(topLayout);
    title->setParentItem(this);

    connect(content,&QWMGeoTrackContent::hoverData,this,&QWMGeoTrackWidget::on_hover_data);
}

void QWMGeoTrackWidget::on_hover_data(QPointF pos,  QString des)
{
    emit this->hoverData(pos,_title->title(),des);
}

