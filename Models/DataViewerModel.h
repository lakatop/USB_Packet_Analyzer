#ifndef DATAVIEWERMODEL_HPP
#define DATAVIEWERMODEL_HPP

#include <QObject>

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <qtablewidget.h>
#include <sstream>
#include <iomanip>

class DataViewerModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	DataViewerModel(QTableWidgetItem* item, bool hexView, HeaderDataType additionalDataType, QObject *parent = nullptr);
	~DataViewerModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
private:
	QVariant GetDataRepresentationType(int index, qint8 dataRepresentation) const;
	QVariant GetPacketData(std::stringstream& stream, bool hexView, int hex) const;

	bool hexView;
	HeaderDataType additionaldataType;
	DataHolder* holder;
	HIDDevices* hidDevices;
	QTableWidgetItem* item;
};

#endif // !DATAVIEWERMODEL_HPP
