#ifndef QWMDATATABLEVIEW_H
#define QWMDATATABLEVIEW_H
#include <QTableView>
#include <QObject>
#include "qwmrotatableproxymodel.h"
class QWMDataTableView : public QTableView
{
    Q_OBJECT
public:
    QWMDataTableView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
    void bindDelegate();
    QRect visualRect(const QModelIndex &index) const override;
    QModelIndex indexAt(const QPoint &p) const override;
    virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;
protected Q_SLOTS:
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override;
    virtual void rowsInserted(const QModelIndex &parent, int start, int end) override;
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    virtual void updateEditorData() override;
    virtual void updateEditorGeometries() override;
    virtual void updateGeometries() override;
private:
    //    QWellDoc * m_doc;
protected slots:
    virtual void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)  override;
    virtual void commitData(QWidget *editor)   override;
    void on_header_clicked(int section);
    void on_mode_change();
    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);
signals:
    void RecordCountChanged(int oldCount,int newCount);
};

#endif // QWMDATATABLEVIEW_H
