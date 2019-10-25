#include <QDesktopWidget>
#include "ukmedia_systemtray_widget.h"
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QMenu>
#include <QPalette>
#include <QLayout>
#include <QGridLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QSize>
#include <QAction>
#include <QMainWindow>
#include <QWindow>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>
#include <QWheelEvent>
#include <QMetaObject>
#include <QProcess>
#include <QScreen>
#include <QDebug>

int count = 0;

UkmediaSystemTrayIcon::UkmediaSystemTrayIcon(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaSystemTrayIcon::~UkmediaSystemTrayIcon()
{

}

UkmediaSystemTrayWidget::UkmediaSystemTrayWidget(QWidget *parent)
    : QMainWindow (parent)
{
    int opVolume,ipVolume;
    bool opStatus,ipStatus;

    inputSystemTrayMenuInit();
    outputSystemTrayMenuInit();
    inputWidget->ipDockWidgetInit();
    inputWidget->ukmediaGetDefaultInputStream();
    ipVolume = inputWidget->getIpVolume();
    ipStatus = inputWidget->getIpMuteStatus();

    inputSystemTrayIconInit(ipVolume,ipStatus);
    inputWidget->inputVolumeNotify();
    inputWidget->inputVolumeChanged();

    //输出声音控制 输出托盘栏设置
    outputWidget->opDockWidgetInit();
    outputWidget->ukmediaGetDefaultOutputStream();
    opVolume = outputWidget->getOpVolume();
    opStatus = outputWidget->getOpMuteStatus();
    outputSystemTrayIconInit(opVolume,opStatus);
    //输出声音改变通知
    outputWidget->outputVolumeNotify();
    outputWidget->outputVolumeChanged();

    //隐藏任务栏图标
    //setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup);
    //在构造中设置窗体的MouseTracking属性
    this->setMouseTracking(true);
    outputWidget->setMouseTracking(true);

    //点击托盘栏声音图标显示音量条
    connect(outputSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason)));
    connect(inputSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason)));
    //设置获取焦点事件
    setFocusPolicy(Qt::ClickFocus);
    //设置静音功能
    connect(outputActionMute,SIGNAL(triggered(bool)),this,SLOT(outputActionMuteTriggered(bool)));
    connect(outputSystemTray,SIGNAL(wheelRollEventSignal(bool)),this,SLOT(acceptOpWheelRollEvent(bool)));

    connect(inputActionMute,SIGNAL(triggered(bool)),this,SLOT(inputActionMuteTriggered(bool)));
    connect(inputSystemTray,SIGNAL(wheelRollEventSignal(bool)),this,SLOT(acceptIpWheelRollEvent(bool)));
    //更改系统托盘图标
    connect(outputWidget->m_opVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(slidChangeOpSystemTrayIcon(int)));
    connect(inputWidget->m_ipVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(slidChangeIpSystemTrayIcon(int)));

    connect(outputWidget->m_opMuteButton,SIGNAL(clicked()),this,SLOT(buttonChangeOpSystemTrayIcon()));
    connect(inputWidget->m_ipMuteButton,SIGNAL(clicked()),this,SLOT(buttonChangeIpSystemTrayIcon()));
    //连接声音首选项
    connect(outputActionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
    connect(inputActionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
    //设置中心窗口
    this->setCentralWidget(outputWidget);
    resize(160,41);
}

void UkmediaSystemTrayWidget::inputSystemTrayMenuInit()
{
    inputSystemTray = new UkmediaSystemTrayIcon(this);
    inputWidget = new UkmediaControlWidget();

    //麦克风添加右击菜单
    inputActionMute = new QAction(this);
    inputActionMute->setCheckable(true);
    inputActionMute->setText(tr("Mute(M)"));

    inputSystemTray->setToolTip((tr("Input volume control")));

    inputActionSoundPreference = new QAction(this);
    inputActionSoundPreference->setIcon(QIcon(":/images/application-audio.png"));
    inputActionSoundPreference->setText(tr("Sound preference(S)"));

    inputMenu = new QMenu(this);
    inputMenu->addAction(inputActionMute);
    inputMenu->addAction(inputActionSoundPreference);
    inputMenu->setFixedWidth(140);

    inputMenu->setStyleSheet(
    "QMenu::item {"
         "padding: 4px 25px 4px 20px;"
         "border: 1px solid transparent;" /* reserve space for selection border */
    "}"
    );

    inputSystemTray->setContextMenu(inputMenu);
}

void UkmediaSystemTrayWidget::outputSystemTrayMenuInit()
{
    outputSystemTray = new UkmediaSystemTrayIcon(this);
    outputWidget = new UkmediaControlWidget();

    //为系统托盘图标添加右击菜单
    outputActionMute = new QAction(this);
    outputActionMute->setCheckable(true);
    outputActionMute->setText(tr("Mute(M)"));

    outputSystemTray->setToolTip(tr("Output volume control"));

    //sound preference
    outputActionSoundPreference = new QAction(this);
    outputActionSoundPreference->setIcon(QIcon(":/images/application-audio.png"));
    outputActionSoundPreference->setText(tr("Sound preference(S)"));

    //设置右键菜单
    outputMenu = new QMenu(this);
    outputMenu->addAction(outputActionMute);
    outputMenu->addAction(outputActionSoundPreference);
    outputMenu->setFixedWidth(140);

    outputMenu->setStyleSheet(
    "QMenu::item {"
         "padding: 4px 25px 4px 20px;"
         "border: 1px solid transparent;" /* reserve space for selection border */
    "}"
    );

    outputSystemTray->setContextMenu(outputMenu);
}

void UkmediaSystemTrayWidget::activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    QRect rect;
    int local_width;
    int local_height;
    switch(reason)
    {
        case QSystemTrayIcon::MiddleClick: {
            if (inputWidget->isHidden()) {
                if (!outputActionMute->isChecked()) {
                    outputActionMute->setChecked(true);
                }
                else {
                    outputActionMute->setChecked(false);
                }
                inputWidget->ipMute();
                bool status = inputWidget->getIpMuteStatus();

                if (status) {
                    inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
                }
                else {
                    int volume = inputWidget->m_ipVolumeSlider->value();
                    updateIpSystemTrayIcon(volume);
                }
            }
            else {
                //hideWindow();
                inputWidget->hide();
            }
            break;
        }
        case QSystemTrayIcon::Trigger: {
            rect = inputSystemTray->geometry();
            local_width = QGuiApplication::screens().at(0)->availableGeometry().width() - this->width();
            local_height = QGuiApplication::screens().at(0)->availableGeometry().height() - this->height();
            if (voiceOnOrOff) {
                if (rect.x()< 1440  && rect.y() > 870) {
                    inputWidget->setGeometry(rect.x()+11-90,local_height,180,41);//底部
                }
                else if (rect.x() < 1440 && rect.y() < 40 ) {
                    inputWidget->setGeometry(rect.x()+11-90,0,180,41);//上
                }
                else if (rect.x() < 40 &&  rect.y()< 870) {
                    inputWidget->setGeometry(0,rect.y()+11-15,180,41);//左
                }
                else if (rect.x()>1570 && rect.y() < 870) {
                    inputWidget->setGeometry(local_width,rect.y()+11-15,1,41);
                }
                inputWidget->show();
                break;
            }
            else {            
                inputWidget->hide();
                break;
            }
        }
        case QSystemTrayIcon::DoubleClick: {
            inputWidget->hide();
            break;
        }
        case QSystemTrayIcon::Context: {
            if (!this->isHidden()) {
                inputWidget->hide();
                inputMenu->setVisible(false);
            }
            else {
                inputMenu->setVisible(true);
            }
            break;
        }
        default:
            break;
    }
}

void UkmediaSystemTrayWidget::activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    QRect rect;
    int local_width;
    int local_height;
    switch(reason)
    {
        case QSystemTrayIcon::MiddleClick: {

            if (this->isHidden()) {
                if (!outputActionMute->isChecked()) {
                    outputActionMute->setChecked(true);
                }
                else {
                    outputActionMute->setChecked(false);
                }
                outputWidget->opMute();
                bool status = outputWidget->getOpMuteStatus();

                if (status) {
                    outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
                }
                else {
                    int volume = outputWidget->m_opVolumeSlider->value();
                    updateOpSystemTrayIcon(volume);
                }

            }
            else {
                hideWindow();
            }
            break;
        }
        case QSystemTrayIcon::Trigger: {
            rect = outputSystemTray->geometry();
            local_width = QGuiApplication::screens().at(0)->availableGeometry().width() - this->width();
            local_height = QGuiApplication::screens().at(0)->availableGeometry().height() - this->height();
            if (voiceOnOrOff) {
                if (rect.x()< 1440  && rect.y() > 870) {
//                    outputWidget->setGeometry(rect.x()+11-90,local_height,180,41);//底部
                    this->setGeometry(rect.x()+11-90,local_height,180,41);//底部
                }
                else if (rect.x() < 1440 && rect.y() < 40 ) {
                    this->setGeometry(rect.x()+11-90,0,180,41);//上
                }
                else if (rect.x() < 40 &&  rect.y()< 870) {
                    this->setGeometry(0,rect.y()+11-15,180,41);//左
                }
                else if (rect.x()>1570 && rect.y() < 870) {
                    this->setGeometry(local_width,rect.y()+11-15,180,41);

                }
                showWindow();
                break;
            }
            else {
                hideWindow();
                break;
            }
        }
        case QSystemTrayIcon::DoubleClick: {
            hideWindow();
            break;
        }
        case QSystemTrayIcon::Context: {
            if (!this->isHidden()) {
                hideWindow();
                outputMenu->setVisible(false);
            }
            else {
                outputMenu->setVisible(true);
            }
            break;
        }
        default:
            break;
    }
}

void UkmediaSystemTrayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    hideWindow();
}

void UkmediaSystemTrayWidget::showWindow()
{
    this->show();
    voiceOnOrOff = 0;
}

void UkmediaSystemTrayWidget::hideWindow()
{
    this->hide();
    voiceOnOrOff = 1;
}

void UkmediaSystemTrayWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    hideWindow();
}

bool UkmediaSystemTrayWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            hideWindow();
        }
    }

    return QWidget::event(event);
}

bool UkmediaSystemTrayIcon::event(QEvent *event)
{
    QWheelEvent *e = static_cast<QWheelEvent *>(event);
    if (event->type() == QEvent::Wheel) {
        bool value = false;
        if (e->delta() > 0) {
            value = true;
            Q_EMIT  wheelRollEventSignal(value);
        }
        else if (e->delta() < 0) {
            value = false;
            Q_EMIT  wheelRollEventSignal(value);
        }
    }
    return QSystemTrayIcon::event(e);

}

void UkmediaSystemTrayWidget::wheelEvent(QWheelEvent *event)
{
    int wheelStep = 65536/20;//滚动大小为5
//    qDebug() << inputWidget->isHidden();

    int volume = outputWidget->getOpVolume();
    if (event->delta() >0 ) {
        volume = volume + wheelStep;
    }
    else if (event->delta() < 0 ) {
       volume = volume - wheelStep;
    }
    if (volume < 0) {
        outputWidget->setOpVolume(0);
    }
    else if (volume >= 0 && volume <= 65535) {

        outputWidget->setOpVolume(volume);
    }
    else if (volume > 65535) {
        outputWidget->setOpVolume(65535);
    }
    event->accept();
}

void UkmediaSystemTrayWidget::jumpControlPanel()
{
    QProcess *m_process = new QProcess;
    m_process->start("ukui-volume-control");
}

void UkmediaSystemTrayWidget::inputSystemTrayIconInit(int volume, bool status)
{
    volume = int(volume*100/65536.0 + 0.5);
    if (status) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
        if (!inputActionMute->isChecked()) {
            inputActionMute->setChecked(true);
        }
    }
    else if (volume <= 0) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
    }
    else if (volume > 0 && volume <= 33 ) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-low.png"));
    }
    else if (volume >33 && volume <= 66 ) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-medium.png"));
    }
    else {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-high.png"));
    }
    inputSystemTray->setVisible(TRUE);
}

void UkmediaSystemTrayWidget::outputSystemTrayIconInit(int volume,bool status)
{
    volume = volume*100/65536.0 + 0.5;
    if (status) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
        if (!outputActionMute->isChecked()) {
            outputActionMute->setChecked(true);
        }
    }
    else if (volume <= 0) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else if (volume > 0 && volume <= 33 ) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-low.png"));
    }
    else if (volume >33 && volume <= 66 ) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-medium.png"));
    }
    else {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-high.png"));
    }
    outputSystemTray->setVisible(TRUE);
}

void UkmediaSystemTrayWidget::slidChangeIpSystemTrayIcon(int volume)
{
    if (volume <= 0) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
    }
    else if (volume > 0 && volume <= 33) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-low.png"));
    }
    else if (volume >33 && volume <= 66) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-medium.png"));
    }
    else {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-high.png"));
    }

    if (mate_mixer_stream_control_get_mute(inputWidget->inputControl)) {
        inputActionMute->setChecked(true);
    }
    else {
        inputActionMute->setChecked(false);
    }

}

void UkmediaSystemTrayWidget::updateIpSystemTrayIcon(int volume)
{
    if (volume <= 0) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
    }
    else if (volume > 0 && volume <= 33) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-low.png"));
    }
    else if (volume >33 && volume <= 66) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-medium.png"));
    }
    else {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-high.png"));
    }
}

void UkmediaSystemTrayWidget::updateOpSystemTrayIcon(int volume)
{
    if (volume <= 0) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else if (volume > 0 && volume <= 33) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-low.png"));
    }
    else if (volume >33 && volume <= 66) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-medium.png"));
    }
    else {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-high.png"));
    }
}

void UkmediaSystemTrayWidget::slidChangeOpSystemTrayIcon(int volume)
{
    updateOpSystemTrayIcon(volume);
    if (mate_mixer_stream_control_get_mute(outputWidget->outputControl)) {
        outputActionMute->setChecked(true);

    }
    else {
        outputActionMute->setChecked(false);
    }
}

void UkmediaSystemTrayWidget::buttonChangeIpSystemTrayIcon()
{
    if (!inputActionMute->isChecked()) {
        inputActionMute->setChecked(true);
    }
    else {
        inputActionMute->setChecked(false);
    }
    bool status = inputWidget->getIpMuteStatus();
    if (status) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
    }
    else {

        int volume = inputWidget->getIpVolume();
        volume = volume * 100 / 65536.0 + 0.5;
        updateIpSystemTrayIcon(volume);
    }
}

void UkmediaSystemTrayWidget::buttonChangeOpSystemTrayIcon()
{
    if (!outputActionMute->isChecked()) {
        outputActionMute->setChecked(true);
    }
    else {
        outputActionMute->setChecked(false);
    }
    bool status = outputWidget->getOpMuteStatus();
    if (status) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else {

        int volume = outputWidget->getOpVolume();
        volume = volume * 100 / 65536.0 + 0.5;
        updateOpSystemTrayIcon(volume);
    }
}

void UkmediaSystemTrayWidget::inputActionMuteTriggered(bool status)
{
    inputWidget->ipMute();
    status = inputWidget->getIpMuteStatus();
    if (status) {
        inputSystemTray->setIcon(QIcon(":/images/audio-input-microphone-muted.png"));
    }
    else {
        int volume = inputWidget->getIpVolume();
        updateIpSystemTrayIcon(volume);
    }
}

void UkmediaSystemTrayWidget::outputActionMuteTriggered(bool status)
{
    outputWidget->opMute();
    status = outputWidget->getOpMuteStatus();
    if (status) {
        outputSystemTray->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else {
        int volume = outputWidget->getOpVolume();
        updateOpSystemTrayIcon(volume);
    }
}

void UkmediaSystemTrayWidget::keyPressEvent(QKeyEvent *event)
{
    int volume;
    if (event->key() == Qt::Key_Home) {
        outputWidget->m_opVolumeSlider->setValue(0);
    }
    else if (event->key() == Qt::Key_End) {
        outputWidget->m_opVolumeSlider->setValue(100);
    }
    else if (event->key() == Qt::Key_Left) {
        volume = outputWidget->m_opVolumeSlider->value();
        outputWidget->m_opVolumeSlider->setValue(volume-1);
    }
    else if (event->key() == Qt::Key_Right) {
        volume = outputWidget->m_opVolumeSlider->value();
        outputWidget->m_opVolumeSlider->setValue(volume+1);
    }
    else if(event->key() == Qt::Key_Up) {
        volume = outputWidget->m_opVolumeSlider->value();
        outputWidget->m_opVolumeSlider->setValue(volume-1);
    }
    else if (event->key() == Qt::Key_Down) {
        volume = outputWidget->m_opVolumeSlider->value();
        outputWidget->m_opVolumeSlider->setValue(volume+1);
    }
    else if (event->key() == Qt::Key_Escape) {
        hideWindow();
    }
}

void UkmediaSystemTrayWidget::acceptIpWheelRollEvent(bool value)
{
    int volume = inputWidget->m_ipVolumeSlider->value();
    if (value) {
        inputWidget->m_ipVolumeSlider->setValue(volume+5);
    }
    else {
        inputWidget->m_ipVolumeSlider->setValue(volume-5);
    }
}

void UkmediaSystemTrayWidget::acceptOpWheelRollEvent(bool value)
{
    int volume = outputWidget->m_opVolumeSlider->value();
    if (value) {
        outputWidget->m_opVolumeSlider->setValue(volume+5);
    }
    else {
        outputWidget->m_opVolumeSlider->setValue(volume-5);
    }
}

UkmediaSystemTrayWidget::~UkmediaSystemTrayWidget()
{

}


