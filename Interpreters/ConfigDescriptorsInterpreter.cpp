#include "ConfigDescriptorsInterpreter.hpp"

ConfigDescriptorsInterpreter::ConfigDescriptorsInterpreter(TreeItem* rootItem, QListWidgetItem* item, AdditionalDataModel* additionalDataModel)
{
	this->rootItem = rootItem;
	this->holder = DataHolder::GetDataHolder();
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->item = item;
	this->additionalDataModel = additionalDataModel;
}

void ConfigDescriptorsInterpreter::InterpretConfigDescriptors()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();

	InterpretConfigDescriptor(packet);

	if (leftoverData.size() == sizeof(USB_CONFIGURATION_DESCRIPTOR))
	{
		return;
	}

	packet += sizeof(USB_CONFIGURATION_DESCRIPTOR);
	size_t currentIndex = sizeof(USB_CONFIGURATION_DESCRIPTOR);

	while (currentIndex != leftoverData.size())
	{
		BYTE descriptorType = *(++packet);
		packet--;
		switch (descriptorType)
		{
		case HID_DESCRIPTOR_ENUM:
		{
			currentIndex += hidDevices->GetHIDDescriptorSize();
			InterpretHIDDescriptor(packet);
			packet += hidDevices->GetHIDDescriptorSize();
		}
		break;
		case INTERFACE_DESCRIPTOR:
		{
			currentIndex += sizeof(USB_INTERFACE_DESCRIPTOR);
			InterpretInterfaceDescriptor(packet);
			packet += sizeof(USB_INTERFACE_DESCRIPTOR);
		}
		break;
		case ENDPOINT_DESCRIPTOR:
		{
			currentIndex += sizeof(USB_ENDPOINT_DESCRIPTOR);
			InterpretEndpointDescriptor(packet);
			packet += sizeof(USB_ENDPOINT_DESCRIPTOR);
		}
		break;
		default:
		{
			currentIndex += (byte)(*packet); // first byte of packet is representing size of descriptor
			InterpretUnknownDescriptor(packet);
			packet += (byte)(*packet);
		}
		break;
		}
	}
}

void ConfigDescriptorsInterpreter::InterpretConfigDescriptor(const unsigned char* packet)
{
	PUSB_CONFIGURATION_DESCRIPTOR configDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{
		std::string((configDescriptor->bDescriptorType == CONFIGURATION_DESCRIPTOR) ? "CONFIGURATION_DESCRIPTOR" :
			"OTHER_SPEED_CONFIGURATION_DESCRIPTOR").c_str(), "", ""}, rootItem));
	TreeItem* configDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", configDescriptor->bLength}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, (std::string("bDescriptorType ") +
		std::string((configDescriptor->bDescriptorType == CONFIGURATION_DESCRIPTOR) ? "CONFIGURATION_DESCRIPTOR" :
			"OTHER_SPEED_CONFIGURATION_DESCRIPTOR")).c_str(), configDescriptor->bDescriptorType}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wTotalLength", configDescriptor->wTotalLength}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bNumInterfaces", configDescriptor->bNumInterfaces}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bConfigurationValue", configDescriptor->bConfigurationValue}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "iConfiguration", configDescriptor->iConfiguration}, configDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bmAttributes", configDescriptor->bmAttributes}, configDescriptorChild));
	TreeItem* bmAttributesChild = configDescriptorChild->Child(configDescriptorChild->ChildCount() - 1);
	BYTE selfPoweredValue = (configDescriptor->bmAttributes & 0x40) >> 6;
	BYTE remoteWakeupValue = (configDescriptor->bmAttributes & 0x20) >> 5;
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 1, configDescriptor->bmAttributes),
		"Reserved(should always be 1)"}, bmAttributesChild));
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(1, 1, configDescriptor->bmAttributes),
		"Self-powered", selfPoweredValue}, bmAttributesChild));
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(2, 1, configDescriptor->bmAttributes),
		(std::string("Remote Wakeup: ") + std::string((remoteWakeupValue == 1) ? "Supported (1)" : "Not Supported (0)")).c_str()},
		bmAttributesChild));
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(3, 5, configDescriptor->bmAttributes),
		"Reserved (0)", selfPoweredValue}, bmAttributesChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	configDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bMaxPower",
		(std::to_string(configDescriptor->MaxPower) + std::string(" = ") + std::to_string(configDescriptor->MaxPower * 2) + std::string(" mA")).c_str()}, configDescriptorChild));
}

void ConfigDescriptorsInterpreter::InterpretInterfaceDescriptor(const unsigned char* packet)
{
	PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"INTERFACE_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* interfaceDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", interfaceDescriptor->bLength}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, (std::string("bDescriptorType") + std::string("(INTERFACE)")).c_str(),
		interfaceDescriptor->bDescriptorType}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bInterfaceNumber", interfaceDescriptor->bInterfaceNumber}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bAlternateSetting", interfaceDescriptor->bAlternateSetting}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bNumEndpoints", interfaceDescriptor->bNumEndpoints}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bInterfaceClass", interfaceDescriptor->bInterfaceClass}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bInterfaceSubClass", interfaceDescriptor->bInterfaceSubClass}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bInterfaceProtocol", interfaceDescriptor->bInterfaceProtocol}, interfaceDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	interfaceDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "iInterface", interfaceDescriptor->iInterface}, interfaceDescriptorChild));
}

void ConfigDescriptorsInterpreter::InterpretEndpointDescriptor(const unsigned char* packet)
{
	PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"ENDPOINT_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* endpointDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", endpointDescriptor->bLength}, endpointDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDescriptorType (ENDPOINT_DESCRIPTOR)", endpointDescriptor->bDescriptorType}, endpointDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bEndpointAddress", endpointDescriptor->bEndpointAddress}, endpointDescriptorChild));

	TreeItem* bEndpointAddressChild = endpointDescriptorChild->Child(endpointDescriptorChild->ChildCount() - 1);
	if ((endpointDescriptor->bmAttributes & 0x03) != 0) //if not Control endpoint
	{
		BYTE direction = (endpointDescriptor->bEndpointAddress & 0x80) >> 7;
		bEndpointAddressChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 1, endpointDescriptor->bEndpointAddress),
			(std::string("Direction ") + std::string((direction == 1) ? "IN (1)" : "OUT (0)")).c_str()}, bEndpointAddressChild));
	}
	bEndpointAddressChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(1, 3, endpointDescriptor->bEndpointAddress), "Reserved"},
		bEndpointAddressChild));
	BYTE endpointNumber = (endpointDescriptor->bEndpointAddress & 0x0F);
	bEndpointAddressChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(4, 4, endpointDescriptor->bEndpointAddress),
		"Endpoint Number: ", endpointNumber}, bEndpointAddressChild));

	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bmAttributes", endpointDescriptor->bmAttributes}, endpointDescriptorChild));

	TreeItem* bmAttributesChild = endpointDescriptorChild->Child(endpointDescriptorChild->ChildCount() - 1);
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 2, endpointDescriptor->bmAttributes), "Reserved"},
		bmAttributesChild));
	BYTE transferType = endpointDescriptor->bmAttributes & 0x03;
	if (transferType == 1) //Isochronous transfer type
	{
		BYTE usageType = (endpointDescriptor->bmAttributes & 0x30) >> 4; //bits 5..4
		BYTE synchronizationType = (endpointDescriptor->bmAttributes & 0x0C) >> 2; //bits 3..2
		bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(2, 2, endpointDescriptor->bmAttributes),
			(std::string("Usage Type: ") + std::string((usageType == 0) ? "Data endpoint" :
				(usageType == 1) ? "Feedback endpoint" :
				(usageType == 2) ? "Implicit feedback Data endpoint" : "Reserved")).c_str()}, bmAttributesChild));
		bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(4, 2, endpointDescriptor->bmAttributes),
			(std::string("Synchronization Type: ") + std::string((synchronizationType == 0) ? "No synchronization" :
				(synchronizationType == 1) ? "Asychronous" :
				(synchronizationType == 2) ? "Adaptive" : "Sychronous")).c_str()}, bmAttributesChild));
	}
	else
	{
		bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(2, 4, endpointDescriptor->bmAttributes), "Reserved"},
			bmAttributesChild));
	}
	bmAttributesChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(6, 2, endpointDescriptor->bmAttributes),
		(std::string("Transfer Type: ") + std::string((transferType == 0) ? "Control (0)" :
			(transferType == 1) ? "Isochronous (1)" :
			(transferType == 2) ? "Bulk (2)" : "Interrupt (3)")).c_str()}, bmAttributesChild));

	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wMaxPacketSize", endpointDescriptor->wMaxPacketSize}, endpointDescriptorChild));

	TreeItem* wMaxPacketSizeChild = endpointDescriptorChild->Child(endpointDescriptorChild->ChildCount() - 1);
	wMaxPacketSizeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 3, endpointDescriptor->wMaxPacketSize),
		"Reserved"}, wMaxPacketSizeChild));
	BYTE transactions = (endpointDescriptor->wMaxPacketSize & 0x1800) >> 11;
	USHORT maxPacketSize = endpointDescriptor->wMaxPacketSize & 0x07FF;
	//assuming only high-speed devices (USB 2.0 and higher)
	wMaxPacketSizeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(3, 2, endpointDescriptor->wMaxPacketSize),
		(std::string("Transactions per microframe: ") + std::string((transactions == 0) ? "1 (0)" :
			(transactions == 1) ? "2 (1)" :
			(transactions == 2) ? "3 (2)" : "Reserved")).c_str()}, wMaxPacketSizeChild));
	wMaxPacketSizeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(5, 11, endpointDescriptor->wMaxPacketSize),
		"Max packet size: ", maxPacketSize}, wMaxPacketSizeChild));

	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	endpointDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bInterval", endpointDescriptor->bInterval}, endpointDescriptorChild));
}

void ConfigDescriptorsInterpreter::InterpretHIDDescriptor(const unsigned char* packet)
{
	HIDDescriptor hidDescriptor = hidDevices->FillUpHIDDescriptor(packet);
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"HID_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* hidDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", hidDescriptor.bLength}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDescriptorType (HID_DESCRIPTOR)", hidDescriptor.bDescriptorType}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bcdHID", hidDescriptor.bcdHID}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bCountry", hidDescriptor.bCountry}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bNumDescriptors", hidDescriptor.bNumDescriptors}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData,
		(std::string("bDescriptorType: ") + holder->GetDescriptorType(hidDescriptor.bReportType)).c_str(), hidDescriptor.bReportType}, hidDescriptorChild));
	additionalDataModel->CharToHexConvert(&packet, 2, hexData);
	hidDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wDescriptorLength", hidDescriptor.wReportLength}, hidDescriptorChild));
}

void ConfigDescriptorsInterpreter::InterpretUnknownDescriptor(const unsigned char* packet)
{
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"UNKNOWN_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* unknownDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);
	BYTE descriptorSize = (*packet);

	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bLength", descriptorSize}, unknownDescriptorChild));

	BYTE descriptorType = (*packet);
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "bDescriptorType", descriptorType}, unknownDescriptorChild));

	additionalDataModel->CharToHexConvert(&packet, descriptorSize - 1, hexData); // -1 for descriptorType
	unknownDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "unspecified"}, unknownDescriptorChild));
}