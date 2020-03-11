#ifndef QWMMAIN_H
#define QWMMAIN_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QStandardItem>
#include <QLabel>
#include <QSortFilterProxyModel>
#include "qwmsortfilterproxymodel.h"
#include "qwmdataeditor.h"
#include "qwmabout.h"
#include "qwmapplication.h"
#include <QDesktopWidget>

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
    inline QWMAbout * splash(){
        return _splash;
    }
    void showSplash();
    void hideSplash();
    void init();
    void initEditors();
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

    void on_actionAbout_triggered();

    void on_actionRefresh_triggered();

    void before_insert(QSqlRecord &record);
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
    QWMAbout * _splash;

};
#endif // QWMMAIN_H
