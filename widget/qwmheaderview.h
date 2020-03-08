#ifndef QWMHEADERVIEW_H
#define QWMHEADERVIEW_H

#include <QObject>
#include <QWidget>
#include <QHeaderView>
class QWMHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit QWMHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~QWMHeaderView();
    virtual void setModel(QAbstractItemModel *model) override;
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
protected  Q_SLOTS:
    void sectionsInserted(const QModelIndex &parent, int logicalFirst, int logicalLast);
};

#endif // QWMHEADERVIEW_H
