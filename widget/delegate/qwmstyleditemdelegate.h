#ifndef QWMSTYLEDITEMDELEGATE_H
#define QWMSTYLEDITEMDELEGATE_H
#include <QObject>
#include <QStyledItemDelegate>

class QWMStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:

    QWMStyledItemDelegate( QObject * parent=nullptr);
    ~QWMStyledItemDelegate(){};

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) const {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override ;
public slots:
signals:
};


#endif // QWMSTYLEDITEMDELEGATE_H
