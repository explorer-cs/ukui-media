#include "ukmedia_application.h"
#include <QKeyEvent>
#include <QDebug>

UkuiApplication::UkuiApplication(int &argc,char **argv):
    QApplication(argc,argv)
{
    widget = new UkmediaSystemTrayWidget;
    GsetWindowInstance(widget);
    widget->hide();
}

UkuiApplication::~UkuiApplication()
{
    delete widget;
}

void UkuiApplication::GsetWindowInstance(UkmediaSystemTrayWidget *wnd)
{
    Q_UNUSED(wnd);
    //widget = wnd;
}

bool UkuiApplication::notify(QObject *obj, QEvent *e)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
   if (e->type() == QEvent::MouseButtonPress) {

        if (mouseEvent->buttons() == Qt::RightButton && !widget->isHidden()) {
            widget->hide();
        }
    }

    return QApplication::notify(obj,e);
}
