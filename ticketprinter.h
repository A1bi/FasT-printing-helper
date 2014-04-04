#ifndef TICKETPRINTER_H
#define TICKETPRINTER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QByteArray;

class TicketPrinter : public QObject
{
    Q_OBJECT
public:
    explicit TicketPrinter(QObject *parent = 0);
    ~TicketPrinter();
    void printTicket(const QString path);

private:
    QNetworkAccessManager *manager;

    void printData(const QByteArray*);

signals:

private slots:
    void finishedDownloading(QNetworkReply*);

};

#endif // TICKETPRINTER_H
