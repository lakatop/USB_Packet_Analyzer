#include "ItemManager.hpp"

ItemManager* ItemManager::itemManager = nullptr;

ItemManager* ItemManager::GetItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent)
{
	if (itemManager == nullptr)
	{
		itemManager = new ItemManager(listWidget, parent);
	}

	return itemManager;
}

ItemManager::ItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->atBottomOfList = false;
	this->representingHIDDescriptor = false;
	this->representingConfigurationDescriptor = false;
	this->listWidget = listWidget;
	this->parent = parent;
	this->dataHolder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
}

void ItemManager::AppendItem()
{
	QListWidgetItem* item = new QListWidgetItem;
	listWidget->insertItem(listWidget->count(), item);
}

void ItemManager::ProcessFile(QString filename, bool liveReading)
{
	if (fileReader.OpenNewFile(filename))
	{
		if (fileReader.ReadFileHeader())
		{
			while (!stopButtonClicked)
			{
				while (!fileReader.EndOfFile())
				{
					atBottomOfList = false;
					QByteArray packetData = fileReader.GetPacket();
					if (!pauseButtonClicked)
					{
						ProcessPacket(packetData);
					}
				}

				parent->Refresh();

				if (!atBottomOfList)
				{
					listWidget->scrollToBottom();
				}

				atBottomOfList = true;

				if (liveReading)
				{
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
		QListWidgetItem* previousItem = listWidget->item(listWidget->count() - 1);
		QByteArray previousLeftoverData = previousItem->data(dataHolder->TRANSFER_LEFTOVER_DATA).toByteArray();
		unsigned char* previousPacket = (unsigned char*)previousLeftoverData.data();
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

	QString itemName = SetItemName(usbh, packet);
	QListWidgetItem* item = new QListWidgetItem(itemName);

	//set USBPCAP header to item
	QByteArray usbhArray((const char*)packet, sizeof(USBPCAP_BUFFER_PACKET_HEADER));
	item->setData(dataHolder->USBPCAP_HEADER_DATA, QVariant(usbhArray));
	packet += sizeof(USBPCAP_BUFFER_PACKET_HEADER);

	if (usbh->transfer == USBPCAP_TRANSFER_ISOCHRONOUS || usbh->transfer == USBPCAP_TRANSFER_CONTROL) //check for optional header data
	{
		int additionalDataSize = usbh->headerLen - sizeof(USBPCAP_BUFFER_PACKET_HEADER);
		if (additionalDataSize > 0)
		{
			//set additional header data to item
			QByteArray additionalDataArray((const char*)(packet), additionalDataSize);
			item->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant(additionalDataArray));
			packet += additionalDataSize;
		}
		else
		{
			item->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant()); //QVariant creates invalid QVariant, later i just need to check with QVariant::isValid()
		}
	}
	else
	{
		item->setData(dataHolder->TRANSFER_OPTIONAL_HEADER, QVariant());
	}

	//set leftover data to item
	QByteArray leftoverDataArray((const char*)packet, usbh->dataLength);
	item->setData(dataHolder->TRANSFER_LEFTOVER_DATA, QVariant(leftoverDataArray));

	listWidget->insertItem(listWidget->count(), item);
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

QString ItemManager::SetItemName(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet)
{
	std::string itemName;
	itemName += std::to_string(listWidget->count()) + '\t';
	if ((usbh->info & 0x01) == 1)
	{
		itemName += std::to_string(usbh->device) + "." + std::to_string(usbh->endpoint & 0x0F);
		itemName += "\t host";
	}
	else
	{
		itemName += "host\t";
		itemName += std::to_string(usbh->device) + '.' + std::to_string(usbh->endpoint & 0x0F);
	}
	itemName += '\t' + std::to_string(usbh->dataLength + usbh->headerLen);
	itemName += '\t' + dataHolder->GetTransferType(usbh->transfer);
	if (usbh->transfer == USBPCAP_TRANSFER_CONTROL && (usbh->info & 0x01) == 0) //control transfer, host->device, additional data = setup packet
	{
		if (usbh->dataLength == sizeof(WINUSB_SETUP_PACKET))
		{
			packet += usbh->headerLen;
			PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)packet;
			if (setupPacket->Request == GET_DESCRIPTOR)
			{
				itemName += '\t' + dataHolder->GetDescriptorType(setupPacket->Value >> 8);
			}
		}
	}
	else if (usbh->transfer == USBPCAP_TRANSFER_IRP_INFO && usbh->function == 0x0002)
	{
		itemName += "\tURB_FUNCTION_ABORT_PIPE";
	}

	return QString(itemName.c_str());
}

HeaderDataType ItemManager::GetDataType(QListWidgetItem* currentItem, QListWidgetItem* previousItem)
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