#include "InterruptTransferInterpreter.hpp"

InterruptTransferInterpreter::InterruptTransferInterpreter(TreeItem* rootItem, QTableWidgetItem* item, AdditionalDataModel* additionalDataModel)
{
	this->rootItem = rootItem;
	this->item = item;
	this->additionalDataModel = additionalDataModel;
	this->hidDevices = HIDDevices::GetHIDDevices();
	this->holder = DataHolder::GetDataHolder();
}

void InterruptTransferInterpreter::InterpretInterruptTransfer()
{
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)item->data(holder->USBPCAP_HEADER_DATA).toByteArray().constData();
	
	if ((usbh->info & 0x01) == 0) // host -> device
	{
		return;
	}
	
	int index = -1;
	for (int i = 0; i < hidDevices->devices.size(); i++)
	{
		if ((hidDevices->devices[i].deviceAddress == usbh->device) && hidDevices->devices[i].obsolete == false)
		{
			index = i;
			break;
		}
	}

	if (index != -1) //some known device
	{
		auto inputParser = GetInputParser(index);
		std::pair<uint32_t, uint32_t> GlobalAndLocalUsagePair = std::make_pair(inputParser.globalItemUsage, inputParser.localItemUsage);
		Supported_Devices dev = hidDevices->GetSupportedDevice(GlobalAndLocalUsagePair);

		auto it = hidDevices->devices[index].hidDescription.find(usbh->endpoint & 0x0F);
		if (it != hidDevices->devices[index].hidDescription.end())
		{
			if (it->second.first) // hid device
			{
				switch (it->second.second)
				{
				case D_MOUSE:
				{
					MouseInterpreter mouseInterpreter(rootItem, item, additionalDataModel, inputParser);
					mouseInterpreter.Interpret();
				}
				return;
				case D_KEYBOARD:
				{
					KeyboardInterpreter keyboardInterpreter(rootItem, item, additionalDataModel, inputParser);
					keyboardInterpreter.Interpret();
				}
				return;
				default:
					break;
				}
			}
		}

		switch (dev) //if it goes to mouse/keyboard it probably wont have default input format, so interpretation will look "ugly"
		{
		case D_MOUSE: 
		{
			MouseInterpreter mouseInterpreter(rootItem, item, additionalDataModel, inputParser);
			mouseInterpreter.Interpret();
		}
		case D_KEYBOARD:
		{
			KeyboardInterpreter keyboardInterpreter(rootItem, item, additionalDataModel, inputParser);
			keyboardInterpreter.Interpret();
		}
		case D_JOYSTICK:
		{
			JoystickInterpreter joystickInterpreter(rootItem, item, additionalDataModel, inputParser);
			joystickInterpreter.Interpret();
		}
		break;
		case D_UNDEFINED:
		{
			UnknownDeviceInterpreter unknownInterpreter(rootItem, item, additionalDataModel, inputParser);
			unknownInterpreter.Interpret();
		}
		break;
		default:
			break;
		}
	}
}

HIDReportDescriptorInputParse InterruptTransferInterpreter::GetInputParser(int index)
{
	HIDReportDescriptorInputParse inputParser;
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)item->data(holder->USBPCAP_HEADER_DATA).toByteArray().constData();
	BYTE endpointNum = usbh->endpoint & 0x0F;
	auto parsersMapIterator = hidDevices->devices[index].inputparser.find(endpointNum);
	if (parsersMapIterator != hidDevices->devices[index].inputparser.end())
	{
		auto parsers = parsersMapIterator->second;
		if (parsers.size() > 1) //more parsers, must be reportDefined
		{
			QByteArray leftoverData = item->data(holder->TRANSFER_LEFTOVER_DATA).toByteArray();
			unsigned char* packet = (unsigned char*)leftoverData.data();
			BYTE reportID = *packet;	//get reportID of input data
			//go through all parsers and choose one that matches with reportID of input data
			for (int i = 0; i < parsers.size(); i++)
			{
				if (parsers[i].reportID == reportID)
				{
					inputParser = parsers[i];
					return inputParser;
				}
			}
		}
		else if (parsers.size() == 1)
		{
			inputParser = parsers[0];
			return inputParser;
		}
	}

	return inputParser;
}