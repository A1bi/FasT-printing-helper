#ifndef TICKETPRINTER_H
#define TICKETPRINTER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QByteArray;

enum TicketPrinterError
{
    DownloadFailed,
    ParseFailed
};

class TicketPrinter : public QObject
{
    Q_OBJECT
public:
    explicit TicketPrinter(QObject *parent = 0);
    ~TicketPrinter();
    void printTicket(QString *path);

private:
    QNetworkAccessManager *manager;

    void printData(const QByteArray*);

signals:
    void finished();
    void error(TicketPrinterError);

private slots:
    void finishedDownloading(QNetworkReply*);

};

#endif // TICKETPRINTER_H
