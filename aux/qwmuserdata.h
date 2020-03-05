#ifndef QWMUSERDATA_H
#define QWMUSERDATA_H
#include <QObjectUserData>
#include <QVariant>
class QWMUserData : public QObjectUserData
{
public:
    QWMUserData();
    QWMUserData(QVariant);
    ~QWMUserData(){
    };
    inline QVariant data(){
        return _data;
    }
    inline void setData(QVariant  v){
        _data=v;
    }
private:
    QVariant _data;
};

#endif // QWMUSERDATA_H
