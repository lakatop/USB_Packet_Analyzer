#pragma once

#include "FilesHandler/FileReader.hpp"
#include <qlistwidget.h>

class ItemManager
{
public:
	ItemManager(QListWidget* listWidget);
	void ProcessFile(QString filename, bool liveReading);
	void ProcessPacket(QByteArray packetData);


	bool stopButtonClicked;
	bool pauseButtonClicked;
	bool atBottomOfList;
private:
	FileReader fileReader;
	QListWidget* listWidget;
};