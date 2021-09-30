#ifndef OPCUA_TABLE_MODEL_H
#define OPCUA_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QOpcUaNode>
#include <memory>

QT_BEGIN_NAMESPACE

class QOpcUaClient;
class OpcUaTableItem;

class OpcUaTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OpcUaTableModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *);
    QOpcUaClient* opcUaClient() const;

    void opcUaRead();

    typedef enum _EColumn
    {
        EColumn_DisplayName = 0,
        EColumn_Value,
        EColumn_TimeStamp,
        EColumn_Description,
        EColumn_BrowseName,
        EColumn_NodeId,
        EColumn_NodeClass,
        EColumn_DataType,

        /*...*/
        EColumn_Size
    } EColumn;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
    void itemChanged(const QStringList& var, const QStringList& data);

private slots:
    void item_dataChanged();
    void timerChanged_timeout();

private:
    QOpcUaClient*           mOpcUaClient = nullptr;
    QList<OpcUaTableItem*>  mNodeList;
    QTimer*                 mTimerChanged = nullptr;

    friend class OpcUaTableItem;
};

QT_END_NAMESPACE

#endif // OPCUA_TABLE_MODEL_H
