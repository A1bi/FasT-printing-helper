#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class MainWindow;
class QTimer;

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int&, char**);
    ~Application();

private:
    MainWindow *window;
    QTimer *windowTimer;

    bool eventFilter(QObject*, QEvent*);

private slots:
    void showWindow();
};

#endif // APPLICATION_H
