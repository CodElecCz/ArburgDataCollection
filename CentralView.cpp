#include "CentralView.h"
#include "browser/BrowserView.h"
#include "opcua/OpcUaView.h"
#include "settings/SettingsView.h"

#include "ui_CentralView.h"

CentralView::CentralView(BrowserView* browserView,
                         OpcUaView* opcuaView,
                         SettingsView *settingsView,
                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CentralView)
{
    ui->setupUi(this);

    ui->stackedWidget->addWidget(opcuaView);
    ui->stackedWidget->addWidget(browserView);
    ui->stackedWidget->addWidget(settingsView);
}

CentralView::~CentralView()
{
    delete ui;
}

void CentralView::setView(EView view)
{
    switch(view)
    {
    case EView_OpcUa:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case EView_DatabaseBrowser:
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case EView_Settings:
        ui->stackedWidget->setCurrentIndex(2);
        break;
    default:
        break;
    }
}

