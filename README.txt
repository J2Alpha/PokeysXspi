############
#PoKeysXspi#
############

A plugin to connect a Pokeys board to xplane and read Serial Peripherial Interface (SPI) sensors.

#spi config
The pokeys is assumed to be a slave device and the sensor the master in the default config. The values from the sensor are 2x16bit position readouts delivered as one 32bit value, they are big endian. the module is in mode 0 for ATMEL-based mcu's. The sensor really performs 2 16bit transmits but CS is not deasserted.

- pokey config:
	- inputs 5V
	- pin1: /cs
	- pin2: sck
	- pin5: mosi
	(- pin6: miso as defined but not used, the sensor ignores this)
- data config: 
	- one 32bit blob
	- contains 2, 16 values
	- both big endian 
- clock config:
	- frequency 100khz 
	- clock is usually not clean
	- clock starts when cs goes low
	- Polarity: normaly 0
	- sample point: low to high transition 0=>1

#What is SPI
it's a hardware thing, practically the standard for simple async comms, I know no µC that does not have a built in SPI module. It's not complicated, but somtimes fickle at high speeds. More info about SPI can be found at https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus

#The code
There are 3 parts:

- The Xplane plugin dll,
     - libpokeysspi.xps, in pokeyspi
- The asynchroneous spi thread,
     - pokeyreader.exe, in pokeyreader
- The pokeys c++ lib, 
     - libpokeylib.dll, in pokeyslib

caveats:

  - IWOMM for now
  - I use eclipse+CDT+mingw,but any IDE compiler combo for recent c++ should do.
  - The xplane sdk is include now, this may change in the future.
  - avoid windows sdk like ebola.
  - many warnings, mainly depreciated conversion and such, They are caused by libpokeylib.dll, avoid repairing this now as it is delivered as such by polabs, see next statement.
  - one problem at a time ok. 