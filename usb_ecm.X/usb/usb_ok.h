/* 
 * USB.h - for PIC32MZ_CDC_ACM
 * Released under FreeBSD License 
 * 
 * (C)2019 - Mark Serran Lewis 
 * mark@lewistechnogroup.com
 * All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without      
 *  modification, are permitted provided that the following conditions are  
 *  met:                                                                    
 *                                                                          
 *  1. Redistributions of source code must retain the above copyright       
 *     notice, this list of conditions and the following disclaimer.        
 *  2. Redistributions in binary form must reproduce the above copyright    
 *     notice, this list of conditions and the following disclaimer in the  
 *     documentation and/or other materials provided with the distribution. 
 *                                                                          
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
 *                                                                          
 *  The views and conclusions contained in the software and documentation   
 *  are those of the authors and should not be interpreted as representing  
 *  official policies, either expressed or implied, of the FreeBSD Project. 
 *
 */

#ifndef _USB_H    /* Guard against multiple inclusion */
#define _USB_H

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


// NOTE: These bits hold their state after notification is sent
// other bits are cleared (single shot)
// see section "6.3.5 SerialState" on the CDC spec

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

#if 0
/* idVendor									*/ 0xD8, 0x04, /* e.g. - 0x1234 */
/* idProduct								*/ 0x01, 0x00, /* e.g. - 0x0001 */
#endif

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))

#define USBD_VID_USB                        0x304
//product id
#define USBD_PID_USB                        0xe457

/*********************************************************************
 Device Descriptors
 *********************************************************************/
unsigned char dev_desc[] = {
    /* Descriptor Length						*/ 18, /* Always 18 or 0x12 */
    /* DescriptorType: DEVICE					*/ 0x01,
    /* bcdUSB (ver 2.0)							*/ 0x00, 0x02,
    /* bDeviceClass								*/ 0x02, /* 0x00 = MIDI, 0x01 = Audio, 0x02 = Comm */
    /* bDeviceSubClass							*/ 0x00, /* unused for MIDI, 0x02 = acm */
    /* bDeviceProtocol							*/ 0x00, /* unused for MIDI, 0x01 = at */
    /* bMaxPacketSize0							*/ 0x08,
    LOBYTE(USBD_VID_USB), /*idVendorL*/
    HIBYTE(USBD_VID_USB), /*idVendorH*/
    LOBYTE(USBD_PID_USB), /*idProductL*/
    HIBYTE(USBD_PID_USB), /*idProductH*/

    /* bcdDevice								*/ 0x00, 0x01, /* e.g. - 01.00 */
    /* iManufacturer							*/ 0x01,
    /* iProduct									*/ 0x02,
    /* iSerialNumber							*/ 0x03, /* unused for midi */
    /* bNumConfigurations						*/ 0x01
};

#define CONFIG_SIZE (9+9+5+5+7+9+0xd+7+7)
/*USB Configuration Descriptor*/
static unsigned char CDC_ConfigDescriptorArray_EHERNET[] = {
    0x09, /*0  bLength*/
    0x02, /*1  bDescriptortype - Configuration*/
    CONFIG_SIZE, 00, //WAS 0X47 /*2  wTotalLength*/
    0x02, /*4  bNumInterfaces*/
    0x01, /*5  bConfigurationValue*/
    0x04, /*6  iConfiguration - index of string*/
    0xC0, /*7  bmAttributes - Bus powered*/
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
    0x02, /*6  bInterfaceSubClass - Ethernet Control Model*/
    0x01, /*7  bInterfaceProtocol - 1 at command, 0 No specific protocol*/
    0x00, /*8  iInterface - No string descriptor*//*Header Functional descriptor*/

    /*********************************************************************
     Header Functional Descriptor
     *********************************************************************/

    0x05, /*0  bLength*/
    0x24, /*1  bDescriptortype, CS_INTERFACE*/
    0x00, /*2  bDescriptorsubtype, HEADER*/
    0x10, 0x01, /*3  bcdCDC*//*Union Functional descriptor*/

    /*********************************************************************
     Union Functional Descriptor
     *********************************************************************/
    0x05, /*0  bLength*/
    0x24, /*1  bDescriptortype, CS_INTERFACE*/
    0x06, /*2  bDescriptorsubtype, UNION*/
    0x00, /*3  bControlInterface - Interface 0*/
    0x01, /*4  bSubordinateInterface0 - Interface 1*//*Ethernet Networking Functional descriptor*/

    /*********************************************************************
     Interrupt IN Endpoint - 1
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 1          */ 0x81, /* was 0x81 */
    /* bmAttributes: INTERRUPT                  */ 0x03, /* was 0x01 */
    /* max packet size (LSB)                    */ 0x08,
    /* max packet size (MSB)                    */ 0x00,
    /* polling interval                         */ 0xFF, /* was 0x02 */

    /*********************************************************************
     Interface 1 - Data Class Interface
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: INTERFACE               */ 0x04,
    /* interface index                          */ 0x01,
    /* alt setting index                        */ 0x00,
    /* bNumEndpoints: 2 endpoint(s)             */ 0x02,
    0x06, /*6  bInterfaceSubClass - Ethernet Control Model*/
    /* bInterfaceSubclass: unused               */ 0x00,
    /* bInterfaceProtocol: None                 */ 0x00,
    /* iInterface                               */ 0x00,

    0x0D, /*0 bLength - 13 bytes*/
    0x24, /*1 bDescriptortype, CS_INTERFACE*/
    0x0F, /*2 bDescriptorsubtype, ETHERNET NETWORKING*/
    0x03, /*3 iMACAddress, Index of MAC address string*/
    0x00, 0x00, 0x00, 0x00, /*4 bmEthernetStatistics - Handles None*/
    0xEA, 05, /*8 wMaxSegmentSize - 1514 bytes*/
    0x00, 0x00, /*10 wNumberMCFilters - No multicast filters*/
    0x00, /*12 bNumberPowerFilters - No wake-up feature*//*Notification Endpoint descriptor*/

    /*********************************************************************
     Endpoint 1 (Bulk IN)
     *********************************************************************/

    /*********************************************************************
     Endpoint 1 (Bulk IN)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 2          */ 0x82, /* was 0x01 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00,
    /*********************************************************************
     Endpoint 2 (Bulk OUT)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: OUT Endpoint 3         */ 0x03, /* was 0x82 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00
};

uint8_t CDC_ConfigDescriptorArray_new[] = {
    /*Configuration Descriptor*/
    0x09, /* bLength: Configuration Descriptor size */
    0x02, /* bDescriptorType: Configuration */
    67, /* wTotalLength:no of returned bytes */
    0x00,
    0x02, /* bNumInterfaces: 2 interface */
    0x01, /* bConfigurationValue: Configuration value */
    0x00, /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0, /* bmAttributes: self powered */
    0x0, /* MaxPower 0 mA */

    /*---------------------------------------------------------------------------*/

    /*Interface Descriptor */
    0x09, /* bLength: Interface Descriptor size */
    0x04, /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x01, /* bNumEndpoints: One endpoints used */
    0x02, /* bInterfaceClass: Communication Interface Class */
    0x02, /* bInterfaceSubClass: Abstract Control Model */
    0x01, /* bInterfaceProtocol: Common AT commands */
    0x00, /* iInterface: */

    /*Header Functional Descriptor*/
    0x05, /* bLength: Endpoint Descriptor size */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x00, /* bDescriptorSubtype: Header Func Desc */
    0x10, /* bcdCDC: spec release number */
    0x01,

    /*Call Management Functional Descriptor*/
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x01, /* bDescriptorSubtype: Call Management Func Desc */
    0x00, /* bmCapabilities: D0+D1 */
    0x01, /* bDataInterface: 1 */

    /*ACM Functional Descriptor*/
    0x04, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x02, /* bDescriptorSubtype: Abstract Control Management desc */
    0x02, /* bmCapabilities */

    /*Union Functional Descriptor*/
    0x05, /* bFunctionLength */
    0x24, /* bDescriptorType: CS_INTERFACE */
    0x06, /* bDescriptorSubtype: Union func desc */
    0x00, /* bMasterInterface: Communication class interface */
    0x01, /* bSlaveInterface0: Data Class Interface */

    /*Endpoint 1 was (2) Descriptor*/
    0x07, /* bLength: Endpoint Descriptor size */
    0x05, /* bDescriptorType: Endpoint */
    0x81, //was 0x82                  /* bEndpointAddress */
    0x03, /* bmAttributes: Interrupt */
    LOBYTE(8), /* wMaxPacketSize: */
    HIBYTE(8),
#ifdef USE_USB_OTG_HS
    0x10, /* bInterval: */
#else
    0xFF, /* bInterval: */
#endif /* USE_USB_OTG_HS */

    /*---------------------------------------------------------------------------*/

    /*Data class interface descriptor*/
    0x09, /* bLength: Endpoint Descriptor size */
    0x04, /* bDescriptorType: */
    0x01, /* bInterfaceNumber: Number of Interface */
    0x00, /* bAlternateSetting: Alternate setting */
    0x02, /* bNumEndpoints: Two endpoints used */
    0x0A, /* bInterfaceClass: CDC */
    0x00, /* bInterfaceSubClass: */
    0x00, /* bInterfaceProtocol: */
    0x00, /* iInterface: */

    /*Endpoint OUT Descriptor*/
    0x07, /* bLength: Endpoint Descriptor size */
    0x05, /* bDescriptorType: Endpoint */
    0x01, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(64), /* wMaxPacketSize: */
    HIBYTE(64),
    0x00, /* bInterval: ignore for Bulk transfer */

    /*Endpoint IN Descriptor*/
    0x07, /* bLength: Endpoint Descriptor size */
    0x05, /* bDescriptorType: Endpoint */
    0x81, /* bEndpointAddress */
    0x02, /* bmAttributes: Bulk */
    LOBYTE(64), /* wMaxPacketSize: */
    HIBYTE(64),
    0x00 /* bInterval: ignore for Bulk transfer */
};

unsigned char CDC_ConfigDescriptorArray[] = {
    // Referenced from: https://gist.github.com/tai/acd59b125a007ad47767
    /*********************************************************************
     Configuration Descriptors 
     *********************************************************************/
    /*  bLength (Descriptor Length)             */ 9,
    /*  bDescriptorType: CONFIG					*/ 0x02,
    /*  wTotalLength							*/ 0x43, 0x00,
    /*  bNumInterfaces							*/ 2,
    /*  bConfigurationValue						*/ 1,
    /*  iConfiguration							*/ 0,
    /*  bmAttributes							*/ 0xC0, /* bit 6 set = bus powered = 0x80, 0xC0 is for self powered */
    /*  bMaxPower								*/ 0x0, /* Value x 2mA, set to 0 for self powered, was 0x32 */
    /*********************************************************************
      Interface 0 - Communications Class
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: INTERFACE               */ 0x04,
    /* bInterfaceNumber                         */ 0x00,
    /* bAlternateSetting                        */ 0x00,
    /* bNumEndpoints: 0 endpoint(s)             */ 0x01, /* was 0x01 */
    /* bInterfaceClass: CDC                     */ 0x02,
    /* bInterfaceSubclass:Abstract Control Model*/ 0x02,
    /* bInterfaceProtocol:AT Commands V.25ter   */ 0x01, /* was 0x01 */
    /* iInterface                               */ 0x00,
    /*********************************************************************
     Header Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: HEADER FD            */ 0x00,
    /* bcdADC                                   */ 0x10, 0x01,
    /*********************************************************************
     Abstract Control Model Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x04,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: ACM-FD               */ 0x02,
    /* bmCapabilities                           */ 0x02, /* was 0x03 */
    /*********************************************************************
     Union Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: Union FD             */ 0x06,
    /* bControlInterface                        */ 0x00, /* was 0x00 */
    /* bSubordinateInterface                    */ 0x01,
    /*********************************************************************
     Call Management Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: CM-FD                */ 0X01,
    /* bmCapabilities                           */ 0x00, /* was 0x00 */
    /* bDataInterface                           */ 0x01,
    /*********************************************************************
     Interrupt IN Endpoint - 1
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 1          */ 0x81, /* was 0x81 */
    /* bmAttributes: INTERRUPT                  */ 0x03, /* was 0x01 */
    /* max packet size (LSB)                    */ 0x08,
    /* max packet size (MSB)                    */ 0x00,
    /* polling interval                         */ 0xFF, /* was 0x02 */
    /*********************************************************************
     Interface 1 - Data Class Interface
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: INTERFACE               */ 0x04,
    /* interface index                          */ 0x01,
    /* alt setting index                        */ 0x00,
    /* bNumEndpoints: 2 endpoint(s)             */ 0x02,
    /* bInterfaceClass: CDC-Data                */ 0x0A,
    /* bInterfaceSubclass: unused               */ 0x00,
    /* bInterfaceProtocol: None                 */ 0x00,
    /* iInterface                               */ 0x00,
    /*********************************************************************
     Endpoint 1 (Bulk IN)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 2          */ 0x82, /* was 0x01 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00,
    /*********************************************************************
     Endpoint 2 (Bulk OUT)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: OUT Endpoint 3         */ 0x03, /* was 0x82 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00
};

unsigned char CDC_ConfigDescriptorArray_J[] = {
    // Referenced from: https://gist.github.com/tai/acd59b125a007ad47767
    /*********************************************************************
     Configuration Descriptors 
     *********************************************************************/
    /*  bLength (Descriptor Length)             */ 9,
    /*  bDescriptorType: CONFIG					*/ 0x02,
    /*  wTotalLength							*/ 0x43, 0x00,
    /*  bNumInterfaces							*/ 2,
    /*  bConfigurationValue						*/ 1,
    /*  iConfiguration							*/ 0,
    /*  bmAttributes							*/ 0xC0, /* bit 6 set = bus powered = 0x80, 0xC0 is for self powered */
    /*  bMaxPower								*/ 0x0, /* Value x 2mA, set to 0 for self powered, was 0x32 */
    /*********************************************************************
      Interface 0 - Communications Class
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: INTERFACE               */ 0x04,
    /* bInterfaceNumber                         */ 0x00,
    /* bAlternateSetting                        */ 0x00,
    /* bNumEndpoints: 0 endpoint(s)             */ 0x01, /* was 0x01 */
    /* bInterfaceClass: CDC                     */ 0x02,
    /* bInterfaceSubclass:Abstract Control Model*/ 0x02,
    /* bInterfaceProtocol:AT Commands V.25ter   */ 0x01, /* was 0x01 */
    /* iInterface                               */ 0x00,
    /*********************************************************************
     Header Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: HEADER FD            */ 0x00,
    /* bcdADC                                   */ 0x10, 0x01,
    /*********************************************************************
     Abstract Control Model Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x04,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: ACM-FD               */ 0x02,
    /* bmCapabilities                           */ 0x02, /* was 0x03 */
    /*********************************************************************
     Union Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: Union FD             */ 0x06,
    /* bControlInterface                        */ 0x00, /* was 0x00 */
    /* bSubordinateInterface                    */ 0x01,
    /*********************************************************************
     Call Management Functional Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: CM-FD                */ 0X01,
    /* bmCapabilities                           */ 0x00, /* was 0x00 */
    /* bDataInterface                           */ 0x01,
    /*********************************************************************
     Interrupt IN Endpoint - 1
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 1          */ 0x81, /* was 0x81 */
    /* bmAttributes: INTERRUPT                  */ 0x03, /* was 0x01 */
    /* max packet size (LSB)                    */ 0x08,
    /* max packet size (MSB)                    */ 0x00,
    /* polling interval                         */ 0xFF, /* was 0x02 */
    /*********************************************************************
     Interface 1 - Data Class Interface
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: INTERFACE               */ 0x04,
    /* interface index                          */ 0x01,
    /* alt setting index                        */ 0x00,
    /* bNumEndpoints: 2 endpoint(s)             */ 0x02,
    /* bInterfaceClass: CDC-Data                */ 0x06,
    /* bInterfaceSubclass: unused               */ 0x00,
    /* bInterfaceProtocol: None                 */ 0x00,
    /* iInterface                               */ 0x00,
    /*********************************************************************
     Endpoint 1 (Bulk IN)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: IN Endpoint 2          */ 0x82, /* was 0x01 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00,
    /*********************************************************************
     Endpoint 2 (Bulk OUT)
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: ENDPOINT                */ 0x05,
    /* bEndpointAddress: OUT Endpoint 3         */ 0x03, /* was 0x82 */
    /* bmAttributes: BULK                       */ 0x02,
    /* max packet size (LSB)                    */ 0x40,
    /* max packet size (MSB)                    */ 0x00,
    /* bInterval: None for BULK                 */ 0x00
};

/*********************************************************************
 String Descriptor Table (below) - These must be Unicode 16 (UTF-16)
 *********************************************************************/

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
    /* 	Descriptor Length                       */ 32,
    /*  DescriptorType: STRING  				*/ 0x03,
    /*	Vendor Name     						*/ 'P', 0, 'I', 0, 'C', 0, '3', 0, '2', 0,
    'M', 0, 'Z', 0, ' ', 0, 'C', 0, 'D', 0, 'C', 0, ' ', 0, 'A', 0, 'C', 0, 'M', 0
}; // Lewis Techno Group	0x4C,0,0x65,0,0x77,0,0x69,0,0x73,
// 0,0x20,0,0x54,0,0x65,0,0x63,0,0x68,0,0x6E,0,0x6F,0,0x20,0,0x47,0,0x72,0,0x6F,0,
// 0x75,0,0x70,0						

/*********************************************************************
 String Descriptor (Product Name): "USB Example"
 *********************************************************************/
unsigned char string2[] = {
    /* Descriptor Length                       */ 40, /* 40 or 0x28 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Product Name                            */ 'V', 0, 'i', 0, 'r', 0, 't', 0,
    'u', 0, 'a', 0, 'l', 0, ' ', 0, 'S', 0, 'e', 0, 'r', 0, 'i', 0, 'a', 0, 'l', 0, ' ', 0, 'P', 0,
    'o', 0, 'r', 0, 't', 0
}; // USB Example 0x55,0x53,0x42,0x20,
// 0x45,0x78,0x61,0x6D,0x70,0x6C,0x65
// 'U',0,'S',0,'B',0,' ',0,
// 'E',0,'x',0,'a',0,'m',0,'p',0,'l',0,'e',0
/*********************************************************************
 String Descriptor (Serial Number): "xxx-xxxx"
 *********************************************************************/
unsigned char string3[] = {
    /* Descriptor Length                       */ 18, /* 18 or 0x12 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Serial Number                           */ '1', 0, '2', 0, '3', 0, '-', 0,
    '4', 0, '5', 0, '6', 0, '7', 0
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

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _USB_H */

/* *****************************************************************************
 End of File
 */
