#include "ukuiapplication.h"
#include <QKeyEvent>
#include <QDebug>

UkuiApplication::UkuiApplication(int &argc,char **argv):
    QApplication(argc,argv)
{
    widget = new UkuiSystemTryWidget;
    GsetWindowInstance(widget);
}

UkuiApplication::~UkuiApplication()
{
    delete widget;
}

void UkuiApplication::GsetWindowInstance(UkuiSystemTryWidget *wnd)
{
    widget = wnd;
}

bool UkuiApplication::notify(QObject *obj, QEvent *e)
{
    const QMetaObject* objMeta = obj->metaObject();
    bool ret;
   if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);

        if (mouseEvent->buttons() == Qt::RightButton){
             widget->hide();
        }
    }

    return QApplication::notify(obj,e);
}
