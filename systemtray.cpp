#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QtDebug>

#include "systemtray.h"

MySystemTray::MySystemTray(QObject *parent) : QObject(parent)
{
    // Create a context menu with 2 selections: Show and Quit
    QMenu *trayIconMenu = new QMenu();
    this->showWindow = new QAction("Show", this);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);

    trayIconMenu->addAction(showWindow);
    trayIconMenu->addAction(quitAction);


    // Initialize the tray icon, icon image, and specify the tooltip
    this->trayIcon = new QSystemTrayIcon();
    this->trayIcon->setContextMenu(trayIconMenu);
    this->trayIcon->setIcon(QIcon(":/icons/sleeping.png"));
    this->trayIcon->show();
    this->trayIcon->setToolTip("QtNoSleep");

    /* handle clicking on the system tray icon */
    connect(this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

MySystemTray::~MySystemTray() {
    if (this->trayIcon) {
        delete this->trayIcon->contextMenu();
        delete this->trayIcon;
    }
}

void MySystemTray::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        //qDebug() << "double clicked on the icon";
        emit trayIconDoubleClick();
        break;
    case QSystemTrayIcon::Trigger:
        // single click
        emit signalIconActivated();
        break;
    default:
        break;
    }
}
