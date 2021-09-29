#ifndef OPCUA_TREEITEM_H
#define OPCUA_TREEITEM_H

#include <QObject>
#include <QOpcUaNode>
#include <memory>

QT_BEGIN_NAMESPACE

class OpcUaItemModel;
class QOpcUaRange;
class QOpcUaEUInformation;

class OpcUaTreeItem : public QObject
{
    Q_OBJECT
public:
    explicit OpcUaTreeItem(OpcUaItemModel *model);
    OpcUaTreeItem(QOpcUaNode *node, OpcUaItemModel *model, OpcUaTreeItem *parent);
    OpcUaTreeItem(QOpcUaNode *node, OpcUaItemModel *model, const QOpcUaReferenceDescription &browsingData, OpcUaTreeItem *parent);
    ~OpcUaTreeItem();
    OpcUaTreeItem *child(int row);
    int childIndex(const OpcUaTreeItem *child) const;
    int childCount();
    int columnCount() const;
    QVariant data(int column);
    int row() const;
    OpcUaTreeItem *parentItem();
    void appendChild(OpcUaTreeItem *child);
    QPixmap icon(int column) const;
    bool hasChildNodeItem(const QString &nodeId) const;

private slots:
    void startBrowsing();
    void handleAttributes(QOpcUa::NodeAttributes attr);
    void browseFinished(const  QVector<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode);

private:
    QString variantToString(const QVariant &value, const QString &typeNodeId = QString()) const;
    QString localizedTextToString(const QOpcUaLocalizedText &text) const;
    QString rangeToString(const QOpcUaRange &range) const;
    QString euInformationToString(const QOpcUaEUInformation &info) const;
    template <typename T>
    QString numberArrayToString(const QVector<T> &vec) const;

    std::unique_ptr<QOpcUaNode> mOpcNode;
    OpcUaItemModel*             mModel = nullptr;
    bool                        mAttributesReady = false;
    bool                        mBrowseStarted = false;
    QList<OpcUaTreeItem *>      mChildItems;
    QSet<QString>               mChildNodeIds;
    OpcUaTreeItem*              mParentItem = nullptr;

private:
    QString mNodeBrowseName;
    QString mNodeId;
    QString mNodeDisplayName;
    QOpcUa::NodeClass mNodeClass = QOpcUa::NodeClass::Undefined;
};

template <typename T>
QString OpcUaTreeItem::numberArrayToString(const QVector<T> &vec) const
{
    QString list(QLatin1Char('['));
    for (int i = 0, size = vec.size(); i < size; ++i) {
        if (i)
            list.append(QLatin1Char(';'));
        list.append(QString::number(vec.at(i)));
    }
    list.append(QLatin1Char(']'));
    return list;
}

QT_END_NAMESPACE

#endif // OPCUA_TREEITEM_H
