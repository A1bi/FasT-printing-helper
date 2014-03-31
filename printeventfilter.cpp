#include "printeventfilter.h"
#include <QEvent>
#include <QFileOpenEvent>
#include <QDebug>

bool PrintEventFilter::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::FileOpen) {
        qDebug() << static_cast<QFileOpenEvent *>(event)->file();
    }

    return false;
}
