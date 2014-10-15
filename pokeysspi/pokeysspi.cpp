/*
 * pokeysspi.cpp
 *
 *  Created on: 13 okt. 2014
 *      Author: J2Alpha
 */

//dependent on XPLM, XPWidgets and libpokeyslib libraries

//set architecture for xplane plugins
#include "prefixer.h"
//standard headers from c++
#include <string.h>
#include <stdio.h>
#include <math.h>
//piping stuff headers
#include <tchar.h>
//#include <strsafe.h>  we are not using win sdk if it can be helped!
//xplane sdk plugin headers
#include "XPLMDefs.h"
#include "XPLMDisplay.h"
#include "XPLMUtilities.h"
#include "XPLMCamera.h"
#include "XPLMDataAccess.h"
//pokeylib header?
#include "PoKeysLib.h"
//other headers
#include "pokeysspi.h"
//pipe stuff
#include "bagpipes.h"


#if IBM
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif //IBM

PLUGIN_API int XPluginStart ( char * outName, char * outSignature, char * outDescription ){
	strcpy(outName, NAME);
	strcpy(outSignature, ID);
	strcpy(outDescription, DESCRIPTION);
	return 1;
}
PLUGIN_API void	XPluginStop(void)
{

}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API int XPluginEnable(void)
{
	BOOL   fConnected = FALSE;
	DWORD  dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	// The main loop creates an instance of the named pipe and
	// then waits for a client to connect to it. When the client
	// connects, a thread is created to handle communications
	// with that client, and this loop is free to wait for the
	// next client connect request. It is an infinite loop.

	for (;;)
	{
	  _tprintf( TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
	  hPipe = CreateNamedPipe(
		  lpszPipename,             // pipe name
		  PIPE_ACCESS_DUPLEX,       // read/write access
		  PIPE_TYPE_MESSAGE |       // message type pipe
		  PIPE_READMODE_MESSAGE |   // message-read mode
		  PIPE_WAIT,                // blocking mode
		  PIPE_UNLIMITED_INSTANCES, // max. instances
		  BUFSIZE,                  // output buffer size
		  BUFSIZE,                  // input buffer size
		  0,                        // client time-out
		  NULL);                    // default security attribute

	  if (hPipe == INVALID_HANDLE_VALUE)
	  {
		  _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
		  return -1;
	  }
	  /**** start client here
	   *
	   */
	  // Wait for the client to connect; if it succeeds,
	  // the function returns a nonzero value. If the function
	  // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.

	  fConnected = ConnectNamedPipe(hPipe, NULL) ?
		 TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

	  if (fConnected)
	  {
		 printf("Client connected, creating a processing thread.\n");

		 // Create a thread for this client.
		 hThread = CreateThread(
			NULL,              // no security attribute
			0,                 // default stack size
			InstanceThread,    // thread proc
			(LPVOID) hPipe,    // thread parameter
			0,                 // not suspended
			&dwThreadId);      // returns thread ID

		 if (hThread == NULL)
		 {
			_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
			return -1;
		 }
		 else CloseHandle(hThread);
	   }
	  else
		// The client could not connect, so close the pipe.
		 CloseHandle(hPipe);
	}

	return 0;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho, long	inMessage,void * inParam)
{
}

void	MyHotKeyCallback(void * inRefcon)
{
	/* This is our hot key handler.  Note that we don't know what key stroke
	 * was pressed!  We can identify our hot key by the 'refcon' value though.
	 * This is because our hot key could have been remapped by the user and we
	 * wouldn't know it. */
	//XPLMSpeakString("Hello World!");
}

