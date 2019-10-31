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
    outputStream = new MateMixerStream;
    inputStream = new MateMixerStream;

    mateMixerInit();
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup);
    this->setFixedSize(300,80);
}

/*
    mate初始化设置
*/
void UkmediaControlWidget::mateMixerInit()
{
//    GSettings *settings;
//    settings = g_settings_new(KEY_SOUNDS_SCHEMA);
//    Q_UNUSED(settings);

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

/*
    设置麦克风值及托盘图标
*/
void UkmediaControlWidget::setIpSystemTrayIconVolume()
{
    ipIsMute = getIpMuteStatus();
    int volume = getIpVolume();

    volume = int(volume*100/65536.0+0.5);
    if(!ipIsMute) {
        m_volumeSlider->setValue(volume);
    }
    else {
        muteWidget(volume,ipIsMute);
    }
}

/*
    设置声音值及托盘图标
*/
void UkmediaControlWidget::setOpSystemTrayIconVolume()
{
    isMute = getOpMuteStatus();
    int volume = getOpVolume();
    volume = int(volume*100/65536.0+0.5);
    if (!isMute) {
        m_volumeSlider->setValue(volume);
    }
    else {
        muteWidget(volume,isMute);
    }
}

/*
    弹出窗口静音设置
*/
void UkmediaControlWidget::muteWidget(int volume,bool status)
{
    QString muteButtonIcon;
    QIcon icon;
    QIcon::setThemeName("ukui-icon-theme");
    switch (trayIconType) {
        case SYSTEMTRAYICON_MICROPHONE: {
            m_volumeSlider->setValue(volume);
            m_displayVolumeValue->setNum(volume);
            mate_mixer_stream_control_set_mute(inputControl,status);
            muteButtonIcon = "audio-input-microphone-muted";
            icon = QIcon::fromTheme(muteButtonIcon);
            m_muteButton->setIcon(icon);
            break;
        }
        case SYSTEMTRAYICON_VOLUME: {
            m_volumeSlider->setValue(volume);
            m_displayVolumeValue->setNum(volume);
            mate_mixer_stream_control_set_mute(outputControl,status);
            muteButtonIcon = "audio-volume-muted";
            icon = QIcon::fromTheme(muteButtonIcon);
            m_muteButton->setIcon(icon);
            break;
        }
        default:
            break;
    }
}

/*
    获取默认输入流及设置图标
*/
void UkmediaControlWidget::ukmediaGetDefaultInputStream()
{
    int volumeRead;
    int voiceState;
    int volume;

    volumeRead = mate_mixer_stream_control_get_volume(inputControl);//获取音量值
    ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
    voiceState = mate_mixer_stream_control_get_mute(inputControl);//获取声音的状态
    //初始化label的值

    if (!voiceState) {
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_displayVolumeValue->setNum(volume);
        m_volumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(inputControl)) {
            value = value*100/65536.0+0.5;
            m_displayVolumeValue->setNum(value);
        }
        else {
            m_displayVolumeValue->setNum(0);
        }

    }
    volume = m_volumeSlider->value();
    setIpMuteButtonIcon(volume);
}

/*
    麦克风声音通知更改麦克风的值
*/
void UkmediaControlWidget::inputVolumeNotify()
{
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect (inputControl,"notify::volume",G_CALLBACK (inputControlVolumeNotify),this);
    connect(this,SIGNAL(micMixerMuteSignal(bool,guint)),this,SLOT(acceptIpMixerMute(bool,guint)));
}

/*
    麦克风值改变
*/
void UkmediaControlWidget::inputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptIpVolume);
}

/*
    获取默认输出流
*/
void UkmediaControlWidget::ukmediaGetDefaultOutputStream()
{
    int volumeRead;
    int volume;
    int voiceState;

    volumeRead = mate_mixer_stream_control_get_volume(outputControl);//获取音量值
    isMute = mate_mixer_stream_control_get_mute(outputControl);
    voiceState = mate_mixer_stream_control_get_mute(outputControl);//获取声音的状态
    //初始化label的值
    if (!voiceState) {
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_displayVolumeValue->setNum(volume);
        m_volumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(outputControl)) {
            value = value*100/65536.0+0.5;
            m_displayVolumeValue->setNum(value);
        }
        else {
            m_displayVolumeValue->setNum(0);
        }
    }
    volume = m_volumeSlider->value();
    setOpMuteButtonIcon(volume);
}

/*
    声音通知
*/
void UkmediaControlWidget::outputVolumeNotify()
{
    bool isMute = mate_mixer_stream_control_get_mute(outputControl);

    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect(outputControl,"notify::volume",G_CALLBACK (outputControlVolumeNotify),this);
    connect(this,SIGNAL(soundMixerMuteSignal(bool,guint)),this,SLOT(acceptOpMixerMute(bool,guint)));
}

/*
    输出声音值改变
*/
void UkmediaControlWidget::outputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptOpVolume);
}

/*
    弹出窗口初始化
*/
void UkmediaControlWidget::dockWidgetInit()
{
    QFont font("",16,75);
    m_displaySpeakerLabel = new QLabel(this);
    m_displayVolumeValue = new QLabel(this);
    m_muteButton = new QPushButton(this);
    m_volumeSlider = new UkmediaSlider(this);

    m_muteButton->setFlat(true);
    m_displaySpeakerLabel->setText(tr("Speaker(Realtek(R) Audio)"));

    m_displayVolumeValue->setObjectName("displayVolumeLabel");
    m_displaySpeakerLabel->setObjectName("displayDescriptionLabel");

    m_muteButton->setFixedSize(16,16);
    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setOrientation(Qt::Horizontal);
    m_volumeSlider->setFixedSize(180,24);
    m_volumeSlider->setSingleStep(10);

    //弹出框的控件布局
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    hLayout = new QHBoxLayout();
    this->setFixedWidth(300);
    hLayout->setSpacing(15);
    hLayout->addWidget(m_muteButton);
    hLayout->addWidget(m_volumeSlider);
    hLayout->addWidget(m_displayVolumeValue);

    outputStream = mate_mixer_context_get_default_output_stream(this->ukuiContext);
    outputControl = mate_mixer_stream_get_default_control(outputStream);

    inputStream = mate_mixer_context_get_default_input_stream(ukuiContext);
    inputControl = mate_mixer_stream_get_default_control(inputStream);

    vLayout->addWidget(m_displaySpeakerLabel);
    vLayout->addLayout(hLayout);
    this->setLayout(vLayout);
    //当滑动条条发生改变时改变音量
    connect(m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(volumeSliderChanged(int)));
    //静音按钮设置静音
    connect(m_muteButton,SIGNAL(clicked()),this,SLOT(muteButtonClicked()));
}

/*
    ukui-media静音功能
*/
void UkmediaControlWidget:: mute()
{
    QString muteButtonIcon;
    QIcon icon;
    QIcon::setThemeName("ukui-icon-theme");
    switch (trayIconType) {
        case SYSTEMTRAYICON_MICROPHONE: {
            ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
            int ipVolume = mate_mixer_stream_control_get_volume(inputControl);
            ipVolume = ipVolume*100/65536.0 + 0.5;
            if (ipIsMute) {
                m_displayVolumeValue->setNum(ipVolume);
                mate_mixer_stream_control_set_mute(inputControl,FALSE);
                setIpMuteButtonIcon(ipVolume);
            }
            else {
                mate_mixer_stream_control_set_mute(inputControl,TRUE);
                muteButtonIcon = "audio-input-microphone-muted";
                icon = QIcon::fromTheme(muteButtonIcon);
                m_muteButton->setIcon(icon);
            }
            break;
        }

        case SYSTEMTRAYICON_VOLUME: {
            isMute = mate_mixer_stream_control_get_mute(outputControl);
            int opVolume = mate_mixer_stream_control_get_volume(outputControl);
            opVolume = opVolume*100/65536.0 + 0.5;
            if (isMute) {
                m_displayVolumeValue->setNum(opVolume);
                mate_mixer_stream_control_set_mute(outputControl,FALSE);
                setOpMuteButtonIcon(opVolume);
            }
            else {
                mate_mixer_stream_control_set_mute(outputControl,TRUE);
                muteButtonIcon = "audio-volume-muted";
                icon = QIcon::fromTheme(muteButtonIcon);
                m_muteButton->setIcon(icon);
            }
            break;
        }
        default:
        break;
    }
}

/*
    麦克风声音通知
*/
void UkmediaControlWidget::inputControlVolumeNotify(MateMixerStreamControl *inputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);
    trayIconType = SYSTEMTRAYICON_MICROPHONE;
    guint value = mate_mixer_stream_control_get_volume(inputControl);
    guint volume = guint(value*100/65536.0+0.5);
    bool isMute = mate_mixer_stream_control_get_mute(inputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE ) {
        Q_EMIT p->micMixerMuteSignal(isMute,volume);
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
     Q_EMIT p->emitVolume(volume);
    }
}

/*
    输出声音通知
*/
void UkmediaControlWidget::outputControlVolumeNotify(MateMixerStreamControl *outputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);
    trayIconType = SYSTEMTRAYICON_VOLUME;
    guint value = mate_mixer_stream_control_get_volume(outputControl);
    guint volume ;
    volume = guint(value*100/65536.0+0.5);
    bool isMute = mate_mixer_stream_control_get_mute(outputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    //静音可读并且处于静音
    if ((control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE) ) {
        Q_EMIT p->soundMixerMuteSignal(isMute,volume);
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
        Q_EMIT p->emitVolume(volume);
    }
}

/*
    接收麦克风静音
*/
void UkmediaControlWidget::acceptIpMixerMute(bool status,guint volume)
{
    if (status) {
        trayIconType = SYSTEMTRAYICON_MICROPHONE;
        mate_mixer_stream_control_set_mute(inputControl,status);
        muteWidget(int(volume),status);
    }
    else {
        mate_mixer_stream_control_set_mute(inputControl,status);
        setIpMuteButtonIcon(volume);
    }
    Q_EMIT updateSystemTrayIconSignal(volume,trayIconType,status);
}

/*
    接收输出静音
*/
void UkmediaControlWidget::acceptOpMixerMute(bool status,guint volume)
{
    if (status) {
        trayIconType = SYSTEMTRAYICON_VOLUME;
        mate_mixer_stream_control_set_mute(outputControl,status);
        muteWidget(int(volume),status);
    }
    else {
        mate_mixer_stream_control_set_mute(outputControl,status);
        setOpMuteButtonIcon(volume);
    }
    Q_EMIT updateSystemTrayIconSignal(volume,trayIconType,status);
}

/*
    设置输入滑动条的值
*/
void UkmediaControlWidget::acceptIpVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

/*
    设置输出滑动条的值
*/
void UkmediaControlWidget::acceptOpVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

/*
    获取麦克风声音的值
*/
int UkmediaControlWidget::getIpVolume()
{
    return mate_mixer_stream_control_get_volume(inputControl);
}

/*
    获取输出声音的值
*/
int UkmediaControlWidget::getOpVolume()
{
    return mate_mixer_stream_control_get_volume(outputControl);
}

/*
    设置麦克风声音
*/
void UkmediaControlWidget::setIpVolume(int volume)
{
    mate_mixer_stream_control_set_volume(inputControl,volume);
}

/*
    设置输出声音
*/
void UkmediaControlWidget::setOpVolume(int volume)
{
    mate_mixer_stream_control_set_volume(outputControl,volume);
}

/*
    获取麦克风静音状态
*/
bool UkmediaControlWidget::getIpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(inputControl);
}

/*
    获取输出声音静音状态
*/
bool UkmediaControlWidget::getOpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(outputControl);
}

/*
    移动滑动条改变输入输出声音的值
*/
void UkmediaControlWidget::volumeSliderChanged(int volume)
{
    switch (trayIconType) {
        //麦克风托盘图标
        case SYSTEMTRAYICON_MICROPHONE: {
            if (volume > 0) {
                mate_mixer_stream_control_set_mute(inputControl,FALSE);
                m_displayVolumeValue->setNum(volume);
                mate_mixer_stream_control_set_volume(inputControl,volume*65536/100);
            }
            else if (volume <= 0) {
                mate_mixer_stream_control_set_mute(inputControl,TRUE);
                mate_mixer_stream_control_set_volume(inputControl,0);
                m_displayVolumeValue->setNum(0);
            }
            Q_EMIT sliderSystemTrayIcon(trayIconType);
            setIpMuteButtonIcon(volume);
            break;
        }
        //输出声音托盘图标
        case SYSTEMTRAYICON_VOLUME: {
            if (volume > 0) {
                mate_mixer_stream_control_set_mute(outputControl,FALSE);
                m_displayVolumeValue->setNum(volume);
                mate_mixer_stream_control_set_volume(outputControl,volume*65536/100);
            }
            else if (volume <= 0) {
                mate_mixer_stream_control_set_mute(outputControl,TRUE);
                mate_mixer_stream_control_set_volume(outputControl,0);
                m_displayVolumeValue->setNum(0);
            }
            Q_EMIT sliderSystemTrayIcon(trayIconType);
            setOpMuteButtonIcon(volume);
            break;
        }
        default:
        break;
    }
}

/*
    点击窗口以外的部分隐藏窗口
*/
bool UkmediaControlWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if(QApplication::activeWindow() != this) {
            hide();
        }
    }
    return QWidget::event(event);
}

/*
    点击时设置托盘图标类型为麦克风
*/
void UkmediaControlWidget::acceptIpSystemTrayIconTriggered(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    滚动时设置托盘图标类型为麦克风
*/
void UkmediaControlWidget::acceptIpSystemTrayIconRoll(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    滚动时设置托盘图标类型为声音
*/
void UkmediaControlWidget::acceptOpSystemTrayIconRoll(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    点击时设置托盘图标类型为声音
*/
void UkmediaControlWidget::acceptOpSystemTrayIconTriggered(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    点击静音按钮
*/
void UkmediaControlWidget::muteButtonClicked()
{
    switch (trayIconType) {
        case SYSTEMTRAYICON_MICROPHONE : {
            mute();
        break;
        }
        case SYSTEMTRAYICON_VOLUME:{
            mute();
        break;
        }
    }
}

/*
    设置麦克风静音按钮图标
*/
void UkmediaControlWidget::setIpMuteButtonIcon(int volume)
{
    QString inputMuteButtonIcon;
    QIcon icon;
    QIcon::setThemeName("ukui-icon-theme");
    if (volume <= 0) {
        inputMuteButtonIcon = "audio-input-microphone-muted";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 0 && volume <= 33) {
        inputMuteButtonIcon = "audio-input-microphone-low";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 33 && volume <= 66) {
        inputMuteButtonIcon = "audio-input-microphone-medium";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else {
        inputMuteButtonIcon = "audio-input-microphone-high";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
}

/*
    设置输出声音静音按钮图标
*/
void UkmediaControlWidget::setOpMuteButtonIcon(int volume)
{
    QString outputMuteButtonIcon;
    QIcon icon;
    QIcon::setThemeName("ukui-icon-theme");
    if (volume <= 0) {
        outputMuteButtonIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 0 && volume <= 33) {
        outputMuteButtonIcon = "audio-volume-low";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 33 && volume <= 66) {
        outputMuteButtonIcon = "audio-volume-medium";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else {
        outputMuteButtonIcon = "audio-volume-high";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
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
