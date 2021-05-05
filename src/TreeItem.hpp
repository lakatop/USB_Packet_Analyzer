#ifndef TREEITEM_HPP
#define TREEITEM_HPP

#include <qvector.h>
#include <qvariant.h>

/// <summary>
/// Class for representing item in TreeView.
/// </summary>
class TreeItem
{
public:
	TreeItem(QVector<QVariant>& data_, TreeItem* parent_ = nullptr)
		: data(data_), parent(parent_)
	{
	}
	void AppendChild(TreeItem* child);
	TreeItem* Child(int row);
	int ChildCount() const;
	int ColumnCount() const;
	QVariant Data(int column) const;
	int Row() const;
	TreeItem* ParentItem();
private:
	/// <summary>
	/// shared_ptr to all childs of this item.
	/// </summary>
	QVector<std::shared_ptr<TreeItem>> childs;
	/// <summary>
	/// Data that will be presented.
	/// </summary>
	QVector<QVariant> data;
	/// <summary>
	/// Pointer to parent item.
	/// </summary>
	TreeItem* parent;
};

#endif // !TREEITEM_HPP