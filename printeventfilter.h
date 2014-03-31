#ifndef PRINTEVENTFILTER_H
#define PRINTEVENTFILTER_H

#include <QObject>

class QEvent;

class PrintEventFilter : public QObject
{
    Q_OBJECT
public:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // PRINTEVENTFILTER_H
