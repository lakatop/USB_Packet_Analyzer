#include "HexdumpModel.h"

/// <summary>
/// Constructor for DataViewerModel.
/// </summary>
/// <param name="item"><see cref="item"/></param>
/// <param name="hexView"><see cref="hexView"/></param>
/// <param name="additionalDataType"><see cref="additionalDataType"/></param>
/// <param name="parent">Dialog <see cref="DataViewer"/></param>
HexdumpModel::HexdumpModel(QTableWidgetItem* item, bool hexView, HeaderDataType additionalDataType, QObject *parent)
	: QAbstractTableModel(parent)
{
	this->hexView = hexView;
	this->additionaldataType = additionalDataType;
	this->holder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->item = item;
}

/// <summary>
/// Destructor of DataViewerModel.
/// </summary>
HexdumpModel::~HexdumpModel()
{
}

/// <summary>
/// Computes of how many rows hexdump consists.
/// </summary>
/// <param name="parent">Given parent. Not important in this scenario because number of rows is computed from QByteArray length which holds packet data</param>
/// <returns>Number of hexdump rows</returns>
int HexdumpModel::rowCount(const QModelIndex& parent) const
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	int size = leftoverData.size() + sizeof(USBPCAP_BUFFER_PACKET_HEADER);
	if (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid())
	{
		size += item->data(holder->TRANSFER_OPTIONAL_HEADER).toByteArray().size();
	}

	return ((size % holder->BYTES_ON_ROW) == 0) ? size / holder->BYTES_ON_ROW : (size / holder->BYTES_ON_ROW) + 1;
}

/// <summary>
/// Computes of how many columns hexdump consists.
/// </summary>
/// <param name="parent">Given parent. Not important in this scenario because number of columns is fixed to BYTES_ON_ROW variable</param>
/// <returns>BYTES_ON_ROW variable</returns>
int HexdumpModel::columnCount(const QModelIndex& parent) const
{
	return holder->BYTES_ON_ROW; //16B on one line
}

/// <summary>
/// Creates vertical header data = address in hex starting from 0 and incrementing by 16
/// </summary>
/// <param name="section">Row number</param>
/// <param name="orientation">Defines orientation of header</param>
/// <param name="role">Defines Qt role</param>
/// <returns>QVariant with header data</returns>
QVariant HexdumpModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		std::stringstream stream;
		stream << std::uppercase << std::setw(6) << std::setfill('0') << std::hex << section * 16;
		return QVariant(stream.str().c_str());
	}

	return QVariant();
}

/// <summary>
/// Returns data on specified index and role
/// </summary>
/// <param name="index">Index of data</param>
/// <param name="role">Role of data</param>
/// <returns>QVariant on specified index and role of <see cref="item"/></returns>
QVariant HexdumpModel::data(const QModelIndex& index, int role) const
{
	//compute size of data
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	QByteArray usbHeaderData = item->data(holder->USBPCAP_HEADER_DATA).toByteArray();
	int size = leftoverData.size() + usbHeaderData.size();
	QByteArray additionalHeaderData;
	if (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid())
	{
		additionalHeaderData = item->data(holder->TRANSFER_OPTIONAL_HEADER).toByteArray();
		size += additionalHeaderData.size();
	}

	//get index of data that are being requested
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

		//index is in packet header
		if (dataIndex < sizeof(USBPCAP_BUFFER_PACKET_HEADER))
		{
			int hex = usbHeaderData.at(dataIndex);
			QVariant packetDataVariant = GetPacketData(hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,QVariant(HEADER_DATA) });
		}
		//index is in addition packet header data
		else if ((item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid()) &&
			dataIndex < (sizeof(USBPCAP_BUFFER_PACKET_HEADER) + additionalHeaderData.size()))
		{
			int hex = additionalHeaderData.at(dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER));
			QVariant packetDataVariant = GetPacketData(hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,QVariant(ADDITIONAL_HEADER_DATA) });
		}
		//index is in additional packet data
		else
		{
			int additionalDataIndex = (item->data(holder->TRANSFER_OPTIONAL_HEADER).isValid()) ? 
				dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER) - additionalHeaderData.size() :
				dataIndex - sizeof(USBPCAP_BUFFER_PACKET_HEADER);
			QVariant dataRepresantationVariant = GetDataRepresentationType(additionalDataIndex, additionaldataType);
			int hex = leftoverData.at(additionalDataIndex);
			QVariant packetDataVariant = GetPacketData(hexView, hex);
			return QVariant(QVariantList{ packetDataVariant,dataRepresantationVariant });
		}
	}
	else
	{
		return QVariant();
	}
}

/// <summary>
/// Defines of what concerete type are additional packet data. 
/// Mainly implemented because of descriptors that may follow up configuration descriptor since we need to know exactly which it is according to index.
/// </summary>
/// <param name="index">index of data that are being requested</param>
/// <param name="dataRepresentation"><see cref="additionalDataType"/></param>
/// <returns>QVariant with specific descriptor/transfer type</returns>
QVariant HexdumpModel::GetDataRepresentationType(int index, HeaderDataType dataRepresentation) const
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

	return QVariant();
}

/// <summary>
/// Get data which will be presented in hexdump. According to hexView parameter decides
///  whether data will be presented as hex number or printable character
/// </summary>
/// <param name="hexView"><see cref="hexView"/></param>
/// <param name="hex">Concrete data which will be presented</param>
/// <returns>QVariant of data which will be represented in hexdump</returns>
QVariant HexdumpModel::GetPacketData(bool hexView, int hex) const
{
	std::stringstream stream;
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