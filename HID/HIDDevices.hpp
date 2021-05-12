#ifndef HIDDEVICES_HPP
#define HIDDEVICES_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"

#include <qbytearray.h>

/// <summary>
/// Class for handling HID device specific tasks
/// </summary>
class HIDDevices
{
public:
	static HIDDevices* GetHIDDevices();
	void ParseHIDReportDescriptor(QByteArray packetData, USHORT interfaceIndex);
	void CreateDevice(QByteArray packetData);
	size_t GetHIDDescriptorSize();
	HIDDescriptor FillUpHIDDescriptor(const unsigned char* packet);	
	Supported_Devices GetSupportedDevice(std::pair<uint32_t, uint32_t> key);
	template<typename T>
	void CharToNumberConvert(const unsigned char* addr, T& number, int size);

	/// <summary>
	/// Delete copy constructor due to Singleton pattern
	/// </summary>
	HIDDevices(const HIDDevices&) = delete;
	/// <summary>
	/// Delete copy assignment due to Singleton pattern
	/// </summary>
	HIDDevices& operator=(const HIDDevices&) = delete;
	
	/// <summary>
	/// List of devices that were/are connected to the bus in time of packet sniffering.
	/// </summary>
	std::vector<BusDevice> devices;
private:
	HIDDevices();
	void FillSupportedDeviceMap();

	/// <summary>
	/// Instance of this class
	/// </summary>
	static HIDDevices* hidDevices;
	/// <summary>
	/// Size of HID Descriptor
	/// </summary>
	size_t HIDDescriptorSize;
	/// <summary>
	/// Map whose key is Global and Local Usage, and value is Supported_Devices enum 
	/// representing whether this application supports device given by key.
	/// </summary>
	std::map<std::pair<uint32_t, uint32_t>, Supported_Devices> deviceMap;
};


/// <summary>
/// Converts char buffer to decimal number
/// </summary>
/// <typeparam name="T">Type of decimal number we want to convert to.</typeparam>
/// <param name="addr">Pointer to char buffer</param>
/// <param name="number">Serves as output parameter for decimal number</param>
/// <param name="size">Number of chars we want to convert</param>
template<typename T>
void HIDDevices::CharToNumberConvert(const unsigned char* addr, T& number, int size)
{
	for (int i = size; i > 0; i--)
	{
		number = (number << 8) | addr[i - 1];
	}
}

#endif // !HIDDEVICES_HPP
