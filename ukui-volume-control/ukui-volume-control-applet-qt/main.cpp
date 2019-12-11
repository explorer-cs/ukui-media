#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include <QSharedMemory>
#include "ukmedia_systemtray_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    //加载qm翻译文件
    QString locale = QLocale::system().name();
    QTranslator translator;
//    translator.load("/home/fzx/fzx/ukui-media/ukui-media/ukui-volume-control/ukui-volume-control-applet-qt/translations/ukui-media-volume-control-applet-qt-zh_CN.qm");
//    a.installTranslator(&translator);
    //    QString qmFile = QString(TRANSLATIONS_DIR"/%1.qm").arg(locale);
    if (locale == "zh_CN") {
        if (translator.load("translations/ukui-media-volume-control-applet-qt-zh_CN.qm")) {
            a.installTranslator(&translator);
        }
        else {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }
//    qDebug() << locale;
    //加载qss文件
    QFile qss(":/data/qss/ukuimedia.qss");
    bool ok = qss.open(QFile::ReadOnly);
    if (!ok)
        qDebug() << "加载失败";
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    QSharedMemory shared("a");
    if (shared.attach()) {
        qDebug() << "ukui-volume-control-applet is running,exit!";
        return 0;
    }
    else {
    }
    shared.create(1);

    UkmediaSystemTrayWidget w;
    return a.exec();
}

