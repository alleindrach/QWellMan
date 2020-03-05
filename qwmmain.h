#ifndef QWMMAIN_H
#define QWMMAIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QStandardItem>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <qwmsortfilterproxymodel.h>
//#include "qwmrotatableproxymodel.h"
#include "qwmdataeditor.h"
QT_BEGIN_NAMESPACE
namespace Ui { class QWMMain; }
QT_END_NAMESPACE

class QWMMain : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QWMDataEditor* currentEditor READ currentEditor WRITE setCurrentEditor)
public:
    QWMMain(QWidget *parent = nullptr);
    ~QWMMain();

    void loadAllWells();
    void loadFavoriateWells();
    void loadRecentWells();
    void resizeEvent(QResizeEvent *event) override;
    void showStatus(QString status);
    void showProfile(QString profile);
    void showUnitSetting(QString unit);
    void showReferenceDatum(QString datum);
    QWMDataEditor * currentEditor() const;
protected:
    void setCurrentEditor(QWMDataEditor *);
private:
    void editWell(QString idWell);
    void showWellGrid(QWMSortFilterProxyModel *  model);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
private slots:

    void init_record_on_prime_insert(int row, QSqlRecord &record);

    void on_actionChangeDB_triggered();

    void on_trvCatalogs_clicked(const QModelIndex &index);

    void on_actionFavorite_triggered();

    void on_tbvWells_entered(const QModelIndex &index);

    void on_actionUnFavorite_triggered();

    void on_actionUnit_triggered();

    void on_actionProfile_triggered();

    void on_actionDatumPref_triggered();

    void on_actionListColumn_triggered();

    void on_well_view_header_fields_change();
    void on_actionEdit_triggered();

    void on_actionNew_triggered();

    void on_actionDelete_triggered();

    friend class QWMDataEditor;

private:
    Ui::QWMMain *ui;
    QStandardItemModel * _catalogModel;
    QStandardItem * _qsiRecentOpenWell;
    QStandardItem * _qsiAllWells;
    QStandardItem * _qsiFavoriate;
    QStandardItem * _qsiQuery;
    QStandardItem * _qsiQuickQuery;
    QWMDataEditor *  _currentEditor{nullptr};
    QLabel * _lblStatus ;
    QLabel * _lblProfile ;
    QLabel * _lblUnit;
    QLabel * _lblReferenceDatum ;
    QLabel * _lblMessage;

    QString _TableStyle="/*tablewidget 样式*/ \
            QTableView { \
            font-size:10px ;\
            selection-background-color:rgb(155, 0, 2);\
            alternate-background-color: gray; \
            background-color:white;/*整个表格的背景色，这里为白色*/ \
            border:1px solid #E0DDDC;/*边框为1像素，灰色*/ \
            gridline-color:lightgray;/*这个是表格的格子线的颜色，为亮灰*/ \
} \
            QTableView QTableCornerButton::section{\
            border:0px solid lightgray; \
            background-color: rgb(50,50,50); \
            selection-background-color: darkblue ;\
            color:white;\
}  \
            \
            QHeaderView::section { \
            color:white;\
            font-size:10px ;\
            background-color: rgb(50,50,50); \
            selection-background-color: darkblue; \
            padding-left: 4px; \
            border-right: 1px solid lightgray; \
            border-bottom: 1px solid lightgray; \
} \
            QHeaderView::section:checked { \
            color:white;\
            background-color:rgb(50, 50, 50); \
} \
            QHeaderView::section:unchecked { \
            color:black;\
            background-color: white; \
} \
            QTableView::indicator { \
            width: 17px; \
            height: 17px; \
} \
            QTableView::indicator:enabled:unchecked { \
            image: url(:/images/icons/uncheck.png); \
}\
            QTableView::indicator:enabled:checked { \
            image: url(:/images/icons/checked.png);\
}\
            ";

};
#endif // QWMMAIN_H
