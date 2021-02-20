#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include "../DefinedStructs/PacketExternStructs.hpp"
#include"../DefinedStructs/USBPcapStructs.hpp"
#include <qfile.h>

#include <fstream>
#include <string>

class FileReader
{
public:
	FileReader() {}
	~FileReader();
	void ReadFile(bool live);
	QByteArray& GetPacket();
	bool ReadFileHeader();
	bool EndOfFile();
	bool OpenNewFile(QString filename);
private:
	pcap_file_header gheader;
	QFile file;
};

#endif
