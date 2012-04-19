#include <QtGui/QApplication>
#include "symbols.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Symbols w;
    w.show();
    return a.exec();
}
