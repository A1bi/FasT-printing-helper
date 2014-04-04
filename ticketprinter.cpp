#include "ticketprinter.h"
#include "application.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QSettings>
#include <poppler-qt5.h>

#ifdef QT_DEBUG
#define FAST_HOST "127.0.0.1:3000"
#else
#define FAST_HOST "theater-kaisersesch.de"
#endif

TicketPrinter::TicketPrinter(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedDownloading(QNetworkReply*)));
}

TicketPrinter::~TicketPrinter()
{
    delete manager;
}

void TicketPrinter::printTicket(const QString path)
{
    QUrl url;
    url.setScheme("https");
    url.setHost(FAST_HOST);
    url.setPath(path);
    QNetworkRequest request(url);
    manager->get(request);
}

void TicketPrinter::printData(const QByteArray *data)
{
    Poppler::Document *doc = Poppler::Document::loadFromData(*data);

    QString printerName = static_cast<Application *>(qApp)->getSettings()->value(Application::printerNameSetting).toString();
    QPrinter printer(QPrinter::HighResolution);
    if (!printerName.isEmpty()) {
        printer.setPrinterName(printerName);
    }

    QPainter painter(&printer);
    for (int i = 0; i < doc->numPages(); i++) {
        Poppler::Page *page = doc->page(i);
        QImage image = page->renderToImage(300, 300, 0, 0);
        painter.drawImage(0, 0, image);
        if (i < doc->numPages()-1) {
            printer.newPage();
        }
    }
    painter.end();

    emit finished();
}

void TicketPrinter::finishedDownloading(QNetworkReply *reply)
{
    QByteArray const data = reply->readAll();
    printData(&data);
}
