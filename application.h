#ifndef APPLICATION_H
#define APPLICATION_H

#include "ticketprinter.h"
#include <QApplication>

class MainWindow;
class QTimer;
class QSettings;
class QTranslator;

class Application : public QApplication
{
    Q_OBJECT
public:
    static const QString printerNameSetting;

    explicit Application(int&, char**);
    ~Application();
    QSettings* getSettings();

private:
    MainWindow *window;
    QTimer *windowTimer;
    QSettings *settings;
    TicketPrinter *printer;
    QString *queuedTicket;
    QTranslator *translator;

    bool eventFilter(QObject*, QEvent*);
    void loadTranslator();

private slots:
    void showWindow();
    void windowSubmitted();
    void finishedPrinting();
    void printingError(TicketPrinterError);

};

#endif // APPLICATION_H
