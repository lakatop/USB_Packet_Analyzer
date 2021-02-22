#include "HIDDevices.hpp"

HIDDevices* HIDDevices::hidDevices = nullptr;

HIDDevices* HIDDevices::GetHIDDevices()
{
	if (hidDevices == nullptr)
	{
		hidDevices = new HIDDevices;
	}

	return hidDevices;
}

HIDDevices::HIDDevices()
{
	HIDDescriptorSize = 9;
}

void HIDDevices::ParseHIDDescriptor()
{
}

void HIDDevices::CreateDevice(QByteArray packetData)
{
	const unsigned char* packet = (unsigned char*)packetData.data();
	PUSBPCAP_BUFFER_PACKET_HEADER usbh = (PUSBPCAP_BUFFER_PACKET_HEADER)packet;
	EndpointDevice device;
	packet += usbh->headerLen;
	packet += sizeof(USB_CONFIGURATION_DESCRIPTOR);
	unsigned int currentIndex = sizeof(USB_CONFIGURATION_DESCRIPTOR);
	BYTE interfaceNum = 0;

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
			packet += sizeof(USB_INTERFACE_DESCRIPTOR);
			currentIndex += sizeof(USB_INTERFACE_DESCRIPTOR);
		}
		break;
		case ENDPOINT_DESCRIPTOR:
		{
			PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR)packet;
			if ((endpointDescriptor->bEndpointAddress & 0xF0) > 0) // IN endpoint
			{
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