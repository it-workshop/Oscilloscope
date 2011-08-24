#include <QtGui/QApplication>
#include "desktop_osc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    desktop_osc w;
    w.show();
    return a.exec();
}
