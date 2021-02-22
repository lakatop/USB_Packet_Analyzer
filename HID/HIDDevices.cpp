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
}

void HIDDevices::ParseHIDDescriptor()
{

}