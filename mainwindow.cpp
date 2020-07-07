#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QTimer>
#include <QSettings>
#include <QDesktopWidget>
#include <random>

#include <Windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , startMinimized(false)
    , mySystray(new MySystemTray)
    , noSleep(false)
    , sawTooltipMessage(false)
    , saveSetting(false)
    , runOnStartup(false)
    , startedMinimized(false)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    this->statusBar()->setSizeGripEnabled(false);
    this->driveToKeepAwake = QString("");
    this->status = QString("");
    this->awakeFile = NULL;
    this->awakeTimer = new QTimer();
    this->awakeTimer->setInterval(RAND_WRITE_FREQUENCY_MS);

    QAction *showWindowAction = this->mySystray->showWindow;
    connect(showWindowAction, &QAction::triggered, this, &MainWindow::onShowWindow);

    // handle signal from MySystemTray class
    connect(this->mySystray, SIGNAL(trayIconDoubleClick()), this, SLOT(onTrayIconDoubleClick()));

    QSettings runReg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    QVariant runEntry = runReg.value("QtNoSleep", "");
    if (!runEntry.toString().isEmpty())
        this->runOnStartup = true;

    // Fill the items of the ComboBox with drive letters such as C:\ D:\ etc...
    auto drives = QDir::drives();
    foreach(QFileInfo item, drives){
        switch (GetDriveTypeA(item.absoluteFilePath().toStdString().c_str())) { /* call WinAPI */
        case 3: /* fixed drive */
            ui->mycomboBox->addItem(item.absoluteFilePath());
        }
    }

    if (this->readSettings() == 0) {
        bool foundMatch = false;
        for (int i = 0; i < ui->mycomboBox->count(); i++) {
            if (ui->mycomboBox->itemText(i) == this->driveToKeepAwake) {
                // emit NoSleep signal
                ui->mycomboBox->setCurrentIndex(i);
                ui->mycheckbox->setChecked(true);
                this->saveSetting = true;
                this->on_mypushButton_clicked();
                foundMatch = true;
            }
        }

        if (!foundMatch)
            this->driveToKeepAwake = "";
    }
}

MainWindow::~MainWindow()
{
    this->writeSettings();

    delete ui;

    destroyNoSleepFile();
    if (awakeFile)
        delete awakeFile;
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtNoSleep", "config");

    settings.beginGroup("MainWindow");
    settings.setValue("restore", this->saveSetting);
    settings.setValue("drive", this->driveToKeepAwake);
    settings.setValue("startMinimized", this->startMinimized);
    settings.endGroup();
}

int MainWindow::readSettings()
{
    int ret = 1;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtNoSleep", "config");

    settings.beginGroup("MainWindow");
    QVariant restore = settings.value("restore", false);
    QVariant driveToKeepAwake = settings.value("drive", "");
    if (!driveToKeepAwake.toString().isEmpty() && restore.toBool() == true) {
        qDebug() << "read settings: drive: " + driveToKeepAwake.toString() + ", restore: " + restore.toString();
        //this->noSleep = true;  // not needed here, a call to this->on_mypushButton_clicked() will set noSleep to true
        this->driveToKeepAwake = driveToKeepAwake.toString();
        ret = 0;
    }
    this->startMinimized = settings.value("startMinimized", false).toBool();
    settings.endGroup();

    return ret;
}

// Get drive info from mount letter like C: or D:
QString MainWindow::get_driveInfo(const QString d) {
    QStorageInfo storage(d);
    unsigned int driveCapacity = storage.bytesTotal()/1000/1000;

    return QString::number(driveCapacity).append("MB");
}

// Keep the drive awake
void MainWindow::on_mypushButton_clicked()
{
    //QMessageBox::information(this, "Item Selection", this->driveToKeepAwake);

    if (!this->noSleep) { // if not awake, make it awake
        if (this->createNoSleepFile())
            return;
        connect(this->awakeTimer, SIGNAL(timeout()), this, SLOT(onAwakeFileTimeout()));
        this->awakeTimer->start();

        ui->mypushButton->setText("Stop");
        ui->mycomboBox->setDisabled(true);
        this->status = "keeping awake...";
        ui->labelStatus->setText(this->status);

        this->noSleep = true;
    }
    else { // already awake, undo...
        //this->status.clear();
        this->status = "idle";
        ui->mypushButton->setText("No Sleep");
        ui->mycomboBox->setEnabled(true);
        ui->labelStatus->setText(this->status);

        this->noSleep = false;
        this->awakeTimer->stop();
        this->destroyNoSleepFile();
    }
}

void MainWindow::on_myactionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_mycheckbox_clicked() {
    this->saveSetting = !this->saveSetting;
}

void MainWindow::onAwakeFileTimeout()
{
    doRandomWrite(this->awakeFile);
}

void MainWindow::onShowWindow()
{
    if (this->isVisible())
        this->hide();
    else
        this->show();
}

void MainWindow::onTrayIconDoubleClick() {
#if 0
    if (this->windowState() == Qt::WindowMinimized) {
        int w = 320, h = 180;
        int screenWidth;
        int screenHeight;
        int x, y;

        QDesktopWidget *desktop = QApplication::desktop();
        screenWidth = desktop->width();
        screenHeight = desktop->height();
        x = (screenWidth - w) / 2;
        y = (screenHeight - h) / 2;

        this->setWindowState(Qt::WindowMaximized);
        this->move(x, y);
    } else {
        this->show();
    }
#endif
    this->show();
}

void MainWindow::onOKSettingsDialog() {
    this->mySettings->close();
}

void MainWindow::onSettingsDialogRunCheckbox() {
    QSettings runReg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (this->runOnStartup) {
        runReg.remove("QtNoSleep");
    } else {
        // add Run regkey
        runReg.setValue("QtNoSleep", "\"" + QCoreApplication::applicationFilePath().replace('/','\\') + "\"");
    }

    this->runOnStartup = !this->runOnStartup;
}

void MainWindow::onSettingsDialogStartMinimizedCheckbox() {
    this->startMinimized = !this->startMinimized;
}

// Override user X'ing out of program, instead put the application into taskbar
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (this->noSleep == false) {
        event->accept();
        return;
    }

    this->hide();
    if (this->sawTooltipMessage == false) {
        this->mySystray->trayIcon->showMessage("QtNoSleep", "Running in the background...");
        this->sawTooltipMessage = true;
    }

    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event)
{
# if 0
    if (this->startMinimized && this->startedMinimized == false) {
        this->startedMinimized = true;
        this->sawTooltipMessage = true; // Don't show a notification, be silent
        this->setWindowState(Qt::WindowMinimized);
        //this->hide()
        event->ignore();
    } else {
        event->accept();
    }
#endif
    event->accept();
}

void MainWindow::on_mycomboBox_currentIndexChanged(const QString &arg1)
{
    this->driveToKeepAwake = arg1;
    QString driveInfo = this->get_driveInfo(arg1);
    ui->label->setText(arg1 + " " + driveInfo);
}


int MainWindow::createNoSleepFile() {
    QFile *file = new QFile(this->driveToKeepAwake + ".nosleep");
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        //qDebug() << file.errorString();
        QMessageBox::information(this, "Unable to write file", file->errorString());
        return 1;
    }

    this->awakeFile = file;
    return 0;
}

int MainWindow::destroyNoSleepFile() {
    if (this->awakeFile) {
        this->awakeFile->remove();
        this->awakeFile->close();
    }

    return 0;
}

void doRandomWrite(QFile *file) {
    // seek to the start and sets the file size to 0 (since .pos() returns 0 after seek to beginning)
    file->reset();
    file->resize(file->pos());

    QDataStream out(file);
    const std::string *randomString = generateRandomBase64(RAND_STRING_SIZE);
    out.writeRawData(randomString->c_str(), RAND_STRING_SIZE);
    file->flush();

    delete randomString;
}


const std::string *generateRandomBase64(size_t strSize)
{
    std::string *retStr = new std::string("");
    retStr->reserve(strSize);

    std::string randStr = "";
    static const std::string base = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

    static std::random_device rd;
    static std::mt19937 generator(rd());
    static std::uniform_int_distribution<int> dist(0, base.size() - 2);

    for (unsigned int i = 0; i < strSize/1024; ++i)
        randStr.append(&base[dist(generator)]);
    randStr.append("\n");

    for (unsigned int i = 0; i < strSize/1024; ++i) // append the randStr strSize/1024 times
        retStr->append(randStr);

    return retStr;
}

void MainWindow::on_actionSettings_triggered()
{
    MySettings *dialog = new MySettings(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setGeometry(this->x() + 20, this->y() + 50, 200, this->height()/2);
    dialog->setFixedSize(dialog->size());

    if (this->runOnStartup) {
        dialog->getRunCheckbox()->setChecked(true);
    }
    if (this->startMinimized) {
        dialog->getStartCheckbox()->setChecked(true);
    }

    this->mySettings = dialog;
    connect(this->mySettings, SIGNAL(okSettingsDialog()), this, SLOT(onOKSettingsDialog()));
    connect(this->mySettings, SIGNAL(settingsDialogRunCheckbox()), this, SLOT(onSettingsDialogRunCheckbox()));
    connect(this->mySettings, SIGNAL(settingsDialogStartMinimizedCheckbox()), this, SLOT(onSettingsDialogStartMinimizedCheckbox()));

    dialog->show();
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog *about = new QDialog(this);
    about->setAttribute(Qt::WA_DeleteOnClose);
    about->setGeometry(this->x() + 20, this->y() + 50, this->width(), this->height()/2);
    about->setFixedSize(about->size());
    about->setWindowTitle("About");

    QHBoxLayout *layout = new QHBoxLayout();
    QString labelText = QString("QtNoSleep: v") + QString(PROGRAM_VERSION) + QString(". github repo: <a href=\"https://github.com/fatalhalt/QtNoSleep\">QtNoSleep</a>");
    QLabel *label = new QLabel(labelText, this);
    label->setOpenExternalLinks(true);
    layout->addWidget(label);
    about->setLayout(layout);

    about->show();
}
