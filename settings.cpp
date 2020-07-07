#include "settings.h"
#include "ui_settings.h"

MySettings::MySettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MySettings)
{
    ui->setupUi(this);
}

MySettings::~MySettings()
{
    delete ui;
}

QCheckBox *MySettings::getRunCheckbox(void) {
    return ui->mySettingsCheckBox;
}

QCheckBox *MySettings::getStartCheckbox(void) {
    return ui->mySettingsStartCheckbox;
}

void MySettings::on_mySettingsPushButton_clicked()
{
    //emit okSettingsDialog();
    this->close();
}

void MySettings::on_mySettingsCheckBox_clicked()
{
    emit settingsDialogRunCheckbox();
}

void MySettings::on_mySettingsStartCheckbox_clicked()
{
    emit settingsDialogStartMinimizedCheckbox();
}
