#include "SetupPacketInterpreter.hpp"

SetupPacketInterpreter::SetupPacketInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
	:BaseInterpreter(rootItem, item, additionalDataModel)
{
	this->holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Interprets Setup Packet Descriptor.
/// </summary>
void SetupPacketInterpreter::Interpret()
{
	QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
	const unsigned char* packet = (unsigned char*)leftoverData.constData();
	PWINUSB_SETUP_PACKET setupPacket = (PWINUSB_SETUP_PACKET)packet;
	rootItem->AppendChild(new TreeItem(QVector<QVariant>{"SETUP_DESCRIPTOR", "", ""}, rootItem));
	TreeItem* setupDescriptorChild = rootItem->Child(rootItem->ChildCount() - 1);

	//bmRequestType field
	QString hexData;
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "Request Type", setupPacket->RequestType}, setupDescriptorChild));
	BYTE direction = (setupPacket->RequestType & 0x80) >> 7; //MSb
	BYTE requestType = (setupPacket->RequestType & 0x60) >> 5; //bits 6..5
	BYTE recipient = setupPacket->RequestType & 0x1F; //bits 4..0
	
	//bmRequestType defines meaning on bit level -> show those bits and their meaning
	TreeItem* requestTypeChild = setupDescriptorChild->Child(setupDescriptorChild->ChildCount() - 1);
	requestTypeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 1, setupPacket->RequestType),
		(std::string("Direction: ") + std::string((direction == 1) ? "Device -> Host (1)" : "Host -> Device (0)")).c_str()}, requestTypeChild));
	requestTypeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(1, 2, setupPacket->RequestType), (std::string("Type: ") +
		std::string((requestType == 0) ? "Standard (0)" :
			(requestType == 1) ? "Class (1)" :
			(requestType == 2) ? "Vendor (2)" : "Reserved (3)")).c_str()}, requestTypeChild));
	requestTypeChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(3, 5, setupPacket->RequestType), (std::string("Recipient:") +
		std::string((recipient == 0) ? "Device (0)" :
			(recipient == 1) ? "Interface (1)" :
			(recipient == 2) ? "Endpoint (2)" :
			(recipient == 3) ? "Other (3)" : "Reserved (4..31)")).c_str()}, requestTypeChild));
	
	//bmRequestType is fully interpreted, continue with setupDescriptorChild tree item.
	additionalDataModel->CharToHexConvert(&packet, 1, hexData);
	setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, ("bRequest: " + holder->GetSetupPacketRequest(setupPacket->Request)).c_str(), setupPacket->RequestType}, setupDescriptorChild));
	
	//switch interpretation according to standard device requests
	switch (setupPacket->Request)
	{
	case GET_STATUS:
	{
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case CLEAR_FEATURE:
	case SET_FEATURE:
	{
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wValue: ", std::string((setupPacket->Value == 0) ? "ENDPOINT_HALT (0)" :
			(setupPacket->Value == 1) ? "DEVICE_REMOTE_WAKEUP (1)" :
			"TEST_MODE (2)").c_str()}, setupDescriptorChild));
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		if (setupPacket->Request == SET_FEATURE && setupPacket->Value == 2)
		{
			TreeItem* wIndexChild = setupDescriptorChild->Child(setupDescriptorChild->ChildCount() - 1);
			BYTE testSelector = (setupPacket->Index >> 8);
			wIndexChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 8, setupPacket->Value),
				("Test Mode Selector: " + holder->GetUSBTestSelector(testSelector)).c_str()}, wIndexChild));

		}
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case SET_ADDRESS:
	{
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wValue (Device Address)", setupPacket->RequestType}, setupDescriptorChild));
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case GET_DESCRIPTOR:
	case SET_DESCRIPTOR:
	{
		BYTE descriptorType = (setupPacket->Value >> 8); //upper byte
		BYTE descriptorIndex = setupPacket->Value;	//lower byte
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		TreeItem* wValueChild = setupDescriptorChild->Child(setupDescriptorChild->ChildCount() - 1);
		wValueChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 8, setupPacket->Value),
			("Descriptor Type: " + holder->GetDescriptorType(descriptorType)).c_str()}, wValueChild));
		wValueChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(8, 8, setupPacket->Value), "Descriptor Index",
			descriptorIndex}, wValueChild));
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, (std::string("wIndex") + 
			std::string((descriptorType == STRING_DESCRIPTOR) ? " (Language ID)" : "")).c_str(), setupPacket->Index}, setupDescriptorChild));
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case GET_CONFIGURATION:
	{
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case SET_CONFIGURATION:
	{
		BYTE configuration = setupPacket->Value; //lower byte
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		TreeItem* wValueChild = setupDescriptorChild->Child(setupDescriptorChild->ChildCount() - 1);
		wValueChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(0, 8, setupPacket->Value), "Reserved"}, wValueChild));
		wValueChild->AppendChild(new TreeItem(QVector<QVariant>{additionalDataModel->ShowBits(8, 8, setupPacket->Value), "Configuration",
			configuration}, wValueChild));
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case GET_INTERFACE:
	{
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwIndex(&packet, setupPacket, setupDescriptorChild);
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case SET_INTERFACE:
	{
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wValue (Alternate Setting)", setupPacket->Value}, setupDescriptorChild));
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wIndex (Interface)", setupPacket->Index}, setupDescriptorChild));
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	case SYNCH_FRAME:
	{
		AppendSetupwValue(&packet, setupPacket, setupDescriptorChild);
		additionalDataModel->CharToHexConvert(&packet, 2, hexData);
		setupDescriptorChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wIndex (Endpoint)", setupPacket->Index}, setupDescriptorChild));
		AppendSetupwLength(&packet, setupPacket, setupDescriptorChild);
	}
	break;
	default:
		break;
	}
}

/// <summary>
/// Interprets wValue field.
/// </summary>
/// <param name="packet">Pointer to wValue data</param>
/// <param name="setupPacket">Pointer to setup packet which is being interpreted</param>
/// <param name="setupChild">Tree item to which we are appending items</param>
void SetupPacketInterpreter::AppendSetupwValue(const unsigned char** packet, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild)
{
	QString hexData;
	additionalDataModel->CharToHexConvert(packet, 2, hexData);
	setupChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wValue", setupPacket->Value}, setupChild));
}

/// <summary>
/// Interprets wIndex field.
/// </summary>
/// <param name="packet">Pointer to wIndex data</param>
/// <param name="setupPacket">Pointer to setup packet which is being interpreted</param>
/// <param name="setupChild">Tree item to which we are appending items</param>
void SetupPacketInterpreter::AppendSetupwIndex(const unsigned char** packet, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild)
{
	QString hexData;
	additionalDataModel->CharToHexConvert(packet, 2, hexData);
	setupChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wIndex", setupPacket->Index}, setupChild));
}

/// <summary>
/// Interprets wLength field.
/// </summary>
/// <param name="packet">Pointer to wLength data</param>
/// <param name="setupPacket">Pointer to setup packet which is being interpreted</param>
/// <param name="setupChild">Tree item to which we are appending items</param>
void SetupPacketInterpreter::AppendSetupwLength(const unsigned char** packet, PWINUSB_SETUP_PACKET setupPacket, TreeItem* setupChild)
{
	QString hexData;
	additionalDataModel->CharToHexConvert(packet, 2, hexData);
	setupChild->AppendChild(new TreeItem(QVector<QVariant>{hexData, "wLength", setupPacket->Length}, setupChild));
}
