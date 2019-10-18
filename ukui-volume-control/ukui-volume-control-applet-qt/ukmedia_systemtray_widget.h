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

class UkmediaSystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    UkmediaSystemTrayIcon(QWidget *parent = 0);
    ~UkmediaSystemTrayIcon();
private:
    //UkuiSystemTrayWidget *uw;
protected:
    bool event(QEvent *e) ;
//    void mousePressEvent(QMouseEvent *event);
};

class UkmediaSystemTrayWidget : public QMainWindow
{
    Q_OBJECT
public:
     bool voiceOnOrOff = 1;
    UkmediaSystemTrayWidget(QWidget *parent = 0);
    ~UkmediaSystemTrayWidget();
    void contextMenuEvent(QContextMenuEvent *event);
    void showWindow();
    void hideWindow();
    void setSystemTrayInitIcon(int volume,bool status);
    friend class UkmediaSystemTrayIcon;
Q_SIGNALS:
    void customContextMenuRequested(const QPoint&);

public Q_SLOTS:
    //void showContexmenu(const QPoint&);
    void activatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void jumpControlPanel();
    void changSystemTrayIcon(int volume);
    void onActionMuteTriggered(bool);
private:
    QPushButton *btnVoice;
    QAction *outputActionMute;
    QAction *outputActionSoundPreference;
    QSystemTrayIcon *systemTrayOutput;
    //UkuiSystemTrayIcon *systemTray;
    UkmediaControlWidget *outputWidget;
    QMenu *outputMenu ;

protected:
    //将窗口设置为随着窗口变化而变化
    bool event(QEvent *event);//重写窗口事件
    void mousePressEvent(QMouseEvent *event);
    void QWhellEvent(QWheelEvent *event);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    bool eventFilter(QObject *obj,QEvent *e);
};

#endif // UKMEDIA_SYSTEMTRAY_WIDGET_H
