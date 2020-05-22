#include "ventana.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    Ventana w;
    w.show();
    return a.exec();
}
