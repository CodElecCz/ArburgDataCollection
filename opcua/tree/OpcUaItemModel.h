#ifndef OPCUA_ITEM_MODEL_H
#define OPCUA_ITEM_MODEL_H

#include "OpcUaTreeItem.h"
#include <QAbstractItemModel>
#include <QOpcUaNode>
#include <memory>

QT_BEGIN_NAMESPACE

class QOpcUaClient;
class OpcUaTreeItem;

class OpcUaItemModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    OpcUaItemModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *);
    QOpcUaClient* opcUaClient() const;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<OpcUaTreeItem> mRootItem;

    friend class OpcUaTreeItem;
};

QT_END_NAMESPACE

#endif // OPCUA_ITEM_MODEL_H
