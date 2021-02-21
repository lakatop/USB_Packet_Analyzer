#ifndef CONSTDATAHOLDER_HPP
#define CONSTDATAHOLDER_HPP

#include "PacketExternStructs.hpp"
#include <string>
#include <map>

class DataHolder
{
public:
	static DataHolder* GetDataHolder();

	std::string GetDescriptorType(BYTE type);
	std::string GetSetupPacketRequest(BYTE request);
	std::string GetUSBTestSelector(BYTE selector);
	std::string GetReportItemType(BYTE type);
	std::string GetReportTag(BYTE tag, BYTE type);
	std::string GetReportCollectionType(BYTE collection);
	std::string GetGlobalUsagePage(BYTE value);
	std::string GetUsage(BYTE globalUsage, BYTE value);
	std::string GetGenericDesktopUsage(BYTE value);

	uint8_t BYTES_ON_ROW;
	int8_t TRANSFER_LEFTOVER_DATA;
	int8_t TRANSFER_OPTIONAL_HEADER;
	int8_t USBPCAP_HEADER_DATA;
	int8_t REPORT_DESC_TREE_INDEX;
	std::map<HeaderDataType, DataTypeColor> DataColors;
private:
	DataHolder();
	void FillDataColorsMap();
	static DataHolder* holder;
};

#endif // !CONSTDATAHOLDER_HPP