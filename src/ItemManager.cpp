#include "ItemManager.hpp"

ItemManager* ItemManager::itemManager = nullptr;

/// <summary>
/// Static method to get instance of this class.
/// </summary>
/// <param name="tableWidget"><see cref="tableWidget"/></param>
/// <param name="parent"><see cref="parent"/></param>
/// <returns></returns>
ItemManager* ItemManager::GetItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent)
{
	if (itemManager == nullptr)
	{
		itemManager = new ItemManager(tableWidget, parent);
	}

	return itemManager;
}

/// <summary>
/// Constructor for ItemManager class.
/// </summary>
/// <param name="tableWidget"><see cref="tableWidget"/></param>
/// <param name="parent"><see cref="parent"/></param>
ItemManager::ItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->representingHIDReportDescriptor = false;
	this->representingConfigurationDescriptor = false;
	this->itemIndex = 0;
	this->parent = parent;
	this->tableWidget = tableWidget;
	processingFile = false;
	this->dataHolder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
}

/// <summary>
/// Sequentially processing pcap file and extracting packet data into individual items
/// </summary>
/// <param name="filename">Name of file to be processed</param>
/// <param name="liveReading">Whether we are processing live capture or not</param>
void ItemManager::ProcessFile(QString filename, bool liveReading)
{
	if (fileReader.OpenNewFile(filename))
	{
		if (fileReader.ReadFileHeader())
		{
			if (liveReading)
			{
				//start timer to 1 sec interval. Then updateLiveFile() will be called every timer tick.
				parent->timer->start(1000);
			}
			ProcessFileTillEnd(liveReading);
			parent->Refresh();
		}
	}
}

/// <summary>
/// Contiunes processing file from 
/// </summary>
/// <param name="liveReading"></param>
void ItemManager::ProcessFileTillEnd(bool liveReading)
{
	qint64 value = sizeof(pcap_hdr_t); //global file header size
	processingFile = true;
	if (!liveReading)
	{
		parent->GetProgressBar()->setRange(0, fileReader.FileSize());
	}
	while (!fileReader.EndOfFile())
	{
		if (stopButtonClicked)
		{
			return;
		}

		QByteArray packetData = fileReader.GetPacket();
		//check for errors while reading file
		if (packetData.isEmpty())
		{
			return;
		}
		if (!liveReading)
		{
			value += packetData.size() + sizeof(pcaprec_hdr_t); //size of packet + pcap packet header
			parent->GetProgressBar()->setValue(value);
		}
		if (!pauseButtonClicked)
		{
			tableWidget->setRowCount(tableWidget->rowCount() + 1);
			ProcessPacket(packetData);
		}
	}

	processingFile = false;
	tableWidget->scrollToBottom();
	tableWidget->resizeColumnsToContents();
}

/// <summary>
/// Process one concrete packet.
/// </summary>
/// <param name="packetData">Data that represents one concrete packet</param>
void ItemManager::ProcessPacket(QByteArray packetData)
{
	//create new device
	if (representingConfigurationDescriptor)
	{
		const unsigned char* packet = (unsigned char*)packetData.data();
		PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;

		if (usbh->dataLength > sizeof(USB_CONFIGURATION_DESCRIPTOR))
		{
			hidDevices->CreateDevice(packetData);
		}

		representingConfigurationDescriptor = false;
	}

	//parse HID Report Descriptor
	if (representingHIDReportDescriptor)
	{
		QTableWidgetItem* previousItem = tableWidget->item(tableWidget->rowCount() - 2, 0);
		QByteArray previousLeftoverData = previousItem->data(dataHolder->TRANSFER_LEFTOVER_DATA).toByteArray();
		const unsigned char* previousPacket = (unsigned char*)previousLeftoverData.constData();
		PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)previousPacket;

		hidDevices->ParseHIDReportDescriptor(packetData, setupPacket->Index);

		representingHIDReportDescriptor = false;
	}



	FillUpItem(packetData);
	CheckForSetupPacket(packetData);
}

/// <summary>
/// Fills up first item in the row with all data needed for later analysis.
/// </summary>
/// <param name="packetData">Data that represents one concrete packet</param>
void ItemManager::FillUpItem(QByteArray packetData)
{
	const unsigned char* packet = (unsigned char*)packetData.data();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;

	InsertRow(usbh, packet);
	QTableWidgetItem* tableItem = tableWidget->item(tableWidget->rowCount() - 1, 0);

	//set USBPCAP header to item
	QByteArray usbhArray((const char*)packet, sizeof(USBPCAP_BUFFER_PACKET_HEADER));
	tableItem->setData(dataHolder->USBPCAP_HEADER_DATA, QVariant(usbhArray));
	packet += sizeof(USBPCAP_BUFFER_PACKET_HEADER);

	if (usbh->transfer == USBPCAP_TRANSFER_ISOCHRONOUS || usbh->transfer == USBPCAP_TRANSFER_CONTROL) //check for optional header data
	{
		int additionalDataSize = usbh->headerLen - sizeof(USBPCAP_BUFFER_PACKET_HEADER);
		if (additionalDataSize > 0)
		{
			//set additional header data to item
			QByteArray additionalDataArray((const char*)(packet), additionalDataSize);
			tableItem->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant(additionalDataArray));
			packet += additionalDataSize;
		}
		else
		{
			tableItem->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant());
		}
	}
	else
	{
		tableItem->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant());
	}

	//set leftover data to item
	QByteArray leftoverDataArray((const char*)packet, usbh->dataLength);
	tableItem->setData(dataHolder->TRANSFER_LEFTOVER_DATA, QVariant(leftoverDataArray));
}

/// <summary>
/// Analyzing Setup Packet and setting up <see cref="representingHIDDescriptor"/> and <see cref="representingConfigurationDescriptor"/>
/// </summary>
/// <param name="packetData">Data that represents Setup Packet</param>
void ItemManager::CheckForSetupPacket(QByteArray packetData)
{
	const unsigned char* packet = (unsigned char*)packetData.data();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;

	if (((usbh->info & 0x01) == 0) && (usbh->transfer == USBPCAP_TRANSFER_CONTROL)) //host->device and control transfer ... additional packet data = setup packet
	{
		packet += usbh->headerLen;
		PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)packet;
		if (setupPacket->Request == GET_DESCRIPTOR) //next packet will consist some descriptor
		{
			BYTE descriptorType = setupPacket->Value >> 8;
			if (descriptorType == HID_REPORT_DESCRIPTOR)
			{
				representingHIDReportDescriptor = true;
			}
			else if (descriptorType == CONFIGURATION_DESCRIPTOR)
			{
				representingConfigurationDescriptor = true;
			}
		}
	}
}

/// <summary>
/// Creates next row in <see cref="tableWidget"/>
/// </summary>
/// <param name="usbh">USB packet header in USBPcap format</param>
/// <param name="packet">pointer to the start of packet data</param>
void ItemManager::InsertRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet)
{
	std::string name;
	int column = 0;
	name = std::to_string(itemIndex++);
	//inserts Index cell
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));

	//inserts Source and Destination cells
	if ((usbh->info & 0x01) == 1)
	{
		name = std::to_string(usbh->device) + "." + std::to_string(usbh->endpoint & 0x0F);
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
		name = "host";
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
	}
	else
	{
		name = "host";
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
		name = std::to_string(usbh->device) + '.' + std::to_string(usbh->endpoint & 0x0F);
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
	}

	//inserts Length cell
	name = std::to_string(usbh->dataLength + usbh->headerLen);
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));

	//inserts Transfer type cell
	name = dataHolder->GetTransferType(usbh->transfer);
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
	
	//inserts Function cell
	if (usbh->transfer == USBPCAP_TRANSFER_CONTROL && (usbh->info & 0x01) == 0) //control transfer, host->device, additional data = setup packet
	{
		if (usbh->dataLength == sizeof(WINUSB_SETUP_PACKET))
		{
			packet += usbh->headerLen;
			PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)packet;
			if (setupPacket->Request == GET_DESCRIPTOR)
			{
				name = dataHolder->GetDescriptorType(setupPacket->Value >> 8);
				tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
			}
			else
			{
				tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem());
			}
		}
	}
	else if (usbh->transfer == USBPCAP_TRANSFER_IRP_INFO && usbh->function == 0x0002)
	{
		name = "URB_FUNCTION_ABORT_PIPE";
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));

		//"invalidate" ejected device
		for (int i = 0; i < hidDevices->devices.size(); i++)
		{
			if (hidDevices->devices[i].deviceAddress == usbh->device)
			{
				hidDevices->devices[i].validation = --itemIndex;
				hidDevices->devices[i].obsolete = true;
				itemIndex++;
			}
		}
	}
	else
	{
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem());
	}

	ColorRow(usbh);
}

/// <summary>
/// Colors rows in <see cref="tableWidget"/>
/// </summary>
/// <param name="usbh">Header of packet. Determines transfer type</param>
void ItemManager::ColorRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh)
{
	if ((usbh->info & 0x01) == 1) //device->host transfer
	{
		QColor backgroundColor;
		DataTypeColor col;
		switch (usbh->transfer)
		{
		case USBPCAP_TRANSFER_ISOCHRONOUS:
			col = dataHolder->DataColors[ISOCHRO_TRANSFER];
			break;
		case USBPCAP_TRANSFER_INTERRUPT:
			col = dataHolder->DataColors[INTERR_TRANSFER];
			break;
		case USBPCAP_TRANSFER_CONTROL:
			col = dataHolder->DataColors[CONTROL_TRANSFER];
			break;
		case USBPCAP_TRANSFER_BULK:
			col = dataHolder->DataColors[BULK_TRANSFER];
			break;
		case USBPCAP_TRANSFER_IRP_INFO:
			col = dataHolder->DataColors[IRP_INFO_TRANSFER];
			break;
		case USBPCAP_TRANSFER_UNKNOWN:
			col = dataHolder->DataColors[UNKNOWN_TRANSFER];
			break;
		default:
			col = DataTypeColor{ 255, 255, 255, 255 };
			break;
		}

		for (int i = 0; i < tableWidget->columnCount(); i++)
		{
			tableWidget->item(tableWidget->rowCount() - 1, i)->setBackgroundColor(QColor(col.red, col.green, col.blue, 50));
		}
	}
}

/// <summary>
/// Determines concrete data type of packet
/// </summary>
/// <param name="currentItem">current item that holds data</param>
/// <param name="previousItem">item that stands tight before <see cref="currentItem"/></param>
/// <returns></returns>
HeaderDataType ItemManager::GetDataType(QTableWidgetItem* currentItem, QTableWidgetItem* previousItem)
{
	QByteArray usbhArr = currentItem->data(dataHolder->USBPCAP_HEADER_DATA).toByteArray();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)usbhArr.constData();

	switch (usbh->transfer)
	{
	case USBPCAP_TRANSFER_BULK:
	{
		return BULK_TRANSFER;
	}
	case USBPCAP_TRANSFER_ISOCHRONOUS:
	{
		return ISOCHRO_TRANSFER;
	}
	case USBPCAP_TRANSFER_INTERRUPT:
	{
		return INTERR_TRANSFER;
	}
	case USBPCAP_TRANSFER_IRP_INFO:
	{
		return IRP_INFO_TRANSFER;
	}
	case USBPCAP_TRANSFER_UNKNOWN:
	{
		return UNKNOWN_TRANSFER;
	}
	case USBPCAP_TRANSFER_CONTROL:
	{
		if ((usbh->info & 0x01) == 0) //host->device and control transfer ... additional packet data = setup packet
		{
			return CONTROL_TRANSFER_SETUP;
		}

		// device->host and control transfer ... some descriptor is being sent to the host

		if (previousItem == nullptr)
		{
			return UNKNOWN_TRANSFER;
		}
		QByteArray previousData = previousItem->data(dataHolder->TRANSFER_LEFTOVER_DATA).toByteArray();
		PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)previousData.constData();
		BYTE descriptorType = ((setupPacket->Value >> 8) & 0xFF);
		switch (descriptorType)
		{
		case DEVICE_DESCRIPTOR:
		{
			return CONTROL_TRANSFER_DEVICE_DESC;
		}
		case STRING_DESCRIPTOR:
		{
			return CONTROL_TRANSFER_STRING_DESC;
		}
		case CONFIGURATION_DESCRIPTOR:
		{
			return CONTROL_TRANSFER_CONFIG_DESC;
		}
		case DEVICE_QUALIFIER:
		{
			return CONTROL_TRANSFER_DEVICE_QUALIFIER_DESC;
		}
		case OTHER_SPEED_CONFIGURATION:
		{
			return CONTROL_TRANSFER_OTHER_SPEED_CONF_DESC;
		}
		case HID_REPORT_DESCRIPTOR:
		{
			return CONTROL_TRANSFER_HID_REPORT_DESC;
		}
		default:
		{
			return CONTROL_TRANSFER_UNSPEC_DESC;
		}
		}
	}
	default:
	{
		return UNKNOWN_TRANSFER;
	}
	}
}