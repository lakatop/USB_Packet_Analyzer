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

	std::vector<EndpointDevice> devices;
private:
	HIDDevices();
	template<typename T> 
	void CharToIntConvert(const unsigned char* addr, T& number, size_t size);

	static HIDDevices* hidDevices;
	size_t HIDDescriptorSize;
};


template<typename T>
void HIDDevices::CharToIntConvert(const unsigned char* addr, T& number, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		number = (number << 8) | addr[i];
	}
}

#endif // !HIDDEVICES_HPP
