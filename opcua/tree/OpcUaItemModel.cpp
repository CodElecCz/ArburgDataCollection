#include "OpcUaItemModel.h"
#include "OpcUaTreeItem.h"

#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QIcon>

QT_BEGIN_NAMESPACE

OpcUaItemModel::OpcUaItemModel(QObject *parent) : QAbstractItemModel(parent)
{
}

void OpcUaItemModel::setOpcUaClient(QOpcUaClient *client)
{
    beginResetModel();
    mOpcUaClient = client;
    if (mOpcUaClient)
        mRootItem.reset(new OpcUaTreeItem(client->node("ns=0;i=84"), this /* model */, nullptr /* parent */));
    else
        mRootItem.reset(nullptr);
    endResetModel();
}

QOpcUaClient *OpcUaItemModel::opcUaClient() const
{
    return mOpcUaClient;
}

QVariant OpcUaItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<OpcUaTreeItem *>(index.internalPointer());        

    switch (role)
    {
    case Qt::DisplayRole:
        return item->data(index.column());
    case Qt::DecorationRole:
        if (index.column() == 0)
            return item->icon(index.column());
        break;
    default:
        break;
    }

    return QVariant();
}

QVariant OpcUaItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QStringLiteral("Row %1").arg(section);

    switch (section)
    {
    case 0:
        return QStringLiteral("BrowseName");
    case 1:
        return QStringLiteral("Value");
    case 2:
        return QStringLiteral("NodeClass");
    case 3:
        return QStringLiteral("DataType");
    case 4:
        return QStringLiteral("NodeId");
    case 5:
        return QStringLiteral("DisplayName");
    case 6:
        return QStringLiteral("Description");
    default:
        break;
    }
    return QStringLiteral("Column %1").arg(section);
}

QModelIndex OpcUaItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    OpcUaTreeItem *item = parent.isValid()
        ? static_cast<OpcUaTreeItem*>(parent.internalPointer())->child(row)
        : mRootItem.get();

    return item ? createIndex(row, column, item) : QModelIndex();
}

QModelIndex OpcUaItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<OpcUaTreeItem*>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    if (childItem == mRootItem.get() || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int OpcUaItemModel::rowCount(const QModelIndex &parent) const
{
    if (!mOpcUaClient)
        return 0;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return 1; // only one root item

    auto parentItem = static_cast<OpcUaTreeItem*>(parent.internalPointer());
    return parentItem ? parentItem->childCount() : 0;
}

int OpcUaItemModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<OpcUaTreeItem*>(parent.internalPointer())->columnCount();

    return mRootItem ? mRootItem->columnCount() : 0;
}

QT_END_NAMESPACE
