#ifndef HIDDEVICES_HPP
#define HIDDEVICES_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"

class HIDDevices
{
public:
	static HIDDevices* GetHIDDevices();
	void ParseHIDDescriptor();

	std::vector<EndpointDevice> devices;
private:
	HIDDevices();

	static HIDDevices* hidDevices;
};

#endif // !HIDDEVICES_HPP
