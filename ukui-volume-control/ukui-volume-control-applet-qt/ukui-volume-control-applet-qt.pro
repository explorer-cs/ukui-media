######################################################################
# Automatically generated by qmake (3.1) Fri Oct 11 17:35:00 2019
######################################################################

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = ukui-media-volume-control-applet-qt
INCLUDEPATH += .

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
target.path = /usr/bin
INSTALLS += target

CONFIG += \
    c++11 \
    no_keywords link_pkgconfig \

PKGCONFIG += \
    gtk+-3.0 \
    glib-2.0 \
    libmatemixer \
    mate-desktop-2.0 \
    unique-1.0 \
    libcanberra \
# Input

HEADERS += \
    ukmedia_application.h \
    ukmedia_control_widget.h \
    ukmedia_systemtray_widget.h



SOURCES += \
    ukmedia_application.cpp \
    ukmedia_control_widget.cpp \
    ukmedia_systemtray_widget.cpp\
    main.cpp

RESOURCES += \
    ../../data/images/images.qrc


TRANSLATIONS += \
    ../../po/zh_CN.ts
