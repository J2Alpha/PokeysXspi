/*
 * pokeyconfigures.h
 *
 *  Created on: 14 okt. 2014
 *      Author: J2Alpha
 */

#ifndef POKEYCONFIGURES_H_
#define POKEYCONFIGURES_H_

#include "PoKeysLib.h"
#include <stdio.h>
#include <vector>

#define vector std::vector
#define BITLENGTH 32
#define CS 0
#define SCK 1
#define MOSI 4
#define MISO 5

/**
* todo implement entry point completely
* todo make all code comments doxygen compatible
**/

//entry point : find a (optinally specific) pokeys and bind it
/*-parameters
 * pokeysnum, not implemented future serialnumber or such
 * method: 0 any, 1 usb, 2 ethernet
 * specific, not implemented selct wether to connect to any pokey or use serialnum to choose one
 */
int pokeys_search_connect(int pokeysnum,int method,int specific);
//subversions called by the pokeyssearchandconnect
int pokeys_search_any_connect(int pokeysnum, int specific);
int pokeys_search_usb_connect(int pokeysnum, int specific);
int pokeys_search_net_connect(int pokeysnum, int specific);
//set IO config of connected pokey
//todo do this rigth
int pokeys_setIO(vector<vector<int> > *box);
//get pinstate in aperpin fashin
/**
 * todo implement setting whole pokey at once
 **/
int pokeys_pinstate(int pin);
//perform an spi read
int pokeys_spi_read(int * destination);
//perform read but force waiting
int pokeys_spi_read_force(int * destination);
//close connections
int pokey_close(void);

/*
 * pinflags
 *
	PK_PinCap_pinRestricted  = 0,           // Pin is not used
    PK_PinCap_reserved       = 1,           // --
    PK_PinCap_digitalInput   = 2,           // Digital input
    PK_PinCap_digitalOutput  = 4,           // Digital output
    PK_PinCap_analogInput    = 8,           // Analog input (only on selected pins)
    PK_PinCap_analogOutput   = 16,          // Analog output (only on selected pins)
    PK_PinCap_triggeredInput = 32,          // Triggered input
    PK_PinCap_digitalCounter = 64,          // Digital counter (only on selected pins)
    PK_PinCap_invertPin      = 128
*/

#endif /* POKEYCONFIGURES_H_ */