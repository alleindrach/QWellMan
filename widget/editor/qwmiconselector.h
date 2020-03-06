#ifndef QWMICONSELECTOR_H
#define QWMICONSELECTOR_H
#include <QCoreApplication>
#include <QWidget>
#include <QList>
#include <QFileInfo>
#include <QListWidgetItem>
#include "qwmabstracteditor.h"
#include "qwmabstractdelegate.h"
#include "QDir"
#include "QDirIterator"
namespace Ui {
class QWMIconSelector;
}

class QWMIconSelector : public QWMAbstractEditor
{
    Q_OBJECT
public:
    explicit QWMIconSelector(QWidget *parent = nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    void selectFile(QString  filename);
    QString file();
    QList<QFileInfo> folders();
    QList<QFileInfo> files(QString subFolder);
    virtual QList<QWidget *> taborders() override;
    static QString defaultDir(){
        return QString("%1%2%3").arg( QCoreApplication::applicationDirPath()).arg(QDir::separator()).arg(ICON_ROOT);
    }
    static QFileInfo findIcon(QString iconname){
        QString subPath=defaultDir();
        QString  fileName=QString("%1%3").arg(iconname).arg(ICON_SUFFIX);
        QDirIterator it(subPath, QStringList() << fileName, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QList<QFileInfo> list;
        while(it.hasNext()){
            it.next();
            list<<it.fileInfo();
        }
        if(list.size()>0)
            return list.first();
        return QFileInfo();
    }
signals:
//    void  accepted(QWMIconSelector * );
//    void  rejected(QWMIconSelector * );
private slots:
    void on_btn_clicked() override;
    void on_item_doubleclick(const QModelIndex &index);
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_item_click(const QModelIndex &);
    void on_listWidget_itemSelectionChanged();

private:
    Ui::QWMIconSelector *ui;
    QList<QFileInfo> _files;
    void showIcon(QFileInfo );
    friend class QWMAbstractDelegate;
};

#endif // QWMICONSELECTOR_H
