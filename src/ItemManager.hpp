#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include "../FilesHandler/FileReader.hpp"
#include <qlistwidget.h>

#include "USB_Packet_Analyzer.h"
#include "../DefinedStructs/ConstDataHolder.hpp"
#include "../HID/HIDDevices.hpp"

#include <winusb.h>

class USB_Packet_Analyzer; //forward declaration

class ItemManager
{
public:
	ItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent);
	void ProcessFile(QString filename, bool liveReading);
	void ProcessPacket(QByteArray packetData);
	void AppendItem();
	HeaderDataType GetDataType(QListWidgetItem* currentItem, QListWidgetItem* previousItem);


	bool stopButtonClicked;
	bool pauseButtonClicked;
	bool atBottomOfList;
private:
	QString SetItemName(PUSBPCAP_BUFFER_PACKET_HEADER usbh, const unsigned char* packet);
	void FillUpItem(QByteArray packetData);
	void CheckForSetupPacket(QByteArray packetData);

	FileReader fileReader;
	QListWidget* listWidget;
	USB_Packet_Analyzer* parent;
	DataHolder* dataHolder;
	HIDDevices* hidDevices;
	bool representingHIDDescriptor;
	bool representingConfigurationDescriptor;
};

#endif