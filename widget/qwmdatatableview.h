#ifndef QWMDATATABLEVIEW_H
#define QWMDATATABLEVIEW_H
#include <QTableView>
#include <QObject>
#include "qwmrotatableproxymodel.h"
#include "qwmreflookupdelegate.h"
class QWMDataTableView : public QTableView
{
    Q_OBJECT
public:
    QWMDataTableView(QWidget *parent = nullptr);
    virtual void bindDelegate() ;
    virtual void setModel(QAbstractItemModel *model) override;

protected Q_SLOTS:
    virtual void onModeChange() ;
    virtual void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)  override;
    void onRowsChanged();

    void setHorizontalHeader(QHeaderView *header);
    void setVerticalHeader(QHeaderView *header);

    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);
signals:
    void RecordCountChanged(int oldCount,int newCount);
private:
//    QWMStyledItemDelegate * _itemDelegate;
    QWMRefLookupDelegate * _itemDelegate;
};


#endif // QWMDATATABLEVIEW_H
