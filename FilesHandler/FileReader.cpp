#include "FileReader.hpp"

/// <summary>
/// Desctructor of FileReader
/// </summary>
FileReader::~FileReader()
{
	if (file.isOpen())
	{
		file.close();
	}
}

/// <summary>
/// Processes one packet from file. Saves dat arepresenting given packet into QByteArray and returns it.
/// </summary>
/// <returns>QByteArray representing one packet</returns>
QByteArray FileReader::GetPacket()
{
	if (!QFile::exists(file.fileName()))
	{
		return QByteArray();
	}
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

/// <summary>
/// Reads pcap file header.
/// </summary>
/// <returns>Wheter read was successful</returns>
bool FileReader::ReadFileHeader()
{
	if (file.isOpen())
	{
		qint64 bytesRead = file.read((char*)&gheader, sizeof(pcap_hdr_t));
		return (bytesRead == sizeof(pcap_hdr_t));
	}

	return false;
}

/// <summary>
/// Determines whether we are at the end of file.
/// </summary>
/// <returns>Whether we are at the ed of file</returns>
bool FileReader::EndOfFile()
{
	if (file.isOpen())
	{
		return file.atEnd();
	}

	return true;
}

/// <summary>
/// Opens new file for read.
/// </summary>
/// <param name="filename">Name of file which should be opened for read</param>
/// <returns>Whether opening was successful</returns>
bool FileReader::OpenNewFile(const QString filename)
{
	if (file.isOpen())
	{
		file.close();
	}

	file.setFileName(filename);
	return file.open(QIODevice::ReadOnly);
}

/// <summary>
/// Get size of actual file.
/// </summary>
/// <returns>Size of file</returns>
qint64 FileReader::FileSize()
{
	return file.size();
}