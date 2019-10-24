#include <QApplication>
#include "ukmedia_application.h"
#include <QDebug>
#include <QTranslator>
#include "ukmedia_systemtray_widget.h"



int main(int argc, char *argv[])
{
    //UkuiApplication a(argc,argv);
    QApplication a(argc,argv);
    QString locale = QLocale::system().name();
    QTranslator translator;
    QString qmFile = QString(TRANSLATIONS_DIR"/%1.qm").arg(locale);
    if (locale == "zh_CN") {

        if (translator.load(qmFile)) {
            a.installTranslator(&translator);
        }
        else {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }

    UkmediaSystemTrayWidget w;
    //    w.show();
    return a.exec();
}

