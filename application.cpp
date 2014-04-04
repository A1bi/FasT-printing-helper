#include "application.h"
#include "mainwindow.h"
#include <QEvent>
#include <QFileOpenEvent>
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <Carbon/Carbon.h>

const QString Application::printerNameSetting = "printerName";

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    settings = new QSettings("Albisigns", "FasT-ticket-printer");

    installEventFilter(this);

    windowTimer = new QTimer(this);
    connect(windowTimer, SIGNAL(timeout()), this, SLOT(showWindow()));
    windowTimer->setSingleShot(true);
    windowTimer->start(100);
}

Application::~Application()
{
    delete window;
    delete settings;
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
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

QSettings* Application::getSettings()
{
    return settings;
}
