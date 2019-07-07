/* 
 * usb.h for PIC32MZ Basic USB
 * 
 * Released under FreeBSD License 
 * 
 * (C)2019 - Mark Serran Lewis
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
/* bcdUSB (ver 2.0)							*/ 0x00,0x02,
/* bDeviceClass								*/ 0x02,	/* 0x01 = Audio, 0x02 = Comm */
/* bDeviceSubClass							*/ 0x02,	/* 0x03 for MIDI, 0x02 = acm */
/* bDeviceProtocol							*/ 0x01,	/* unused for MIDI, 0x01 = at */
/* bMaxPacketSize0							*/ 64,
/* idVendor									*/ 0x34,0x12, /* e.g. - 0x1234 */
/* idProduct								*/ 0x01,0x00, /* e.g. - 0x0001 */
/* bcdDevice								*/ 0x00,0x01, /* e.g. - 01.00 */
/* iManufacturer							*/ 0x01,
/* iProduct									*/ 0x02,
/* iSerialNumber							*/ 0x03,
/* bNumConfigurations						*/ 0x01
};
/*********************************************************************
 Config Descriptor
 *********************************************************************/
unsigned char config_desc1[] = {
/*Config Descriptor Length					*/ 9, /* 9 or 0x09 */
/*  DescriptorType: CONFIG					*/ 0x02,
/*  wTotalLength							*/ 0x24,0x00,
/*  bNumInterfaces							*/ 1,
/*  bConfigurationValue						*/ 2,
/*  iConfiguration							*/ 4, /* Index of string descriptor describing this configuration. */
/*  bmAttributes							*/ 0xC0, /* bit 6 set = self powered */
/*  bMaxPower								*/ 0     /* Value x 2mA */
};/* NOTE: wTotalLength = Combined length of all descriptors, including this one */
/*********************************************************************
 Interface Descriptor
 *********************************************************************/
unsigned char intrface1[] = { 
/*Interface Descriptor Length				*/ 9, /* 9 or 0x09 */
/*  DescriptorType: INTERFACE				*/ 0x04,
/*  bInterfaceNumber						*/ 0x00,
/*  bAlternateSetting						*/ 0,
/*  bNumEndpoints							*/ 3,
/*  bInterfaceClass							*/ 0x01, /* 0x01 = Audio, 0x02 = Comm */
/*  bInterfaceSubClass						*/ 0x03, /* 0x03 for MIDI, 0x02 = acm */
/*  bInterfaceProtocol						*/ 0x00, /* unused for MIDI, 0x01 = at */
/*  iInterface								*/ 0
};
/*********************************************************************
 Endpoint-0 Descriptor
 *********************************************************************/
 unsigned char ep0desc[] = {
/*Endpoint Descriptor Length				*/ 6, /* 6 or 0x06 */	
/*  DescriptorType: ENDPOINT				*/ 0,
/*  bEndpointAddress						*/ 0,
/*  bmAttributes							*/ 0x00,
/*  wMaxPacketSize							*/ 64,
/*  bInterval								*/ 32
};
/*********************************************************************
 Endpoint-1 Descriptor
 *********************************************************************/
 unsigned char ep1desc[] = {
/*Endpoint Descriptor Length				*/ 6, /* 6 or 0x06 */	
/*  DescriptorType: ENDPOINT				*/ 0,
/*  bEndpointAddress						*/ 1,
/*  bmAttributes							*/ 0x00,
/*  wMaxPacketSize							*/ 64,
/*  bInterval								*/ 32
};
 /*********************************************************************
 Endpoint-2 Descriptor
 *********************************************************************/
 unsigned char ep2desc[] = {
/*Endpoint Descriptor Length				*/ 6, /* 6 or 0x06 */
/*  DescriptorType: ENDPOINT				*/ 0,
/*  bEndpointAddress						*/ 2,
/*  bmAttributes							*/ 0x00,
/*  wMaxPacketSize							*/ 64,
/*  bInterval								*/ 32
}; 


/*********************************************************************
 String Descriptor Table (below) - These must be Unicode 16 (UTF-16)
 *********************************************************************/

 /*********************************************************************
Language ID Descriptor
*********************************************************************/
unsigned char string0[] = {	
/* 	Descriptor Length                       */	4, /* 4 or 0x04 */
/*  DescriptorType: STRING  				*/	0x03,
/* 	Language ID: English					*/	0x09,0x04
												}; 
/* *******************************************************************
String Descriptor: "Lewis Technologies Group"
*********************************************************************/
unsigned char string1[] = {	
/* 	Descriptor Length                       */	44, /* 44 or 0x2C */
/*  DescriptorType: STRING  				*/  0x03,
/*	Vendor Name     						*/	'A',0,' ',0,'P',0,'s',0,'e',0,
'u',0,'d',0,'o',0,' ',0,'M',0,'a',0,'n',0,'u',0,'f',0,'a',0,'c',0,'t',0,'u',0,
'r',0,'e',0,'r',0
};							
/*********************************************************************
 String Descriptor (Product Name): "USB Example"
 *********************************************************************/
 unsigned char string2[] = {
 /* Descriptor Length                       */ 	46, /* 46 or 0x2E */
/*  DescriptorType: STRING  				*/  0x03,
 /* Product Name                            */	'Y',0,'o',0,'u',0,'r',0,
' ',0,'P',0,'r',0,'o',0,'d',0,'u',0,'c',0,'t',0,' ',0,'N',0,'a',0,'m',0,
'e',0,' ',0,'H',0,'e',0,'r',0,'e',0
 }; 
 /*********************************************************************
 String Descriptor (Serial Number): "xxx-xxxx"
 *********************************************************************/
 unsigned char string3[] = {
 /* Descriptor Length                       */ 	18, /* 18 or 0x12 */
/*  DescriptorType: STRING  				*/  0x03,
 /* Serial Number                           */	'1',0,'2',0,'3',0,'-',0,
'4',0,'5',0,'6',0,'7',0
 };	

 unsigned char string4[] = {
 /* Descriptor Length                       */ 	4, /* 4 or 0x04 */
/*  DescriptorType: STRING  				*/  0x03,
 /* Configuration                           */	0x01, 0x00
 };
 
 unsigned char string5[] = {
 /* Descriptor Length                       */ 	8, /* 8 or 0x08 */
/*  DescriptorType: STRING  				*/  0x03,
 /* Interface                               */	'C',0,'D',0,'C',0
 };
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _USB_H */

/* *****************************************************************************
 End of File
 */
