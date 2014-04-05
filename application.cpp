#include "application.h"
#include "mainwindow.h"
#include "ticketprinter.h"
#include <QEvent>
#include <QFileOpenEvent>
#include <QTimer>
#include <QSettings>
#include <QTranslator>
#include <QRegExp>
#include <QDebug>
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

const QString Application::printerNameSetting = "printerName";

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    setOrganizationName("Albisigns");
    setOrganizationDomain("albisigns.de");
    setApplicationName("FasT-printing-helper");
    settings = new QSettings(this);

    printer = new TicketPrinter;
    connect(printer, SIGNAL(finished()), this, SLOT(finishedPrinting()));

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
    delete queuedTicket;
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::FileOpen) {
        QString requestUrl = static_cast<QFileOpenEvent *>(event)->file();
        QRegExp rx("fastprint:/([a-z]+)(!(.+))?");
        rx.indexIn(requestUrl);
        QStringList list = rx.capturedTexts();
        if (list[1] == "print" && !list[3].isEmpty()) {
            queuedTicket = new QString(list[3]);

            windowTimer->stop();
            if (settings->value(printerNameSetting).toString().isEmpty()) {
                if (!window) showWindow();
                window->enableMissingPrinterMode();
            } else {
                printer->printTicket(queuedTicket);
            }
        } else if (list[1] != "test") {
            if (!window) quit();
        }
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

    QTranslator translator;
    translator.load(QLocale::system(), "app", "_", ":/i18n");
    installTranslator(&translator);

    window = new MainWindow();
    connect(window, SIGNAL(submitted()), this, SLOT(windowSubmitted()));
    window->show();
}

QSettings* Application::getSettings()
{
    return settings;
}

void Application::windowSubmitted()
{
    if (queuedTicket) {
        printer->printTicket(queuedTicket);
    } else {
        quit();
    }
}

void Application::finishedPrinting()
{
    delete queuedTicket;
    queuedTicket = NULL;
    quit();
}
