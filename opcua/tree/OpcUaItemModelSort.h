#ifndef OPCUA_ITEM_MODELSORT_H
#define OPCUA_ITEM_MODELSORT_H

#include <QSortFilterProxyModel>

class OpcUaItemModelSort : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    OpcUaItemModelSort(QObject *parent = nullptr);

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const override;
};

#endif // OPCUA_ITEM_MODELSORT_H
