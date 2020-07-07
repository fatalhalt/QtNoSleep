#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QStorageInfo>
#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>

#include "systemtray.h"
#include "settings.h"

#define RAND_STRING_SIZE 1024*1024
#define RAND_WRITE_FREQUENCY_MS 30000
#define PROGRAM_VERSION "0.1"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const std::string *generateRandomBase64(size_t);
void doRandomWrite(QFile *);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int createNoSleepFile(void);
    int destroyNoSleepFile(void);

    bool startMinimized;

public slots:
    void onTrayIconDoubleClick();
    void onOKSettingsDialog();
    void onSettingsDialogRunCheckbox();
    void onSettingsDialogStartMinimizedCheckbox();

private slots:
    void on_mypushButton_clicked();
    void on_myactionExit_triggered();
    void on_mycheckbox_clicked();
    void onAwakeFileTimeout();
    void onShowWindow();
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);

    void on_mycomboBox_currentIndexChanged(const QString &arg1);

    void on_actionSettings_triggered();

    void on_actionAbout_triggered();

private:
    MySystemTray *mySystray;
    MySettings *mySettings;
    int readSettings(void);
    void writeSettings(void);
    QString get_driveInfo(const QString);

    bool noSleep;
    bool sawTooltipMessage;
    bool saveSetting;
    bool runOnStartup;
    bool startedMinimized;
    Ui::MainWindow *ui;
    QString driveToKeepAwake;
    QString status;
    QFile *awakeFile;
    QTimer *awakeTimer;
};
#endif // MAINWINDOW_H
