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
#include "XPLMProcessing.h"

#define DllImport   __declspec( dllimport )

//other headers
#include "pokeysspi.h"
//pipe stuff


//DllImport XPLM_API void XPLMRegisterFlightLoopCallback(
       // XPLMFlightLoop_f     inFlightLoop,
       // float                inInterval,
      //  void *               inRefcon);

float SerialPortAccessCB(float elapsedMe, float elapsedSim, int counter, void *refcon);
void SerialPortAccessCallback(XPLMWindowID inWindowID, void *inRefcon);

bool CommsEnabled;
HANDLE hComms;
DWORD dwRetFlag;

char debugloopback=0;
unsigned short com0;
unsigned short com1;

#if IBM
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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

float SerialPortAccessCB(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
	char outbuf[1]={'B'};
	char inbuf[4]={0,0,0,0};
	//send trigger
	SendToComPort(strlen(outbuf), (unsigned char *) outbuf);//
	if(debugloopback)//jumper rx to tx first
	{
		SendToComPort(strlen(outbuf), (unsigned char *) outbuf);//
		SendToComPort(strlen(outbuf), (unsigned char *) outbuf);//
		SendToComPort(strlen(outbuf), (unsigned char *) outbuf);//
	}
	//receive 4 8bit sections
	ReceiveFromComPort(inbuf);
	//transform it back to 2 16 bit values
	com0=((((unsigned short)inbuf[1])<<8) | inbuf[0]);
	com1=((((unsigned short)inbuf[3])<<8) | inbuf[2]);
	// now these have the positions of the 2 qdecs
	//todo translate this to the right range, it depends on what you want to set I think?
	//todo then tell xplane where it goes this using a dataref?

    return 0.01;
}

PLUGIN_API int XPluginStart ( char * outName, char * outSignature, char * outDescription ){
	strcpy(outName, NAME);
	strcpy(outSignature, ID);
	strcpy(outDescription, DESCRIPTION);
	//this is where it goes wrong with me, I get:
	//pokeysspi.cpp:67: undefined reference to `_imp__XPLMRegisterFlightLoopCallback'
	//XPLMFlightLoop_f flf=&SerialPortAccessCB;
	XPLMRegisterFlightLoopCallback(SerialPortAccessCB, 1.0, NULL);
	return 1;

}
PLUGIN_API void	XPluginStop(void)
{

}

PLUGIN_API void XPluginDisable(void)
{
	CloseComms();
}

PLUGIN_API int XPluginEnable(void)
{

	OpenComms();
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho, long	inMessage,void * inParam)
{

}



/*void GetLatestDataRefs(void)
{
    float FloatValue[MAX_ITEMS];
    char Buffer[512];
    int Item;

    for (Item=0; Item<MAX_ITEMS; Item++)
    {
        FloatValue[Item] = XPLMGetDataf(gPositionDataRef[Item]);
        sprintf(Buffer, "%f\n\r", FloatValue[Item]);
        SendToComPort(strlen(Buffer), (unsigned char *)Buffer);
    }
}*/

//---------------------------------------------------------------------------
void OpenComms(void)
{
    DCB Dcb;
    COMMTIMEOUTS CommTimeouts;
    char ErrorString[80];
    //char *PortString = "\\\\.\\COMx";

    if (hComms == 0)
    {
        //PortString[7] = (char) 4 + 48;
    	//TCHAR *pcCommPort = TEXT("COM4");
        hComms = CreateFile("\\\\.\\COM4",
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                            NULL);

        if (hComms == INVALID_HANDLE_VALUE)
        {
            sprintf(ErrorString, "CreateFile Error = %d", GetLastError());
            ShowMessage(ErrorString);
        }
        else
        {
            dwRetFlag = GetCommState(hComms, &Dcb);

            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            Dcb.DCBlength = sizeof(Dcb);

            Dcb.BaudRate = CBR_57600;

            Dcb.ByteSize = 8;
            Dcb.Parity = NOPARITY;
            Dcb.StopBits = ONESTOPBIT;
            Dcb.fTXContinueOnXoff = TRUE;

            Dcb.fOutxCtsFlow = FALSE;//TRUE;                  // disable CTS output flow control
            Dcb.fOutxDsrFlow = FALSE;                  // disable DSR output flow control
            Dcb.fDtrControl = DTR_CONTROL_HANDSHAKE  /*DTR_CONTROL_DISABLE DTR_CONTROL_ENABLE*/;
            Dcb.fDsrSensitivity = FALSE;               // enable DSR sensitivity

            Dcb.fOutX = FALSE;                        // disable XON/XOFF out flow control
            Dcb.fInX = FALSE;                         // disable XON/XOFF in flow control
            Dcb.fErrorChar = FALSE;                   // disable error replacement
            Dcb.fNull = FALSE;                        // disable null stripping
            Dcb.fRtsControl = RTS_CONTROL_HANDSHAKE /* RTS_CONTROL_ENABLE  RTS_CONTROL_DISABLE*/;   //  enable RTS line
            Dcb.fAbortOnError = TRUE;                 // don't abort reads/writes on error

            dwRetFlag = SetCommState(hComms, &Dcb);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            dwRetFlag = GetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            CommTimeouts.ReadIntervalTimeout         = 50;    //Don't use interval timeouts
            CommTimeouts.ReadTotalTimeoutMultiplier  = 50;    //Don't use multipliers
            CommTimeouts.ReadTotalTimeoutConstant    = 50;    //150ms total read timeout
            CommTimeouts.WriteTotalTimeoutMultiplier = 5;//Don't use multipliers
            CommTimeouts.WriteTotalTimeoutConstant      = 5;//2200ms total write timeout

            dwRetFlag = SetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
        }
    }
    else
    {
        ShowMessage("Comm port already open");
    }
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
    if (hComms != 0)
    {
        CloseHandle(hComms);
        hComms = 0;
    }
    else
    {
        ShowMessage("Comm port already closed");
    }
}
//---------------------------------------------------------------------------

void SendToComPort(DWORD ResponseLength, unsigned char *Buffer)
{
    DWORD dwBytesWritten;

    if (hComms != 0)
        dwRetFlag = WriteFile(hComms, Buffer, ResponseLength, &dwBytesWritten, NULL);
    else
    {
        ShowMessage("Comm port not open");
    }
}
//---------------------------------------------------------------------------
void ReceiveFromComPort(char *Buffer)
{
    DWORD dwBytesRead = 0;

    if (hComms != 0)
    {
        while (dwBytesRead == 0)
            dwRetFlag = ReadFile(hComms, Buffer, sizeof(Buffer), &dwBytesRead, NULL);
    }
    else
    {
        ShowMessage("Comm port not open");
    }
}
//---------------------------------------------------------------------------
void ShowMessage(char *pErrorString)
{
    MessageBox(NULL, pErrorString, "Error", MB_OK);
}

