
PoKeysXspi
==========

A plugin to connect xplane via UART.

uart config
-----------

- baudrate	: 57600
- Bytesize	: 8
- stopbits	: 1
- parity		: no parity
- flowcontrol	: none

transmit code
--------------

- TX 	=>	RX
- 'B'	=>	(QDEC0<<16)|QDEC1

- the responce is 32bit wide and is sent as 4x8bit charcters
- the recieve buffer cuts this into pieces and reassembles the 2 16 bit values

caveats:
--------

- IWOMM for now
- I use eclipse+CDT+mingw,but any IDE compiler combo for recent c++ should do.
- The xplane sdk is include now, this may change in the future.
- avoid windows sdk like ebola.
- many warnings, mainly depreciated conversion and such
- one problem at a time ok.
