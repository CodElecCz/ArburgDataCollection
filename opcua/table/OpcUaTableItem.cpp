#include "OpcUaTableItem.h"
#include "OpcUaTableModel.h"

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
#include <QTimer>

QT_BEGIN_NAMESPACE

OpcUaTableItem::OpcUaTableItem(OpcUaTableModel *model, QObject* parent) :
    QObject(parent),
    mModel(model)
{
}

OpcUaTableItem::OpcUaTableItem(QOpcUaNode *node, OpcUaTableModel *model, int row, bool changeEvent, int msec, QObject* parent) :
    QObject(parent),
    mOpcNode(node),
    mModel(model),
    mRow(row),
    mTimer(new QTimer(this)),
    mChangeEvent(changeEvent)
{    
    mNodeId = node->nodeId();

    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &OpcUaTableItem::handleAttributes);
    connect(mTimer, &QTimer::timeout, this, &OpcUaTableItem::timer_timeout);

    opcUaRead();

    if(msec>0)
    {
        mTimer->setSingleShot(false);
        mTimer->start(msec);
    }
}

OpcUaTableItem::~OpcUaTableItem()
{    
    delete mTimer;
}


void OpcUaTableItem::opcUaRead()
{
    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::BrowseName
                            | QOpcUa::NodeAttribute::NodeClass
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
                            | QOpcUa::NodeAttribute::Value
                            | QOpcUa::NodeAttribute::DisplayName
                            ))
    {
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
    }
}

void OpcUaTableItem::timer_timeout()
{
    opcUaRead();
}

QVariant OpcUaTableItem::data(int column)
{
    switch(column)
    {
    case OpcUaTableModel::EColumn::EColumn_BrowseName:
        return mNodeBrowseName;
    case OpcUaTableModel::EColumn::EColumn_Value:
        if (!mAttributesReady)
            return tr("Loading ...");

        return mNodeValue;
    case OpcUaTableModel::EColumn::EColumn_NodeClass:
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
            QString name = metaEnum.valueToKey(int(mNodeClass));
            return name + " (" + QString::number(int(mNodeClass)) + ')';
        }
    case OpcUaTableModel::EColumn::EColumn_DataType:
        {
            if (!mAttributesReady)
                return tr("Loading ...");

            const QString typeId = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
            auto enumEntry = QOpcUa::namespace0IdFromNodeId(typeId);
            if (enumEntry == QOpcUa::NodeIds::Namespace0::Unknown)
                return typeId;

            return QOpcUa::namespace0IdName(enumEntry) + " (" + typeId + ")";
        }
    case OpcUaTableModel::EColumn::EColumn_NodeId:
        return mNodeId;
    case OpcUaTableModel::EColumn::EColumn_DisplayName:
        return mNodeDisplayName;
    case OpcUaTableModel::EColumn::EColumn_Description:
        return mAttributesReady
            ? mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>().text()
            : tr("Loading ...");
    case OpcUaTableModel::EColumn::EColumn_TimeStamp:
        if (!mAttributesReady)
            return tr("Loading ...");

        return mTimeStamp.toString("hh:mm:ss.zzz yyyy.MM.dd");
    }
    return QVariant();
}

void OpcUaTableItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();

    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();

    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text();

    const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
    const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);

    QString nodeValue = variantToString(value, type);
    nodeValue = nodeValue.simplified();

    mTimeStamp = QDateTime::currentDateTime();

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(mRow, 0, this), mModel->createIndex(mRow, mModel->columnCount() - 1, this));

    if(mChangeEvent)
    {
        if(nodeValue != mNodeValue)
        {
            mNodeValue = nodeValue;
            emit dataChanged();
        }
    }
    else
        mNodeValue = nodeValue;
}

QString OpcUaTableItem::variantToString(const QVariant &value, const QString &typeNodeId) const
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

QString OpcUaTableItem::localizedTextToString(const QOpcUaLocalizedText &text) const
{
    return QStringLiteral("[Locale: \"%1\", Text: \"%2\"]").arg(text.locale()).arg(text.text());
}

QString OpcUaTableItem::rangeToString(const QOpcUaRange &range) const
{
    return QStringLiteral("[Low: %1, High: %2]").arg(range.low()).arg(range.high());
}

QString OpcUaTableItem::euInformationToString(const QOpcUaEUInformation &info) const
{
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]").arg(info.unitId()).arg(
                info.namespaceUri()).arg(localizedTextToString(info.displayName())).arg(localizedTextToString(info.description()));
}

QT_END_NAMESPACE
