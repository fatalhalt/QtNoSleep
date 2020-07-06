#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QObject>
#include <QAction>
#include <QSystemTrayIcon>

class MySystemTray : public QObject
{
    Q_OBJECT
public:
    explicit MySystemTray(QObject *parent = 0);
    ~MySystemTray();

signals:
    void trayIconDoubleClick();
    void signalIconActivated();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

public:
    QSystemTrayIcon *trayIcon;
    QAction *showWindow;
private:

};

#endif // SYSTEMTRAY_H
