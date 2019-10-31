#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include "ukmedia_systemtray_widget.h"



int main(int argc, char *argv[])
{
    QApplication a(argc,argv);

    //加载qm翻译文件
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

    //加载qss文件
    QFile qss(":/data/qss/ukuimedia.qss");
    bool ok = qss.open(QFile::ReadOnly);
    if (!ok)
        qDebug() << "加载失败";
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    UkmediaSystemTrayWidget w;
    return a.exec();
}

