#include "ItemManager.hpp"

ItemManager::ItemManager(QListWidget* listWidget)
{
	this->stopButtonClicked = false;
	this->pauseButtonClicked = false;
	this->atBottomOfList = false;
	this->listWidget = listWidget;
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

				listWidget->update();

				if (!atBottomOfList)
				{
					listWidget->scrollToBottom();
				}

				if (liveReading)
				{
					Sleep(50);
				}
			}
		}
	}

}
void ItemManager::ProcessPacket(QByteArray packetData)
{

}