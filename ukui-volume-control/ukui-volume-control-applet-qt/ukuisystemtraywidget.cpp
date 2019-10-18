#include "ukuisystemtraywidget.h"
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
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QMetaObject>
#include <QProcess>
#include <QScreen>

UkmediaSystemTrayIcon::UkmediaSystemTrayIcon(QWidget *parent)
{

}

UkmediaSystemTrayIcon::~UkmediaSystemTrayIcon()
{

}

UkmediaSystemTrayWidget::UkmediaSystemTrayWidget(QWidget *parent)
    : QMainWindow (parent)
{
    int volume;
    bool status;
    //添加
    systemTrayOutput = new QSystemTrayIcon(this);
    outputWidget = new UkmediaControlWidget();

    //为系统托盘图标添加右击菜单
    outputActionMute = new QAction(this);
    outputActionMute->setCheckable(true);
    outputActionMute->setText("静音(M)");

    volume = outputWidget->getVolume();
    status = outputWidget->getMuteStatus();
    setSystemTrayInitIcon(volume,status);

    systemTrayOutput->setToolTip("输出声音设置");
    systemTrayOutput->setVisible(TRUE);

    //隐藏任务栏图标
    //setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    //在构造中设置窗体的MouseTracking属性
    this->setMouseTracking(true);
    outputWidget->setMouseTracking(true);

    outputActionSoundPreference = new QAction(this);
    outputActionSoundPreference->setIcon(QIcon(":/images/application-audio.png"));
    outputActionSoundPreference->setText("声音首选项");

    //事件过滤器

    this->installEventFilter(this);
    //设置右键菜单
    outputMenu = new QMenu(this);
    outputMenu->addAction(outputActionMute);
    outputMenu->addAction(outputActionSoundPreference);

    systemTrayOutput->setContextMenu(outputMenu);

    //点击托盘栏声音图标显示音量条
    connect(systemTrayOutput,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    //设置获取焦点事件
    setFocusPolicy(Qt::ClickFocus);
    //设置静音功能
    connect(outputActionMute,SIGNAL(triggered(bool)),this,SLOT(onActionMuteTriggered(bool)));

    //更改系统托盘图标
    connect(outputWidget->m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(changSystemTrayIcon(int)));
    //连接声音首选项
    connect(outputActionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
    //设置中心窗口
    this->setCentralWidget(outputWidget);

    resize(160,30);
}

void UkmediaSystemTrayWidget::activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    bool status;
    QRect rect;
    int local_width;
    int local_height;

    switch(reason)
    {
        case QSystemTrayIcon::MiddleClick:
            if (this->isHidden()) {
                status = outputActionMute->isChecked();
                if (!outputActionMute->isChecked()) {
                    outputActionMute->setChecked(true);
                }
                else {
                    outputActionMute->setChecked(false);
                }
                outputWidget->mute();
            }
            else {
                hideWindow();
            }

            break;
        case QSystemTrayIcon::Trigger:
            rect = systemTrayOutput->geometry();
            local_width = QGuiApplication::screens().at(0)->availableGeometry().width() - this->width();
            //local_width = QApplication::desktop()->availableGeometry().width() - this->width();
            local_height = QApplication::desktop()->availableGeometry().height() - this->height();

            if (voiceOnOrOff) {
                if (rect.x()< 1440  && rect.y() > 870) {
                    this->setGeometry(rect.x()+11-80,local_height,160,40);//底部
                }
                else if (rect.x() < 1440 && rect.y() < 40 ) {
                    this->setGeometry(rect.x()+11-80,0,160,40);//上
                }
                else if (rect.x() < 40 &&  rect.y()< 870) {
                    this->setGeometry(0,rect.y()+11-20,160,40);//左
                }
                else if (rect.x()>1570 && rect.y() < 870) {
                    this->setGeometry(local_width,rect.y()+11-20,160,40);

                }
                showWindow();
                break;
            }
            else {
                hideWindow();
                break;
            }
    }
}

void UkmediaSystemTrayWidget::contextMenuEvent(QContextMenuEvent *event)
{
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

//void UkuiSystemTrayWidget::showContexmenu(const QPoint&)
//{
//    hideWindow();
//}

void UkmediaSystemTrayWidget::mousePressEvent(QMouseEvent *event)
{

    hideWindow();
//    if (event->buttons() == Qt::LeftButton ) {
//        qDebug() << "鼠标左键按下";
//    }
//    else if (event->buttons() == Qt::MidButton) {
//        qDebug() << "鼠标中间键按下";
//    }
//    else if (event->buttons() == Qt::RightButton) {
//        qDebug() << "鼠标右键按下";
//    }

}

bool UkmediaSystemTrayWidget::event(QEvent *event)
{
    //qDebug() << "事件类型" << event->type();
/*    if (event->type() == QEvent::FocusOut)
    {
        hideWindow();
    }
    else */if (event->type() == QEvent::ActivationChange) {
        qDebug() << "activate window";
        if(QApplication::activeWindow() != this) {
            hideWindow();
        }
    }

    return QWidget::event(event);
}

bool UkmediaSystemTrayIcon::event(QEvent *event)
{
    //UkuiMediaControlWidget *w = new UkuiMediaControlWidget;
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    if (event->type() == QEvent::MouseButtonPress) {

         //if (mouseEvent->buttons() == Qt::RightButton && !w->isHidden()){
            // w->hide();
        // }

     }
    return QSystemTrayIcon::event(event);

}

void UkmediaSystemTrayWidget::QWhellEvent(QWheelEvent *event)
{
    guint wheelStep = 65536/100;//滚动大小为5
    guint volume = outputWidget->getVolume();
    qDebug() << "滚动滚轮";
    if (event->delta() >0 ) {
        volume = volume + wheelStep;
    }
    else if (event->delta() < 0 ) {
       volume = volume - wheelStep;
    }
    outputWidget->setVolume(volume);
    event->accept();
}

void UkmediaSystemTrayWidget::jumpControlPanel()
{
    QProcess *m_process = new QProcess;
    m_process->start("ukui-volume-control");
}

void UkmediaSystemTrayWidget::setSystemTrayInitIcon(int volume,bool status)
{
    volume = volume*100/65536.0 + 0.5;
    if (status) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-muted.png"));
        if (!outputActionMute->isChecked()) {
            outputActionMute->setChecked(true);
        }
    }
    else if (volume <= 0) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else if (volume > 0 && volume <= 33 ) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-low.png"));
    }
    else if (volume >33 && volume <= 66 ) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-medium.png"));
    }
    else {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-high.png"));
    }
}

void UkmediaSystemTrayWidget::changSystemTrayIcon(int volume)
{
    bool status = outputWidget->getMuteStatus();
    if (volume <= 0) {
        //outputActionMute->setChecked(true);
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-muted.png"));
    }
    else if (volume > 0 && volume <= 33) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-low.png"));
    }
    else if (volume >33 && volume <= 66) {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-medium.png"));
    }
    else {
        systemTrayOutput->setIcon(QIcon(":/images/audio-volume-high.png"));
    }
    if (mate_mixer_stream_control_get_mute(outputWidget->ukuiControl)) {
        outputActionMute->setChecked(true);
    }
    else {
        outputActionMute->setChecked(false);
    }
}

void UkmediaSystemTrayWidget::onActionMuteTriggered(bool status)
{
    hideWindow();
    outputWidget->mute();
}

void UkmediaSystemTrayWidget::focusInEvent(QFocusEvent *e)
{

}

void UkmediaSystemTrayWidget::focusOutEvent(QFocusEvent *e)
{
    hideWindow();
}

bool UkmediaSystemTrayWidget::eventFilter(QObject *obj,QEvent *e)
{
    if (obj == this) {

    }
    if (!this->isActiveWindow()) {
        if (e->type() == QEvent::MouseButtonPress) {

            qDebug() << "关闭托盘栏";
        }

    }
    //if (e->type() == QEvent::ActivationChange) {
//        if(QApplication::activeWindow() != this && e->type() == QEvent::MouseButtonPress) {
//            qDebug() << "活跃窗口已改变";
//            hideWindow();
//        }
    //}

        /*if (e->type() == QEvent::ActivationChange) {
//        qDebug() << "活跃窗口以改变" << isActiveWindow() << QApplication::activeWindow();

    }*/
    return QObject::event(e);
}

UkmediaSystemTrayWidget::~UkmediaSystemTrayWidget()
{

}


