#include "ItemManager.hpp"

ItemManager::ItemManager(QListWidget* listWidget, USB_Packet_Analyzer* parent)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->atBottomOfList = false;
	this->listWidget = listWidget;
	this->parent = parent;
}
void ItemManager::ProcessFile(QString filename, bool liveReading)
{
	if (fileReader.OpenNewFile(filename))
	{
		if (fileReader.ReadFileHeader())
		{
			while (!stopButtonClicked)
			{
				while (!fileReader.EndOfFile())
				{
					QByteArray packetData = fileReader.GetPacket();
					if (!pauseButtonClicked)
					{
						ProcessPacket(packetData);
					}
				}

				parent->Refresh();

				if (!atBottomOfList)
				{
					listWidget->scrollToBottom();
				}

				if (liveReading)
				{
					Sleep(50);
				}
				else
				{
					return;
				}
			}
		}
	}

}
void ItemManager::ProcessPacket(QByteArray packetData)
{
}