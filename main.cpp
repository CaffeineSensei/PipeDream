#include "pipedream.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PipeDream w;
    w.show();
    return a.exec();
}
