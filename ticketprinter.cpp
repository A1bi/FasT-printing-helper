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

    QSettings *settings = static_cast<Application *>(qApp)->getSettings();
    QString printerName = settings->value(Application::printerNameSetting).toString();
    int alignment = settings->value(Application::alignmentSetting, QVariant(0)).toInt();
    QPrinter printer(QPrinter::HighResolution);
    if (!printerName.isEmpty()) {
        printer.setPrinterName(printerName);
    }

    if (!alignment) {
        QSize ticketSize = doc->page(0)->pageSize();
        QSize paperSize = QSize(ticketSize.height(), ticketSize.width());
        printer.setPageSize(QPageSize(paperSize));
    }

    printer.setOrientation(QPrinter::Landscape);
    printer.setFullPage(true);
    printer.setPageMargins(QMarginsF(0, 0, 0, 0));

    QPainter painter(&printer);
    QRect paperRect = painter.viewport();

    for (int i = 0; i < doc->numPages(); i++) {
        Poppler::Page *page = doc->page(i);
        QImage image = page->renderToImage(printer.resolution(), printer.resolution());
        int pos = 0;
        switch (alignment) {
        case 2:
            pos = paperRect.height() / 2 - image.size().height() / 2;
            break;
        case 3:
            pos = paperRect.height() - image.size().height();
            break;
        }
        painter.drawImage(0, pos, image);
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
