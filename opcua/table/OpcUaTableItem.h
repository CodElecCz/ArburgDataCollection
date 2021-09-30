#ifndef OPCUA_TABLE_ITEM_H
#define OPCUA_TABLE_ITEM_H

#include <QObject>
#include <QOpcUaNode>
#include <memory>

QT_BEGIN_NAMESPACE

class OpcUaTableModel;
class QOpcUaRange;
class QOpcUaEUInformation;

class OpcUaTableItem : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaTableItem(OpcUaTableModel *model, QObject* parent = nullptr);
    OpcUaTableItem(QOpcUaNode *node, OpcUaTableModel *model, int row, bool changeEvent, int msec = 4000, QObject* parent = nullptr);
    ~OpcUaTableItem();

    QVariant data(int column);

    void opcUaRead();

signals:
    void dataChanged();

private slots:    
    void handleAttributes(QOpcUa::NodeAttributes attr);
    void timer_timeout();

private:
    QString variantToString(const QVariant &value, const QString &typeNodeId = QString()) const;
    QString localizedTextToString(const QOpcUaLocalizedText &text) const;
    QString rangeToString(const QOpcUaRange &range) const;
    QString euInformationToString(const QOpcUaEUInformation &info) const;
    template <typename T>
    QString numberArrayToString(const QVector<T> &vec) const;

private:
    std::unique_ptr<QOpcUaNode> mOpcNode;
    OpcUaTableModel*            mModel = nullptr;
    int                         mRow = -1;
    bool                        mAttributesReady = false;
    QTimer*                     mTimer = nullptr;
    bool                        mChangeEvent = false;

    QString mNodeBrowseName;
    QString mNodeId;
    QString mNodeDisplayName;
    QString mNodeValue;
    QDateTime mTimeStamp;
    QOpcUa::NodeClass mNodeClass = QOpcUa::NodeClass::Undefined;
};

template <typename T>
QString OpcUaTableItem::numberArrayToString(const QVector<T> &vec) const
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

#endif // OPCUA_TABLE_ITEM_H
