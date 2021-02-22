#ifndef HIDDEVICES_HPP
#define HIDDEVICES_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"

#include <qbytearray.h>

class HIDDevices
{
public:
	static HIDDevices* GetHIDDevices();
	void ParseHIDDescriptor();
	void CreateDevice(QByteArray packetData);

	std::vector<EndpointDevice> devices;
private:
	HIDDevices();

	static HIDDevices* hidDevices;
	size_t HIDDescriptorSize;
};

#endif // !HIDDEVICES_HPP
