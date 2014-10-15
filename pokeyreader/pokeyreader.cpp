/*
 * pokeyreader.cpp
 *
 *  Created on: 13 okt. 2014
 *      Author: J2Alpha
 */

#include "pokeyreader.h"

vector<vector<int > >ioset ={{CS,PK_PinCap_digitalInput},
							{SCK,PK_PinCap_digitalInput},
							{MOSI,PK_PinCap_digitalInput},
							{MISO,PK_PinCap_digitalInput}};
LPTSTR lpvMessage=TEXT("Default message from client.");

int main(int argc, TCHAR *argv[]){

	if( argc > 1 )lpvMessage=argv[1];
	//TCHAR buffy(pipe_getbufsize());
    //commencepiping();
	// The pipe connected; change to message-read mode.
    //setpipemode(PIPE_READMODE_MESSAGE);
	// Send a message to the pipe server.
    //pipe_stuff(lpvMessage);
	//printf("\nMessage sent to server, receiving reply as follows:\n");
	//pipe_smoke(&buffy);
	//printf("\n<End of message, press ENTER to terminate connection and exit>");
	//open pokey
	pokeys_search_connect(0,0,0);
	//send pokey config
	pokeys_setIO(&ioset);
	//check cs, do a read
	for(unsigned int i=0;i<20;i++){
		printf("<spi try %d>\n", i+1);
		unsigned int target =0;
		pokeys_spi_read_force(&target);
	}
	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();
	//pipe_extinguish();
	pokey_close();
	return 0;
}


