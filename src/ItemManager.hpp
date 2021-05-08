#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include "../FilesHandler/FileReader.hpp"
#include <qprogressbar.h>
#include <qfilesystemwatcher.h>
#include <qtimer.h>

#include "USB_Packet_Analyzer.h"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <winusb.h>

class USB_Packet_Analyzer; //forward declaration

/// <summary>
/// Class that handles pcap file processing and extracting packet data into individual items
/// </summary>
class ItemManager
{
public:
	static ItemManager* GetItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent);
	void ProcessFile(QString filename, bool liveReading);
	void ProcessPacket(QByteArray packetData);
	HeaderDataType GetDataType(QTableWidgetItem* currentItem, QTableWidgetItem* previousItem);
	void ProcessFileTillEnd(bool liveReading);

	bool processingFile;
	/// <summary>
	/// Represents whether Stop Button is clicked.
	/// </summary>
	bool stopButtonClicked;
	/// <summary>
	/// Represents whether Pause Button is clicked.
	/// </summary>
	bool pauseButtonClicked;
	/// <summary>
	/// Represents current index of last item.
	/// </summary>
	unsigned long long itemIndex;
	/// <summary>
	/// FileReader instance for reading files.
	/// </summary>
	FileReader fileReader;
private:
	ItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent);
	void InsertRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet);
	void FillUpItem(QByteArray packetData);
	void CheckForSetupPacket(QByteArray packetData);
	void ColorRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh);

	/// <summary>
	/// Stataic pointer to instance of this class.
	/// </summary>
	static ItemManager* itemManager;
	/// <summary>
	/// Pointer to tableWidget that represents general packet data
	/// </summary>
	QTableWidget* tableWidget;
	/// <summary>
	/// Pointer to main application class.
	/// </summary>
	USB_Packet_Analyzer* parent;
	/// <summary>
	/// Pointer to DataHolder class.
	/// </summary>
	DataHolder* dataHolder;
	/// <summary>
	/// Pointer to HIDDevices class.
	/// </summary>
	HIDDevices* hidDevices;
	/// <summary>
	/// Whether packet data represents HID Report Descriptor
	/// </summary>
	bool representingHIDDescriptor;
	/// <summary>
	/// Whetherpacket data represents Configuration Descriptor
	/// </summary>
	bool representingConfigurationDescriptor;
};

#endif // !ITEMMANAGER_HPP