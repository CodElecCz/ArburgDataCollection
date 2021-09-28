#ifndef OPCUA_VIEW_H
#define OPCUA_VIEW_H

#include <QWidget>
#include <QOpcUaClient>

QT_BEGIN_NAMESPACE

class QOpcUaProvider;
class OpcUaModel;

namespace Ui {
class OpcUaView;
}

class OpcUaView : public QWidget
{
    Q_OBJECT

public:
    explicit OpcUaView(const QString &initialUrl, QWidget *parent = nullptr);
    ~OpcUaView();

    Q_INVOKABLE void log(const QString &text, const QString &context, const QColor &color);
    void log(const QString &text, const QColor &color = Qt::black);

private slots:
    void connectToServer();
    void findServers();
    void findServersComplete(const QVector<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode);

    void getEndpoints();
    void getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);

    void clientConnected();
    void clientDisconnected();

    void namespacesArrayUpdated(const QStringList &namespaceArray);

    void clientError(QOpcUaClient::ClientError);
    void clientState(QOpcUaClient::ClientState);
    void clientConnectError(QOpcUaErrorState *errorState);

private:
    void createClient();

    void setupPkiConfiguration();
    bool createPkiFolders();
    bool createPkiPath(const QString &path);

private:    
    Ui::OpcUaView*              ui;
    OpcUaModel*                 mOpcUaModel;
    QOpcUaProvider*             mOpcUaProvider;
    QOpcUaClient*               mOpcUaClient = nullptr;
    QVector<QOpcUaEndpointDescription> mEndpointList;
    bool                        mClientConnected = false;
    QOpcUaApplicationIdentity   m_identity;
    QOpcUaPkiConfiguration      m_pkiConfig;
    QOpcUaEndpointDescription   m_endpoint; // current endpoint used to connect
};

QT_END_NAMESPACE

#endif // OPCUA_VIEW_H
