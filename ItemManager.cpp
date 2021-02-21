#include "ItemManager.hpp"

ItemManager::ItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->atBottomOfList = false;
	this->listWidget = listWidget;
	this->parent = parent;
	this->dataHolder = DataHolder::GetDataHolder();
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
	const unsigned char* packet = (unsigned char*)packetData.data();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;

	QListWidgetItem* item = new QListWidgetItem;
	SetItemName(item, usbh);

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

void ItemManager::SetItemName(QListWidgetItem* item, PUSBPCAP_BUFFER_PACKET_HEADER usbh)
{
	QString itemName;
	itemName += listWidget->count() + '\t';
	if (usbh->info & 0x01 == 1)
	{
		itemName.append(usbh->device);
		itemName.append(std::string("\t host").c_str());
	}
	else
	{
		itemName.append(std::string("host\t").c_str());
		itemName.append(usbh->device);
	}
	
	//add transfer type
}