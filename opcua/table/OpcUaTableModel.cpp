#include "OpcUaTableModel.h"
#include "OpcUaTableItem.h"

#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QTimer>

QT_BEGIN_NAMESPACE

typedef struct _SNode
{
    QString id;
    bool    changeEvent;
} SNode;

namespace
{
const QList<SNode> cNodeList = {
    {"ns=2;i=412862", false },
    {"ns=2;i=412872", false },
    {"ns=2;i=238922", false },
    {"ns=2;i=416422", false },
    {"ns=2;i=239002", true },
    {"ns=2;i=239022", true }
};
}

OpcUaTableModel::OpcUaTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    mTimerChanged(new QTimer(this))
{
    mTimerChanged->setSingleShot(true);

    connect(mTimerChanged, &QTimer::timeout, this, &OpcUaTableModel::timerChanged_timeout);
}

void OpcUaTableModel::setOpcUaClient(QOpcUaClient *client)
{
    beginResetModel();
    mOpcUaClient = client;
    if (mOpcUaClient)
    {
        int index = 0;
        foreach(auto node, cNodeList)
        {
            OpcUaTableItem* item = new OpcUaTableItem(client->node(node.id), this, index, node.changeEvent, -1, this);
            mNodeList.append(item);
            index++;

            connect(item, &OpcUaTableItem::dataChanged, this, &OpcUaTableModel::item_dataChanged);
        }
    }
    else        
    {
        qDeleteAll(mNodeList);
        mNodeList.clear();
    }
    endResetModel();
}

void OpcUaTableModel::opcUaRead()
{
    foreach(auto node, mNodeList)
    {
        node->opcUaRead();
    }
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

void OpcUaTableModel::item_dataChanged()
{
    //hack: wait for all values complete
    mTimerChanged->start(500);
}

void OpcUaTableModel::timerChanged_timeout()
{
    QStringList var;
    QStringList data;
    foreach(auto item, mNodeList)
    {
        QString v = item->data(EColumn_DisplayName).toString();
        if(v.contains("-"))
            v = v.split("-").at(0);

        var.append(v);
        data.append(item->data(EColumn_Value).toString());
    }

    emit itemChanged(var, data);
}

QT_END_NAMESPACE
