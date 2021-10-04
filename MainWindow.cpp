#include "MainWindow.h"
#include "CentralView.h"
#include "browser/BrowserView.h"
#include "opcua/OpcUaView.h"
#include "opcua/OpcUaCheck.h"
#include "settings/SettingsView.h"
#include "settings/Settings.h"
#include "support/controls/StatusIndicator.h"
#include "log/LogView.h"

#include "ui_MainWindow.h"

#include <QFileInfo>
#include <QFile>
#include <QSettings>
#include <QtSql>
#include <QToolButton>

void addConnectionsFromCommandline(const QStringList &args, BrowserView *browser);
bool addConnectionsFromSettings(BrowserView *browser);

namespace
{
    const QString cStatusConnect("Connect");
    const QString cStatusDisconnect("Disconnect");

    const QString cStatusEnabled("Enabled");
    const QString cStatusDisabled("Disabled");

    const QString cStatusError("Error");
    const QString cStatusWarning("Warning");
}

MainWindow::MainWindow(QWidget *parent) :
    MainWindowExt(parent),
    ui(new Ui::MainWindow),

    m_browserView(new BrowserView(this)),
    m_opcuaView(new OpcUaView("", this)),
    m_settingsView(new SettingsView(this)),

    m_centralView(new CentralView(m_browserView, m_opcuaView, m_settingsView, this)),

    m_logView(nullptr),

    m_opcuaCheck(nullptr),

    m_toolBarButtons(new QButtonGroup(this)),
    m_lastView(CentralView::EView_none),    

    m_statusDbs(new StatusIndicator("Database", Qt::green, this)),
    m_statusOpcua(new StatusIndicator("OpcUa", Qt::green, this))
{
    ui->setupUi(this);

    this->setCentralWidget(m_centralView);   

    createLogWindow("Log", true);

    initializeMenuBar();
    initializeToolBar();
    initializeStatusBar();

    initializeWindow();

    initializeOdbc();
    initializeOpcua();
}

MainWindow::~MainWindow()
{    
    delete ui;
    delete m_browserView;
    delete m_opcuaView;
    delete m_settingsView;
    delete m_centralView;
    delete m_toolBarButtons;
    delete m_statusDbs;
    delete m_statusOpcua;

}

void MainWindow::initializeMenuBar()
{
    //style
    createStyleMenu(ui->menuStyle);

    //application close
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutDialog()));
}

void MainWindow::initializeStatusBar()
{
    ui->statusbar->addPermanentWidget(m_statusDbs);
    m_statusDbs->setMsg(cStatusDisconnect);

    ui->statusbar->addPermanentWidget(m_statusOpcua);
    m_statusOpcua->setMsg(cStatusDisabled);
}

void MainWindow::initializeToolBar()
{
    //toolBar
    QToolButton* fileManagerButton = new QToolButton(ui->toolbar);
    fileManagerButton->setAccessibleName("FileManager");
    fileManagerButton->setText("OPC UA\nClient");
    fileManagerButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileManagerButton->setToolTip(tr("File Manager View"));
    fileManagerButton->setCheckable(true);
    fileManagerButton->setChecked(true);
    fileManagerButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QToolButton* dbsBrowserButton = new QToolButton(ui->toolbar);
    dbsBrowserButton->setAccessibleName("DatabaseBrowser");
    dbsBrowserButton->setText("Database\nBrowser");
    dbsBrowserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    dbsBrowserButton->setToolTip(tr("Database Browser View"));
    dbsBrowserButton->setCheckable(true);
    dbsBrowserButton->setChecked(false);
    dbsBrowserButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QToolButton* settingsButton = new QToolButton(ui->toolbar);
    settingsButton->setAccessibleName("Settings");
    settingsButton->setText("Settings");
    settingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    settingsButton->setToolTip(tr("Application settings"));
    settingsButton->setCheckable(true);
    settingsButton->setChecked(false);
    settingsButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    ui->toolbar->addWidget(fileManagerButton);
    ui->toolbar->addWidget(dbsBrowserButton);
    ui->toolbar->addWidget(spacer);
    ui->toolbar->addWidget(settingsButton);

    m_toolBarButtons->addButton(fileManagerButton, CentralView::EView_OpcUa);
    m_toolBarButtons->addButton(dbsBrowserButton, CentralView::EView_DatabaseBrowser);
    m_toolBarButtons->addButton(settingsButton, CentralView::EView_Settings);
    m_toolBarButtons->setExclusive(true);

    connect(m_toolBarButtons, SIGNAL(buttonClicked(int)), this, SLOT(toolBarButtons_buttonClicked(int)));

    setView(CentralView::EView_OpcUa);
}

void MainWindow::toolBarButtons_buttonClicked(int eView)
{
    CentralView::EView view = static_cast<CentralView::EView>(eView);
    setView(view);
}

void MainWindow::setView(CentralView::EView view)
{
    //close
    if(m_lastView != view)
    {
        switch(m_lastView)
        {
        case CentralView::EView_OpcUa:
            break;
        case CentralView::EView_DatabaseBrowser:
            break;
        case CentralView::EView_Settings:
            break;
        case CentralView::EView_none:
            break;
        default:
            break;
        }

        m_lastView = view;
    }

    m_centralView->setView(view);

    //open
    switch(view)
    {
    case CentralView::EView_OpcUa:
        break;
    case CentralView::EView_DatabaseBrowser:
        break;
    case CentralView::EView_Settings:
        break;
    case CentralView::EView_none:
        break;
    default:
        break;
    }
}

void MainWindow::initializeOdbc()
{    
    connect(m_browserView, SIGNAL(statusMessage(int, const QString &)), this, SLOT(browserView_statusMessage(int, const QString &)));    

    //addConnectionsFromCommandline(app.arguments(), &browser);
    bool succeed = addConnectionsFromSettings(m_browserView);
    if(!succeed)
    {
        m_statusDbs->setOn(false);
        m_statusDbs->setMsg(cStatusDisconnect);
    }
    else
    {
        m_statusDbs->setOn(true);
        m_statusDbs->setMsg(cStatusConnect, Qt::green);        
    }

    if (QSqlDatabase::connectionNames().isEmpty())
    {
        /*
         * void BrowserView::addConnectionDialog(const QString &driver, const QString &dbName, const QString &host,
                                      const QString &user, const QString &passwd, int port)
         */
#if 0
        QMetaObject::invokeMethod(m_browserView, "addConnectionDialog", Qt::QueuedConnection);
#else
        QSettings config(Settings::iniFilePath(), QSettings::IniFormat);
        config.setIniCodec("UTF-8");

        QString driver = config.value(QString("odbc/driver")).toString();
        QString host = config.value(QString("odbc/host")).toString();
        QString database = config.value(QString("odbc/database")).toString();
        QString user = config.value(QString("odbc/user")).toString();
        QString pasword = config.value(QString("odbc/pasword")).toString();

        QMetaObject::invokeMethod(m_browserView, "addConnectionDialog", Qt::QueuedConnection,
                                  Q_ARG(QString, driver),
                                  Q_ARG(QString, database),
                                  Q_ARG(QString, host),
                                  Q_ARG(QString, user),
                                  Q_ARG(QString, pasword));
#endif
    }
}

void MainWindow::initializeOpcua()
{
    QSettings config(Settings::iniFilePath(), QSettings::IniFormat);
    config.setIniCodec("UTF-8");

    QString server = config.value(QString("opcua/url")).toString();

    connect(m_opcuaView, SIGNAL(statusMessage(int, const QString &)), this, SLOT(opcuaView_statusMessage(int, const QString &)));
    m_opcuaView->getEndpoints(server);

    //check    
    QUrl url(server);
    m_opcuaCheck = new OpcUaCheck(url.host(), this);    

    QString table = config.value(QString("opcuacheck/table")).toString();
    m_opcuaCheck->setDatabase(m_browserView->currentDatabase(), table);

    connect(m_opcuaView, &OpcUaView::dataChanged, m_opcuaCheck, &OpcUaCheck::opcuaView_dataChanged);
}

void MainWindow::browserView_statusMessage(int type, const QString &msg)
{
    ui->statusbar->showMessage(msg);
    m_statusDbs->setToolTip(msg);

    MessageType msgType = static_cast<MessageType>(type);

    switch(msgType)
    {
    case MessageType::Info:
        m_statusDbs->setOn(true);
        m_statusDbs->setMsg(cStatusConnect, Qt::green);

        qInfo().noquote() << msg;
        break;
    case MessageType::Warning:
        m_statusDbs->setOn(true);
        m_statusDbs->setMsg(cStatusWarning, Qt::yellow);

        qWarning().noquote() << msg;
        break;
    case MessageType::Error:
        m_statusDbs->setOn(true);
        m_statusDbs->setMsg(cStatusError, Qt::red);

        qCritical().noquote() << msg;
        break;
    case MessageType::Connect:
    case MessageType::Enable:
        m_statusDbs->setOn(true);
        m_statusDbs->setMsg(cStatusConnect, Qt::green);

        qInfo().noquote() << msg;
        break;
    case MessageType::Disconnect:
    case MessageType::Disable:
        m_statusDbs->setOn(false);
        m_statusDbs->setMsg(cStatusDisconnect);

        qInfo().noquote() << msg;
        break;
    }
};

void MainWindow::opcuaView_statusMessage(int type, const QString &msg)
{
    ui->statusbar->showMessage(msg);
    m_statusOpcua->setToolTip(msg);

    MessageType msgType = static_cast<MessageType>(type);

    switch(msgType)
    {
    case MessageType::Info:
        m_statusOpcua->setOn(true);
        m_statusOpcua->setMsg(cStatusConnect, Qt::green);

        qInfo().noquote() << msg;
        break;
    case MessageType::Warning:
        m_statusOpcua->setOn(true);
        m_statusOpcua->setMsg(cStatusWarning, Qt::yellow);

        qWarning().noquote() << msg;
        break;
    case MessageType::Error:
        m_statusOpcua->setOn(true);
        m_statusOpcua->setMsg(cStatusError, Qt::red);

        qCritical().noquote() << msg;
        break;
    case MessageType::Connect:
    case MessageType::Enable:
        m_statusOpcua->setOn(true);
        m_statusOpcua->setMsg(cStatusConnect, Qt::green);

        qInfo().noquote() << msg;
        break;
    case MessageType::Disconnect:
    case MessageType::Disable:
        m_statusOpcua->setOn(false);
        m_statusOpcua->setMsg(cStatusDisconnect);

        qInfo().noquote() << msg;
        break;
    }
};

void MainWindow::createLogWindow(const QString& name, bool showWindow)
{
    QDockWidget *dock = new QDockWidget(name, this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setFeatures(dock->features() & ~QDockWidget::DockWidgetFloatable);

    m_logView = new LogView(dock);
    m_logView->setObjectName("LogView");

    dock->setWidget(m_logView);
    dock->setObjectName(name);

    ui->menuWindow->addAction(dock->toggleViewAction());

    MainWindowExt::tabifyDockWidget(dock, name, Qt::BottomDockWidgetArea);
    //addDockWidget(Qt::LeftDockWidgetArea, dock);

    if(showWindow)
        dock->show();
    else
        dock->hide();
}

void addConnectionsFromCommandline(const QStringList &args, BrowserView *browser)
{
    for (int i = 1; i < args.count(); ++i)
    {
        QUrl url(args.at(i), QUrl::TolerantMode);
        if (!url.isValid())
        {
            qWarning("Invalid URL: %s", qPrintable(args.at(i)));
            continue;
        }

        QSqlError err = browser->addConnection(url.scheme(),
                                               url.path().mid(1),
                                               url.host(),
                                               url.userName(),
                                               url.password(),
                                               url.port(-1));

        if (err.type() != QSqlError::NoError)
        {
            qCritical() << "Unable to open connection:" << err;
        }
    }
}

bool addConnectionsFromSettings(BrowserView *browser)
{    
    QFile iniFile(Settings::iniFilePath());
    if(!iniFile.exists())
        return false;

    QSettings config(Settings::iniFilePath(), QSettings::IniFormat);
    config.setIniCodec("UTF-8");

    QUrl url;
    url.setScheme(config.value(QString("odbc/driver")).toString());
    url.setPath(config.value(QString("odbc/database")).toString());
    url.setHost(config.value(QString("odbc/host")).toString());
    url.setUserName(config.value(QString("odbc/user")).toString());
    url.setPassword(config.value(QString("odbc/pasword")).toString());
    url.setPort(-1);

    QSqlError err = browser->addConnection(url.scheme().toUpper(),
                                           url.path(),
                                           url.host(),
                                           url.userName(),
                                           url.password(),
                                           url.port(-1));

    if (err.type() != QSqlError::NoError)
    {
        qCritical() << "Unable to open connection:" << err;

        return false;
    }

    return true;
}

void MainWindow::applicationQuit()
{
    //qInfo() << "applicationQuit() " << m_opcuaView->isConnected();

    if(m_opcuaView->isConnected())
    {        
        QEventLoop loop;
        connect(m_opcuaView, &OpcUaView::disconnectedFromServer, &loop, &QEventLoop::quit);

        m_opcuaView->disconnectFromServer();

        loop.exec();
    }
    else
    {
        QApplication::quit();
    }
}
