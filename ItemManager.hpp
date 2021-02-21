#ifndef ITEMMANAGER_HPP
#define ITEMMANAGER_HPP

#include "FilesHandler/FileReader.hpp"
#include <qlistwidget.h>

#include "USB_Packet_Analyzer.h"
#include "DefinedStructs/ConstDataHolder.hpp"

class USB_Packet_Analyzer; //forward declaration

class ItemManager
{
public:
	ItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent);
	void ProcessFile(QString filename, bool liveReading);
	void ProcessPacket(QByteArray packetData);


	bool stopButtonClicked;
	bool pauseButtonClicked;
	bool atBottomOfList;
private:
	void SetItemName(QListWidgetItem* item, PUSBPCAP_BUFFER_PACKET_HEADER usbh);

	FileReader fileReader;
	QListWidget* listWidget;
	USB_Packet_Analyzer* parent;
	DataHolder* dataHolder;
};

#endif