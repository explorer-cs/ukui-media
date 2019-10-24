#ifndef UKMEDIA_CONTROL_WIDGET_H
#define UKMEDIA_CONTROL_WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QCoreApplication>
#include <QApplication>
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

class UkmediaSlider : public QSlider
{

public:
    UkmediaSlider(QWidget *parent = nullptr);
    ~UkmediaSlider();

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

class UkmediaControlWidget : public QWidget
{
    Q_OBJECT

public:
    UkmediaControlWidget(QWidget *parent = nullptr);
    ~UkmediaControlWidget();
    void opDockWidgetInit();
    void ipDockWidgetInit();
    void opMute();
    void ipMute();
    void setFocus();
    void scrollUp();
    void scrollDown();
    static void outputControlVolumeNotify(MateMixerStreamControl *control, GParamSpec *pspec, UkmediaControlWidget *p);
    static void inputControlVolumeNotify(MateMixerStreamControl *control, GParamSpec *pspec, UkmediaControlWidget *p);

    int getIpVolume();
    void setIpVolume(int volume);
    bool getIpMuteStatus();

    int getOpVolume();
    void setOpVolume(int volume);
    bool getOpMuteStatus();

    void mateMixerInit();
    //声音输出托盘栏设置
    void ukmediaGetDefaultOutputStream();
    void outputVolumeNotify();
    void outputVolumeChanged();

    //麦克风托盘栏设置
    void ukmediaGetDefaultInputStream();
    void inputVolumeNotify();
    void inputVolumeChanged();

    friend class UkmediaSystemTrayWidget ;
    friend class UkmediaSystemTrayIcon;
private:
    QLabel *m_opMuteIconLabel;
    QLabel *m_opDisplayVolumeValue;
    UkmediaSlider *m_opVolumeSlider;
    QLabel *m_ipMuteIconLabel;
    QLabel *m_ipDisplayVolumeValue;
    UkmediaSlider *m_ipVolumeSlider;
    MateMixerContext *ukuiContext;
    MateMixerStreamControl *outputControl;
    MateMixerStreamControl *inputControl;
    MateMixerStream *outputStream;
    MateMixerStream *inputStream;

Q_SIGNALS:
    void valueChangedSignal(int);
    void emitVolume(int);

public Q_SLOTS:
    void acceptOpVolume(int);
    void acceptIpVolume(int);
    void outputVolumeSliderChanged(int volume);
    void inputVolumeSliderChanged(int volume);

protected:
    bool event(QEvent *event);//重写窗口事件
};

#endif // UKMEDIA_CONTROL_WIDGET_H
