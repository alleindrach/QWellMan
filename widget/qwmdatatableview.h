#ifndef QWMDATATABLEVIEW_H
#define QWMDATATABLEVIEW_H
#include <QTableView>
#include <QObject>
#include "qwmrotatableproxymodel.h"
#include "qrotatabletableview.h"

class QWMDataTableView : public QRotatableTableView
{
    Q_OBJECT
public:
    QWMDataTableView(QWidget *parent = nullptr);
    virtual void bindDelegate() override;
    virtual void setModel(QAbstractItemModel *model) override;

protected Q_SLOTS:
    virtual void onModeChange() override;
    virtual void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)  override;

};


#endif // QWMDATATABLEVIEW_H
