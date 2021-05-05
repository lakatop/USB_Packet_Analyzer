#ifndef CONSTDATAHOLDER_HPP
#define CONSTDATAHOLDER_HPP

#include "PacketExternStructs.hpp"
#include <string>
#include <map>

/// <summary>
/// Class used for holding global variables and for converting data constants to string.
/// </summary>
class DataHolder
{
public:
	static DataHolder* GetDataHolder();

	std::string GetTransferType(BYTE transfer);
	std::string GetDescriptorType(BYTE type);
	std::string GetSetupPacketRequest(BYTE request);
	std::string GetUSBTestSelector(BYTE selector);
	std::string GetReportItemType(BYTE type);
	std::string GetReportTag(BYTE tag, BYTE type);
	std::string GetReportCollectionType(BYTE collection);
	std::string GetGlobalUsagePage(BYTE value);
	std::string GetUsage(BYTE globalUsage, BYTE value);
	std::string GetGenericDesktopUsage(BYTE value);

	/// <summary>
	/// Constant representing bytes on one hexdump row.
	/// </summary>
	uint8_t BYTES_ON_ROW;
	/// <summary>
	/// Representing Qt UserRole for leftover data.
	/// </summary>
	int TRANSFER_LEFTOVER_DATA;
	/// <summary>
	/// Representing Qt UserRole for header optional data.
	/// </summary>
	int TRANSFER_OPTIONAL_HEADER;
	/// <summary>
	/// Representing Qt UserRole for USBPcap header data.
	/// </summary>
	int USBPCAP_HEADER_DATA;

	/// <summary>
	/// Map that associate HeaderDataType to its color highlightion
	/// </summary>
	std::map<HeaderDataType, DataTypeColor> DataColors;
private:
	DataHolder();
	void FillDataColorsMap();

	/// <summary>
	/// Instance of this class.
	/// </summary>
	static DataHolder* holder;
};

#endif // !CONSTDATAHOLDER_HPP