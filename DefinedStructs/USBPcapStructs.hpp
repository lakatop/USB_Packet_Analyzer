#ifndef USBPCAPSTRUCTS_HPP
#define USBPCAPSTRUCTS_HPP

#include <Windows.h>

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

#endif