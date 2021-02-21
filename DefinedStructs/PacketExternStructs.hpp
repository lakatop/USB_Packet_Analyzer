#ifndef PACKETEXTERNSTRUCTS_HPP
#define PACKETEXTERNSTRUCTS_HPP

#include <windows.h>

typedef LONG USBD_STATUS;

#pragma pack(1)
typedef struct USBPCAP_BUFFER_PACKET_HEADER
{
    USBPCAP_BUFFER_PACKET_HEADER(USHORT hLen, UINT64 iID, USBD_STATUS stts, USHORT fnct, UCHAR inf, USHORT bs, USHORT dvc, UCHAR ndpnt, UCHAR trnsfr, UINT32 dLen) :
        headerLen(hLen), irpId(iID), status(stts), function(fnct), info(inf), bus(bs), device(dvc), endpoint(ndpnt), transfer(trnsfr), dataLength(dLen)
    {
    }
    USBPCAP_BUFFER_PACKET_HEADER() {}
    USHORT       headerLen; /* This header length */
    UINT64       irpId;     /* I/O Request packet ID */
    USBD_STATUS  status;    /* USB status code
                               (on return from host controller) */
    USHORT       function;  /* URB Function */
    UCHAR        info;      /* I/O Request info */

    USHORT       bus;       /* bus (RootHub) number */
    USHORT       device;    /* device address */
    UCHAR        endpoint;  /* endpoint number and transfer direction */
    UCHAR        transfer;  /* transfer type */

    UINT32       dataLength;/* Data length */
} USBPCAP_BUFFER_PACKET_HEADER, * PUSBPCAP_BUFFER_PACKET_HEADER;

#pragma pack(1)
typedef struct
{
    ULONG        offset;
    ULONG        length;
    USBD_STATUS  status;
} USBPCAP_BUFFER_ISO_PACKET, * PUSBPCAP_BUFFER_ISO_PACKET;

#pragma pack(1)
typedef struct USBPCAP_BUFFER_ISOCH_HEADER
{
    //USBPCAP_BUFFER_PACKET_HEADER  header;
    ULONG                         startFrame;
    ULONG                         numberOfPackets;
    ULONG                         errorCount;
    USBPCAP_BUFFER_ISO_PACKET     packet[1];
} USBPCAP_BUFFER_ISOCH_HEADER, * PUSBPCAP_BUFFER_ISOCH_HEADER;

#define USBPCAP_CONTROL_STAGE_SETUP    0
#define USBPCAP_CONTROL_STAGE_DATA     1
#define USBPCAP_CONTROL_STAGE_STATUS   2
#define USBPCAP_CONTROL_STAGE_COMPLETE 3

#pragma pack(1)
typedef struct USBPCAP_BUFFER_CONTROL_HEADER
{
    //USBPCAP_BUFFER_PACKET_HEADER  header;
    UCHAR                         stage;
} USBPCAP_BUFFER_CONTROL_HEADER, * PUSBPCAP_BUFFER_CONTROL_HEADER;


//ADDITIONAL STRUCTS

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


typedef struct DataTypeColor
{
    int red;
    int green;
    int blue;
    int alpha;
} DataTypeColor;

//PCAP STRUCTS 

struct pcap_file_header
{
    unsigned int magic;
    u_short version_major;
    u_short version_minor;
    int thiszone;	/* gmt to local correction */
    unsigned int sigfigs;	/* accuracy of timestamps */
    unsigned int snaplen;	/* max length saved portion of each pkt */
    unsigned int linktype;	/* data link type (LINKTYPE_*) */
};

struct pcap_pkthdr
{
    struct timeval ts;	/* time stamp */
    unsigned int caplen;	/* length of portion present */
    unsigned int len;	/* length this packet (off wire) */
};


//ENUMS

enum Transfer_Header { USBPCAP_TRANSFER_ISOCHRONOUS, USBPCAP_TRANSFER_INTERRUPT, USBPCAP_TRANSFER_CONTROL, USBPCAP_TRANSFER_BULK, IRP_INFO = 0xFE };

enum HeaderDataType
{
    INTERR_TRANSFER, ISOCHRO_TRANSFER, BULK_TRANSFER, CONTROL_TRANSFER, CONTROL_TRANSFER_RESPONSE, CONTROL_TRANSFER_DEVICE_DESC, CONTROL_TRANSFER_CONFIG_DESC, CONTROL_TRANSFER_STRING_DESC,
    CONTROL_TRANSFER_INTERF_DESC, CONTROL_TRANSFER_ENDPOI_DESC, CONTROL_TRANSFER_HID_DESC, CONTROL_TRANSFER_HID_REPORT_DESC, CONTROL_TRANSFER_SETUP,
    CONTROL_TRANSFER_DEVICE_QUALIFIER_DESC, CONTROL_TRANSFER_OTHER_SPEED_CONF_DESC, CONTROL_TRANSFER_UNSPEC_DESC, ADDITIONAL_HEADER_DATA, HEADER_DATA
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
#endif