#include "HIDDevices.hpp"

HIDDevices* HIDDevices::hidDevices = nullptr;

/// <summary>
/// Get instance of HIDDevices class
/// </summary>
/// <returns>Pointer to instance of HIDDevices class</returns>
HIDDevices* HIDDevices::GetHIDDevices()
{
	if (hidDevices == nullptr)
	{
		hidDevices = new HIDDevices;
	}

	return hidDevices;
}

/// <summary>
/// Constructor of HIDDevices.
/// </summary>
HIDDevices::HIDDevices()
{
	HIDDescriptorSize = 9; //must be set manually because of padding
	FillSupportedDeviceMap();
}

/// <summary>
/// Get size of HID Descriptor
/// </summary>
/// <returns>Size of HID Descriptor without memory padding</returns>
size_t HIDDevices::GetHIDDescriptorSize()
{
	return HIDDescriptorSize;
}

/// <summary>
/// Get Supported_device enum representing whether this device is supported by this application or not.
/// </summary>
/// <param name="key">Key to deviceMap</param>
/// <returns>Supported_Devices enum representing whether this device is supported by this application or not</returns>
SupportedDevices HIDDevices::GetSupportedDevice(const std::pair<uint32_t, uint32_t> key)
{
	auto deviceMapIterator = deviceMap.find(key);
	if (deviceMapIterator == deviceMap.end())
	{
		return D_UNDEFINED;
	}
	else
	{
		return deviceMapIterator->second;
	}
}
/// <summary>
/// HID Report Descriptor parser. Parses descriptor so we can later provide semantic analysis
/// </summary>
/// <param name="packetData">QByteArray representing Report Descriptor data</param>
/// <param name="interfaceIndex">Interface number of device</param>
void HIDDevices::ParseHIDReportDescriptor(const QByteArray packetData, const USHORT interfaceIndex)
{
	const unsigned char* packet = (unsigned char*)packetData.constData();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;

	//find device to put parsers in
	int index = -1;
	for (int i = 0; i < devices.size(); i++)
	{
		//if obsolete is true, we dont need to assign Report Descriptor to this device because it doesnt belong to this device 
		//since it was already disconnected from bus
		if (devices[i].deviceAddress == usbh->device && devices[i].obsolete == false)
		{
			index = i;
			break;
		}
	}
	
	//prepare vectors:
	//inputParsers for all report descriptors which will be parsed out of this packet
	std::vector<HIDReportDescriptorInputParse> inputParsers;
	//ends are all endpoints that will have this parsers assigned
	std::vector<BYTE> ends;

	if (index != -1)
	{
		auto endpointsPos = devices[index].endpoints.find(interfaceIndex);
		if (endpointsPos != devices[index].endpoints.end())
		{
			ends = endpointsPos->second;
		}
	}

	//move packet to descriptor position
	packet += usbh->headerLen;
	uint parsed = 0;
	uint depth = 0;
	InputValues inputValues;
	HIDReportDescriptorInputParse inputParse;

	while (parsed != usbh->dataLength)
	{
		BYTE tag = (*packet & 0xF0) >> 4;  // header = 1 byte, but only 4 top bits represents tag
		BYTE size = (*packet & 0x03);   // header = 1 byte, size = bottom 2 bits
		BYTE itemType = (*packet & 0x0C) >> 2; // header = 1 byte, type = bit 2 and 3
		packet += 1;
		parsed += 1 + size;

		switch (itemType)
		{
		case MAIN:
		{
			switch (tag)
			{
			case MAIN_INPUT:
			{
				BYTE input = (*packet);
				bool constant = ((input & 0x1) == 0x1); // if first byte is unset, then it is constant
				bool var = ((input & 0x2) == 0x2); // if second bit is set, then it is variable
				inputValues.Variable = var;
				inputValues.Constant = constant;
				inputParse.inputSize += inputValues.ReportCount * inputValues.ReportSize;

				if (inputValues.LocalUsageNames.empty())    //if true, then global usage has been defined and should be used as "usage name"
				{
					inputValues.LocalUsageNames.push_back(inputValues.GlobalUsagePage);
				}

				inputParse.inputValues.push_back(inputValues);
				inputValues.LocalUsageNames.clear();
				packet += size;
			}
			break;
			case OUTPUT:
			{
				packet += size;
			}
			break;
			case FEATURE:
			{
				packet += size;
			}
			break;
			case COLLECTION:
			{
				depth++;
				inputParse.inputValues.clear();     //starting new collection, clear all previous inputs...
				inputValues.LocalUsageNames.clear();  //...and clear all previous usages
				inputParse.inputSize = 0;
				packet += size;
			}
			break;
			case END_COLLECTION:
			{
				depth--;
				if (inputParse.inputSize != 0) // if inputSize != 0 then i encountered report_id and i also have one inputParse filled up, so i need to push it to the vector and start filling another inputParse
				{
					inputParsers.push_back(inputParse);
				}
				inputParse.inputSize = 0;
				packet += size;
			}
			break;
			default:
				break;
			}
		}
		break;
		case GLOBAL:
		{
			switch (tag)
			{
			case USAGE_PAGE:
			{
				if (depth == 0)
				{
					inputParse.globalItemUsage = 0;
					CharToNumberConvert(packet, inputParse.globalItemUsage, size);
				}

				inputValues.GlobalUsagePage = 0;
				CharToNumberConvert(packet, inputValues.GlobalUsagePage, size);
				packet += size;
			}
			break;
			case LOGICAL_MINIMUM:
			{
				inputValues.LogicalMinimum = 0;
				CharToNumberConvert(packet, inputValues.LogicalMinimum, size);
				packet += size;
			}
			break;
			case LOGICAL_MAXIMUM:
			{
				inputValues.LogicalMaximum = 0;
				CharToNumberConvert(packet, inputValues.LogicalMaximum, size);
				packet += size;
			}
			break;
			case REPORT_SIZE:
			{
				inputValues.ReportSize = 0;
				CharToNumberConvert(packet, inputValues.ReportSize, size);
				packet += size;
			}
			break;
			case REPORT_COUNT:
			{
				inputValues.ReportCount = 0;
				CharToNumberConvert(packet, inputValues.ReportCount, size);
				packet += size;
			}
			break;
			case REPORT_ID:
			{
				inputParse.inputSize = 0;
				inputParse.reportDefined = true;
				inputParse.reportID = 0;
				CharToNumberConvert(packet, inputParse.reportID, size);
				packet += size;
			}
			break;
			default:
			{
				//physical_{minimum,maximum}, unit, unit_exponent, push, pop
				packet += size;
			}
			break;
			}
		}
		break;
		case LOCAL:
		{
			switch (tag)
			{
			case USAGE:
			{
				uint32_t usage = 0;
				CharToNumberConvert(packet, usage, size);

				//if depth == 0, that means that local item usage doesnt represent any 
				//collection usage (any input button etc.), but combined with 
				//global item usage it represent type of device (e.g. mouse, keyboard, joystick,...)
				if (depth == 0)
				{
					inputParse.localItemUsage = usage;
				}
				else
				{
					inputValues.LocalUsageNames.push_back(usage);
				}
				packet += size;
			}
			break;
			case USAGE_MINIMUM:
			{
				inputValues.UsageMinimum = 0;
				CharToNumberConvert(packet, inputValues.UsageMinimum, size);
				packet += size;
			}
			break;
			case USAGE_MAXIMUM:
			{
				inputValues.UsageMaximum = 0;
				CharToNumberConvert(packet, inputValues.UsageMaximum, size);
				packet += size;
			}
			break;
			default:
			{
				//designator_{index, minimum, maximum}, string_{index, minimum, maximum}, delimiter
				packet += size;
			}
			break;
			}
		}
		break;
		default:
			break;
		}
	}


	if (index != -1)
	{
		for (int i = 0; i < ends.size(); i++)
		{
			devices[index].inputparser[ends[i]] = inputParsers;
		}
	}
}

/// <summary>
/// Creating new BusDevice. Called when new Configuration Descriptor was sent to host.
/// </summary>
/// <param name="packetData">QByteArray representing data of Control Transfer</param>
void HIDDevices::CreateDevice(const QByteArray packetData)
{
	const unsigned char* packet = (unsigned char*)packetData.constData();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;
	BusDevice device(usbh->device);
	packet += usbh->headerLen;
	packet += sizeof(USB_CONFIGURATION_DESCRIPTOR);
	unsigned int currentIndex = sizeof(USB_CONFIGURATION_DESCRIPTOR);
	BYTE interfaceNum = 0;
	UCHAR bInterfaceProtocol = 0;
	bool isHID = false;

	while (currentIndex != usbh->dataLength)
	{
		BYTE descriptorType = *(++packet);
		packet--;
		switch (descriptorType)
		{
		case HID_DESCRIPTOR_ENUM:
		{
			packet += HIDDescriptorSize;
			currentIndex += HIDDescriptorSize;
		}
		break;
		case INTERFACE_DESCRIPTOR:
		{
			PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)packet;
			interfaceNum = interfaceDescriptor->bInterfaceNumber;
			bInterfaceProtocol = interfaceDescriptor->bInterfaceProtocol;
			isHID = (interfaceDescriptor->bInterfaceClass == 0x03);
			packet += sizeof(USB_INTERFACE_DESCRIPTOR);
			currentIndex += sizeof(USB_INTERFACE_DESCRIPTOR);
		}
		break;
		case ENDPOINT_DESCRIPTOR:
		{
			PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)packet;
			if ((endpointDescriptor->bEndpointAddress & 0xF0) > 0) // IN endpoint
			{
				device.hidDescription.insert(std::make_pair(endpointDescriptor->bEndpointAddress & 0x0F, std::make_pair(isHID, bInterfaceProtocol)));
				device.endpoints[interfaceNum].push_back(endpointDescriptor->bEndpointAddress & 0x0F);
			}
			packet += sizeof(USB_ENDPOINT_DESCRIPTOR);
			currentIndex += sizeof(USB_ENDPOINT_DESCRIPTOR);
		}
		break;
		default:
		{
			byte size = (byte)(*packet);
			currentIndex += size; // first byte of packet is representing size of descriptor
			packet += size;
		}
		break;
		}
	}

	devices.push_back(device);
}

/// <summary>
/// Fills up new HID Descriptor. Is done manually because of possible memory padding.
/// </summary>
/// <param name="packet">Pointer to HID Descriptor data</param>
/// <returns>Filled HID Descriptor</returns>
HIDDescriptor HIDDevices::FillUpHIDDescriptor(const unsigned char* packet)
{
	HIDDescriptor hidDescriptor;
	hidDescriptor.bLength = (UCHAR)(*packet);
	packet += 1;
	hidDescriptor.bDescriptorType = (UCHAR)(*packet);
	packet += 1;
	hidDescriptor.bcdHID = (UCHAR)(*packet);
	hidDescriptor.bcdHID *= 16;
	hidDescriptor.bcdHID += (UCHAR)(*(packet + 1));
	packet += 2;
	hidDescriptor.bCountry = (UCHAR)(*packet);
	packet += 1;
	hidDescriptor.bNumDescriptors = (UCHAR)(*packet);
	packet += 1;
	hidDescriptor.bReportType = (UCHAR)(*packet);
	packet += 1;
	hidDescriptor.wReportLength = (USHORT)(*packet);
	packet += 2;

	return hidDescriptor;
}

/// <summary>
/// Fill up deviceMap with supported devices.
/// </summary>
void HIDDevices::FillSupportedDeviceMap()
{
	deviceMap.emplace(std::make_pair(0x01, 0x02), D_MOUSE);
	deviceMap.emplace(std::make_pair(0x01, 0x04), D_JOYSTICK);
	deviceMap.emplace(std::make_pair(0x01, 0x06), D_KEYBOARD);
}