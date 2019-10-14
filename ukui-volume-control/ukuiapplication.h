#ifndef UKUIAPPLICATION_H
#define UKUIAPPLICATION_H

#include <QApplication>
#include "ukuisystemtrywidget.h"

class UkuiApplication : public QApplication
{
public:
    UkuiApplication(int&argc,char **argv);
    ~UkuiApplication();
    bool notify(QObject*, QEvent *);
    void GsetWindowInstance(UkuiSystemTryWidget *wnd);

private:
    UkuiSystemTryWidget *widget;
};

#endif // UKUIAPPLICATION_H
