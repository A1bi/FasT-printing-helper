#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPrinterInfo>
#include <QDebug>
#include <QSettings>

const QString MainWindow::printerNameSetting = "printerName";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new QSettings;

    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printers.size(); ++i) {
        QPrinterInfo printer = printers.at(i);
        ui->printerSelect->addItem(printer.description(), QVariant(printer.printerName()));
        if (settings->value(printerNameSetting) == printer.printerName()) {
            ui->printerSelect->setCurrentIndex(i);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::save()
{
    settings->setValue(printerNameSetting, ui->printerSelect->currentData());
    close();
}
