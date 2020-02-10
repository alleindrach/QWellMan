#include "qwmmain.h"

#include <QApplication>
#include  <qwmapplication.h>
int main(int argc, char *argv[])
{
    QWMApplication a(argc, argv);
    QWMMain w;
    w.show();
    return a.exec();
}
