#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class MySettings;
}

class MySettings : public QDialog
{
    Q_OBJECT

public:
    explicit MySettings(QWidget *parent = nullptr);
    ~MySettings();

    QCheckBox *getCheckbox(void);

signals:
    void okSettingsDialog();
    void settingsDialogRunCheckbox();

private slots:
    void on_mySettingsPushButton_clicked();

    void on_mySettingsCheckBox_clicked();

private:
    Ui::MySettings *ui;
};

#endif // SETTINGS_H
