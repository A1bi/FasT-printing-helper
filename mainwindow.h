#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ticketprinter.h"

namespace Ui {
    class MainWindow;
}

class QPrinterInfo;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setMissingPrinterMode(bool enabled);

private:
    Ui::MainWindow *ui;
    QSettings *settings;

private slots:
    void submit();

signals:
    void submitted();

};

#endif // MAINWINDOW_H
