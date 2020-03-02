#ifndef QROTATABLETABLEVIEW_P_H
#define QROTATABLETABLEVIEW_P_H
#include "private/qtableview_p.h"
#include "qrotatabletableview.h"
QT_REQUIRE_CONFIG(tableview);

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT  QRotatableTableViewPrivate:public  QTableViewPrivate
{
    Q_DECLARE_PUBLIC(QRotatableTableView)

public:
    QRotatableTableViewPrivate(){};
    virtual ~QRotatableTableViewPrivate(){};

};
#endif // QWMDATATABLEVIEW_P_H
