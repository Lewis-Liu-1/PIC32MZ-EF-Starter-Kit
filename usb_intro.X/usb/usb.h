/* 
 * USB.h - for PIC32MZ_Basic_USB_MIDI
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

/*********************************************************************
 Device Descriptors
 *********************************************************************/
unsigned char dev_desc[] = {
    /* Descriptor Length						*/ 18, /* Always 18 or 0x12 */
    /* DescriptorType: DEVICE					*/ 0x01,
    /* bcdUSB (ver 2.0)							*/ 0x00, 0x02,
    /* bDeviceClass								*/ 0x00, /* was 0x00 */
    /* bDeviceSubClass							*/ 0x00, /* was 0x00 */
    /* bDeviceProtocol							*/ 0x00,
    /* bMaxPacketSize0							*/ 0x40, /* 0x40 = 64 decimal */
    /* idVendor									*/ 0xD8, 0x04, /* e.g. - 0x1234 */
    /* idProduct								*/ 0x01, 0x00, /* e.g. - 0x0001 */
    /* bcdDevice								*/ 0x00, 0x00, /* e.g. - 01.00 */
    /* iManufacturer							*/ 0x01,
    /* iProduct									*/ 0x02,
    /* iSerialNumber							*/ 0x03,
    /* bNumConfigurations						*/ 0x01
};

unsigned char configDescriptors[] = {
    /*********************************************************************
     Config Descriptor
     *********************************************************************/
    /*  bLength (Descriptor Length)             */ 9, /* 9 or 0x09 */
    /*  bDescriptorType: CONFIG					*/ 0x02,
    /*  wTotalLength							*/ 0x65, 0x00, /* was 0x65 */
    /*  bNumInterfaces							*/ 2, /* 1 interface(s) - MIDI (later Audio) */
    /*  bConfigurationValue						*/ 1, /* was 0 */
    /*  iConfiguration							*/ 8, /* Index of string descriptor describing this configuration. */
    /*  bmAttributes							*/ 0x80, /* bit 6 set = bus powered = 0xC0, 0x80 is for self powered */
    /*  bMaxPower								*/ 0x32, /* Value x 2mA, set to 0 for self powered */
    /*********************************************************************
     Interface 0 Descriptor
     *********************************************************************/
    /*  bLength                                 */ 9, /* 9 or 0x09 */
    /*  bDescriptorType: INTERFACE				*/ 0x04,
    /*  bInterfaceNumber						*/ 0x00,
    /*  bAlternateSetting						*/ 0,
    /*  bNumEndpoints							*/ 2, /* Number of Endpoints descriptors to follow */
    /*  bInterfaceClass							*/ 0x01, /* 0x01 = Audio Class, 0x02 = Communications */
    /*  bInterfaceSubClass						*/ 0x02, /* 0x02 = Audio Streaming */
    /*  bInterfaceProtocol						*/ 0x00, /* Bulk, was 1 */
    /*  iInterface								*/ 6, /* was 6 */
    /*********************************************************************
     Class Specific Interface (Audio)
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType: CS_INTERFACE            */ 0x24,
    /* bDescriptorSubtype: HEADER subtype       */ 0x02, /* was 0x01 = HEADER, 0x02 = INPUT_TERMINAL */
    /* bcdADC: Rev of class specification - 1.0 */ 0x00, 0x01,
    /* wTotalLength:                            */ 0x9, 0x00, /* was 9 */
    /* bInCollection: Num of streaming intrfcs  */ 0x01,
    /* baInterfaceNr(1): MIDIStreaming intrfce 1*/ 0x01,
    /*********************************************************************
     Interface 1 Descriptor
     *********************************************************************/
    /* bLength                                  */ 9, /* 9 or 0x09 */
    /* bDescriptorType: INTERFACE				*/ 0x04,
    /* bInterfaceNumber                         */ 0x01,
    /* bAlternateSetting						*/ 0,
    /* bNumEndpoints							*/ 2,
    /* bInterfaceClass							*/ 0x01, /* 0x01 = Audio */
    /* bInterfaceSubClass						*/ 0x03, /* 0x03 = MIDIStreaming */
    /* bInterfaceProtocol						*/ 0x00, /* Iso */
    /* iInterface								*/ 7, /* was 7 */
    /*********************************************************************
     Class-Specific MIDIStreaming (MS) Interface Header Descriptor
     *********************************************************************/
    /* bLength                                  */ 0x07,
    /* bDescriptorType: CS_INTERFACE des. type. */ 0x24,
    /* bDescriptorSubtype: MS_HEADER subtype.   */ 0x01,
    /* bcdMSC: BCD                              */ 0x00, 0x01,
    /* wTotalLength: Number                     */ 0x41, 0x00,
    /*********************************************************************
     MIDI IN Jack 1 Descriptor (Embedded)
     *********************************************************************/
    /*  bLength                 				*/ 0x06, /* 6 or 0x06 */
    /*  bDescriptorType: CS_INTERFACE type.     */ 0x24, /* CS_INTERFACE descriptor */
    /*  bDescriptorSubtype:MIDI_IN_JACK subtype */ 0x02, /* MIDI_IN_JACK subtype */
    /*  bJackType: EMBEDDED                     */ 0x01, /* EMBEDDED 0x01 or EXTERNAL 0x02 */
    /*  bJackID                                 */ 0x01,
    /*  iJack: Index                            */ 0x00, /* Was 8, Index of a string descriptor, describing the MIDI IN Jack. */
    /*********************************************************************
     MIDI IN  Jack 2 Descriptor (External)
     *********************************************************************/
    /*  bLength                 				*/ 0x06, /* 6 or 0x06 */
    /*  bDescriptorType         				*/ 0x24,
    /*  bDescriptorSubtype						*/ 0x02, /* MIDI_IN_JACK desc subtype */
    /*  bJackType                               */ 0x02, /* EXTERNAL */
    /*  bJackID                                 */ 0x02,
    /*  iJack                                   */ 0x00,
    /*********************************************************************
     MIDI OUT Jack 3 Descriptor (Embedded)
     *********************************************************************/
    /*  bLength                 				*/ 0x09, /* 9 or 0x09 */
    /*  bDescriptorType                         */ 0x24, /* CS_INTERFACE descriptor */
    /*  bDescriptorSubtype						*/ 0x03, /* MIDI_OUT_JACK subtype */
    /*  bJackType                               */ 0x01, /* EMBEDDED */
    /*  bJackID                                 */ 0x03, /* was 2, ID of this Jack */
    /*  bNrInputPins                            */ 0x01, /* Number of Input Pins of this Jack */
    /*  BaSourceID(1)                           */ 0x02, /* ID of Entity to which this pin is connected */
    /*  BaSourcePin(1)                          */ 0x01, /* Output Pin number of the Entity to which this Input Pin is connected */
    /*  iJack: Index                            */ 0x00, /* was 9,Index of a string descriptor, describing the MIDI IN Jack. */
    /*********************************************************************
     MIDI OUT Jack 4 Descriptor (External)
     *********************************************************************/
    /*  bLength                 				*/ 0x09, /* 9 or 0x09 */
    /*  bDescriptorType         				*/ 0x24, /* CS_INTERFACE descriptor */
    /*  bDescriptorSubtype						*/ 0x03, /* MIDI_OUT_JACK subtype */
    /*  bJackType                               */ 0x02, /* EXTERNAL */
    /*  bJackID                                 */ 0x04, /* ID of this Jack */
    /*  bNrInputPins                            */ 0x01, /* Number of Input Pins of this Jack */
    /*  BaSourceID(1)                           */ 0x01, /* ID of Entity to which this pin is connected */
    /*  BaSourcePin(1)                          */ 0x01, /* Output Pin number of the Entity to which this Input Pin is connected */
    /*  iJack                                   */ 0x00, /* Unused */
    /*********************************************************************
     Standard Bulk Endpoint OUT
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType                          */ 0x05,
    /* bDescriptorSubtype                       */ 0x01, /* bEndpointAddress OUT endpoint number 1 */
    /*                                          */ 0x03, /* 2:Bulk, 3:Interrupt endpoint */
    /*                                          */ 0x08, 0x00, /* wMaxPacketSize */
    /*                                          */ 0x0A, /* bInterval in ms */
    /*                                          */ 0x00, /* bRefresh */
    /*                                          */ 0x00, /* bSyncAddress */
    /*********************************************************************
     Class Specific MIDIStreaming (MS) Endpoint IN Jack
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType                          */ 0x25,
    /* bDescriptorSubtype                       */ 0x01,
    /*                                          */ 0x01, /* bNumEmbMIDIJack (0) */
    /*                                          */ 0x03, /* baAssocJackID (0) */
    /*********************************************************************
     Standard Bulk Endpoint IN
     *********************************************************************/
    /* bLength                                  */ 0x09,
    /* bDescriptorType                          */ 0x05,
    /* bDescriptorSubtype                       */ 0x81,
    /*                                          */ 0x03, /* 2: Bulk, 3: Interrupt endpoint */
    /*                                          */ 0x08, 0x00, /* wMaxPacketSize */
    /*                                          */ 0x0A, /* bInterval in ms */
    /*                                          */ 0x00, /* bRefresh */
    /*                                          */ 0x00, /* bSyncAddress */
    /*********************************************************************
     Class Specific MIDIStreaming (MS) Endpoint OUT Jack
     *********************************************************************/
    /* bLength                                  */ 0x05,
    /* bDescriptorType                          */ 0x25,
    /* bDescriptorSubtype                       */ 0x01,
    /* bDescriptorSubtype                       */ 0x01, /* bNumEmbMIDIJack  */
    /*                                          */ 0x01 /* baAssocJackID (0) */
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
}; // 0x0409 for U.S. English
/* *******************************************************************
String Descriptor: "Lewis Technologies Group"
 *********************************************************************/
unsigned char string1[] = {
    /* 	Descriptor Length                       */ 30,
    /*  DescriptorType: STRING  				*/ 0x03,
    /*	Vendor Name     						*/ 'P', 0, 's', 0, 'e', 0, 'u', 0, 'd', 0,
    'o', 0, ' ', 0, 'C', 0, 'o', 0, 'm', 0, 'p', 0, 'a', 0, 'n', 0, 'y', 0
};

/*********************************************************************
 String Descriptor (Product Name): "USB Example"
 *********************************************************************/
unsigned char string2[] = {
    /* Descriptor Length                        */ 36,
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Product Name                             */ 'Y', 0, 'o', 0, 'u', 0, 'r', 0,
    ' ', 0, 'P', 0, 'r', 0, 'o', 0, 'd', 0, 'u', 0, 'c', 0, 't', 0, ' ', 0, 'N', 0, 'a', 0, 'm', 0,
    'e', 0
};

/*********************************************************************
String Descriptor (Serial Number): "xxx-xxxx"
 *********************************************************************/
unsigned char string3[] = {
    /* Descriptor Length                        */ 18, /* 18 or 0x12 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Serial Number                            */ '1', 0, '2', 0, '3', 0, '-', 0,
    '4', 0, '5', 0, '6', 0, '7', 0
}; // 123-4567

unsigned char string4[] = {
    /* Descriptor Length                        */ 4, /* 4 or 0x04 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Configuration                            */ 0x01, 0x00
};

unsigned char string5[] = {
    /* Descriptor Length                        */ 22, /* 22 or 0x16 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Interface                                */ 'M', 0, 'i', 0, 'd', 0, 'i', 0, '&', 0, 'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0
};

unsigned char string6[] = {
    /* Descriptor Length                        */ 12, /* 12 or 0x0C */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Audio                                    */ 'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0
};

unsigned char string7[] = {
    /* Descriptor Length                        */ 10, /* 10 or 0x0A */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* MIDI                                     */ 'M', 0, 'I', 0, 'D', 0, 'I', 0
};

unsigned char string8[] = {
    /* Descriptor Length                        */ 16, /* 16 or 0x10 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Audio                                    */ 'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'I', 0, 'n', 0
};

unsigned char string9[] = {
    /* Descriptor Length                        */ 18, /* 18 or 0x12 */
    /*  DescriptorType: STRING  				*/ 0x03,
    /* Audio                                    */ 'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0
};

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _USB_H */

/* *****************************************************************************
 End of File
 */
