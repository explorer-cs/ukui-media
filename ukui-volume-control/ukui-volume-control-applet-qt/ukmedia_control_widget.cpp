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
#include "ukmedia_control_widget.h"
gboolean isMute;
gboolean ipIsMute;
UkmediaControlWidget::UkmediaControlWidget(QWidget *parent) : QWidget (parent)
{

//    GSettings *settings;
//    int volumeRead;
//    int voiceState;

    outputStream = new MateMixerStream;

    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup);
    this->setFixedWidth(180);
    //mate_mixer初始化
    mateMixerInit();
//    settings = g_settings_new(KEY_SOUNDS_SCHEMA);
//    if (mate_mixer_init() == FALSE) {
//        qDebug() << "libmatemixer initialization failed, exiting";
//    }
//    this->ukuiContext = mate_mixer_context_new();
//    mate_mixer_context_set_backend_type(ukuiContext,MATE_MIXER_BACKEND_ALSA);
//    mate_mixer_context_set_app_name (this->ukuiContext,_("Ukui Volume Control App"));//设置app名
//    mate_mixer_context_set_app_id(this->ukuiContext, GVC_APPLET_DBUS_NAME);
//    mate_mixer_context_set_app_version(this->ukuiContext,VERSION);
//    mate_mixer_context_set_app_icon(this->ukuiContext,"ukuimedia-volume-control");

//    if G_UNLIKELY (mate_mixer_context_open(this->ukuiContext) == FALSE) {
//        g_warning ("Failed to connect to a sound system**********************");
//    }





    //输出控件获取
//    outputStream = mate_mixer_context_get_default_output_stream(this->ukuiContext);
//    ukuiControl = mate_mixer_stream_get_default_control(outputStream);
//    volumeRead = mate_mixer_stream_control_get_volume(ukuiControl);//获取音量值
//    isMute = mate_mixer_stream_control_get_mute(ukuiControl);
//    voiceState = mate_mixer_stream_control_get_mute(ukuiControl);//获取声音的状态
    //初始化label的值
//    if (!voiceState) {
//        int volume;
//        volume = volumeRead * 100.0 / 65536.0 + 0.5;
//        m_displayVolumeValue->setNum(volume);
//        m_volumeSlider->setValue(volume);
//    }
//    else {
//        if (int value = mate_mixer_stream_control_get_volume(ukuiControl)) {
//            value = value*100/65536.0+0.5;
//            m_displayVolumeValue->setNum(value);
//        }
//        else {
//            m_displayVolumeValue->setNum(0);
//        }

//    }

//    //输出声音控制
//    ukmediaGetDefaultOutputStream();
//    //输出声音改变通知
//    outputVolumeNotify();
//    outputVolumeChanged();


//    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(ukuiControl);
//    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
//        g_signal_connect (ukuiControl,"notify::volume",G_CALLBACK (outputControlVolumeNotify),this);
//    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptVolume);
//    //当音量条发生改变时
//    connect(m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(outputVolumeSliderChanged(int)));


}

void UkmediaControlWidget::mateMixerInit()
{
    GSettings *settings;
    settings = g_settings_new(KEY_SOUNDS_SCHEMA);
    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }
    ukuiContext = mate_mixer_context_new();
    mate_mixer_context_set_backend_type(ukuiContext,MATE_MIXER_BACKEND_ALSA);
    mate_mixer_context_set_app_name (ukuiContext,_("Ukui Volume Control App"));//设置app名
    mate_mixer_context_set_app_id(ukuiContext, GVC_APPLET_DBUS_NAME);
    mate_mixer_context_set_app_version(ukuiContext,VERSION);
    mate_mixer_context_set_app_icon(ukuiContext,"ukuimedia-volume-control");

    if G_UNLIKELY (mate_mixer_context_open(ukuiContext) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }

}

void UkmediaControlWidget::ukmediaGetDefaultInputStream()
{
    int volumeRead;
    int voiceState;
    int volume;
    inputStream = mate_mixer_context_get_default_input_stream(ukuiContext);
    inputControl = mate_mixer_stream_get_default_control(inputStream);


    volumeRead = mate_mixer_stream_control_get_volume(inputControl);//获取音量值
    ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
    voiceState = mate_mixer_stream_control_get_mute(inputControl);//获取声音的状态
    //初始化label的值

    if (!voiceState) {
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_ipDisplayVolumeValue->setNum(volume);
        m_ipVolumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(inputControl)) {
            value = value*100/65536.0+0.5;
            m_ipDisplayVolumeValue->setNum(value);

        }
        else {
            m_ipDisplayVolumeValue->setNum(0);

        }

    }
    volume = m_ipVolumeSlider->value();
    if (volume <= 0) {
        QPixmap pixmap = QPixmap(":/images/emblem-unreadable.png");
        m_ipMuteIconLabel->setPixmap(pixmap);
    }

}

void UkmediaControlWidget::inputVolumeNotify()
{
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect (inputControl,"notify::volume",G_CALLBACK (inputControlVolumeNotify),this);
}

void UkmediaControlWidget::inputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptIpVolume);

    //当音量条发生改变时
    connect(m_ipVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(inputVolumeSliderChanged(int)));
}

void UkmediaControlWidget::ukmediaGetDefaultOutputStream()
{
    int volumeRead;
    int volume;
    int voiceState;
    outputStream = mate_mixer_context_get_default_output_stream(this->ukuiContext);
    outputControl = mate_mixer_stream_get_default_control(outputStream);
    volumeRead = mate_mixer_stream_control_get_volume(outputControl);//获取音量值
    isMute = mate_mixer_stream_control_get_mute(outputControl);
    voiceState = mate_mixer_stream_control_get_mute(outputControl);//获取声音的状态
    //初始化label的值
    if (!voiceState) {

        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_opDisplayVolumeValue->setNum(volume);
        m_opVolumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(outputControl)) {
            value = value*100/65536.0+0.5;
            m_opDisplayVolumeValue->setNum(value);
        }
        else {
            m_opDisplayVolumeValue->setNum(0);
        }

    }
    volume = m_opVolumeSlider->value();
    if (volume <= 0) {
        QPixmap pixmap = QPixmap(":/images/emblem-unreadable.png");
        m_opMuteIconLabel->setPixmap(pixmap);
    }

}

void UkmediaControlWidget::outputVolumeNotify()
{
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect (outputControl,"notify::volume",G_CALLBACK (outputControlVolumeNotify),this);

}

void UkmediaControlWidget::outputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptOpVolume);

    //当音量条发生改变时
    connect(m_opVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(outputVolumeSliderChanged(int)));
}

void UkmediaControlWidget::ipDockWidgetInit()
{
    m_ipDisplayVolumeValue = new QLabel(this);
    m_ipMuteIconLabel = new QLabel(this);
    m_ipVolumeSlider = new UkmediaSlider;

    m_ipMuteIconLabel->setVisible(true);
    m_ipVolumeSlider->setMaximum(100);
    m_ipVolumeSlider->setOrientation(Qt::Horizontal);
    m_ipVolumeSlider->setFixedSize(90,23);

    m_ipVolumeSlider->setSingleStep(10);
    //设置样式表
    m_ipVolumeSlider->setStyleSheet(
    "QSlider::groove:horizontal {"
        "border: 0px solid #bbb;"
    "}"
    "QSlider::sub-page:horizontal { "
         "background: #356ab7;"
         "border-radius: 0px;"
         "margin-top:10px;"
         "margin-bottom:10px;"
    "}"
    "QSlider::add-page:horizontal {"
         "background:  #cccccc;"
         "border: 0px solid #777;"
         "border-radius: 2px;"
         "margin-top:10px;"
         "margin-bottom:10px;"
    "}"
    "QSlider::handle:horizontal {"
        "background:#356ab7;"
        "width: 6px;"
        "height:16px;"
        "border-radius: 7px;"
        "margin-top:2px;"
        "margin-bottom:2px;"
        "}"
     );

    QHBoxLayout *layout;
    layout = new QHBoxLayout();
    this->setFixedWidth(180);
    layout->setSpacing(10);
    layout->addWidget(m_ipMuteIconLabel);
    layout->addWidget(m_ipVolumeSlider);
    layout->addWidget(m_ipDisplayVolumeValue);
    this->setLayout(layout);
}

void UkmediaControlWidget::opDockWidgetInit()
{
    m_opDisplayVolumeValue = new QLabel(this);
    m_opMuteIconLabel = new QLabel(this);
    m_opVolumeSlider = new UkmediaSlider;
    m_opMuteIconLabel->setVisible(true);
    m_opVolumeSlider->setMaximum(100);
    m_opVolumeSlider->setOrientation(Qt::Horizontal);
    m_opVolumeSlider->setFixedSize(90,23);

    m_opVolumeSlider->setSingleStep(10);
    //设置样式表
    m_opVolumeSlider->setStyleSheet(
    "QSlider::groove:horizontal {"
        "border: 0px solid #bbb;"
    "}"
    "QSlider::sub-page:horizontal { "
         "background: #356ab7;"
         "border-radius: 0px;"
         "margin-top:10px;"
         "margin-bottom:10px;"
    "}"
    "QSlider::add-page:horizontal {"
         "background:  #cccccc;"
         "border: 0px solid #777;"
         "border-radius: 2px;"
         "margin-top:10px;"
         "margin-bottom:10px;"
    "}"
    "QSlider::handle:horizontal {"
        "background:#356ab7;"
        "width: 6px;"
        "height:16px;"
        "border-radius: 7px;"
        "margin-top:2px;"
        "margin-bottom:2px;"
        "}"
     );

    QHBoxLayout *layout;
    layout = new QHBoxLayout();
    this->setFixedWidth(180);
    layout->setSpacing(10);
    layout->addWidget(m_opMuteIconLabel);
    layout->addWidget(m_opVolumeSlider);
    layout->addWidget(m_opDisplayVolumeValue);
    this->setLayout(layout);
}

void UkmediaControlWidget::ipMute()
{
    ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
    int volume = mate_mixer_stream_control_get_volume(inputControl);
    volume = volume*100/65536.0 + 0.5;
    if (ipIsMute) {
        m_ipVolumeSlider->setValue(volume);
        m_ipDisplayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_mute(inputControl,FALSE);
    }
    else {
        mate_mixer_stream_control_set_mute(inputControl,TRUE);
        m_ipVolumeSlider->setValue(0);
    }
}

void UkmediaControlWidget:: opMute()
{
    isMute = mate_mixer_stream_control_get_mute(outputControl);
    int volume = mate_mixer_stream_control_get_volume(outputControl);
    volume = volume*100/65536.0 + 0.5;
    if (isMute) {
        m_opVolumeSlider->setValue(volume);
        m_opDisplayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_mute(outputControl,FALSE);
    }
    else {
        mate_mixer_stream_control_set_mute(outputControl,TRUE);
        m_opVolumeSlider->setValue(0);
    }
}

void UkmediaControlWidget::inputControlVolumeNotify(MateMixerStreamControl *inputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);

    guint value = mate_mixer_stream_control_get_volume(inputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE)
        mate_mixer_stream_control_set_mute (inputControl, (value <= 0));
     if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
         guint volume ;
         volume = value*100/65536.0+0.5;
         Q_EMIT p->emitVolume(volume);
     }
}

void UkmediaControlWidget::outputControlVolumeNotify(MateMixerStreamControl *outputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);
    guint value = mate_mixer_stream_control_get_volume(outputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE)
             mate_mixer_stream_control_set_mute (outputControl, (value <= 0));
     if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
         guint volume ;
         volume = value*100/65536.0+0.5;
         Q_EMIT p->emitVolume(volume);
     }
}

void UkmediaControlWidget::acceptIpVolume(int volume)
{
    m_ipVolumeSlider->setValue(volume);
}

void UkmediaControlWidget::acceptOpVolume(int volume)
{
    m_opVolumeSlider->setValue(volume);
}

int UkmediaControlWidget::getIpVolume()
{
    return mate_mixer_stream_control_get_volume(inputControl);
}

int UkmediaControlWidget::getOpVolume()
{
    return mate_mixer_stream_control_get_volume(outputControl);
}

void UkmediaControlWidget::setIpVolume(int volume)
{
    mate_mixer_stream_control_set_volume(inputControl,volume);
}

void UkmediaControlWidget::setOpVolume(int volume)
{
    mate_mixer_stream_control_set_volume(outputControl,volume);
}

bool UkmediaControlWidget::getIpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(inputControl);
}

bool UkmediaControlWidget::getOpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(outputControl);
}

void UkmediaControlWidget::inputVolumeSliderChanged(int volume)
{
    int value;
    ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
    //连接到系统声音
    MateMixerStreamControlRole role = mate_mixer_stream_control_get_role(inputControl);
    if (role == MATE_MIXER_STREAM_CONTROL_ROLE_MASTER) {
        if (!ipIsMute) {
            mate_mixer_stream_control_set_volume(inputControl,volume*65536/100);
            value = mate_mixer_stream_control_get_volume(inputControl);
        }
    }
    if (volume > 0) {
        mate_mixer_stream_control_set_mute(inputControl,FALSE);
        QPixmap pixmap = QPixmap(":/images/blank.png");
        m_ipMuteIconLabel->setPixmap(pixmap);
        m_ipDisplayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_volume(inputControl,volume*65536/100);
    }
    else if (volume <= 0) {
        QPixmap pixmap = QPixmap(":/images/emblem-unreadable.png");
        m_ipMuteIconLabel->setPixmap(pixmap);
        mate_mixer_stream_control_set_mute(inputControl,TRUE);
        value = mate_mixer_stream_control_get_volume(inputControl);
        if(value) {
            value = value*100/65536.0 + 0.5;
            m_ipDisplayVolumeValue->setNum(value);

        }
        else {
            m_ipDisplayVolumeValue->setNum(0);
        }

    }
}

void UkmediaControlWidget::outputVolumeSliderChanged(int volume)
{
    int value;
    isMute = mate_mixer_stream_control_get_mute(outputControl);
    //连接到系统声音
    MateMixerStreamControlRole role = mate_mixer_stream_control_get_role(outputControl);
    if (role == MATE_MIXER_STREAM_CONTROL_ROLE_MASTER) {
        if (!isMute) {
            mate_mixer_stream_control_set_volume(outputControl,volume*65536/100);
            value = mate_mixer_stream_control_get_volume(outputControl);
        }
    }
    if (volume > 0) {
        mate_mixer_stream_control_set_mute(outputControl,FALSE);
        QPixmap pixmap = QPixmap(":/images/blank.png");
        m_opMuteIconLabel->setPixmap(pixmap);
        m_opDisplayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_volume(outputControl,volume*65536/100);
    }
    else if (volume <= 0) {
        QPixmap pixmap = QPixmap(":/images/emblem-unreadable.png");
        m_opMuteIconLabel->setPixmap(pixmap);
        mate_mixer_stream_control_set_mute(outputControl,TRUE);
        value = mate_mixer_stream_control_get_volume(outputControl);
        if(value) {
            value = value*100/65536.0 + 0.5;
            m_opDisplayVolumeValue->setNum(value);

        }
        else {
            m_opDisplayVolumeValue->setNum(0);
        }

    }
}

bool UkmediaControlWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if(QApplication::activeWindow() != this) {
            hide();
        }
    }

    return QWidget::event(event);
}

UkmediaSlider::UkmediaSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaSlider::~UkmediaSlider()
{

}

UkmediaControlWidget::~UkmediaControlWidget()
{

}


