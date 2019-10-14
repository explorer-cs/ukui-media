#ifndef UKUISYSTEMTRYWIDGET_H
#define UKUISYSTEMTRYWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QAction>
#include <QSystemTrayIcon>
#include "ukuimediacontrolwidget.h"
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QRadioButton>
#include <QWheelEvent>

class UkuiSystemTryWidget : public QMainWindow
{
    Q_OBJECT
public:
     bool voiceOnOrOff = 1;
    UkuiSystemTryWidget(QWidget *parent = 0);
    ~UkuiSystemTryWidget();
    void contextMenuEvent(QContextMenuEvent *event);
    void show_window();
    void hide_window();

Q_SIGNALS:
    void customContextMenuRequested(const QPoint&);

public Q_SLOTS:
    void show_contexmenu(const QPoint&);
    void activatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void jumpControlPanel();
private:
    QPushButton *btnVoice;
    QAction *actionMute;
    QAction *actionVoice;
    QWidget *myWidget;
    QAction *actionSoundPreference;
    QSystemTrayIcon *systemTray;
    UkuiMediaControlWidget *vWidget;
    QMenu *menu ;
    QRadioButton *radBtn;

protected:
    //将窗口设置为随着窗口变化而变化
    bool event(QEvent *event);//重写窗口事件
    void mousePressEvent(QMouseEvent *event);
    void QWhellEvent(QWheelEvent *event);
};

#endif // UKUISYSTEMTRYWIDGET_H
