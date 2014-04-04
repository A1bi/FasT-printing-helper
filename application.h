#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class MainWindow;
class QTimer;
class QSettings;
class TicketPrinter;

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

    bool eventFilter(QObject*, QEvent*);

private slots:
    void showWindow();
    void windowSubmitted();
    void finishedPrinting();

};

#endif // APPLICATION_H
