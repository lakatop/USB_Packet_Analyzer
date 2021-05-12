#ifndef DATAVIEWERMODEL_HPP
#define DATAVIEWERMODEL_HPP

#include <QObject>

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <qtablewidget.h>
#include <sstream>
#include <iomanip>

/// <summary>
/// Model class for hexdump.
/// </summary>
class HexdumpModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	HexdumpModel(QTableWidgetItem* item, bool hexView, HeaderDataType additionalDataType, QObject *parent = nullptr);
	~HexdumpModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
private:
	QVariant GetDataRepresentationType(int index, HeaderDataType dataRepresentation) const;
	QVariant GetPacketData(bool hexView, int hex) const;

	/// <summary>
	/// Whether model should act as model for hexTabeView(true) or byteTableView(false).
	/// </summary>
	bool hexView;
	/// <summary>
	/// Specific type of data transfer.
	/// </summary>
	HeaderDataType additionaldataType;
	/// <summary>
	/// Pointer to DataHolder class instance.
	/// </summary>
	DataHolder* holder;
	/// <summary>
	/// Pointer to HIDDevice class instance.
	/// </summary>
	HIDDevices* hidDevices;
	/// <summary>
	/// Item which holds packet data.
	/// </summary>
	QTableWidgetItem* item;
};

#endif // !DATAVIEWERMODEL_HPP
