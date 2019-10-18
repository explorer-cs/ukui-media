#ifndef UKMEDIA_APPLICATION_H
#define UKMEIDA_APPLICATION_H

#include <QApplication>
#include "ukmedia_systemtray_widget.h"

class UkuiApplication : public QApplication
{
public:
    UkuiApplication(int&argc,char **argv);
    ~UkuiApplication();
    bool notify(QObject*, QEvent *);
    void GsetWindowInstance(UkmediaSystemTrayWidget *wnd);

private:
    UkmediaSystemTrayWidget *widget;
};

#endif // UKUIAPPLICATION_H
