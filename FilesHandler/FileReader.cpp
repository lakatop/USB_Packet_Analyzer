#include "FileReader.hpp"

FileReader::~FileReader()
{
	if (file.isOpen())
	{
		file.close();
	}
}

QByteArray FileReader::GetPacket()
{
	QByteArray packetData;
	//read .pcap packet header data
	pcaprec_hdr_t packetHeader;
	qint64 bytesRead = file.read((char*)&packetHeader, sizeof(pcaprec_hdr_t));
	if (bytesRead != sizeof(pcaprec_hdr_t))
	{
		return QByteArray();
	}
	//read usbpcap packet header data
	USBPCAP_BUFFER_PACKET_HEADER usbh;
	bytesRead = file.read((char*)&usbh, sizeof(USBPCAP_BUFFER_PACKET_HEADER));
	if (bytesRead != sizeof(USBPCAP_BUFFER_PACKET_HEADER))
	{
		return QByteArray();
	}
	if (!file.seek(file.pos() - sizeof(USBPCAP_BUFFER_PACKET_HEADER)))
	{
		return QByteArray();
	}

	packetData.resize(usbh.dataLength + usbh.headerLen);
	bytesRead = file.read(packetData.data(), ((qint64)usbh.dataLength) + usbh.headerLen);
	if (bytesRead != ((qint64)usbh.dataLength) + usbh.headerLen)
	{
		return QByteArray();
	}

	return packetData;
}

bool FileReader::ReadFileHeader()
{
	if (file.isOpen())
	{
		qint64 bytesRead = file.read((char*)&gheader, sizeof(pcap_hdr_t));
		return (bytesRead == sizeof(pcap_hdr_t));
	}

	return false;
}

bool FileReader::EndOfFile()
{
	if (file.isOpen())
	{
		return file.atEnd();
	}

	return true;
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

qint64 FileReader::FileSize()
{
	return file.size();
}