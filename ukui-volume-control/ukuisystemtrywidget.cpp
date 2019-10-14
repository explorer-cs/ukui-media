#include "ukuisystemtrywidget.h"
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

UkuiSystemTryWidget::UkuiSystemTryWidget(QWidget *parent)
    : QMainWindow (parent)
{
    //添加
    systemTray = new QSystemTrayIcon(this);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);//隐藏边框
    systemTray->setIcon(QIcon("/usr/share/icons/ukui-icon-theme/16x16/panel/audio-volume-medium-panel.png"));
    systemTray->setToolTip("声音设置");
    systemTray->setVisible(TRUE);
    //为系统托盘图标添加右击菜单
    actionMute = new QAction(this);
    actionMute->setCheckable(true);
    actionMute->setText("静音(M)");

    actionSoundPreference = new QAction(this);
    actionSoundPreference->setIcon(QIcon("/usr/share/icons/ukui-icon-theme/16x16/mimetypes/application-audio-playlist.svg"));
    actionSoundPreference->setText("声音首选项");

    menu = new QMenu(this);
    menu->addAction(actionMute);
    menu->addAction(actionSoundPreference);
    systemTray->setContextMenu(menu);

    //在构造中设置窗体的MouseTracking属性
    vWidget = new UkuiMediaControlWidget(this);
    this->setMouseTracking(true);
    vWidget->setMouseTracking(true);
    //点击托盘栏声音图标显示音量条
    connect(systemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    //设置获取焦点事件
    setFocusPolicy(Qt::ClickFocus);
    //设置静音功能
    connect(actionMute,&QAction::triggered,vWidget,[=] {
        hide_window();
        vWidget->mute();
    });

    //连接声音首选项
    connect(actionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
    //设置中心窗口
    this->setCentralWidget(vWidget);
    resize(160,30);
}

void UkuiSystemTryWidget::activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case 3:
            QRect rect;
            int local_width;
            int local_height;
            rect = systemTray->geometry();
            local_width = QApplication::desktop()->availableGeometry().width() - this->width();
            local_height = QApplication::desktop()->availableGeometry().height() - this->height();

            if (voiceOnOrOff) {
                show_window();
                if(rect.x()< 1440  && rect.y() > 870) {
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
                break;
            }
            else {
                hide_window();
                break;
            }
    }
}

void UkuiSystemTryWidget::contextMenuEvent(QContextMenuEvent *event)
{
    hide_window();
}

void UkuiSystemTryWidget::show_window()
{
    this->show();
    vWidget->show();
    voiceOnOrOff = 0;
}

void UkuiSystemTryWidget::hide_window()
{
    vWidget->hide();
    this->hide();
    voiceOnOrOff = 1;
}

void UkuiSystemTryWidget::show_contexmenu(const QPoint&)
{
    hide_window();
}

void UkuiSystemTryWidget::mousePressEvent(QMouseEvent *event)
{

    if(event->button()==Qt::LeftButton)
    {
        hide_window();
    }
    else if(event->button()==Qt::RightButton)
    {
        //hide_window();
    }
    else if(event->button()==Qt::MidButton)
    {
        vWidget->mute();
    }
}

bool UkuiSystemTryWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if(QApplication::activeWindow() != this)
        {
            hide_window();
        }
    }

    return QWidget::event(event);
}

void UkuiSystemTryWidget::QWhellEvent(QWheelEvent *event)
{
    guint wheelStep = 65536/100;//滚动大小为5
    guint volume = vWidget->getVolume();
    if (event->delta() >0 ) {
        volume = volume + wheelStep;
        qDebug() << "向上滚动";
    }
    else if (event->delta() < 0 ) {
       volume = volume - wheelStep;
       qDebug() << "向下滚动";
    }
    vWidget->setVolume(volume);
    event->accept();
}

void UkuiSystemTryWidget::jumpControlPanel()
{
    QProcess *m_process = new QProcess;
    m_process->start("ukui-volume-control");

}

UkuiSystemTryWidget::~UkuiSystemTryWidget()
{

}
