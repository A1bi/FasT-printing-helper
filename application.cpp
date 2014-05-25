#include "application.h"
#include "mainwindow.h"
#include <QEvent>
#include <QFileOpenEvent>
#include <QTimer>
#include <QSettings>
#include <QTranslator>
#include <QRegExp>
#include <QMessageBox>
#include <QDebug>
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

const QString Application::urlScheme = "fastprint";
const QString Application::printerNameSetting = "printerName";

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    setOrganizationName("Albisigns");
    setOrganizationDomain("albisigns.de");
    setApplicationName("FasT-printing-helper");
    settings = new QSettings(this);

    window = NULL;
    translator = NULL;
    queuedTicket = NULL;

    printer = new TicketPrinter;
    connect(printer, SIGNAL(finished()), this, SLOT(finishedPrinting()));
    connect(printer, SIGNAL(error(TicketPrinterError)), this, SLOT(printingError(TicketPrinterError)));

    installEventFilter(this);

    windowTimer = new QTimer(this);
    connect(windowTimer, SIGNAL(timeout()), this, SLOT(showWindow()));
    windowTimer->setSingleShot(true);
    windowTimer->start(100);

#ifdef Q_OS_WIN32
    QSettings registry("HKEY_CURRENT_USER\\Software\\Classes\\" + urlScheme, QSettings::NativeFormat);
    registry.setValue(".", "URL:" + urlScheme + " Protocol");
    registry.setValue("URL Protocol", "");
    registry.setValue("shell/open/command/.", QString("\"" + qApp->applicationFilePath().replace("/", "\\") + "\" \"\%1\""));

    if (argc > 1) {
        QString request(argv[1]);
        request.remove(urlScheme + ":");
        handleRequest(&request);
    }
#endif
}

Application::~Application()
{
    delete window;
    delete settings;
    delete queuedTicket;
    delete translator;
}

bool Application::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::FileOpen) {
        QString requestUrl = static_cast<QFileOpenEvent *>(event)->file();
        requestUrl.remove(urlScheme + ":/");
        handleRequest(&requestUrl);
        return true;
    }

    return false;
}

void Application::handleRequest(QString *request)
{
    QRegExp rx("([a-z]+)(!(.+))?");
    rx.indexIn(*request);
    QStringList list = rx.capturedTexts();
    if (list[1] == "print" && !list[3].isEmpty()) {
        queuedTicket = new QString(list[3]);

        windowTimer->stop();
        bool missingPrinterMode = false;
        if (settings->value(printerNameSetting).toString().isEmpty()) {
            if (!window) showWindow();
            missingPrinterMode = true;
        } else {
            printer->printTicket(queuedTicket);
        }
        if (window) window->setMissingPrinterMode(missingPrinterMode);
    } else if (list[1] != "test") {
        if (!window) quit();
    }
}

void Application::showWindow()
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == noErr) {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    }
#endif

    loadTranslator();

    window = new MainWindow();
    connect(window, SIGNAL(submitted()), this, SLOT(windowSubmitted()));
    window->show();
}

QSettings* Application::getSettings()
{
    return settings;
}

void Application::loadTranslator()
{
    if (translator) return;
    translator = new QTranslator;
    translator->load(QLocale::system(), "app", "_", ":/i18n");
    installTranslator(translator);
}

void Application::windowSubmitted()
{
    if (queuedTicket) {
        printer->printTicket(queuedTicket);
        window->setMissingPrinterMode(false);
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

void Application::printingError(TicketPrinterError error)
{
    loadTranslator();

    QString errorMessage;
    switch (error) {
    case DownloadFailed:
        errorMessage = tr("Download failed");
        break;
    case ParseFailed:
        errorMessage = tr("Unable to open ticket");
        break;
    default:
        errorMessage = tr("Unknown error");
        break;
    }

    QMessageBox box(window);
    box.setText(tr("The ticket could not be printed."));
    box.setInformativeText(tr("Error: %1").arg(errorMessage));
    box.setIcon(QMessageBox::Critical);
    box.setWindowModality(Qt::WindowModal);
    box.exec();
    if (!window) quit();
}
