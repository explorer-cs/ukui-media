#ifndef UKMEDIA_SYSTEMTRAY_WIDGET_H
#define UKMEDIA_SYSTEMTRAY_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QAction>
#include <QSystemTrayIcon>
#include "ukmedia_control_widget.h"
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QRadioButton>
#include <QWheelEvent>
#include <QLineEdit>
#include <QMenu>

#define TRANSLATIONS_DIR "/home/fzx/f/ukui-media-git/ukui-media/ukui-media/po"

class UkmediaSystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    UkmediaSystemTrayIcon(QWidget *parent = nullptr);
    ~UkmediaSystemTrayIcon();

Q_SIGNALS:
    void wheelRollEventSignal(bool);

protected:
    bool event(QEvent *e) ;
};

class UkmediaSystemTrayWidget : public QMainWindow
{
    Q_OBJECT
public:
     bool voiceOnOrOff = 1;
    UkmediaSystemTrayWidget(QWidget *parent = nullptr);
    ~UkmediaSystemTrayWidget();
    void showWindow();
    void hideWindow();
    void outputSystemTrayIconInit(int volume,bool status);
    void inputSystemTrayIconInit(int volume,bool status);
    void outputSystemTrayMenuInit();
    void inputSystemTrayMenuInit();
    friend class UkmediaSystemTrayIcon;

Q_SIGNALS:
    void customContextMenuRequested(const QPoint&);

public Q_SLOTS:
    void activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason);
    void activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason);
    void jumpControlPanel();
    void changeOpSystemTrayIcon(int volume);
    void changeIpSystemTrayIcon(int volume);
    void outputActionMuteTriggered(bool);
    void inputActionMuteTriggered(bool);
    void acceptOpWheelRollEvent(bool);
    void acceptIpWheelRollEvent(bool);
private:
    QPushButton *btnVoice;
    QAction *inputActionMute;
    QAction *inputActionSoundPreference;
    QAction *outputActionMute;
    QAction *outputActionSoundPreference;
    UkmediaSystemTrayIcon *inputSystemTray;
    UkmediaSystemTrayIcon *outputSystemTray;
    UkmediaControlWidget *outputWidget;
    UkmediaControlWidget *inputWidget;
    QMenu *outputMenu ;
    QMenu *inputMenu;

protected:
    //将窗口设置为随着窗口变化而变化
    bool event(QEvent *event);//重写窗口事件
    void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // UKMEDIA_SYSTEMTRAY_WIDGET_H
