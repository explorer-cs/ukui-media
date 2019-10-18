#include "ukuiapplication.h"
#include <QKeyEvent>
#include <QDebug>

UkuiApplication::UkuiApplication(int &argc,char **argv):
    QApplication(argc,argv)
{
    widget = new UkmediaSystemTrayWidget;
    GsetWindowInstance(widget);
}

UkuiApplication::~UkuiApplication()
{
    delete widget;
}

void UkuiApplication::GsetWindowInstance(UkmediaSystemTrayWidget *wnd)
{
    widget = wnd;
}

bool UkuiApplication::notify(QObject *obj, QEvent *e)
{
    const QMetaObject* objMeta = obj->metaObject();
    bool ret;
    //qDebug() << activeWindow();
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
   if (e->type() == QEvent::MouseButtonPress) {

        if (mouseEvent->buttons() == Qt::RightButton && !widget->isHidden()){
            widget->hide();
        }
//        else if (mouseEvent->buttons() == Qt::MidButton && !widget->isHidden()) {
//            widget->hide();
//        }
    }

    return QApplication::notify(obj,e);
}
