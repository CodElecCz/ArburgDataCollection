#include "OpcUaTableModel.h"
#include "OpcUaTableItem.h"

#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QIcon>

QT_BEGIN_NAMESPACE

namespace
{
const QStringList cNodeList = {
    "ns=2;i=412862",
    "ns=2;i=412872",
    "ns=2;i=239002",
    "ns=2;i=239022",
    "ns=2;i=238922",
    "ns=2;i=416422"
};
}

OpcUaTableModel::OpcUaTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void OpcUaTableModel::setOpcUaClient(QOpcUaClient *client)
{
    beginResetModel();
    mOpcUaClient = client;
    if (mOpcUaClient)
    {
        int index = 0;
        foreach(auto nodeId, cNodeList)
        {
            OpcUaTableItem* item = new OpcUaTableItem(client->node(nodeId), this, index, this);
            mNodeList.append(item);
            index++;
        }
    }
    else        
    {
        foreach(auto node, mNodeList)
        {
            delete node;
        }
        mNodeList.clear();
    }
    endResetModel();
}

QOpcUaClient *OpcUaTableModel::opcUaClient() const
{
    return mOpcUaClient;
}

QVariant OpcUaTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = mNodeList[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
        return item->data(index.column());
    default:
        break;
    }

    return QVariant();
}

QVariant OpcUaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QStringLiteral("Row %1").arg(section);

    switch (section)
    {
    case EColumn_BrowseName:
        return QStringLiteral("BrowseName");
    case EColumn_Value:
        return QStringLiteral("Value");
    case EColumn_NodeClass:
        return QStringLiteral("NodeClass");
    case EColumn_DataType:
        return QStringLiteral("DataType");
    case EColumn_NodeId:
        return QStringLiteral("NodeId");
    case EColumn_DisplayName:
        return QStringLiteral("DisplayName");
    case EColumn_Description:
        return QStringLiteral("Description");
    case EColumn_TimeStamp:
        return QStringLiteral("TimeStamp");
    default:
        break;
    }
    return QStringLiteral("Column %1").arg(section);
}

int OpcUaTableModel::rowCount(const QModelIndex &parent) const
{
    return mNodeList.size();
}

int OpcUaTableModel::columnCount(const QModelIndex &parent) const
{
    return EColumn_Size;
}

QT_END_NAMESPACE
