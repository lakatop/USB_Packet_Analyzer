#include "FileReader.hpp"

FileReader::~FileReader()
{
	if (file.isOpen())
	{
		file.close();
	}
}

void FileReader::ReadFile(bool live)
{
	//std::ifstream ifs(fileName, std::fstream::binary | std::fstream::in, _SH_DENYNO);
	//if (ifs.is_open())
	//{
	//	pcap_file_header gheader;
	//	ifs.read((char*)&gheader, sizeof(struct pcap_file_header));
	//	auto a = gheader.linktype;
	//}
	//ifs.close();
	//QFile file(fileName.c_str());
	//QByteArray arr;
	//arr.resize(sizeof(struct pcap_file_header));
	//if (file.open(QIODevice::ReadOnly))
	//{
	//	file.read(arr.data(), sizeof(struct pcap_file_header));
	//	pcap_file_header* gheader2 = (pcap_file_header*)arr.data();
	//	auto a = gheader2->linktype;
	//}
	//file.close();
}

QByteArray& FileReader::GetPacket()
{
	QByteArray packetData;
	USBPCAP_BUFFER_PACKET_HEADER usbh;
	qint64 bytesRead = file.read((char*)&usbh, sizeof(USBPCAP_BUFFER_PACKET_HEADER));
	if (bytesRead != sizeof(USBPCAP_BUFFER_PACKET_HEADER))
	{
		return QByteArray();
	}
	if (!file.seek(file.pos() - sizeof(USBPCAP_BUFFER_PACKET_HEADER)))
	{
		return QByteArray();
	}
	packetData.resize(usbh.dataLength + usbh.headerLen);
	bytesRead = file.read(packetData.data(), usbh.dataLength + usbh.headerLen);
	if (bytesRead != usbh.dataLength + usbh.headerLen)
	{
		return QByteArray();
	}

	return packetData;
}

bool FileReader::ReadFileHeader()
{
	if (file.isOpen())
	{
		qint64 bytesRead = file.read((char*)&gheader, sizeof(struct pcap_file_header));
		return (bytesRead == sizeof(struct pcap_file_header));
	}
}

bool FileReader::EndOfFile()
{
	if (file.isOpen())
	{
		return file.atEnd();
	}
}

bool FileReader::OpenNewFile(QString filename)
{
	if (file.isOpen())
	{
		file.close();
	}

	file.setFileName(filename);
	return file.open(QIODevice::ReadOnly);
}