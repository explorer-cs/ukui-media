#ifndef UKUIMEDIACONTROLWIDGET_H
#define UKUIMEDIACONTROLWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QWidget>
#include <QWheelEvent>
extern "C" {

#include <libmatemixer/matemixer.h>
#include <gtk/gtk.h>
#include <canberra-gtk.h>

}
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_APPLET_DBUS_NAME    "org.mate.VolumeControlApplet"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"

class UkuiMediaControlWidget : public QWidget
{
    Q_OBJECT

public:
    UkuiMediaControlWidget(QWidget *parent = nullptr);
    ~UkuiMediaControlWidget();
    void dockWidgetInit();
    void mute();
    void setFocus();
    void scrollUp();
    void scrollDown();
    static void onControlVolumeNotify(MateMixerStreamControl *control, GParamSpec *pspec, UkuiMediaControlWidget *p);
    int getVolume();
    void setVolume(int volume);
private:
    //QPushButton *muteButton;
    QLabel *m_displayVolumeValue;
    QSlider *m_volumeSlider;
    MateMixerContext *ukuiContext;
    MateMixerStreamControl *ukuiControl;

Q_SIGNALS:
    void valueChangedSignal(int);
    void emitVolume(int);

public Q_SLOTS:
    void addVolume();
    void subVolume();
    void acceptVolume(int);

protected:
    virtual void QWhellEvent(QWheelEvent *event);

};


class UkuiSlider : public QSlider
{

public:
    UkuiSlider(QWidget *parent = 0) : QSlider(parent)
    {
    }

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
    }

};

#endif // UKUIMEDIACONTROLWIDGET_H
