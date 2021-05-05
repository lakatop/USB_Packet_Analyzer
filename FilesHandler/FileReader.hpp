#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include <qfile.h>

/// <summary>
/// Class for handling file operations
/// </summary>
class FileReader
{
public:
    /// <summary>
    /// Constructor of FileReader.
    /// </summary>
    FileReader() {}
	~FileReader();
	QByteArray GetPacket();
	bool ReadFileHeader();
	bool EndOfFile();
	bool OpenNewFile(QString filename);
	qint64 FileSize();
private:
	/// <summary>
	/// Global header of pcap file.
	/// </summary>
	pcap_hdr_t gheader{};
	/// <summary>
	/// QFile instance for handling file operations
	/// </summary>
	QFile file;
};

#endif // !FILEREADER_HPP
