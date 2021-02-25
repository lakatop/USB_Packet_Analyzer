/*
 * Copyright (c) 2013-2019 Tomasz Moñ <desowin@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef USBPCAP_H
#define USBPCAP_H

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <usb.h>

    typedef struct
    {
        UINT32  size;
    } USBPCAP_IOCTL_SIZE, * PUSBPCAP_IOCTL_SIZE;

#pragma pack(push)
#pragma pack(1)
    /* USBPCAP_ADDRESS_FILTER is parameter structure to IOCTL_USBPCAP_START_FILTERING. */
    typedef struct _USBPCAP_ADDRESS_FILTER
    {
        /* Individual device filter bit array. USB standard assigns device
         * numbers 1 to 127 (0 is reserved for initial configuration).
         *
         * If address 0 bit is set, then we will automatically capture from
         * newly connected devices.
         *
         * addresses[0] - 0 - 31
         * addresses[1] - 32 - 63
         * addresses[2] - 64 - 95
         * addresses[3] - 96 - 127
         */
        UINT32 addresses[4];

        /* Filter all devices */
        BOOLEAN filterAll;
    } USBPCAP_ADDRESS_FILTER, * PUSBPCAP_ADDRESS_FILTER;
#pragma pack(pop)

#define IOCTL_USBPCAP_SETUP_BUFFER \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_USBPCAP_START_FILTERING \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_USBPCAP_STOP_FILTERING \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_USBPCAP_GET_HUB_SYMLINK \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_USBPCAP_SET_SNAPLEN_SIZE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS)

    /* USB packets, beginning with a USBPcap header */
#define DLT_USBPCAP         249

#pragma pack(push, 1)
    typedef struct pcap_hdr_s
    {
        UINT32 magic_number;   /* magic number */
        UINT16 version_major;  /* major version number */
        UINT16 version_minor;  /* minor version number */
        INT32  thiszone;       /* GMT to local correction */
        UINT32 sigfigs;        /* accuracy of timestamps */
        UINT32 snaplen;        /* max length of captured packets, in octets */
        UINT32 network;        /* data link type */
    } pcap_hdr_t;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct pcaprec_hdr_s
    {
        UINT32 ts_sec;         /* timestamp seconds */
        UINT32 ts_usec;        /* timestamp microseconds */
        UINT32 incl_len;       /* number of octets of packet saved in file */
        UINT32 orig_len;       /* actual length of packet */
    } pcaprec_hdr_t;
#pragma pack(pop)

    /* All multi-byte fields are stored in .pcap file in little endian */

#define USBPCAP_TRANSFER_ISOCHRONOUS 0
#define USBPCAP_TRANSFER_INTERRUPT   1
#define USBPCAP_TRANSFER_CONTROL     2
#define USBPCAP_TRANSFER_BULK        3
#define USBPCAP_TRANSFER_IRP_INFO    0xFE
#define USBPCAP_TRANSFER_UNKNOWN     0xFF

/* info byte fields:
 * bit 0 (LSB) - when 1: PDO -> FDO
 * bits 1-7: Reserved
 */
#define USBPCAP_INFO_PDO_TO_FDO  (1 << 0)

#pragma pack(push, 1)
    typedef struct
    {
        USHORT       headerLen; /* This header length */
        UINT64       irpId;     /* I/O Request packet ID */
        USBD_STATUS  status;    /* USB status code (on return from host controller) */
        USHORT       function;  /* URB Function */
        UCHAR        info;      /* I/O Request info */

        USHORT       bus;       /* bus (RootHub) number */
        USHORT       device;    /* device address */
        UCHAR        endpoint;  /* endpoint number and transfer direction */
        UCHAR        transfer;  /* transfer type */

        UINT32       dataLength;/* Data length */
    } USBPCAP_BUFFER_PACKET_HEADER, * PUSBPCAP_BUFFER_PACKET_HEADER;
#pragma pack(pop)

    /* USBPcap versions before 1.5.0.0 recorded control transactions as two
     * or three pcap packets:
     *   * USBPCAP_CONTROL_STAGE_SETUP with 8 bytes USB SETUP data
     *   * Optional USBPCAP_CONTROL_STAGE_DATA with either DATA OUT or IN
     *   * USBPCAP_CONTROL_STAGE_STATUS without data on IRP completion
     *
     * Such capture was considered unnecessary complex. Due to that, since
     * USBPcap 1.5.0.0, the control transactions are recorded as two packets:
     *   * USBPCAP_CONTROL_STAGE_SETUP with 8 bytes USB SETUP data and
     *     optional DATA OUT
     *   * USBPCAP_CONTROL_STAGE_COMPLETE without payload or with the DATA IN
     *
     * The merit behind this change was that Wireshark dissector, since the
     * very first time when Wireshark understood USBPcap format, was really
     * expecting the USBPCAP_CONTROL_STAGE_SETUP to contain SETUP + DATA OUT.
     * Even when they Wireshark doesn't recognize USBPCAP_CONTROL_STAGE_COMPLETE
     * it will still process the payload correctly.
     */
#define USBPCAP_CONTROL_STAGE_SETUP    0
#define USBPCAP_CONTROL_STAGE_DATA     1
#define USBPCAP_CONTROL_STAGE_STATUS   2
#define USBPCAP_CONTROL_STAGE_COMPLETE 3

#pragma pack(push, 1)
    typedef struct
    {
        USBPCAP_BUFFER_PACKET_HEADER  header;
        UCHAR                         stage;
    } USBPCAP_BUFFER_CONTROL_HEADER, * PUSBPCAP_BUFFER_CONTROL_HEADER;
#pragma pack(pop)

    /* Note about isochronous packets:
     *   packet[x].length, packet[x].status and errorCount are only relevant
     *   when USBPCAP_INFO_PDO_TO_FDO is set
     *
     *   packet[x].length is not used for isochronous OUT transfers.
     *
     * Buffer data is attached to:
     *   * for isochronous OUT transactions (write to device)
     *       Requests (USBPCAP_INFO_PDO_TO_FDO is not set)
     *   * for isochronous IN transactions (read from device)
     *       Responses (USBPCAP_INFO_PDO_TO_FDO is set)
     */
#pragma pack(push, 1)
    typedef struct
    {
        ULONG        offset;
        ULONG        length;
        USBD_STATUS  status;
    } USBPCAP_BUFFER_ISO_PACKET, * PUSBPCAP_BUFFER_ISO_PACKET;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct
    {
        USBPCAP_BUFFER_PACKET_HEADER  header;
        ULONG                         startFrame;
        ULONG                         numberOfPackets;
        ULONG                         errorCount;
        USBPCAP_BUFFER_ISO_PACKET     packet[1];
    } USBPCAP_BUFFER_ISOCH_HEADER, * PUSBPCAP_BUFFER_ISOCH_HEADER;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* USBPCAP_H */


#ifndef EXTERNSTRUCTS_HPP
#define EXTERNSTRUCTS_HPP

#include <vector>
#include <map>
#include <qbytearray.h>
#include <memory>

//ENUMS

//enum Transfer_Header { USBPCAP_TRANSFER_ISOCHRONOUS, USBPCAP_TRANSFER_INTERRUPT, USBPCAP_TRANSFER_CONTROL, USBPCAP_TRANSFER_BULK, IRP_INFO = 0xFE };

enum HeaderDataType
{
    INTERR_TRANSFER, ISOCHRO_TRANSFER, BULK_TRANSFER, CONTROL_TRANSFER, CONTROL_TRANSFER_RESPONSE, CONTROL_TRANSFER_DEVICE_DESC, CONTROL_TRANSFER_CONFIG_DESC, CONTROL_TRANSFER_STRING_DESC,
    CONTROL_TRANSFER_INTERF_DESC, CONTROL_TRANSFER_ENDPOI_DESC, CONTROL_TRANSFER_HID_DESC, CONTROL_TRANSFER_HID_REPORT_DESC, CONTROL_TRANSFER_SETUP,
    CONTROL_TRANSFER_DEVICE_QUALIFIER_DESC, CONTROL_TRANSFER_OTHER_SPEED_CONF_DESC, CONTROL_TRANSFER_UNSPEC_DESC, ADDITIONAL_HEADER_DATA, HEADER_DATA,
    IRP_INFO_TRANSFER, UNKNOWN_TRANSFER
};

enum Descriptor_Types
{
    DEVICE_DESCRIPTOR = 1, CONFIGURATION_DESCRIPTOR, STRING_DESCRIPTOR, INTERFACE_DESCRIPTOR, ENDPOINT_DESCRIPTOR,
    DEVICE_QUALIFIER, OTHER_SPEED_CONFIGURATION, HID_DESCRIPTOR_ENUM = 0x21, HID_REPORT_DESCRIPTOR = 0x22
};

enum Setup_Packet_bRequest
{
    GET_STATUS, CLEAR_FEATURE, SET_FEATURE = 3, SET_ADDRESS = 5, GET_DESCRIPTOR, SET_DESCRIPTOR, GET_CONFIGURATION, SET_CONFIGURATION,
    GET_INTERFACE, SET_INTERFACE, SYNCH_FRAME
};

enum USB_Test_Selectors { TEST_J = 1, TEST_K, TEST_SE0_NAK, TEST_PACKET, TEST_FORCE_ENABLE };

enum Report_Desc_Item_Type { MAIN, GLOBAL, LOCAL };

enum Report_Desc_Main_Tags { MAIN_INPUT = 0x8, OUTPUT = 0x9, COLLECTION = 0xA, FEATURE = 0xB, END_COLLECTION = 0xC };

enum Report_Desc_Global_Tags
{
    USAGE_PAGE, LOGICAL_MINIMUM, LOGICAL_MAXIMUM, PHYSICAL_MINIMUM, PHYSICAL_MAXIMUM, UNIT_EXPONENT, UNIT, REPORT_SIZE, REPORT_ID,
    REPORT_COUNT, PUSH, POP
};

enum Report_Desc_Local_Tags
{
    USAGE, USAGE_MINIMUM, USAGE_MAXIMUM, DESIGNATOR_INDEX, DESIGNATOR_MINIMUM, DESIGNATOR_MAXIMUM, STRING_INDEX = 0X7,
    STRING_MINIMUM = 0X8, STRING_MAXIMUM = 0X9, DELIMITER = 0XA
};

enum Report_Desc_Collection_Type { PHYSICAL, APPLICATION, LOGICAL, REPORT, NAMED_ARRAY, USAGE_SWITCH, USAGE_MODIFIER };

enum Global_Usage_Pages { GENERIC_DESKTOP_PAGE = 0x01, KEYBOARD_PAGE = 0x07, LED_PAGE, BUTTON_PAGE };

enum Generic_Desktop_Usages { UNDEFINED, POINTER, MOUSE, RESERVED, JOYSTICK, GAMEPAD, KEYBOARD, KEYPAD, X = 0X30, Y, Z, RX, RY, RZ, WHEEL = 0X38, HAT_SWITCH };


//STRUCTS

typedef struct DataTypeColor
{
    int red;
    int green;
    int blue;
    int alpha;
} DataTypeColor;

struct ReportDescTreeStruct
{
    ReportDescTreeStruct() : tag(0), size(0), itemType(0), root(false), childs(), data(NULL) {}
    ReportDescTreeStruct(BYTE _tag, BYTE _size, BYTE _itemType, bool _root) :
        tag(_tag), size(_size), itemType(_itemType), root(_root), childs(), data(NULL)
    {
    }
    BYTE tag;
    BYTE size;
    BYTE itemType;
    QByteArray data;
    bool root;

    std::vector<std::shared_ptr<ReportDescTreeStruct>> childs;
};

typedef struct HIDDescriptor
{
    UCHAR                     bLength;
    UCHAR                     bDescriptorType;
    USHORT                    bcdHID;
    UCHAR                     bCountry;
    UCHAR                     bNumDescriptors;
    UCHAR                     bReportType;
    USHORT                    wReportLength;
} HIDDescriptor, * PHIDDescriptor;

typedef struct InputValues
{
    InputValues() : GlobalUsagePage(0), ReportSize(0), ReportCount(0), UsageMinimum(0), UsageMaximum(0),
        LogicalMinimum(0), LogicalMaximum(0), Variable(true), LocalUsageNames()
    {
    }

    //uint32 because i am assuming short items, and they can have data up to 4 bytes
    std::vector<uint32_t> LocalUsageNames;
    uint32_t GlobalUsagePage;
    uint32_t ReportSize;
    uint32_t ReportCount;
    uint32_t UsageMinimum;
    uint32_t UsageMaximum;
    uint32_t LogicalMinimum;
    uint32_t LogicalMaximum;
    bool Variable;
}InputValues, * PInputValues;

typedef struct HIDReportDescriptorInputParse   //just for parsing input, not for interpreting HID Report Descriptor
{
    HIDReportDescriptorInputParse() : inputSize(0), reportDefined(false), reportID(0), deviceAddress(0),
        globalItemUsage(0), localItemUsage(0), inputValues()
    {
    }

    int inputSize;
    bool reportDefined;  //true, if input is prepended with its ID
    uint32_t reportID;
    uint32_t globalItemUsage;
    uint32_t localItemUsage;
    USHORT deviceAddress;
    std::vector<InputValues> inputValues;
} HIDReportDescriptorInputParse, * PHIDReportDescriptorInputParse;

typedef struct EndpointDevice
{
	EndpointDevice() : deviceAddress(0), obsolete(true), inputparser(), endpoints() {}
	EndpointDevice(USHORT deviceAddress) : deviceAddress(deviceAddress), obsolete(false), inputparser(), endpoints() {}
	USHORT deviceAddress;
	bool obsolete;
	std::map<BYTE, std::vector<HIDReportDescriptorInputParse>> inputparser; // <endpointNum, parsers> // parsers that parse input from this endpoint. There may be more of them, in that case, they must have ReportID
	std::map<BYTE, std::vector<BYTE>> endpoints; //<interfaceNum, endpointsNum> endpoints attached to this interface number
} EndpointDevice, * PEndpointDevice;

#endif // !EXTERNSTRUCTS_HPP