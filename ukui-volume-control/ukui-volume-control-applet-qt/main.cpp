#include <QApplication>
#include <QMouseEvent>
#include "ukmedia_application.h"
#include <QDebug>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <libintl.h>
#include <gio/gio.h>
#include <libmatemixer/matemixer.h>


int main(int argc, char *argv[])
{
    UkuiApplication a(argc,argv);
    //QApplication a(argc,argv);
    //UkuiMediaControlWidget w;
    //setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    return a.exec();
}
