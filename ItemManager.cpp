#include "ItemManager.hpp"

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
		itemName += std::to_string(usbh->device);
		itemName += "\t host";
	}
	else
	{
		itemName += "host\t";
		itemName += std::to_string(usbh->device);
	}
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

	return QString(itemName.c_str());
}