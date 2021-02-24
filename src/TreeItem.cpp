#include "TreeItem.hpp"

void TreeItem::AppendChild(TreeItem* child)
{
	childs.append(std::make_shared<TreeItem>(*child));
}

TreeItem* TreeItem::Child(int row)
{
	if (row < 0 || row >= childs.size())
	{
		return nullptr;
	}

	return childs.at(row).get();
}
int TreeItem::ChildCount() const
{
	return childs.size();
}
int TreeItem::ColumnCount() const
{
	return data.count();
}
QVariant TreeItem::Data(int column) const
{
	if (column < 0 || column >= data.size())
	{
		return QVariant();
	}

	return data.at(column);
}
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
TreeItem* TreeItem::ParentItem()
{
	return parent;
}