#include "frameBuffer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    frameBuffer fb;
    fb.show();

    return a.exec();
}
