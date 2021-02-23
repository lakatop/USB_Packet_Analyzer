#pragma once

#include <QObject>

#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <qlistwidget.h>
#include <sstream>
#include <iomanip>

class DataViewerModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	DataViewerModel(QListWidgetItem* item, bool hexView, quint8 additionalDataType, QObject *parent = nullptr);
	~DataViewerModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
private:
	QVariant GetDataRepresentationType(int index, qint8 dataRepresentation) const;
	QVariant GetPacketData(std::stringstream& stream, bool hexView, int hex) const;

	bool hexView;
	quint8 additionaldataType;
	DataHolder* holder;
	HIDDevices* hidDevices;
	QListWidgetItem* item;
};
