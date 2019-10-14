#include <QBoxLayout>
#include <QMainWindow>
#include <QWindow>
#include <QSlider>
#include <QMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <string>
extern "C" {
#define MATE_DESKTOP_USE_UNSTABLE_API
#include <libmate-desktop/mate-desktop-utils.h>
#include <canberra.h>
#include <gdk/gdk.h>
#include <glib-object.h>
#include <glib/gi18n.h>
}
#include "ukuimediacontrolwidget.h"
gboolean isMute;

UkuiMediaControlWidget::UkuiMediaControlWidget(QWidget *parent) : QWidget (parent)
{
    QString str;
    guint ret;
    MateMixerStream *stream;
    GSettings *settings;
    int volumeRead;
    int voiceState;

    this->setFixedWidth(200);
    settings = g_settings_new(KEY_SOUNDS_SCHEMA);
    str = g_settings_get_string(settings,"default-mixer-device");
    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }
    this->ukuiContext = mate_mixer_context_new();
    mate_mixer_context_set_backend_type(ukuiContext,MATE_MIXER_BACKEND_ALSA);
    mate_mixer_context_set_app_name (this->ukuiContext,_("MATE Volume Control App"));//设置app名
    mate_mixer_context_set_app_id(this->ukuiContext, GVC_APPLET_DBUS_NAME);
    mate_mixer_context_set_app_version(this->ukuiContext,VERSION);
    mate_mixer_context_set_app_icon(this->ukuiContext,"multimedia-volume-control");

    if G_UNLIKELY (mate_mixer_context_open(this->ukuiContext) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }

    stream = mate_mixer_context_get_default_output_stream(this->ukuiContext);
    ukuiControl = mate_mixer_stream_get_default_control(stream);
    volumeRead = mate_mixer_stream_control_get_volume(ukuiControl);//获取音量值
    isMute = mate_mixer_stream_control_get_mute(ukuiControl);
    voiceState = mate_mixer_stream_control_get_mute(ukuiControl);//获取声音的状态

    //获取上一次退出状态的值

    m_displayVolumeValue = new QLabel;
    m_volumeSlider = new QSlider;

    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setOrientation(Qt::Horizontal);
    m_volumeSlider->setFixedSize(80,20);

    //设置样式表
    m_volumeSlider->setStyleSheet(
                "\
                QSlider::groove:horizontal {   \
                border: 0px solid #bbb;\
                }\
                QSlider::sub-page:horizontal \
                background: #356ab7;\
                border-radius: 0px;\
                margin-top:10px;\
                margin-bottom:10px;\
                }\
                QSlider::add-page:horizontal {\
                background:  #cccccc;\
                border: 0px solid #777;\
                border-radius: 2px;\
                  margin-top:10px;\
                margin-bottom:10px;\
                }\
                QSlider::handle:horizontal {\
                background:#356ab7;\
                width: 8px;\
                height:22px;\
                border-radius: 7px;\
                margin-top:0px;\
                margin-bottom:0px;\
                } "
     );

    if (!voiceState) {
        int volume;
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_displayVolumeValue->setNum(volume);
        m_volumeSlider->setValue(volume);
    }
    else {
        m_displayVolumeValue->setNum(0);
    }
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(ukuiControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect (ukuiControl,"notify::volume",
                          G_CALLBACK (onControlVolumeNotify),
                          this);

    connect(this,&UkuiMediaControlWidget::emitVolume,this,&UkuiMediaControlWidget::acceptVolume);
    //当音量条发生改变时
    connect(m_volumeSlider,&QSlider::valueChanged,this,[=](int value) {
        isMute = mate_mixer_stream_control_get_mute(ukuiControl);
        //连接到系统声音
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role(ukuiControl);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_MASTER) {
            if (!isMute) {
                mate_mixer_stream_control_set_volume(ukuiControl,value*65536/100);
                int volume = mate_mixer_stream_control_get_volume(ukuiControl);
            }
        }
        m_displayVolumeValue->setNum(value);
        m_volumeSlider->setValue(value);
        if (value > 0) {
            //muteButton->setIcon(QIcon(":/icon/voice.jpg"));
            mate_mixer_stream_control_set_mute(ukuiControl,FALSE);
        }
        else if (value <= 0) {
            //muteButton->setIcon(QIcon(":/icon/mute.jpg"));
            mate_mixer_stream_control_set_mute(ukuiControl,TRUE);
        }
    });

//    connect(muteButton,&QPushButton::pressed,slid,[=]() {
//        mute();
//   });
    dockWidgetInit();//水平布局
}

void UkuiMediaControlWidget::dockWidgetInit()
{
    QHBoxLayout *layout;
    QWidget *dockWidget;
    layout = new QHBoxLayout();
    dockWidget = new QWidget(this);
    //layout->addWidget(muteButton);
    layout->addWidget(m_volumeSlider);
    layout->addWidget(m_displayVolumeValue);
    dockWidget->setLayout(layout);
}

void UkuiMediaControlWidget::mute()
{
//    slid->setValue(0);
//    muteButton->setIcon(QIcon(":/icon/mute.jpg"));
    isMute = mate_mixer_stream_control_get_mute(ukuiControl);
    int volume = mate_mixer_stream_control_get_volume(ukuiControl);
    volume = volume*100/65536.0 + 0.5;

    if (isMute) {
        m_volumeSlider->setValue(volume);
        m_displayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_mute(ukuiControl,FALSE);
    }
    else {
        mate_mixer_stream_control_set_mute(ukuiControl,TRUE);
        //mute();
        m_volumeSlider->setValue(0);
       // muteButton->setIcon(QIcon(":/icon/mute.jpg"));
    }
}

void UkuiMediaControlWidget::QWhellEvent(QWheelEvent *event)
{
    guint wheelStep = 65536/100;//滚动大小为5
    guint volume = mate_mixer_stream_control_get_volume(ukuiControl);
    if (event->delta() >0 ) {
        volume = volume + wheelStep;
        qDebug() << "向上滚动";
    }
    else if (event->delta() < 0 ) {
       volume = volume - wheelStep;
       qDebug() << "向下滚动";
    }
    mate_mixer_stream_control_set_volume(ukuiControl,volume);
    event->accept();
}

void UkuiMediaControlWidget::onControlVolumeNotify(MateMixerStreamControl *ukuiControl, GParamSpec *pspec, UkuiMediaControlWidget *p)
{
    guint value = mate_mixer_stream_control_get_volume(ukuiControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(ukuiControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE)
             mate_mixer_stream_control_set_mute (ukuiControl, (value <= 0));
     if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
         guint volume ;
         volume = value*100/65536.+0.5;
         Q_EMIT p->emitVolume(volume);
     }
}

void UkuiMediaControlWidget::acceptVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

int UkuiMediaControlWidget::getVolume()
{
    return mate_mixer_stream_control_get_volume(ukuiControl);
}

void UkuiMediaControlWidget::setVolume(int volume)
{
    mate_mixer_stream_control_set_volume(ukuiControl,volume);
}

UkuiMediaControlWidget::~UkuiMediaControlWidget()
{

}
