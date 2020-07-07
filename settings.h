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

    QCheckBox *getRunCheckbox(void);
    QCheckBox *getStartCheckbox(void);

signals:
    void okSettingsDialog();
    void settingsDialogRunCheckbox();
    void settingsDialogStartMinimizedCheckbox();

private slots:
    void on_mySettingsPushButton_clicked();
    void on_mySettingsCheckBox_clicked();
    void on_mySettingsStartCheckbox_clicked();

private:
    Ui::MySettings *ui;
};

#endif // SETTINGS_H
