#include "TreeItem.hpp"

/// <summary>
/// Appends another child to this item.
/// </summary>
/// <param name="child">Child which will be appended</param>
void TreeItem::AppendChild(TreeItem* child)
{
	childs.append(std::make_shared<TreeItem>(*child));
}

/// <summary>
/// Gets child on concrete position.
/// </summary>
/// <param name="row">Index to <see cref="childs"/> vector</param>
/// <returns>TreeItem in <see cref="childs"/> on row index</returns>
TreeItem* TreeItem::Child(int row)
{
	if (row < 0 || row >= childs.size())
	{
		return nullptr;
	}

	return childs.at(row).get();
}

/// <summary>
/// Returns number of childs of this item.
/// </summary>
/// <returns>Size of <see cref="childs"/> vector</returns>
int TreeItem::ChildCount() const
{
	return childs.size();
}

/// <summary>
/// Returns number of data columns which this item holds
/// </summary>
/// <returns>Size of <see cref="data"/> vector</returns>
int TreeItem::ColumnCount() const
{
	return data.count();
}

/// <summary>
/// Gets data in given column
/// </summary>
/// <param name="column">Index to <see cref="data"/> vector</param>
/// <returns>QVariant on given column from <see cref="data"/> vector</returns>
QVariant TreeItem::Data(int column) const
{
	if (column < 0 || column >= data.size())
	{
		return QVariant();
	}

	return data.at(column);
}

/// <summary>
/// Get row in which is located this item
/// </summary>
/// <returns>Row of this item</returns>
int TreeItem::Row() const
{
	if (parent != nullptr)
	{
		for (int i = 0; i < parent->childs.size(); i++)
		{
			if (parent->childs.at(i).get() == this)
			{
				return i;
			}
		}
	}

	return 0;
}

/// <summary>
/// Returns parent of this item.
/// </summary>
/// <returns>Parent of this item</returns>
TreeItem* TreeItem::ParentItem()
{
	return parent;
}