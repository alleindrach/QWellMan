#include "qwmheaderview.h"
#include <QDebug>
#include <QWidget>
#include "qwmrotatableproxymodel.h"
#include "qwmapplication.h"
QWMHeaderView::QWMHeaderView(Qt::Orientation orientation, QWidget *parent ):QHeaderView(orientation,parent)
{

}

QWMHeaderView::~QWMHeaderView()
{

}

void QWMHeaderView::setModel(QAbstractItemModel *model)
{
    QHeaderView::setModel(model);
//    QWMRotatableProxyModel  * rmodel=qobject_cast<QWMRotatableProxyModel*>(model);
    if(this->orientation()==Qt::Horizontal){

        //        QObject::disconnect(model, SIGNAL(columnsInserted(QModelIndex,int,int)),0,0);
        //        QObject::connect(model,SIGNAL(columnsInserted(QModelIndex,int,int)),rmodel, SLOT(columnsInsertedAdaptor(QModelIndex,int,int)));
        //        QObject::connect(rmodel, SIGNAL(columnsInserted(QModelIndex,int,int)),
        //                                     this, SLOT(sectionsInserted(QModelIndex,int,int)));

        //        QObject::disconnect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)),0,0);
        //        QObject::connect(model,SIGNAL(columnsRemoved(QModelIndex,int,int)),rmodel, SLOT(columnsRemovedAdaptor(QModelIndex,int,int)));
        //        QObject::connect(rmodel, &QWMRotatableProxyModel::columnsRemoved,
        //                                     this, &QHeaderView::section);

    }
}

void QWMHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter,rect,logicalIndex);

//    QStyleOptionHeader opt;
//    initStyleOption(&opt);
//    opt.rect = rect;
//    opt.section = logicalIndex;
//    opt.icon=APP->icon("data@1x");
//    opt.iconAlignment = Qt::AlignRight;
//    opt.text = model()->headerData(logicalIndex, orientation(),
//                                    Qt::DisplayRole).toString();

//    int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

//    const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, 0, this));
//    const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
//    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
//        margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, 0, this);

//    const QVariant variant = model()->headerData(logicalIndex, orientation,
//                                                  Qt::DecorationRole);
//    opt.icon = qvariant_cast<QIcon>(variant);
//    if (opt.icon.isNull())
//        opt.icon = qvariant_cast<QPixmap>(variant);
//    if (!opt.icon.isNull()) // see CT_HeaderSection
//        margin += style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this) +
//                  style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

//    if (d->textElideMode != Qt::ElideNone) {
//        const QRect textRect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
//        opt.text = opt.fontMetrics.elidedText(opt.text, d->textElideMode, textRect.width() - margin);
//    }

//    QVariant foregroundBrush = d->model->headerData(logicalIndex, d->orientation,
//                                                    Qt::ForegroundRole);
//    if (foregroundBrush.canConvert<QBrush>())
//        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

//    QPointF oldBO = painter->brushOrigin();
//    QVariant backgroundBrush = d->model->headerData(logicalIndex, d->orientation,
//                                                    Qt::BackgroundRole);
//    if (backgroundBrush.canConvert<QBrush>()) {
//        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
//        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
//        painter->setBrushOrigin(opt.rect.topLeft());
//    }

//    // the section position
//    int visual = visualIndex(logicalIndex);
//    Q_ASSERT(visual != -1);
//    bool first = d->isFirstVisibleSection(visual);
//    bool last = d->isLastVisibleSection(visual);
//    if (first && last)
//        opt.position = QStyleOptionHeader::OnlyOneSection;
//    else if (first)
//        opt.position = d->reverse() ? QStyleOptionHeader::End : QStyleOptionHeader::Beginning;
//    else if (last)
//        opt.position = d->reverse() ? QStyleOptionHeader::Beginning : QStyleOptionHeader::End;
//    else
//        opt.position = QStyleOptionHeader::Middle;
//    opt.orientation = d->orientation;
//    // the selected position
//    bool previousSelected = d->isSectionSelected(this->logicalIndex(visual - 1));
//    bool nextSelected =  d->isSectionSelected(this->logicalIndex(visual + 1));
//    if (previousSelected && nextSelected)
//        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
//    else if (previousSelected)
//        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
//    else if (nextSelected)
//        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
//    else
//        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
//    style()->drawControl(QStyle::CE_Header, &opt, painter, this);
}

void QWMHeaderView::sectionsInserted(const QModelIndex &parent,
                                     int logicalFirst, int logicalLast)
{
    return QHeaderView::sectionsInserted(parent,logicalFirst,logicalLast);
}
