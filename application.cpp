#include "application.h"
#include "mainwindow.h"
#include <QEvent>
#include <QFileOpenEvent>
#include <QTimer>
#include <QDebug>
#include <Carbon/Carbon.h>

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    installEventFilter(this);

    windowTimer = new QTimer(this);
    connect(windowTimer, SIGNAL(timeout()), this, SLOT(showWindow()));
    windowTimer->setSingleShot(true);
    windowTimer->start(100);
}

Application::~Application()
{
    delete window;
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    qDebug() << event->type();
    if (event->type() == QEvent::FileOpen) {
        windowTimer->stop();
        qDebug() << static_cast<QFileOpenEvent *>(event)->file();
        quit();
        return true;
    }

    return false;
}

void Application::showWindow()
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == noErr) {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    }
#endif
    window = new MainWindow();
    window->show();
}
