#include "mainwindow.h"
#include "printeventfilter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.installEventFilter(new PrintEventFilter());

    MainWindow w;
    w.show();

    return app.exec();
}
