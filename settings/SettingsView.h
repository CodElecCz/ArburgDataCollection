#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QWidget>

namespace Ui {
class SettingsView;
}

class SettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsView(QWidget *parent = nullptr);
    ~SettingsView();

private:
    void openSettingsFile();
    void saveSettingsFile();

private slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::SettingsView *ui;
};

#endif // SETTINGSVIEW_H
