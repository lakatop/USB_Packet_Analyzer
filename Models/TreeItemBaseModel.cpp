#include "TreeItemBaseModel.h"

TreeItemBaseModel::TreeItemBaseModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	holder = DataHolder::GetDataHolder();
}

QModelIndex TreeItemBaseModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem.get();
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* childItem = parentItem->Child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeItemBaseModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }
    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->ParentItem();

    if (parentItem == rootItem.get())
        return QModelIndex();

    return createIndex(parentItem->Row(), 0, parentItem);
}

int TreeItemBaseModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem.get();
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->ChildCount();
}

int TreeItemBaseModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->ColumnCount();
    return rootItem->ColumnCount();
}

QColor TreeItemBaseModel::GetDataTypeColor(HeaderDataType dataType) const
{
    DataTypeColor col = holder->DataColors[dataType];
    return QColor::fromRgb(col.red, col.green, col.blue, col.alpha);
}

void TreeItemBaseModel::CharToHexConvert(const unsigned char** addr, const unsigned int len, QString& data)
{
    std::stringstream stream;
    data.clear();
    for (int i = 0; i < len; i++)
    {
        int hex = **addr;
        stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << hex << " ";
        *addr += 1;
    }
    stream << "\t";
    data.append(stream.str().c_str());
}
