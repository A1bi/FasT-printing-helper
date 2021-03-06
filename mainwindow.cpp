#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "application.h"
#include <QPrinterInfo>
#include <QDebug>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = static_cast<Application *>qApp->getSettings();

    connect(ui->submitBtn, SIGNAL(clicked()), this, SLOT(submit()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printers.size(); ++i) {
        QPrinterInfo printer = printers.at(i);
        ui->printerSelect->addItem(printer.description(), QVariant(printer.printerName()));
        if (settings->value(Application::printerNameSetting) == printer.printerName()) {
            ui->printerSelect->setCurrentIndex(i);
        }
    }

    ui->alignmentSelect->setCurrentIndex(settings->value(Application::alignmentSetting).toInt());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMissingPrinterMode(bool enabled)
{
    ui->submitBtn->setText(enabled ? tr("print") : tr("save"));
}

void MainWindow::submit()
{
    settings->setValue(Application::printerNameSetting, ui->printerSelect->currentData());
    settings->setValue(Application::alignmentSetting, ui->alignmentSelect->currentIndex());
    emit submitted();
}
