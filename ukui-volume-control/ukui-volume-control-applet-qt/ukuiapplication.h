#ifndef UKUIAPPLICATION_H
#define UKUIAPPLICATION_H

#include <QApplication>
#include "ukuisystemtraywidget.h"

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
