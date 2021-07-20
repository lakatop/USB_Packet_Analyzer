#include "ConstDataHolder.hpp"
#include "DescriptorStruct.hpp"


DataHolder* DataHolder::holder = nullptr;

/// <summary>
/// Get instance of this class
/// </summary>
/// <returns>Pointer to instance of DataHolder class</returns>
DataHolder* DataHolder::GetDataHolder()
{
	if (holder == nullptr)
	{
		holder = new DataHolder;
	}

	return holder;
}

/// <summary>
/// Constructor of DataHolder.
/// </summary>
DataHolder::DataHolder()
{
	FillDataColorsMap();
	BYTES_ON_ROW = 16;
	TRANSFER_LEFTOVER_DATA = Qt::UserRole + 1;
	TRANSFER_OPTIONAL_HEADER = Qt::UserRole + 2;
	USBPCAP_HEADER_DATA = Qt::UserRole + 3;
	DescriptorPath = "./Descriptors/";
}

/// <summary>
/// Fills up DataColors.
/// </summary>
void DataHolder::FillDataColorsMap()
{
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(INTERR_TRANSFER, { 255,0,0,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(ISOCHRO_TRANSFER, { 255,255,0,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(BULK_TRANSFER, { 128,128,128,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER, { 0,205,0,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_RESPONSE, { 255,165,0,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_DEVICE_DESC, { 0,0,153,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_CONFIG_DESC, { 0,153,153,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_STRING_DESC, { 64,64,64,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_INTERF_DESC, { 153,0,153,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_ENDPOI_DESC, { 153,0,0,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_HID_DESC, { 153,153,0,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_SETUP, { 0,0,0,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_UNSPEC_DESC, { 255,153,255,128 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(CONTROL_TRANSFER_HID_REPORT_DESC, { 153,0,76,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(ADDITIONAL_HEADER_DATA, { 0,128,255,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(HEADER_DATA, { 255,0,255,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(IRP_INFO_TRANSFER, { 179,0,179,255 }));
	DataColors.insert(std::pair<HeaderDataType, DataTypeColor>(UNKNOWN_TRANSFER, { 0,0,255,255 }));
}

/// <summary>
/// Tries to load new descriptor from file if description for given descriptor exists
/// </summary>
/// <param name="descType">Type of descriptor we want to get</param>
/// <returns>Pointer to loaded descriptor if exists, else nullptr</returns>
DescriptorStruct* DataHolder::TryLoadNewDescriptor(BYTE descType)
{
	std::stringstream stream(DescriptorPath);
	stream << "Desc";
	stream << std::setw(2) << std::setfill('0') << descType;
	stream << ".txt";

	if (std::filesystem::exists(stream.str().c_str()))
	{
		descriptors.emplace_back(std::make_unique<DescriptorStruct>(stream.str().c_str(), descType));
		return descriptors[descriptors.size() - 1].get();
	}

	return nullptr;
}

/// <summary>
/// Get string representation for USBPcap transfer.
/// </summary>
/// <param name="transfer">USBPcap transfer</param>
/// <returns>String representation for USBPcap transfer</returns>
std::string DataHolder::GetTransferType(const BYTE transfer)
{
	switch (transfer)
	{
	case USBPCAP_TRANSFER_BULK:
		return std::string("BULK TRANSFER");
	case USBPCAP_TRANSFER_CONTROL:
		return std::string("CONTROL TRANSFER");
	case USBPCAP_TRANSFER_INTERRUPT:
		return std::string("INTERRUPT TRANSFER");
	case USBPCAP_TRANSFER_ISOCHRONOUS:
		return std::string("ISOCHRONOUS TRANSFER");
	case USBPCAP_TRANSFER_IRP_INFO:
		return std::string("IRP INFO");
	case USBPCAP_TRANSFER_UNKNOWN:
		return std::string("UNKNOWN TRANSFER");
	default:
		return std::to_string(transfer);
		break;
	}
}

/// <summary>
/// Get string representation for Descriptor_Types enum.
/// </summary>
/// <param name="type">Descriptor_Types enum</param>
/// <returns>String representation for Descriptor_Types enum</returns>
std::string DataHolder::GetDescriptorType(const BYTE type)
{
	switch (type)
	{
	case DEVICE_DESCRIPTOR:
	{
		return std::string("DEVICE_DESCRIPTOR");
	}
	case CONFIGURATION_DESCRIPTOR:
	{
		return std::string("CONFIGURATION_DESCRIPTOR");
	}
	case STRING_DESCRIPTOR:
	{
		return std::string("STRING_DESCRIPTOR");
	}
	case INTERFACE_DESCRIPTOR:
	{
		return std::string("INTERFACE_DESCRIPTOR");
	}
	case ENDPOINT_DESCRIPTOR:
	{
		return std::string("ENDPOINT_DESCRIPTOR");
	}
	case DEVICE_QUALIFIER:
	{
		return std::string("DEVICE_QUALIFIER");
	}
	case OTHER_SPEED_CONFIGURATION:
	{
		return std::string("OTHER_SPEED_CONFIGURATION");
	}
	case HID_DESCRIPTOR_ENUM:
	{
		return std::string("HID_DESCRIPTOR");
	}
	case HID_REPORT_DESCRIPTOR:
	{
		return std::string("HID_REPORT_DESCRIPTOR");
	}
	default:
		return std::to_string(type);
	}
}

/// <summary>
/// Get string representation for Setup_Packet_bRequest enum.
/// </summary>
/// <param name="request">Setup_Packet_bRequest enum</param>
/// <returns>String representation for Setup_Packet_bRequest enum</returns>
std::string DataHolder::GetSetupPacketRequest(const BYTE request)
{
	switch (request)
	{
	case GET_STATUS:
	{
		return std::string("GET_STATUS");
	}
	case CLEAR_FEATURE:
	{
		return std::string("CLEAR_FEATURE");
	}
	case SET_FEATURE:
	{
		return std::string("SET_FEATURE");
	}
	case SET_ADDRESS:
	{
		return std::string("SET_ADDRESS");
	}
	case GET_DESCRIPTOR:
	{
		return std::string("GET_DESCRIPTOR");
	}
	case SET_DESCRIPTOR:
	{
		return std::string("SET_DESCRIPTOR");
	}
	case GET_CONFIGURATION:
	{
		return std::string("GET_CONFIGURATION");
	}
	case SET_CONFIGURATION:
	{
		return std::string("SET_CONFIGURATION");
	}
	case GET_INTERFACE:
	{
		return std::string("GET_INTERFACE");
	}
	case SET_INTERFACE:
	{
		return std::string("SET_INTERFACE");
	}
	case SYNCH_FRAME:
	{
		return std::string("SYNCH_FRAME");
	}
	default:
		return std::to_string(request);
	}
}

/// <summary>
/// Get string representation for USB_Test_Selectors enum.
/// </summary>
/// <param name="selector">USB_Test_Selectors enum</param>
/// <returns>String representation for USB_Test_Selectors enum</returns>
std::string DataHolder::GetUSBTestSelector(const BYTE selector)
{
	switch (selector)
	{
	case TEST_J:
	{
		return std::string("TEST_J");
	}
	case TEST_K:
	{
		return std::string("TEST_K");
	}
	case TEST_SE0_NAK:
	{
		return std::string("TEST_SE0_NAK");
	}
	case TEST_PACKET:
	{
		return std::string("TEST_PACKET");
	}
	case TEST_FORCE_ENABLE:
	{
		return std::string("TEST_FORCE_ENABLE");
	}
	default:
		return std::string("Reserved");
	}
}

/// <summary>
/// Get string representation for Report_Desc_Item_Type enum.
/// </summary>
/// <param name="type">Report_Desc_Item_Type enum</param>
/// <returns>String representation for Report_Desc_Item_Type enum</returns>
std::string DataHolder::GetReportItemType(const BYTE type)
{
	switch (type)
	{
	case MAIN:
	{
		return std::string("MAIN");
	}
	case GLOBAL:
	{
		return std::string("GLOBAL");
	}
	case LOCAL:
	{
		return std::string("LOCAL");
	}
	default:
		return std::to_string(type);
	}
}

/// <summary>
/// Get string representation for Report Descriptor tag enums
/// </summary>
/// <param name="tag">Report Descriptor tags</param>
/// <param name="type">Report_Desc_Item_Type enum</param>
/// <returns>String representation for Report Descriptor tag enums</returns>
std::string DataHolder::GetReportTag(const BYTE tag, const BYTE type)
{
	switch (type)
	{
	case MAIN:
	{
		switch (tag)
		{
		case MAIN_INPUT:
			return std::string("INPUT");
		case OUTPUT:
			return std::string("OUTPUT");
		case COLLECTION:
			return std::string("COLLECTION");
		case FEATURE:
			return std::string("FEATURE");
		case END_COLLECTION:
			return std::string("END_COLLECTION");
		default:
			return std::to_string(tag);
		}
	}
	case GLOBAL:
	{
		switch (tag)
		{
		case USAGE_PAGE:
			return std::string("USAGE_PAGE");
		case LOGICAL_MINIMUM:
			return std::string("LOGICAL_MINIMUM");
		case LOGICAL_MAXIMUM:
			return std::string("LOGICAL_MAXIMUM");
		case PHYSICAL_MINIMUM:
			return std::string("PSHYSICAL_MINIMUM");
		case PHYSICAL_MAXIMUM:
			return std::string("PHYSICAL_MAXIMUM");
		case UNIT_EXPONENT:
			return std::string("UNIT_EXPONENT");
		case UNIT:
			return std::string("UNIT");
		case REPORT_SIZE:
			return std::string("REPORT_SIZE");
		case REPORT_ID:
			return std::string("REPORT_ID");
		case REPORT_COUNT:
			return std::string("REPORT_COUNT");
		case PUSH:
			return std::string("PUSH");
		case POP:
			return std::string("POP");
		default:
			return std::to_string(tag);
		}
	}
	case LOCAL:
	{
		switch (tag)
		{
		case USAGE:
			return std::string("USAGE");
		case USAGE_MINIMUM:
			return std::string("USAGE_MINIMUM");
		case USAGE_MAXIMUM:
			return std::string("USAGE_MAXIMUM");
		case DESIGNATOR_INDEX:
			return std::string("DESIGNATOR_INDEX");
		case DESIGNATOR_MINIMUM:
			return std::string("DESIGNATOR_MINIMUM");
		case DESIGNATOR_MAXIMUM:
			return std::string("DESIGNATOR_MAXIMUM");
		case STRING_INDEX:
			return std::string("STRING_INDEX");
		case STRING_MINIMUM:
			return std::string("STRING_MINIMUM");
		case STRING_MAXIMUM:
			return std::string("STRING_MAXIMUM");
		case DELIMITER:
			return std::string("DELIMITER");
		default:
			return std::to_string(tag);
		}
	}
	default:
		return std::to_string(type);
	}
}

/// <summary>
/// Get string representation for Report_Desc_Collection_Type enum.
/// </summary>
/// <param name="collection">Report_Desc_Collection_Type enum</param>
/// <returns>String representation for Report_Desc_Collection_Type enum</returns>
std::string DataHolder::GetReportCollectionType(const BYTE collection)
{
	switch (collection)
	{
	case PHYSICAL:
	{
		return std::string("PHYSICAL");
	}
	case APPLICATION:
	{
		return std::string("APPLICATION");
	}
	case LOGICAL:
	{
		return std::string("LOGICAL");
	}
	case REPORT:
	{
		return std::string("REPORT");
	}
	case NAMED_ARRAY:
	{
		return std::string("NAMED_ARRAY");
	}
	case USAGE_SWITCH:
	{
		return std::string("USAGE_SWITCH");
	}
	case USAGE_MODIFIER:
	{
		return std::string("USAGE_MODIFIER");
	}
	default:
		if (collection >= 0x07 && collection <= 0x7F)
		{
			return std::string("RESERVED");
		}
		else
		{
			return std::string("VENDOR-DEFINED");
		}
		break;
	}
}

/// <summary>
/// Get string representation for Global_Usage_Pages enum.
/// </summary>
/// <param name="value">Global_Usage_Pages enum</param>
/// <returns>String representation for Global_Usage_Pages enum</returns>
std::string DataHolder::GetGlobalUsagePage(const BYTE value)
{
	switch (value)
	{
	case GENERIC_DESKTOP_PAGE:
	{
		return std::string("GENERIC_DESKTOP (0x01)");
	}
	case KEYBOARD_PAGE:
	{
		return std::string("KEYBOARD (0x07)");
	}
	case LED_PAGE:
	{
		return std::string("LED (0x08)");
	}
	case BUTTON_PAGE:
	{
		return std::string("BUTTON (0x09)");
	}
	default:
		return std::to_string(value);
	}
}

/// <summary>
/// Get string representation for specific usage name specified by global usage and usage ID
/// </summary>
/// <param name="globalUsage">Global_Usage_Pages enum</param>
/// <param name="value">Usage ID</param>
/// <returns>String representation for specific usage name specified by global usage and usage ID</returns>
std::string DataHolder::GetUsage(const BYTE globalUsage, const BYTE value)
{
	switch (globalUsage)
	{
	case GENERIC_DESKTOP_PAGE:
	{
		return GetGenericDesktopUsage(value);
	}
	default:
		return std::to_string(value);
	}
}

/// <summary>
/// Get string representation for Generic_Desktop_Usages enum.
/// </summary>
/// <param name="value">Generic_Desktop_Usages enum</param>
/// <returns>String representation for Generic_Desktop_Usages enum</returns>
std::string DataHolder::GetGenericDesktopUsage(const BYTE value)
{
	switch (value)
	{
	case UNDEFINED:
	{
		return std::string("UNDEFINED (0x00)");
	}
	case POINTER:
	{
		return std::string("POINTER (0x01)");
	}
	case MOUSE:
	{
		return std::string("MOUSE (0x02)");
	}
	case RESERVED:
	{
		return std::string("RESERVED (0x03)");
	}
	case JOYSTICK:
	{
		return std::string("JOYSTICK (0x04)");
	}
	case GAMEPAD:
	{
		return std::string("GAMEPAD (0x05)");
	}
	case KEYBOARD:
	{
		return std::string("KEYBOARD (0x06)");
	}
	case KEYPAD:
	{
		return std::string("KEYPAD (0x07)");
	}
	case X:
	{
		return std::string("X (0x30)");
	}
	case Y:
	{
		return std::string("Y (0x31)");
	}
	case Z:
	{
		return std::string("Z (0x32)");
	}
	case RX:
	{
		return std::string("RX (0x33)");
	}
	case RY:
	{
		return std::string("RY (0x34)");
	}
	case RZ:
	{
		return std::string("RZ (0x35)");
	}
	case WHEEL:
	{
		return std::string("WHEEL (0x38)");
	}
	case HAT_SWITCH:
	{
		return std::string("HAT_SWITCH (0x39)");
	}
	default:
		return std::to_string(value);
	}
}