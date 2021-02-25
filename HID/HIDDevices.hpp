#ifndef HIDDEVICES_HPP
#define HIDDEVICES_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"

#include <qbytearray.h>

class HIDDevices
{
public:
	static HIDDevices* GetHIDDevices();
	void ParseHIDDescriptor(QByteArray packetData, USHORT interfaceIndex);
	void CreateDevice(QByteArray packetData);
	size_t GetHIDDescriptorSize();
	HIDDescriptor FillUpHIDDescriptor(const unsigned char* packet);

	std::vector<EndpointDevice> devices;
private:
	HIDDevices();
	template<typename T> 
	void CharToNumberConvert(const unsigned char* addr, T& number, int size);

	static HIDDevices* hidDevices;
	size_t HIDDescriptorSize;
};


template<typename T>
void HIDDevices::CharToNumberConvert(const unsigned char* addr, T& number, int size)
{
	for (int i = size; i > 0; i--)
	{
		number = (number << 8) | addr[i - 1];
	}
}

#endif // !HIDDEVICES_HPP
