#ifndef TREEITEMBASEMODEL_HPP
#define TREEITEMBASEMODEL_HPP

#include <QAbstractItemModel>

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../src/TreeItem.hpp"

#include <qcolor.h>
#include <sstream>
#include <iomanip>

/// <summary>
/// Base class for tree item models.
/// </summary>
class TreeItemBaseModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TreeItemBaseModel(QObject *parent);

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role) const = 0;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const = 0;

	void CharToHexConvert(const unsigned char** addr, const unsigned int len, QString& data);
	template <typename T>
	QString ShowBits(uint32_t start, size_t size, T number);

protected:
	QColor GetDataTypeColor(HeaderDataType dataType) const;

	/// <summary>
	/// unique_ptr to root tree item.
	/// </summary>
	std::unique_ptr<TreeItem> rootItem;
	/// <summary>
	/// Pointer to DataHolder instance
	/// </summary>
	DataHolder* holder;
};

/// <summary>
/// Method used to create and return QString which shows individual bits of given value.
/// </summary>
/// <typeparam name="T">Type of value for analysis</typeparam>
/// <param name="start">starting point of showing bits</param>
/// <param name="size">how many bits should be shown</param>
/// <param name="number">Number which will be analyzed</param>
/// <returns>QString of analyzed number which shows its individual bits</returns>
template <typename T>
QString TreeItemBaseModel::ShowBits(uint32_t start, size_t size, T number)
{
	std::stringstream stream;
	for (int i = 0; i < sizeof(T) * 8; i++)
	{
		if (i != 0 && i % 4 == 0)
		{
			stream << ' ';
		}
		if (i < start || i >= start + size)
		{
			stream << std::setw(2) << std::setfill(' ') << '.';
			number = number << 1;
			continue;
		}
		if (number & (0x1 << ((sizeof(T) * 8) - 1)))
		{
			stream << '1';
		}
		else
		{
			stream << '0';
		}
		number = number << 1;
	}

	return QString(stream.str().c_str());
}

#endif // !TREEITEMBASEMODEL_HPP