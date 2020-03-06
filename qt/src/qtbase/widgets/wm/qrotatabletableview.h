#ifndef QROTATABLETABLEVIEW_H
#define QROTATABLETABLEVIEW_H
#include <QTableView>
#include <QObject>
#include <QHeaderView>

QT_REQUIRE_CONFIG(tableview);

QT_BEGIN_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QRotatableTableViewPrivate);

class Q_WIDGETS_EXPORT QRotatableTableView : public QTableView
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QRotatableTableView)
    Q_DISABLE_COPY(QRotatableTableView)
public:
    QRotatableTableView(QWidget *parent = nullptr);

    virtual void setModel(QAbstractItemModel *model) override;
    void setHorizontalHeader(QHeaderView *header);
    void setVerticalHeader(QHeaderView *header);

protected:
    QRotatableTableView(QRotatableTableViewPrivate &, QWidget *parent);
protected Q_SLOTS:
    virtual void bindDelegate()=0;
    void rowCountChanged(int oldCount, int newCount);
    void columnCountChanged(int oldCount, int newCount);
    virtual void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)  override;
    virtual void onModeChange()=0;
private:
    //    QWellDoc * m_doc;

public :
signals:
    void RecordCountChanged(int oldCount,int newCount);
};


#endif // QROTATABLETABLEVIEW_H
