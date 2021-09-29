#include "OpcUaTreeItem.h"
#include "OpcUaItemModel.h"

#include <QOpcUaArgument>
#include <QOpcUaAxisInformation>
#include <QOpcUaClient>
#include <QOpcUaComplexNumber>
#include <QOpcUaDoubleComplexNumber>
#include <QOpcUaEUInformation>
#include <QOpcUaExtensionObject>
#include <QOpcUaLocalizedText>
#include <QOpcUaQualifiedName>
#include <QOpcUaRange>
#include <QOpcUaXValue>
#include <QMetaEnum>
#include <QPixmap>

QT_BEGIN_NAMESPACE

#define COLUMN_NODECLASS    1
#define COLUMN_VALUE        0

const int numberOfDisplayColumns = 7; // NodeId, Value, NodeClass, DataType, BrowseName, DisplayName, Description

OpcUaTreeItem::OpcUaTreeItem(OpcUaItemModel *model) :
    QObject(nullptr),
    mModel(model)
{
}

OpcUaTreeItem::OpcUaTreeItem(QOpcUaNode *node, OpcUaItemModel *model, OpcUaTreeItem *parent) :
    QObject(parent),
    mOpcNode(node),
    mModel(model),
    mParentItem(parent)
{    
    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &OpcUaTreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &OpcUaTreeItem::browseFinished);

    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::BrowseName
#if COLUMN_NODECLASS
                            | QOpcUa::NodeAttribute::NodeClass
#endif
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
#if COLUMN_VALUE
                            | QOpcUa::NodeAttribute::Value
#endif
                            | QOpcUa::NodeAttribute::DisplayName
                            ))
    {
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
    }
}

OpcUaTreeItem::OpcUaTreeItem(QOpcUaNode *node, OpcUaItemModel *model, const QOpcUaReferenceDescription &browsingData, OpcUaTreeItem *parent) :
    OpcUaTreeItem(node, model, parent)
{
    mNodeBrowseName = browsingData.browseName().name();
 #if COLUMN_NODECLASS
    mNodeClass = browsingData.nodeClass();
#endif
    mNodeId = browsingData.targetNodeId().nodeId();
    mNodeDisplayName = browsingData.displayName().text();
}

OpcUaTreeItem::~OpcUaTreeItem()
{
    qDeleteAll(mChildItems);
}

OpcUaTreeItem *OpcUaTreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "OpcUaTreeItem in row" << row << "does not exist.";

    return mChildItems[row];
}

int OpcUaTreeItem::childIndex(const OpcUaTreeItem *child) const
{
    return mChildItems.indexOf(const_cast<OpcUaTreeItem *>(child));
}

int OpcUaTreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

int OpcUaTreeItem::columnCount() const
{
    return numberOfDisplayColumns;
}

QVariant OpcUaTreeItem::data(int column)
{
    switch(column)
    {
    case 0:
        return mNodeBrowseName;
    case 1:
#if COLUMN_VALUE
        {
            if (!mAttributesReady)
                return tr("Loading ...");

            const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
            const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);

            return variantToString(value, type);
        }
#else
        break;
#endif
    case 2:
#if COLUMN_NODECLASS
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
            QString name = metaEnum.valueToKey(int(mNodeClass));
            return name + " (" + QString::number(int(mNodeClass)) + ')';
        }
#else
        break;
#endif
    case 3:
        {
            if (!mAttributesReady)
                return tr("Loading ...");

            const QString typeId = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
            auto enumEntry = QOpcUa::namespace0IdFromNodeId(typeId);
            if (enumEntry == QOpcUa::NodeIds::Namespace0::Unknown)
                return typeId;

            return QOpcUa::namespace0IdName(enumEntry) + " (" + typeId + ")";
        }
    case 4:
        return mNodeId;
    case 5:
        return mNodeDisplayName;
    case 6:
        return mAttributesReady
            ? mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>().text()
            : tr("Loading ...");        
    }
    return QVariant();
}

int OpcUaTreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

OpcUaTreeItem *OpcUaTreeItem::parentItem()
{
    return mParentItem;
}

void OpcUaTreeItem::appendChild(OpcUaTreeItem *child)
{
    if (!child)
        return;

    if (!hasChildNodeItem(child->mNodeId))
    {
        mChildItems.append(child);
        mChildNodeIds.insert(child->mNodeId);
    } else
    {
        child->deleteLater();
    }
}

static QPixmap createPixmap(const QColor &c)
{
    QPixmap p(10,10);
    p.fill(c);
    return p;
}

QPixmap OpcUaTreeItem::icon(int column) const
{
    if (column != 0 || !mOpcNode)
        return QPixmap();

    static const QPixmap defaultPixmap = createPixmap(Qt::gray);

#if COLUMN_NODECLASS
    static const QPixmap objectPixmap = createPixmap(Qt::darkGreen);
    static const QPixmap variablePixmap = createPixmap(Qt::darkBlue);
    static const QPixmap methodPixmap = createPixmap(Qt::darkRed);    

    switch (mNodeClass)
    {
    case QOpcUa::NodeClass::Object:
        return objectPixmap;
    case QOpcUa::NodeClass::Variable:
        return variablePixmap;
    case QOpcUa::NodeClass::Method:
        return methodPixmap;
    default:
        break;
    }
#endif

    return defaultPixmap;
}

bool OpcUaTreeItem::hasChildNodeItem(const QString &nodeId) const
{
    return mChildNodeIds.contains(nodeId);
}

void OpcUaTreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void OpcUaTreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
#if COLUMN_NODECLASS
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
#endif
    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();

    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text();

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

void OpcUaTreeItem::browseFinished(const QVector<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good)
    {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children)
    {
        if (hasChildNodeItem(item.targetNodeId().nodeId()))
            continue;

        auto node = mModel->opcUaClient()->node(item.targetNodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
            continue;
        }

        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + 1);
        appendChild(new OpcUaTreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

QString OpcUaTreeItem::variantToString(const QVariant &value, const QString &typeNodeId) const
{
    if (value.type() == QVariant::List)
    {
        const auto list = value.toList();
        QString concat;
        for (int i = 0, size = list.size(); i < size; ++i) {
            if (i)
                concat.append(QLatin1Char('\n'));
            concat.append(variantToString(list.at(i), typeNodeId));
        }
        return concat;
    }

    if (typeNodeId == QLatin1String("ns=0;i=19")) { // StatusCode
        const char *name = QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(value.toInt());
        return name ? QLatin1String(name) : QLatin1String("Unknown StatusCode");
    }
    if (typeNodeId == QLatin1String("ns=0;i=2")) // Char
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=3")) // SChar
        return QString::number(value.toUInt());
    if (typeNodeId == QLatin1String("ns=0;i=4")) // Int16
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=5")) // UInt16
        return QString::number(value.toUInt());
    if (value.type() == QVariant::ByteArray)
        return QLatin1String("0x") + value.toByteArray().toHex();
    if (value.type() == QVariant::DateTime)
        return value.toDateTime().toString(Qt::ISODate);
    if (value.canConvert<QOpcUaQualifiedName>()) {
        const auto name = value.value<QOpcUaQualifiedName>();
        return QStringLiteral("[NamespaceIndex: %1, Name: \"%2\"]").arg(name.namespaceIndex()).arg(name.name());
    }
    if (value.canConvert<QOpcUaLocalizedText>()) {
        const auto text = value.value<QOpcUaLocalizedText>();
        return localizedTextToString(text);
    }
    if (value.canConvert<QOpcUaRange>()) {
        const auto range = value.value<QOpcUaRange>();
        return rangeToString(range);
    }
    if (value.canConvert<QOpcUaComplexNumber>()) {
        const auto complex = value.value<QOpcUaComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUaDoubleComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaXValue>()) {
        const auto xv = value.value<QOpcUaXValue>();
        return QStringLiteral("[X: %1, Value: %2]").arg(xv.x()).arg(xv.value());
    }
    if (value.canConvert<QOpcUaEUInformation>()) {
        const auto info = value.value<QOpcUaEUInformation>();
        return euInformationToString(info);
    }
    if (value.canConvert<QOpcUaAxisInformation>()) {
        const auto info = value.value<QOpcUaAxisInformation>();
        return QStringLiteral("[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, AxisSteps: %5]").arg(
                    euInformationToString(info.engineeringUnits())).arg(rangeToString(info.eURange())).arg(localizedTextToString(info.title())).arg(
                        info.axisScaleType() == QOpcUa::AxisScale::Linear ? "Linear" : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln" : "Log").arg(
                        numberArrayToString(info.axisSteps()));
    }
    if (value.canConvert<QOpcUaExpandedNodeId>()) {
        const auto id = value.value<QOpcUaExpandedNodeId>();
        return QStringLiteral("[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]").arg(
                    id.nodeId()).arg(id.serverIndex()).arg(id.namespaceUri());
    }
    if (value.canConvert<QOpcUaArgument>()) {
        const auto a = value.value<QOpcUaArgument>();

        return QStringLiteral("[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", ArrayDimensions: %4, Description: %5]").arg(
                    a.name(), a.dataTypeId()).arg(a.valueRank()).arg(numberArrayToString(a.arrayDimensions()),
                    localizedTextToString(a.description()));
    }
    if (value.canConvert<QOpcUaExtensionObject>()) {
        const auto obj = value.value<QOpcUaExtensionObject>();
        return QStringLiteral("[TypeId: \"%1\", Encoding: %2, Body: 0x%3]").arg(obj.encodingTypeId(),
                    obj.encoding() == QOpcUaExtensionObject::Encoding::NoBody ?
                        "NoBody" : (obj.encoding() == QOpcUaExtensionObject::Encoding::ByteString ?
                            "ByteString" : "XML")).arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
}

QString OpcUaTreeItem::localizedTextToString(const QOpcUaLocalizedText &text) const
{
    return QStringLiteral("[Locale: \"%1\", Text: \"%2\"]").arg(text.locale()).arg(text.text());
}

QString OpcUaTreeItem::rangeToString(const QOpcUaRange &range) const
{
    return QStringLiteral("[Low: %1, High: %2]").arg(range.low()).arg(range.high());
}

QString OpcUaTreeItem::euInformationToString(const QOpcUaEUInformation &info) const
{
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]").arg(info.unitId()).arg(
                info.namespaceUri()).arg(localizedTextToString(info.displayName())).arg(localizedTextToString(info.description()));
}

QT_END_NAMESPACE
