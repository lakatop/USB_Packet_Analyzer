#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include "../FilesHandler/FileReader.hpp"
#include <qprogressbar.h>

#include "USB_Packet_Analyzer.h"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <winusb.h>

class USB_Packet_Analyzer; //forward declaration

class ItemManager
{
public:
	static ItemManager* GetItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent);
	void ProcessFile(QString filename, bool liveReading);
	void ProcessPacket(QByteArray packetData);
	HeaderDataType GetDataType(QTableWidgetItem* currentItem, QTableWidgetItem* previousItem);


	bool stopButtonClicked;
	bool pauseButtonClicked;
	bool atBottomOfList;
private:
	ItemManager(QTableWidget* tableWidget, USB_Packet_Analyzer* parent);
	void InsertRow(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet);
	void FillUpItem(QByteArray packetData);
	void CheckForSetupPacket(QByteArray packetData);

	static ItemManager* itemManager;
	FileReader fileReader;
	QTableWidget* tableWidget;
	USB_Packet_Analyzer* parent;
	DataHolder* dataHolder;
	HIDDevices* hidDevices;
	bool representingHIDDescriptor;
	bool representingConfigurationDescriptor;
};

#endif