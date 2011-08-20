#include <QtGui/QApplication>
#include "osc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Osc w;
    w.show();
    return a.exec();
}
