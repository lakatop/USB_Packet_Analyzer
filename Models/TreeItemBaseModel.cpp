#include "TreeItemBaseModel.h"

/// <summary>
/// Constructor of TreeItemBaseModel
/// </summary>
/// <param name="parent">Dialog <see cref="DataViewer"/></param>
TreeItemBaseModel::TreeItemBaseModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Get index of concrete item in model specified by row, column and parent.
/// </summary>
/// <param name="row">Row of item</param>
/// <param name="column">Column of item</param>
/// <param name="parent">Parent of item</param>
/// <returns>QModelIndex of concrete item specified by parameters</returns>
QModelIndex TreeItemBaseModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    TreeItem* parentItem;

    if (!parent.isValid())
    {
        parentItem = rootItem.get();
    }
    else
    {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }

    TreeItem* childItem = parentItem->Child(row);

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

/// <summary>
/// Get parent of item with given index.
/// </summary>
/// <param name="index">Index of item whose parent we want</param>
/// <returns>QModelIndex which indexes parent of item given by index</returns>
QModelIndex TreeItemBaseModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }
    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->ParentItem();

    if (parentItem == rootItem.get())
    {
        return QModelIndex();
    }

    return createIndex(parentItem->Row(), 0, parentItem);
}

/// <summary>
/// Get number of childs that parent has.
/// </summary>
/// <param name="parent">Index of parent</param>
/// <returns>Size of TreeItem.childs vector</returns>
int TreeItemBaseModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem;

    if (!parent.isValid())
    {
        parentItem = rootItem.get();
    }
    else
    {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }

    return parentItem->ChildCount();
}

/// <summary>
/// Get number of columns.
/// </summary>
/// <param name="parent">Item whose columns we want to count</param>
/// <returns>Size of TreeItem.data vector</returns>
int TreeItemBaseModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return static_cast<TreeItem*>(parent.internalPointer())->ColumnCount();
    }

    return rootItem->ColumnCount();
}

/// <summary>
/// Converts char to hex
/// </summary>
/// <param name="addr">char* to data which we want to convert</param>
/// <param name="len">Length of data which we want to be converted</param>
/// <param name="data">QString in which we will insert converted data</param>
void TreeItemBaseModel::CharToHexConvert(const unsigned char** addr, const unsigned int len, QString& data) const
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
