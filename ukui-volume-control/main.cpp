#include <QApplication>
#include <QMouseEvent>
#include "ukuiapplication.h"
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
    return a.exec();
}
