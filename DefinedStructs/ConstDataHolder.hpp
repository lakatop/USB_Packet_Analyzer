#ifndef CONSTDATAHOLDER_HPP
#define CONSTDATAHOLDER_HPP

#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "PacketExternStructs.hpp"

class DescriptorStruct;

/// <summary>
/// Class used for holding global variables and for converting data constants to string.
/// </summary>
class DataHolder
{
public:
	static DataHolder* GetDataHolder();

	std::string GetTransferType(const BYTE transfer);
	std::string GetDescriptorType(const BYTE type);
	std::string GetSetupPacketRequest(const BYTE request);
	std::string GetUSBTestSelector(const BYTE selector);
	std::string GetReportItemType(const BYTE type);
	std::string GetReportTag(const BYTE tag, const BYTE type);
	std::string GetReportCollectionType(const BYTE collection);
	std::string GetGlobalUsagePage(const BYTE value);
	std::string GetUsage(const BYTE globalUsage, const BYTE value);
	std::string GetGenericDesktopUsage(const BYTE value);

	DescriptorStruct* TryLoadNewDescriptor(BYTE descType);

	/// <summary>
	/// Delete copy constructor due to Singleton pattern
	/// </summary>
	DataHolder(const DataHolder&) = delete;
	/// <summary>
	/// Delete copy assignment due to Singleton pattern
	/// </summary>
	DataHolder& operator=(const DataHolder&) = delete;

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
	/// <summary>
	/// Vector holding all yet known descriptor structs
	/// </summary>
	std::vector<std::unique_ptr<DescriptorStruct>> descriptors;
private:
	DataHolder();
	void FillDataColorsMap();

	/// <summary>
	/// Instance of this class.
	/// </summary>
	static DataHolder* holder;

	std::string DescriptorPath;
};

#endif // !CONSTDATAHOLDER_HPP