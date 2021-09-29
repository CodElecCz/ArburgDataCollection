#include "OpcUaItemModelSort.h"

#include <QDebug>

namespace
{
const QStringList cFilter = {"Root" , "Objects", "ProductionControl", "Value"};
}

OpcUaItemModelSort::OpcUaItemModelSort(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

bool OpcUaItemModelSort::filterAcceptsRow(int row, const QModelIndex & parent) const
{
    QModelIndex index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return false;
#if 0
    QString nodeName = index.data().toString();

    if(cFilter.contains(nodeName))
        return true;

    int rows = sourceModel()->rowCount(index);
    for (row = 0; row < rows; row++)
    {
        if (filterAcceptsRow(row, index))
        {
            return true;
        }
    }

    return false;
#else
    return true;
#endif
}
