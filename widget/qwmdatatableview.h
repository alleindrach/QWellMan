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
private:
//    QWellDoc * m_doc;
};

#endif // QWMDATATABLEVIEW_H
