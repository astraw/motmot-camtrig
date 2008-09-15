/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "Descriptors.h"

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(02.00),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x03EB,
	ProductID:              0xDCDC,
	ReleaseNumber:          0x0000,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
};
	
USB_Descriptor_Configuration_t ConfigurationDescriptor PROGMEM =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        1,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        0,
			AlternateSetting:       0,
			
			TotalEndpoints:         2,
				
			Class:                  0xFF,
			SubClass:               0x00,
			Protocol:               0x00,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	DataInEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | SIDESHOW_IN_EPNUM),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           SIDESHOW_IO_EPSIZE,
			PollingIntervalMS:      0x00
		},

	DataOutEndpoint:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},

			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | SIDESHOW_OUT_EPNUM),
			Attributes:             EP_TYPE_BULK,
			EndpointSize:           SIDESHOW_IO_EPSIZE,
			PollingIntervalMS:      0x00
		}
};

USB_Descriptor_String_t LanguageString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t ManufacturerString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(11), Type: DTYPE_String},
		
	UnicodeString:          L"Dean Camera"
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(23), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB Sideshow Demo"
};

USB_Descriptor_String_t SerialNumberString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          L"000000000000"
};

USB_OSDescriptor_t OSDescriptorString PROGMEM =
{
	Header:                 {Size: 0x12, Type: DTYPE_String},
	
	Signature:              L"MSFT100",
	VendorCode:             REQ_GetOSFeatureDescriptor
};

USB_OSCompatibleIDDescriptor_t DevCompatIDs PROGMEM =
{
	TotalLength:            sizeof(USB_OSCompatibleIDDescriptor_t),
	Version:                0x0100,
	Index:                  EXTENDED_COMPAT_ID_DESCRIPTOR,
	TotalSections:          1,
	
	SideshowCompatID:       {FirstInterfaceNumber: 0x00,
	                         CompatibleID: "SIDESHW",
	                         SubCompatibleID: "UNIV1"}
};

bool USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
                       void** const DescriptorAddress, uint16_t* const DescriptorSize)
{
	void*    Address = NULL;
	uint16_t Size    = 0;

	switch (wValue >> 8)
	{
		case DTYPE_Device:
			Address = DESCRIPTOR_ADDRESS(DeviceDescriptor);
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor);
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (wValue & 0xFF)
			{
				case 0x00:
					Address = DESCRIPTOR_ADDRESS(LanguageString);
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = DESCRIPTOR_ADDRESS(ManufacturerString);
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = DESCRIPTOR_ADDRESS(ProductString);
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					Address = DESCRIPTOR_ADDRESS(SerialNumberString);
					Size    = pgm_read_byte(&SerialNumberString.Header.Size);
					break;
				case 0xEE:
					/* Great, another Microsoft-proprietary extention. String address 0xEE is used
					   by Windows for "OS Descriptors", which in this case allows us to indicate that
					   our device is Sideshow compatible. Most people would be happy using the normal
					   0xFF 0x?? 0x?? Class/Subclass/Protocol values like the USBIF intended. */
					   
					Address = DESCRIPTOR_ADDRESS(OSDescriptorString);
					Size    = pgm_read_byte(&OSDescriptorString.Header.Size);
					break;
			}
			
			break;
	}
	
	if (Address != NULL)
	{
		*DescriptorAddress = Address;
		*DescriptorSize    = Size;

		return true;
	}
		
	return false;
}

bool USB_GetOSFeatureDescriptor(const uint16_t wValue, const uint8_t wIndex,
                                void** const DescriptorAddress, uint16_t* const DescriptorSize)
{
	void*    Address = NULL;
	uint16_t Size    = 0;

	/* Check if a device level OS feature descriptor is being requested */
	if (wValue == 0x0000)
	{
		/* Only the Extended Device Compatibility descriptor is supported */
		if (wIndex == EXTENDED_COMPAT_ID_DESCRIPTOR)
		{
			Address = DESCRIPTOR_ADDRESS(DevCompatIDs);
			Size    = sizeof(USB_OSCompatibleIDDescriptor_t);
		}
	}

	if (Address != NULL)
	{
		*DescriptorAddress = Address;
		*DescriptorSize    = Size;

		return true;
	}
		
	return false;
}