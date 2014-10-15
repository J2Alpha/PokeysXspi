/*
 * pokeyconfigures.cpp
 *
 *  Created on: 14 okt. 2014
 *      Author: J2Alpha
 */

#include "pokeyconfigures.h"

// PoKeys device enumerations stuff
int usbDevNum = 0;
int netDevNum = 0;
long devSerial = 0;
sPoKeysDevice * dev;

int pokeys_search_connect(int pokeysnum = 0 ,int method = 0,int specific = 0 ){

	switch (method)
	{
	case 0:
		pokeys_search_net_connect(pokeysnum,specific);
		break;
	case 1:
		pokeys_search_usb_connect(pokeysnum, specific);
		break;
	case 2:
		pokeys_search_any_connect(pokeysnum,specific);
		break;
	default:
		pokeys_search_any_connect(pokeysnum,specific);
		break;
	}
	// This is how the connection with network PoKeys devices is established
	return 0;
}

int pokeys_search_any_connect(int pokeysnum, int specific)
{
	if(pokeys_search_net_connect(pokeysnum,specific)){
		if(pokeys_search_usb_connect(pokeysnum, specific)){
			return 1;
		}
	}
	return 0;
}

int pokeys_search_usb_connect(int pokeysnum, int specific)
{
	if((usbDevNum = PK_EnumerateUSBDevices()))
	{
		// Connect to first USB device
		dev = PK_ConnectToDevice(0);
		if (dev != NULL)
		{
			devSerial = dev->DeviceData.SerialNumber;
			printf("Connection established to USB PoKeys device with serial number %d\n", devSerial);

		}
		else
		{
			printf("Connecting to USB PoKeys device failed!\n");

		}
		return 0;
	}
	return 1;
}
int pokeys_search_net_connect(int pokeysnum, int specific)
{
	sPoKeysNetworkDeviceSummary netDevices[16];
	printf("Enumerating network devices...\t\t ");
	if((netDevNum = PK_EnumerateNetworkDevices(netDevices, 1000)))
	{
		// Connect to first network device...
		dev = PK_ConnectToNetworkDevice(&netDevices[0]);
		if (dev != NULL)
		{
			devSerial = dev->DeviceData.SerialNumber;
			printf("Connection established to network PoKeys device with serial number %d\n", devSerial);
			// Do something here...
		}
		else
		{
			printf("Connecting to network PoKeys device failed!\n");
		}
		return 0;
	}
	return 1;
}
//set IO config of connected pokeys
int pokeys_setIO(vector<vector<int> > *box){
	vector<vector<int> >& boxref = *box;
	for (unsigned int i=0 ; i < box->size() ;i++){

		dev->Pins[boxref[i][0]].PinFunction = boxref[i][1];
		printf("setting pin %d to mode %d \n",boxref[i][0],boxref[i][1]);
	}
	PK_PinConfigurationSet(dev);
	return 0;
}
//get pinstate in a per pin fashion
/**
 * todo implement setting whole pokey at once
 **/
inline int pokeys_pinstate(int pin){
	PK_DigitalIOGet(dev);
	return dev->Pins[pin].DigitalValueGet;
}
//perform an spi read
int pokeys_spi_read(int * destination)
{
	int value = 0x00000000;
	int bit = 0;
	while(!pokeys_pinstate(0))
	{
		if(pokeys_pinstate(1) && (bit < BITLENGTH))
			{
				value |= (pokeys_pinstate(4) << bit);
				bit++;
			}
	}
	printf("read from spi: %d\n", value);
	if(bit < BITLENGTH){
		printf("binary not long enough possibly spi too fast, length: %d\n", bit);
		return 0;
	}
	printf("read from spi ok: %d\n", value);
	*destination = value;
	return value;
}
int pokeys_spi_read_force(int * destination){
	int value = 0x00000000;
	int bit = 0;
	while(pokeys_pinstate(CS)){
		//wait for cs to go low
	}
	while(!pokeys_pinstate(0))
	{
		if(pokeys_pinstate(1) && (bit < BITLENGTH))
			{
				value |= (pokeys_pinstate(4) << bit);
				bit++;
			}
		//while(!pokeys_pinstate(1)){
			//don't wait for high to low on clk? pokey is to slow anyway
		//}
	}
	if(bit < BITLENGTH){
		printf("binary not long enough possibly spi too fast, length: %d\n", bit);
		return 0;
	}
	printf("read from spi ok: %d\n", value);
	*destination = value;
	return value;
}
//close connections
int pokey_close(void){
	PK_DisconnectDevice(dev);
	printf("disconnected from pokeys with serial number %d\n", devSerial);
	return 0;
}


