#include "ItemManager.hpp"

ItemManager* ItemManager::itemManager = nullptr;

ItemManager* ItemManager::GetItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent)
{
	if (itemManager == nullptr)
	{
		itemManager = new ItemManager(tableWidget, parent);
	}

	return itemManager;
}

ItemManager::ItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->atBottomOfList = false;
	this->representingHIDDescriptor = false;
	this->representingConfigurationDescriptor = false;
	this->parent = parent;
	this->tableWidget = tableWidget;
	this->dataHolder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
}

void ItemManager::ProcessFile(QString filename, bool liveReading)
{
	if (fileReader.OpenNewFile(filename))
	{
		if (fileReader.ReadFileHeader())
		{
			while (!stopButtonClicked)
			{
				qint64 value = sizeof(pcap_hdr_t); //global file header size
				if (!liveReading)
				{
					parent->GetProgressBar()->setRange(0, fileReader.FileSize());
				}
				while (!fileReader.EndOfFile())
				{
					atBottomOfList = false;
					QByteArray packetData = fileReader.GetPacket();
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

				parent->Refresh();

				if (!atBottomOfList)
				{
					tableWidget->scrollToBottom();
				}

				atBottomOfList = true;

				if (liveReading)
				{
					tableWidget->resizeColumnsToContents();
					Sleep(50);
				}
				else
				{
					return;
				}
			}
		}
	}
}
void ItemManager::ProcessPacket(QByteArray packetData)
{
	/*
* CO POTREBUJEM ULOZIT DO ITEMU:
*	usbh (QByteArray)
*	optionalHeaderData (QByteArray / NULL)
*	leftoverData (QByteArray)
*
* CO TU POTREBUJEM ESTE SPRAVIT:
*	Nastavit meno itemu
*	Parse HID Report Desc.
*	Create Device
*/
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

	if (representingHIDDescriptor)
	{
		auto a = tableWidget->rowCount();
		QTableWidgetItem* previousItem = tableWidget->item(tableWidget->rowCount() - 2, 0);
		QByteArray previousLeftoverData = previousItem->data(dataHolder->TRANSFER_LEFTOVER_DATA).toByteArray();
		const unsigned char* previousPacket = (unsigned char*)previousLeftoverData.constData();
		PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)previousPacket;

		hidDevices->ParseHIDDescriptor(packetData, setupPacket->Index);

		representingHIDDescriptor = false;
	}

	FillUpItem(packetData);
	CheckForSetupPacket(packetData);
}

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
				representingHIDDescriptor = true;
			}
			else if (descriptorType == CONFIGURATION_DESCRIPTOR)
			{
				representingConfigurationDescriptor = true;
			}
		}
	}
}

void ItemManager::InsertRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet)
{
	std::string name;
	int column = 0;
	name = std::to_string(tableWidget->rowCount() - 1);
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
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
	name = std::to_string(usbh->dataLength + usbh->headerLen);
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
	name = dataHolder->GetTransferType(usbh->transfer);
	tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem(name.c_str()));
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
	}
	else
	{
		tableWidget->setItem(tableWidget->rowCount() - 1, column++, new QTableWidgetItem());
	}

	ColorRow(usbh);
}

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

		// device->host and control tranfser ... some descriptor is being sent to the host
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