#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include <qfile.h>

class FileReader
{
public:
    FileReader() { gheader.magic_number = 0; gheader.version_major = 0; gheader.version_minor = 0; gheader.thiszone = 0; gheader.sigfigs = 0; gheader.snaplen = 0; gheader.network = 0; }
	~FileReader();
	QByteArray GetPacket();
	bool ReadFileHeader();
	bool EndOfFile();
	bool OpenNewFile(QString filename);
private:
	pcap_hdr_t gheader;
	QFile file;
};

#endif
