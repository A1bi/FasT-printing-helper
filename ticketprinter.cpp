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
#define FAST_URL "http://127.0.0.1:3000"
#else
#define FAST_URL "https://theater-kaisersesch.de"
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

void TicketPrinter::printTicket(QString *path)
{
    QUrl url(QString(FAST_URL));
    url.setPath(QString(*path));
    QNetworkRequest request(url);
    manager->get(request);
}

void TicketPrinter::printData(const QByteArray *data)
{
    Poppler::Document *doc = Poppler::Document::loadFromData(*data);
    if (!doc) {
        emit error(ParseFailed);
        return;
    }

    QString printerName = static_cast<Application *>(qApp)->getSettings()->value(Application::printerNameSetting).toString();
    QPrinter printer(QPrinter::HighResolution);
    if (!printerName.isEmpty()) {
        printer.setPrinterName(printerName);
    }

    QSize ticketSize = doc->page(0)->pageSize();
    QSize paperSize = QSize(ticketSize.height(), ticketSize.width());
    printer.setPageSize(QPageSize(paperSize));
    printer.setOrientation(QPrinter::Landscape);
    printer.setFullPage(true);
    printer.setPageMargins(QMarginsF(0, 0, 0, 0));

    QPainter painter(&printer);
    for (int i = 0; i < doc->numPages(); i++) {
        Poppler::Page *page = doc->page(i);
        double newSize = page->pageSize().width() * 2;
        QImage image = page->renderToImage(newSize, newSize);
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
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray const data = reply->readAll();
        printData(&data);
    } else {
        emit error(DownloadFailed);
    }
}
