#ifndef QWMDATATABLEVIEW_H
#define QWMDATATABLEVIEW_H
#include <QTableView>
#include <QObject>

class QWMDataTableView : public QTableView
{
    Q_OBJECT
public:
    QWMDataTableView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
    void bindDelegate();
    QRect visualRect(const QModelIndex &index) const override;
    QModelIndex indexAt(const QPoint &p) const override;
    virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);
private:
    //    QWellDoc * m_doc;
};

#endif // QWMDATATABLEVIEW_H
