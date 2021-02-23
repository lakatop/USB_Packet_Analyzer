#include "DataViewerModel.h"

DataViewerModel::DataViewerModel(QListWidgetItem* item, bool hexView, quint8 additionalDataType, QObject *parent)
	: QAbstractTableModel(parent)
{
	this->hexView = hexView;
	this->additionaldataType = additionalDataType;
	this->holder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->item = item;
}

DataViewerModel::~DataViewerModel()
{
}

int DataViewerModel::rowCount(const QModelIndex& parent) const
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	int size = leftoverData.size() + sizeof(USBPCAP_BUFFER_PACKET_HEADER);
	if (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid())
	{
		size += item->data(holder->TRANSFER_OPTIONAL_HEADER).toByteArray().size();
	}

	return ((size / holder->BYTES_ON_ROW) == 0) ? size / holder->BYTES_ON_ROW : (size / holder->BYTES_ON_ROW) + 1;
}

int DataViewerModel::columnCount(const QModelIndex& parent) const
{
	return holder->BYTES_ON_ROW; //16B on one line
}

QVariant DataViewerModel::data(const QModelIndex& index, int role) const
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	QByteArray usbHeaderData = item->data(holder->USBPCAP_HEADER_DATA).toByteArray();
	int size = leftoverData.size() + usbHeaderData.size();
	QByteArray additionalHeaderData;
	if (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid())
	{
		additionalHeaderData = item->data(holder->TRANSFER_OPTIONAL_HEADER).toByteArray();
		size += additionalHeaderData.size();
	}
	int dataIndex = (index.row() * holder->BYTES_ON_ROW) + index.column();

	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	if (role == Qt::DisplayRole)
	{
		if (!index.isValid())
		{
			return QVariant();
		}
		if (dataIndex >= size)
		{
			return QVariant();
		}

		std::stringstream stream;
		if (dataIndex < sizeof(USBPCAP_BUFFER_PACKET_HEADER))
		{
			int hex = usbHeaderData.at(dataIndex);
			QVariant packetDataVariant = GetPacketData(stream, hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,QVariant(HEADER_DATA) });
		}
		else if ((item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid()) &&
			dataIndex < (sizeof(USBPCAP_BUFFER_PACKET_HEADER) + additionalHeaderData.size()))
		{
			int hex = additionalHeaderData.at(dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER));
			QVariant packetDataVariant = GetPacketData(stream, hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,QVariant(ADDITIONAL_HEADER_DATA) });
		}
		else
		{
			int additionalDataIndex = (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid()) ? 
				dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER) - additionalHeaderData.size() :
				dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER);
			QVariant dataRepresantationVariant = GetDataRepresentationType(additionalDataIndex, additionaldataType);
			int hex = leftoverData.at(additionalDataIndex);
			QVariant packetDataVariant = GetPacketData(stream, hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,dataRepresantationVariant });
		}
	}
	else
	{
		return QVariant();
	}
}

QVariant DataViewerModel::GetDataRepresentationType(int index, qint8 dataRepresentation) const
{
	switch (dataRepresentation)
	{
	case INTERR_TRANSFER:
	case ISOCHRO_TRANSFER:
	case BULK_TRANSFER:
	case CONTROL_TRANSFER_RESPONSE:
	case CONTROL_TRANSFER_SETUP:
	case CONTROL_TRANSFER_DEVICE_DESC:
	case CONTROL_TRANSFER_STRING_DESC:
	case CONTROL_TRANSFER_HID_REPORT_DESC:
	{
		return QVariant(dataRepresentation);
	}
	default: //this represents configuration descriptor. But it may also consist many interface/hid/endpoint descriptors as well
	{
		if (index < sizeof(USB_CONFIGURATION_DESCRIPTOR))
		{
			return QVariant(CONTROL_TRANSFER_CONFIG_DESC);
		}

		QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
		unsigned char* packet = (unsigned char*)leftoverData.data();
		packet += sizeof(USB_CONFIGURATION_DESCRIPTOR);
		unsigned int currentPosition = sizeof(USB_CONFIGURATION_DESCRIPTOR);

		while (currentPosition != leftoverData.size())
		{
			BYTE descriptorType = *(++packet);
			packet--;
			switch (descriptorType)
			{
			case HID_DESCRIPTOR_ENUM:
			{
				currentPosition += hidDevices->GetHIDDescriptorSize();
				if (currentPosition > index)
				{
					return QVariant(CONTROL_TRANSFER_HID_DESC);
				}
				packet += hidDevices->GetHIDDescriptorSize();
			}
			break;
			case INTERFACE_DESCRIPTOR:
			{
				currentPosition += sizeof(USB_INTERFACE_DESCRIPTOR);
				if (currentPosition > index)
				{
					return QVariant(CONTROL_TRANSFER_INTERF_DESC);
				}
				packet += sizeof(USB_INTERFACE_DESCRIPTOR);
			}
			break;
			case ENDPOINT_DESCRIPTOR:
			{
				currentPosition += sizeof(USB_ENDPOINT_DESCRIPTOR);
				if (currentPosition > index)
				{
					return QVariant(CONTROL_TRANSFER_ENDPOI_DESC);
				}
				packet += sizeof(USB_ENDPOINT_DESCRIPTOR);
			}
			break;
			default:
			{
				byte size = (byte)(*packet);
				currentPosition += size; // first byte of packet is representing size of descriptor
				if (currentPosition > index)
				{
					return QVariant(CONTROL_TRANSFER_UNSPEC_DESC);
				}
				packet += size;
			}
			break;
			}
		}
	}
	break;
	}
}
QVariant DataViewerModel::GetPacketData(std::stringstream& stream, bool hexView, int hex) const
{
	hex = (hex < 0) ? hex + 256 : hex;
	if (hexView)
	{
		if (hex < 0x20 || hex > 0x7e)
		{
			return QVariant(".");
		}
		else
		{
			return QVariant(QString(QChar(hex)));
		}
	}
	stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << hex;
	return QVariant(stream.str().c_str());
}