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

QCheckBox *MySettings::getCheckbox(void) {
    return ui->mySettingsCheckBox;
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
