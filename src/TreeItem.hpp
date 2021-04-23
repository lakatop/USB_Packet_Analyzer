#ifndef TREEITEM_HPP
#define TREEITEM_HPP

#include <qvector.h>
#include <qvariant.h>

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
	QVector<std::shared_ptr<TreeItem>> childs;
	QVector<QVariant> data;
	TreeItem* parent;
};

#endif // !TREEITEM_HPP