#include "extrawidgets.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExtraWidgets w;
    w.show();

    return a.exec();
}
