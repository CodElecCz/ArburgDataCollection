#ifndef CENTRALVIEW_H
#define CENTRALVIEW_H

#include <QWidget>

namespace Ui {
class CentralView;
}

class BrowserView;
class OpcUaView;
class SettingsView;

class CentralView : public QWidget
{
    Q_OBJECT

public:
    typedef enum _EView
    {
        EView_none = 0,
        EView_OpcUa,
        EView_DatabaseBrowser,
        EView_Settings,

        /*...*/
        EView_Size
    } EView;

    explicit CentralView(BrowserView* browserView,
                         OpcUaView* opcuaView,
                         SettingsView* settingsView,
                         QWidget *parent = nullptr);
    ~CentralView();

    void setView(EView view);

private:
    Ui::CentralView *ui;
};

#endif // CENTRALVIEW_H
