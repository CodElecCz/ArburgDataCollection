#include "OpcUaView.h"
#include "OpcUaModel.h"
#include "certificate/certificatedialog.h"

#include "ui_OpcUaView.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QOpcUaProvider>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaErrorState>

namespace
{
    const QString cHostUrl = "opc.tcp://localhost:48040";
    const QString cServerUrl = "opc.tcp://192.168.11.111:4880/Arburg";
}

QT_BEGIN_NAMESPACE

OpcUaView::OpcUaView(const QString &initialUrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpcUaView),
    mOpcUaModel(new OpcUaModel(this)),
    mOpcUaProvider(new QOpcUaProvider(this))
{
    ui->setupUi(this);
    ui->server->setText(cServerUrl);

    ui->treeView->setModel(mOpcUaModel);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStringList backendList = QOpcUaProvider::availableBackends();
    if (backendList.count() == 0)
    {
        QMessageBox::critical(this, tr("No OPCUA plugins available"), tr("The list of available OPCUA plugins is empty. No connection possible."));
    }

    setupPkiConfiguration();

    //! [Application Identity]
    m_identity = m_pkiConfig.applicationIdentity();
    //! [Application Identity]            

    getEndpoints();
}

OpcUaView::~OpcUaView()
{

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
    if (mOpcUaClient == nullptr) {
        mOpcUaClient = mOpcUaProvider->createClient(ui->server->text());
        if (!mOpcUaClient) {
            const QString message(tr("Connecting to the given sever failed. See the log for details."));
            log(message, QString(), Qt::red);
            QMessageBox::critical(this, tr("Failed to connect to server"), message);
            return;
        }

        connect(mOpcUaClient, &QOpcUaClient::connectError, this, &OpcUaView::clientConnectError);
        mOpcUaClient->setApplicationIdentity(m_identity);
        mOpcUaClient->setPkiConfiguration(m_pkiConfig);

        if (mOpcUaClient->supportedUserTokenTypes().contains(QOpcUaUserTokenPolicy::TokenType::Certificate)) {
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

void OpcUaView::getEndpoints()
{
    const QString serverUrl = ui->server->text();
    createClient();
    mOpcUaClient->requestEndpoints(serverUrl);
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

void OpcUaView::clientConnected()
{
    mClientConnected = true;    

    connect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcUaView::namespacesArrayUpdated);
    mOpcUaClient->updateNamespaceArray();
}

void OpcUaView::clientDisconnected()
{
    mClientConnected = false;
    mOpcUaClient->deleteLater();
    mOpcUaClient = nullptr;
    mOpcUaModel->setOpcUaClient(nullptr);    
}

void OpcUaView::namespacesArrayUpdated(const QStringList &namespaceArray)
{
    if (namespaceArray.isEmpty())
    {
        qWarning() << "Failed to retrieve the namespaces array";
        return;
    }

    disconnect(mOpcUaClient, &QOpcUaClient::namespaceArrayUpdated, this, &OpcUaView::namespacesArrayUpdated);
    mOpcUaModel->setOpcUaClient(mOpcUaClient);    
}

void OpcUaView::clientError(QOpcUaClient::ClientError error)
{
    qDebug() << "Client error changed" << error;
}

void OpcUaView::clientState(QOpcUaClient::ClientState state)
{
    qDebug() << "Client state changed" << state;
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
        QMessageBox::warning(this, tr("Connection Error"), msg);
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
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    case QOpcUaErrorState::ConnectionStep::CreateSession:
        msg += tr("CreateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    case QOpcUaErrorState::ConnectionStep::ActivateSession:
        msg += tr("ActivateSession failed with error 0x%1 (%2).").arg(errorState->errorCode(), 8, 16, QLatin1Char('0')).arg(statuscode);
        QMessageBox::warning(this, tr("Connection Error"), msg);
        break;
    }
}

QT_END_NAMESPACE
