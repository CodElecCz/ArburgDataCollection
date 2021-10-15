#ifndef OPCUA_VIEW_H
#define OPCUA_VIEW_H

#include <QWidget>
#include <QOpcUaClient>

QT_BEGIN_NAMESPACE

class QOpcUaProvider;
class OpcUaItemModel;
class OpcUaItemModelSort;
class OpcUaTableModel;

namespace Ui {
class OpcUaView;
}

class OpcUaView : public QWidget
{
    Q_OBJECT

public:
    explicit OpcUaView(const QString &serverUrl, QWidget *parent = nullptr);
    ~OpcUaView();

    void findServers();
    void getEndpoints(const QString &serverUrl = "");

    void connectToServer();
    bool disconnectFromServer();
    bool isConnected() { return mClientConnected; }

signals:
    void statusMessage(int type, const QString &message);
    void disconnectedFromServer();
    void dataChanged(const QStringList& var, const QStringList& data);

private slots:        
    void findServersComplete(const QVector<QOpcUaApplicationDescription> &servers, QOpcUa::UaStatusCode statusCode);   
    void getEndpointsComplete(const QVector<QOpcUaEndpointDescription> &endpoints, QOpcUa::UaStatusCode statusCode);

    void clientConnected();
    void clientDisconnected();

    void namespacesArrayUpdated(const QStringList &namespaceArray);

    void clientError(QOpcUaClient::ClientError);
    void clientState(QOpcUaClient::ClientState);
    void clientConnectError(QOpcUaErrorState *errorState);

    void timerReconnect_timeout();
    void timerRead_timeout();

    void on_reconnectButton_clicked();

private:
    void createClient();

    void setupPkiConfiguration();
    bool createPkiFolders();
    bool createPkiPath(const QString &path);

private:    
    Ui::OpcUaView*              ui;
    //QTreeView
    OpcUaItemModel*             mOpcUaItemModel;
    OpcUaItemModelSort*         mOpcUaItemModelSort;
    //QTableView
    OpcUaTableModel*            mOpcUaTableModel;

    QOpcUaProvider*             mOpcUaProvider;
    QOpcUaClient*               mOpcUaClient = nullptr;
    QVector<QOpcUaEndpointDescription> mEndpointList;
    bool                        mClientConnected = false;
    uint32_t                    mReconnect = 0;

    QTimer*                     mTimerReconnect = nullptr;
    QTimer*                     mTimerRead = nullptr;

    QOpcUaApplicationIdentity   m_identity;
    QOpcUaPkiConfiguration      m_pkiConfig;
    QOpcUaEndpointDescription   m_endpoint; // current endpoint used to connect
};

QT_END_NAMESPACE

#endif // OPCUA_VIEW_H
