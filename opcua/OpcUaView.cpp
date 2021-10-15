#include "OpcUaView.h"
#include "tree/OpcUaItemModel.h"
#include "tree/OpcUaItemModelSort.h"
#include "table/OpcUaTableModel.h"
#include "certificate/certificatedialog.h"
#include "settings/Settings.h"

#include "ui_OpcUaView.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QOpcUaProvider>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaErrorState>
#include <QTimer>

namespace
{
    const QString cOpcUaPlugin = "open62541";
    const QString cHostUrl = "opc.tcp://localhost:48040";
    const QString cServerUrl = "opc.tcp://192.168.11.111:4880/Arburg";    

    const int cReconnect_ConnectionError = 5000;
    const int cReconnect_BadConnectionClosed = 30000;
}

QT_BEGIN_NAMESPACE

OpcUaView::OpcUaView(const QString &initialUrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpcUaView),
    mOpcUaItemModel(new OpcUaItemModel(this)),
    mOpcUaItemModelSort(new OpcUaItemModelSort(this)),
    mOpcUaTableModel(new OpcUaTableModel(this)),
    mOpcUaProvider(new QOpcUaProvider(this)),
    mTimerReconnect(new QTimer(this)),
    mTimerRead(new QTimer(this))
{
    ui->setupUi(this);

    if(initialUrl.isEmpty())
        ui->server->setText(cServerUrl);
    else
        ui->server->setText(initialUrl);

    //QTreView
    mOpcUaItemModelSort->setSourceModel(mOpcUaItemModel);
    ui->treeView->setModel(mOpcUaItemModelSort);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);    

    //QTableView
    ui->tableView->setModel(mOpcUaTableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setAlternatingRowColors(true);

    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_DisplayName, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_DisplayName, 80);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_TimeStamp, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_TimeStamp, 80);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_BrowseName, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_BrowseName, 60);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_NodeId, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_NodeId, 100);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_NodeClass, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_NodeClass, 60);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_DataType, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_DataType, 100);
    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_Description, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(OpcUaTableModel::EColumn_Description, 100);

    ui->tableView->horizontalHeader()->setSectionResizeMode(OpcUaTableModel::EColumn_Value, QHeaderView::Stretch);

    connect(mOpcUaTableModel, &OpcUaTableModel::itemChanged, this, &OpcUaView::dataChanged);

    //timer reconnect
    mTimerReconnect->setSingleShot(true);
    connect(mTimerReconnect, &QTimer::timeout, this, &OpcUaView::timerReconnect_timeout);

    //timer read
    mTimerRead->setSingleShot(false);
    connect(mTimerRead, &QTimer::timeout, this, &OpcUaView::timerRead_timeout);

    //OPC UA
    QStringList backendList = QOpcUaProvider::availableBackends();
    if (backendList.count() == 0)
    {        
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));

        qCritical() << "No OPCUA plugins available";
    }
    else
    {
        qInfo() << "OPCUA Plugin found: " << backendList;
    }

    setupPkiConfiguration();

    //! [Application Identity]
    m_identity = m_pkiConfig.applicationIdentity();
    //! [Application Identity]            
}

OpcUaView::~OpcUaView()
{    
    delete ui;
    delete mOpcUaItemModel;
    delete mOpcUaItemModelSort;
    delete mOpcUaTableModel;
    delete mOpcUaProvider;
}

//! [PKI Configuration]
void OpcUaView::setupPkiConfiguration()
{
    QString pkidir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    //pkidir += "../";
#endif
    pkidir += "/pki";
    m_pkiConfig.setClientCertificateFile(pkidir + "/own/certs/opcuaviewer.der");
    m_pkiConfig.setPrivateKeyFile(pkidir + "/own/private/opcuaviewer.pem");
    m_pkiConfig.setTrustListDirectory(pkidir + "/trusted/certs");
    m_pkiConfig.setRevocationListDirectory(pkidir + "/trusted/crl");
    m_pkiConfig.setIssuerListDirectory(pkidir + "/issuers/certs");
    m_pkiConfig.setIssuerRevocationListDirectory(pkidir + "/issuers/crl");

    // create the folders if they don't exist yet
    createPkiFolders();
}
//! [PKI Configuration]

void OpcUaView::createClient()
{
    if (mOpcUaClient == nullptr)
    {
        mOpcUaClient = mOpcUaProvider->createClient(cOpcUaPlugin);
        if (!mOpcUaClient)
        {
            qCritical() << "Connecting to the given sever failed";
            return;
        }

        connect(mOpcUaClient, &QOpcUaClient::connectError, this, &OpcUaView::clientConnectError);
        mOpcUaClient->setApplicationIdentity(m_identity);
        mOpcUaClient->setPkiConfiguration(m_pkiConfig);

        if (mOpcUaClient->supportedUserTokenTypes().contains(QOpcUaUserTokenPolicy::TokenType::Certificate))
        {
            QOpcUaAuthenticationInformation authInfo;
            authInfo.setCertificateAuthentication();
            mOpcUaClient->setAuthenticationInformation(authInfo);
        }

        QOpcUaAuthenticationInformation authInfo;
        authInfo.setUsernameAuthentication("host_computer", " ");
        mOpcUaClient->setAuthenticationInformation(authInfo);

        connect(mOpcUaClient, &QOpcUaClient::connected, this, &OpcUaView::clientConnected);
        connect(mOpcUaClient, &QOpcUaClient::disconnected, this, &OpcUaView::clientDisconnected);
        connect(mOpcUaClient, &QOpcUaClient::errorChanged, this, &OpcUaView::clientError);
        connect(mOpcUaClient, &QOpcUaClient::stateChanged, this, &OpcUaView::clientState);
        connect(mOpcUaClient, &QOpcUaClient::endpointsRequestFinished, this, &OpcUaView::getEndpointsComplete);
        connect(mOpcUaClient, &QOpcUaClient::findServersFinished, this, &OpcUaView::findServersComplete);
    }
}

void OpcUaView::findServers()
{
    QStringList localeIds;
    QStringList serverUris;
    QUrl url(cHostUrl);

    createClient();

    // set default port if missing
    if (url.port() == -1) url.setPort(4840);

    if (mOpcUaClient)
    {
        mOpcUaClient->findServers(url, localeIds, serverUris);
        qDebug() << "Discovering servers on " << url.toString();
    }
}

void OpcUaView::findServersComplete(const QVector<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode)
{
    QOpcUaApplicationDescription server;

    if (isSuccessStatus(statusCode))
    {
        for (const auto &server : servers)
        {
            QVector<QString> urls = server.discoveryUrls();
            for (const auto &url : qAsConst(urls))
            {
                //servers...
            }
        }
    }
}

void OpcUaView::getEndpoints(const QString &serverUrl)
{                                     
    if(!serverUrl.isEmpty())
    {
        ui->server->setText(serverUrl);
    }

    const QString server = ui->server->text();

    createClient();
    if(mOpcUaClient)
    {
        mOpcUaClient->requestEndpoints(server);
        qInfo() << "Server endpoints request...";
    }
}

void OpcUaView::getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode)
{
    int index = 0;
    const char *modes[] = {
        "Invalid",
        "None",
        "Sign",
        "SignAndEncrypt"
    };

    if (isSuccessStatus(statusCode))
    {
        mEndpointList = endpoints;
        for (const auto &endpoint : endpoints)
        {
            if (endpoint.securityMode() > sizeof(modes))
            {
                qWarning() << "Invalid security mode";
                continue;
            }

            const QString EndpointName = QString("%1 (%2)").arg(endpoint.securityPolicy(), modes[endpoint.securityMode()]);
            //endpoints...
            if(index==0)
            {
                ui->endpoint->setText(EndpointName);
                index++;
            }
        }

        if(index>0)
        {
            connectToServer();
        }
    }
    else //error
    {
        QString msg = QString("No connection to OPC UA server, reconnection in %1s...").arg((double)cReconnect_BadConnectionClosed/1000.0);
        emit statusMessage(MessageType::Error, msg);
        mTimerReconnect->start(cReconnect_BadConnectionClosed);
    }
}

void OpcUaView::connectToServer()
{
    if (mClientConnected)
    {
        mOpcUaClient->disconnectFromEndpoint();
        return;
    }

    if(mEndpointList.size()>0)
    {
        m_endpoint = mEndpointList[0];
        createClient();
        mOpcUaClient->connectToEndpoint(m_endpoint);
    }
}

bool OpcUaView::disconnectFromServer()
{
    if (mClientConnected)
    {
        mOpcUaClient->disconnectFromEndpoint();
        return true;
    }

    return false;
}

void OpcUaView::on_reconnectButton_clicked()
{
    bool connected = disconnectFromServer();

    if(connected)
    {
        qInfo() << "Server manual reconnecting in " << (double)cReconnect_ConnectionError/1000.0 << "s...";
        mTimerReconnect->start(cReconnect_ConnectionError);
    }
    else
    {
        qInfo() << "Server manual reconnecting";
        mTimerReconnect->stop();
        timerReconnect_timeout();
    }
}

void OpcUaView::clientConnected()
{
    mClientConnected = true;    

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcUaView::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();

    emit statusMessage(MessageType::Connect, "Connected to server: " + ui->server->text());

    mTimerRead->start(4000);
}

void OpcUaView::clientDisconnected()
{
    mTimerRead->stop();

    mClientConnected = false;
    mOpcUaClient->deleteLater();
    mOpcUaClient = nullptr;       

    mOpcUaItemModel->setOpcUaClient(nullptr);
    mOpcUaTableModel->setOpcUaClient(nullptr);

    emit statusMessage(MessageType::Disconnect, "Disconnected from server: " + ui->server->text());
    emit disconnectedFromServer();
}

void OpcUaView::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty())
    {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcUaView::namespacesArrayUpdated);

    mOpcUaItemModel->setOpcUaClient(mOpcUaClient);
    mOpcUaTableModel->setOpcUaClient(mOpcUaClient);
}

void OpcUaView::clientError(QOpcUaClient::ClientError error)
{
    qDebug() << "Client error changed" << error;

    if(error==QOpcUaClient::ConnectionError)
    {
        mReconnect = cReconnect_ConnectionError;
    }    
    else
    {
        mReconnect = cReconnect_BadConnectionClosed;
    }
}

void OpcUaView::clientState(QOpcUaClient::ClientState state)
{
    qDebug() << "Client state changed" << state;

    if(state==QOpcUaClient::Disconnected && mReconnect>0)
    {
        int msec = mReconnect;
        mReconnect = 0;

        qInfo() << "Server reconnectiong in " << (double)msec/1000.0 << "s...";
        mTimerReconnect->start(msec);
    }
}

void OpcUaView::timerReconnect_timeout()
{
    if(mEndpointList.size()>0)
        connectToServer();
    else
        getEndpoints();
}

void OpcUaView::timerRead_timeout()
{
    if(mClientConnected)
    {
        mOpcUaTableModel->opcUaRead();
    }
}

bool OpcUaView::createPkiPath(const QString &path)
{
    const QString msg = tr("Creating PKI path '%1': %2");

    QDir dir;
    const bool ret = dir.mkpath(path);
    if (ret)
        qDebug() << msg.arg(path, "SUCCESS.");
    else
        qCritical("%s", qPrintable(msg.arg(path, "FAILED.")));

    return ret;
}

bool OpcUaView::createPkiFolders()
{
    bool result = createPkiPath(m_pkiConfig.trustListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.revocationListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.issuerListDirectory());
    if (!result)
        return result;

    result = createPkiPath(m_pkiConfig.issuerRevocationListDirectory());
    if (!result)
        return result;

    return result;
}

void OpcUaView::clientConnectError(QOpcUaErrorState *errorState)
{
    int result = 0;

    const QString statuscode = QOpcUa::statusToString(errorState->errorCode());

    QString msg = errorState->isClientSideError() ? tr("The client reported: ") : tr("The server reported: ");

    switch (errorState->connectionStep())
    {
    case QOpcUaErrorState::ConnectionStep::Unknown:
        msg += tr("Failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        //QMessageBox::warning(this, tr("Connection Error"), msg);
        qWarning() << msg;
        break;
    case QOpcUaErrorState::ConnectionStep::CertificateValidation:
        {
            CertificateDialog dlg(this);
            msg += tr("Server certificate validation failed with error 0x%1 (%2).\nClick 'Abort' to abort the connect, or 'Ignore' to continue connecting.")
                      .arg(static_cast<ulong>(errorState->errorCode()), 8, 16, QLatin1Char('0')).arg(statuscode);
            result = dlg.showCertificate(msg, m_endpoint.serverCertificate(), m_pkiConfig.trustListDirectory());
            errorState->setIgnoreError(result == 1);
        }
        break;
    case QOpcUaErrorState::ConnectionStep::OpenSecureChannel:
        msg += tr("OpenSecureChannel failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        //QMessageBox::warning(this, tr("Connection Error"), msg);
        qWarning() << msg;
        break;
    case QOpcUaErrorState::ConnectionStep::CreateSession:
        msg += tr("CreateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        //QMessageBox::warning(this, tr("Connection Error"), msg);
        qWarning() << msg;
        break;
    case QOpcUaErrorState::ConnectionStep::ActivateSession:
        msg += tr("ActivateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        //QMessageBox::warning(this, tr("Connection Error"), msg);
        qWarning() << msg;
        break;
    }
}

QT_END_NAMESPACE
