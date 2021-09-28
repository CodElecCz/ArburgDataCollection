#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "support/MainWindowExt.h"
#include "CentralView.h"

#include <QMainWindow>
#include <QButtonGroup>

namespace Ui {
class MainWindow;
}

class BrowserView;
class CentralView;
class OpcUaView;
class SettingsView;
class StatusIndicator;
class LogView;

class MainWindow : public MainWindowExt
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeConnection();

    void initializeMenuBar();
    void initializeToolBar();
    void initializeStatusBar();

    void setView(CentralView::EView view);

    void createLogWindow(const QString& name, bool showWindow);

private slots:
    void toolBarButtons_buttonClicked(int);

    void browserView_statusMessage(int type, const QString &message);
    void fileView_statusMessage(int type, const QString &message);

private:
    Ui::MainWindow* ui;

    BrowserView*    m_browserView;
    OpcUaView*      m_opcuaView;
    SettingsView*   m_settingsView;

    CentralView*    m_centralView;

    LogView*        m_logView;

    QButtonGroup*       m_toolBarButtons;
    CentralView::EView  m_lastView;

    StatusIndicator*    m_statusDbs;
    StatusIndicator*    m_statusFile;
};

#endif // MAINWINDOW_H
