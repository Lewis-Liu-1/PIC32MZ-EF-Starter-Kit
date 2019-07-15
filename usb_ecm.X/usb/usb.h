#ifndef USB_H_
#define _USB_H_


// Bitmap for Serial_State notification
// As found here: https://www.microchip.com/forums/m479957.aspx
#define CDC_DCD         0x01
#define CDC_DSR         0x02
#define CDC_BREAK       0x04    // break reception
#define CDC_RI          0x08
#define CDC_FRAME       0x10    // frame error
#define CDC_PARITY      0x20    // parity error
#define CDC_OVRRUN      0x40    // overrun error


#define CDC_SERIAL_STATE_MASK   (CDC_DCD | CDC_DSR)     

/*********************************************************************
 CDC ACM Related Variables
 *********************************************************************/
uint32_t dwDTERate;
uint8_t bCharFormat;
uint8_t bParityType;
uint8_t bDataBits;
// Serial State default: 115200 BAUD, 1 Stop bit, No parity, 8 Data bits.
//uint8_t defaultLineCoding[] = {0x00, 0xC2, 0x01, 0x00, 0x01, 0x00, 0x08};
// Serial State default: 9600 BAUD, 1 Stop bit, No parity, 8 Data bits.
uint8_t defaultLineCoding[] = {0x80, 0x25, 0x00, 0x00, 0x01, 0x00, 0x08};
uint8_t *serialState;
uint16_t controlState;

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))

#define USBD_VID_USB 0x6666
#define USBD_PID_USB 0x8887

/*********************************************************************
 Device Descriptors
 *********************************************************************/
unsigned char dev_desc[] = {
    /* Descriptor Length						*/ 18, /* Always 18 or 0x12 */
    /* DescriptorType: DEVICE					*/ 0x01,
    /* bcdUSB (ver 2.0)							*/ 0x00, 0x02,
    /* bDeviceClass								*/ 0x00, /* 0x00 = MIDI, 0x01 = Audio, 0x02 = Comm */
    /* bDeviceSubClass							*/ 0x00, /* unused for MIDI, 0x02 = acm */
    /* bDeviceProtocol							*/ 0x00, /* unused for MIDI, 0x01 = at */
    /* bMaxPacketSize0							*/ 0x40, //was 0x8
    LOBYTE(USBD_VID_USB), /*idVendorL*/
    HIBYTE(USBD_VID_USB), /*idVendorH*/
    LOBYTE(USBD_PID_USB), /*idProductL*/
    HIBYTE(USBD_PID_USB), /*idProductH*/

    /* bcdDevice								*/ 0x00, 0x02, /* e.g. - 01.00 */
    /* iManufacturer							*/ 0x01,
    /* iProduct									*/ 0x02,
    /* iSerialNumber							*/ 0x03, /* unused for midi */
    /* bNumConfigurations						*/ 0x01
};

static unsigned char CDC_ConfigDescriptorArray[] = {
    0x09, /*0  bLength*/
    0x02, /*1  bDescriptortype - Configuration*/
    71, 00, //WAS 0X47 /*2  wTotalLength*/
    0x02, /*4  bNumInterfaces*/
    0x01, /*5  bConfigurationValue*/
    0x00, /*6  iConfiguration - index of string*/
    0x80, /*7  bmAttributes - Bus powered*/
    0x0, /*8  bMaxPower - 400mA*//*CDC Communication interface*/

    /*********************************************************************
      Interface 0 - Communications Class
     *********************************************************************/
    0x09, /*0  bLength*/
    0x04, /*1  bDescriptorType - Interface*/
    0x00, /*2  bInterfaceNumber - Interface 0*/
    0x00, /*3  bAlternateSetting*/
    0x01, /*4  bNumEndpoints*/
    0x02, /*5  bInterfaceClass*/
    0x06, /*6  bInterfaceSubClass - Ethernet Control Model*/
    0x00, /*7  bInterfaceProtocol - 1 at command, 0 No specific protocol*/
    0x00, /*8  iInterface - No string descriptor*//*Header Functional descriptor*/
    
    0x05, /*0  bLength*/
    0x24, /*1  bDescriptortype, CS_INTERFACE*/
    0x00, /*2  bDescriptorsubtype, HEADER Functional Descriptor*/
    0x20, 0x01, /*3  bcdCDC*//*Union Functional descriptor*/

    0x05, /*0  bLength*/
    0x24, /*1  bDescriptortype, CS_INTERFACE*/
    0x06, /*2  bDescriptorsubtype, Union Functional Descriptor*/
    0x00, //control interface
    0x01, /* subordinate interface*/

    0x0D, /*0 bLength - 13 bytes*/
    0x24, /*1 bDescriptortype, CS_INTERFACE*/
    0x0F, /*2 bDescriptorsubtype, ETHERNET NETWORKING*/
    0x03, /*3 iMACAddress, Index of MAC address string*/
    0x00, 0x00, 0x00, 0x00, /*4 bmEthernetStatistics - Handles None*/
    0xEA, 05, /*8 wMaxSegmentSize - 1514 bytes*/
    0x00, 0x00, /*10 wNumberMCFilters - No multicast filters*/
    0x00, /*12 bNumberPowerFilters - No wake-up feature*//*Notification Endpoint descriptor*/
    
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 1          */ 0x81, /* was 0x01 */
    /* bmAttributes: BULK                       */ 0x03,
    /* max packet size (LSB)                    */ 0x8,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x255,

    /*********************************************************************
      Interface 1 - Communications Class
     *********************************************************************/
    0x09, /*0  bLength*/
    0x04, /*1  bDescriptorType - Interface*/
    0x01, /*2  bInterfaceNumber - Interface 0*/
    0x00, /*3  bAlternateSetting*/
    0x02, /*4  bNumEndpoints*/
    0x0a, /*5  bInterfaceClass*/
    0x00, /*6  bInterfaceSubClass - Ethernet Control Model*/
    0x00, /*7  bInterfaceProtocol - 1 at command, 0 No specific protocol*/
    0x00, /*8  iInterface - No string descriptor*//*Header Functional descriptor*/

    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 2          */ 0x82, /* in endpoint 2 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00,
    
        /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 3          */ 0x3, /* out endpoint 3 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00,
};

/*********************************************************************
Language ID Descriptor
 *********************************************************************/
unsigned char string0[] = {
    /* 	Descriptor Length                       */ 4, /* 4 or 0x04 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* 	Language ID: English					*/ 0x09, 0x04
}; // 0x0409
/* *******************************************************************
String Descriptor: "Lewis Technologies Group"
 *********************************************************************/
unsigned char string1[] = {
    /* 	Descriptor Length                       */ 10,
    /*  DescriptorType: STRING  				*/ 0x03,
    /*	Vendor Name     						*/ 'A', 0, 'c', 0, 'm', 0, 'e', 0,
}; // Lewis Techno Group	0x4C,0,0x65,0,0x77,0,0x69,0,0x73,
// 0,0x20,0,0x54,0,0x65,0,0x63,0,0x68,0,0x6E,0,0x6F,0,0x20,0,0x47,0,0x72,0,0x6F,0,
// 0x75,0,0x70,0						

//[Mon Jul 15 22:33:39 2019] usb 1-7: New USB device strings: Mfr=1, Product=2, SerialNumber=3
//[Mon Jul 15 22:33:39 2019] usb 1-7: Product: ECM
//[Mon Jul 15 22:33:39 2019] usb 1-7: Manufacturer: Acme
//[Mon Jul 15 22:33:39 2019] usb 1-7: SerialNumber: 020255345712

//[Mon Jul 15 22:07:49 2019] usb 1-7: Product: Virtual Serial Port
//[Mon Jul 15 22:07:49 2019] usb 1-7: Manufacturer: PIC32MZ CDC ACM
//[Mon Jul 15 22:07:49 2019] usb 1-7: SerialNumber: 123-4567

/*********************************************************************
 String Descriptor (Product Name): "USB Example"
 *********************************************************************/
unsigned char string2[] = {
    /* Descriptor Length                       */ 8, /* 40 or 0x28 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Product Name                            */ 'E', 0, 'C', 0, 'M', 0,
};

//    'u', 0, 'a', 0, 'l', 0, ' ', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0, ' ', 0, 'P', 0,
//    'o', 0, 'r', 0, 't', 0
//}; // USB Example 0x55,0x53,0x42,0x20,
// 0x45,0x78,0x61,0x6D,0x70,0x6C,0x65
// 'U',0,'S',0,'B',0,' ',0,
// 'E',0,'x',0,'a',0,'m',0,'p',0,'l',0,'e',0
/*********************************************************************
 String Descriptor (Serial Number): "xxx-xxxx"
 *********************************************************************/
//020255345712
unsigned char string3[] = {
    /* Descriptor Length                       */ 18, /* 18 or 0x12 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Serial Number                           */ '0', 0, '2', 0, '2', 0, '5', 0,
    '5', 0, '3', 0, '4', 0, '5', 0, '7',0,'1',0,'2',0
}; // 123-4567

unsigned char string4[] = {
    /* Descriptor Length                       */ 4, /* 4 or 0x04 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Configuration                           */ 0x01, 0x00
};

unsigned char string5[] = {
    /* Descriptor Length                       */ 10, /* 10 or 0x0A */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Interface                               */ 'T', 0, 'e', 0, 's', 0, 't', 0
};

/*********************************************************************
 Status Messages/Replies
 *********************************************************************/
unsigned char replyZero = {0x00};
unsigned char replyDoubleZero[] = {0x00, 0x00};
unsigned char replyZeroOne[] = {0x00, 0x01};

#endif