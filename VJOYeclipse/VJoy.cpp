// VJoy.cpp : Defines the entry point for the console application.
//

#define VJOY_EXPORTS


#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

//heh, from winNT

#define VOID void

//vjoy constants
extern "C" {
#include "VJoy.h"
}
int debugger=0;
bool CommsEnabled;
HANDLE hComms;
DWORD dwRetFlag;

int debugloopback=0;
unsigned short com0;
unsigned short com1;

int signature=1;
int multip=1;
int zero=0;
int umin=32768; //0x0fff
int upos=32767; //0x0ffe
int loopmax=65535; //0xffff
int sticknr=0;
int axistype=0;

int updatejoyaxis(short data,char axis);

JOYSTICK_STATE m_joyState[2] = { 0 };
//m_joyState[0].XAxis = 8th bar down?
//m_joyState[0].YAxis = 7th bar down?
//m_joyState[0].ZAxis = 6th bar down?
//coms and configs

//open port
void OpenComms(void);
//close port
void CloseComms(void);
//send chars
void SendToComPort(DWORD ResponseLength, unsigned char *Buffer);
//get chars
void ReceiveFromComPort(char *Buffer);
int readin(void);
int comvert(unsigned short com, int* newcom);

char name[]= "DCcVthrottle";
char serial[]= "1237942";

int main(int argc, char* argv[])
{
	char exitval=0;
	char command[1] ={'B'};
	char recieve[4] ={0,0,0,0};
	OpenComms();
	VJoy_Initialize(name,serial);
	if(hComms == INVALID_HANDLE_VALUE){
		exitval = 1;
	}
	m_joyState[sticknr].XAxis = 0;
	m_joyState[sticknr].YAxis = 0;
	m_joyState[sticknr].ZAxis = 0;
	m_joyState[sticknr].Buttons = 0x00000000;//no button pressed
	m_joyState[sticknr].POV = (4 << 12) | (4 << 8) | (4 << 4) | 4;//pov? don't know this one
	//send initial to uart
	while(!exitval){
		//send 'B'
		SendToComPort(strlen(command), (unsigned char *) command);
		//get answer
		ReceiveFromComPort( (char *) recieve);
		com0=((0xff00 & (((unsigned short)recieve[1])<<8)) | (0x00ff & (unsigned short) recieve[0]));
		com1=((0xff00 & (((unsigned short)recieve[3])<<8)) | (0x00ff & (unsigned short) recieve[2]));
		printf("raw: %x %x %x %x \n", recieve[0], recieve[1], recieve[2], recieve[3]);
		printf("com0: %d | com1: %d \n",com0,com1);
		int comverted0=0;
		int comverted1=0;
		if(!comvert(com0, &comverted0))
		{
			if(!comvert(com1, &comverted1))
			{
				printf("adjusted com0: %d | com1: %d \n",comverted0,comverted1);
				if(axistype==0){
					m_joyState[sticknr].YAxis =com0; //throttle 2 => 7th bar down?
					m_joyState[sticknr].ZAxis =com1; //throttle => 6th bar down?;
				}
				if(axistype==1){
					m_joyState[sticknr].XAxis =com0; //throttle 2 => 5th bar down?
					m_joyState[sticknr].ZAxis =com1; //throttle => 6th bar down?;
				}
				if(axistype==2){
					m_joyState[sticknr].XAxis =com0; //throttle 2 => 5th bar down?
					m_joyState[sticknr].YAxis =com1; //throttle => 7th bar down?;
				}
			}
		}
		Sleep(10);//allow qdec to stay up to date by sleeping occasionally
		VJoy_UpdateJoyState(sticknr, &m_joyState[sticknr]);
		//printf("updating %d \n",m_joyState[sticknr].XAxis);
		//exitval = getchar_unlocked(); figure out how to avoid locking the console
	}
	//VJoy_UpdateJoyState(0, &m_joyState[0]);
	VJoy_Shutdown();
	printf("unexpected termination, press any key to exit \n");
	fgetchar();
	return 0;
}
int updatejoyaxis(short data,char axis) {
	char buffer [100];
	switch(axis) {
	case 'x':
		m_joyState[0].XAxis =data;
		if(debugger)
		{
			printf(buffer, "updating X axis to %lu", data);
		}
		break;
	case 'y':
		m_joyState[0].YAxis =data;
		if(debugger)
		{
			printf(buffer, "updating Y axis to %lu", data);
		}
		break;
	case 'z':
		m_joyState[0].ZAxis =data;
		if(debugger)
		{
			printf(buffer, "updating Z axis to %lu", data);
		}
		break;
	default:
		printf("axis error, no such axis: %c \n",axis);
	}
	return 1;
}
//-----------------------------------------------------------------------------------------------------------
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
            sprintf(ErrorString, "CreateFile Error = %lu", GetLastError());
        }
        else
        {
            dwRetFlag = GetCommState(hComms, &Dcb);

            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommState Error = %lu", GetLastError());
            }

            Dcb.DCBlength = sizeof(Dcb);

            Dcb.BaudRate = CBR_57600;

            Dcb.ByteSize = 8;
            Dcb.fBinary = TRUE;
            Dcb.Parity = NOPARITY;
            Dcb.StopBits = ONESTOPBIT;
            Dcb.fTXContinueOnXoff = TRUE;

            Dcb.fOutxCtsFlow = FALSE;//TRUE;                  // disable CTS output flow control
            Dcb.fOutxDsrFlow = FALSE;                  // disable DSR output flow control
            Dcb.fDtrControl = DTR_CONTROL_DISABLE  /*DTR_CONTROL_DISABLE DTR_CONTROL_ENABLE*/;
            Dcb.fDsrSensitivity = FALSE;               // enable DSR sensitivity

            Dcb.fOutX = FALSE;                        // disable XON/XOFF out flow control
            Dcb.fInX = FALSE;                         // disable XON/XOFF in flow control
            Dcb.fErrorChar = FALSE;                   // disable error replacement
            Dcb.fNull = FALSE;                        // disable null stripping
            Dcb.fRtsControl = RTS_CONTROL_DISABLE /* RTS_CONTROL_ENABLE  RTS_CONTROL_DISABLE*/;   //  enable RTS line
            Dcb.fAbortOnError = TRUE;                 // don't abort reads/writes on error

            dwRetFlag = SetCommState(hComms, &Dcb);
            if (!dwRetFlag)
            {
                printf( "SetCommState Error = %lu", GetLastError());

            }

            dwRetFlag = GetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                printf("GetCommTimeouts Error = %lu", GetLastError());

            }

            CommTimeouts.ReadIntervalTimeout         = 50;    //Don't use interval timeouts
            CommTimeouts.ReadTotalTimeoutMultiplier  = 50;    //Don't use multipliers
            CommTimeouts.ReadTotalTimeoutConstant    = 50;    //150ms total read timeout
            CommTimeouts.WriteTotalTimeoutMultiplier = 5;//Don't use multipliers
            CommTimeouts.WriteTotalTimeoutConstant      = 5;//2200ms total write timeout

            dwRetFlag = SetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                printf("SetCommTimeouts Error = %lu", GetLastError());

            }
        }
    }
    else
    {
        printf("Error com port in use");
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
        printf("Comm port already closed");
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
        printf("Comm port not open");
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
        printf("Comm port not open");
    }
}
//---------------------------------------------------------------------------
int readin()//todo this is getting too big, split into subfunctions, maybe work with a map of tokens or such
{
	  std::string line;
	  std::ifstream configfile ("VJoyconfig.txt");
	  if (configfile.is_open())
	  {
	    while (std::getline(configfile,line) )
	    {
	    	std::istringstream iss(line);//store line
	  	  	std::string token;//store tokens if tokenised
	  	  	std::vector<int> tempvalues;// temp storage for values to check if complete after reading tokens
	    	switch(line[0]){
	    	case '#':
	    		//use as comment, ignore comment lines
	    		break;
	    	case '$': //translator todo should be a struct for every axis separate

	    		//leave this out for now

	    		/*std::getline(iss, token, ' ');//discard mode signifier
	    		while(std::getline(iss, token, ' '))
	    		{
	    			int tokenvalue;//yes boost::lexicalcast is my favorite too, but I'm going to keep it simple and stupid :P
	    			std::stringstream tokenstream(token);//in space, no one can hear you stream :P
	    			tokenstream >> tokenvalue;
	    			if(!tokenstream)//check for bad bit
	    			{
	    				printf("error tokenization fail: \" %s \" not a valid int", token.c_str());
	    			}
	    			else
	    			{
	    				tempvalues.push_back(tokenvalue);
	    			}
	    		}
	    		//set the translator right
    			if(tempvalues.size()==4){
    				signature=tempvalues[0];//signed data (x) or unsigned (0)
    				multip=tempvalues[1];//multiplier, INT , normally 1 but can be bigger
    				zero=tempvalues[2];//zero point, recieved 16bit value to translate to zero see below for looping sensors
    				//used when data is unsigned
    				upos=tempvalues[3];//minimum - value for looping sensors (is an unsigned value), smaller is positive
    				umin=tempvalues[4];//maximum + value for looping sensors (is an unsigned value), larger is negative
    				//anything in between the above is assumed erroneous
    				loopmax=tempvalues[5];//largest possible value
    			}
    			else{
    				printf("error not a valid $ type line: found %d tokens", tempvalues.size());
    			}*/
	    		break;
	    	case '-':
	    		std::getline(iss, token, ' ');//discard mode signifier
				while(std::getline(iss, token, ' '))
				{
					int tokenvalue;
					std::stringstream tokenstream(token);
					tokenstream >> tokenvalue;
					if(!tokenstream)//check for bad bit
					{
						printf("error tokenization fail: \" %s \" not a valid int", token.c_str());
					}
					else
					{
						tempvalues.push_back(tokenvalue);
					}
				}
				//set the translator right
				if(tempvalues.size()==2)
				{
					sticknr = tempvalues[0];
					axistype =	tempvalues[1];
				}
				else{
					printf("error not a valid - type line: found %d tokens", tempvalues.size());
				}
				break;
	    	default:
	    		printf("error not a valid linetype: found %d linetoken", line[0]);
	    		break;
	       }
	    }
	    configfile.close();
	  }

	  else printf("Unable to open file");

	  return 0;
}
//----------------------------------------------------------------------------------------------------------------------------
//int umin=32768; //0x1000
//int upos=32767; //0x0fff
//int loopmax=65535; //0xffff

int comvert(unsigned short com, int* newcom){
	if(signature){
		if(com<=upos)
		{
			*newcom = com;
			printf("conversion <= upos succes\n");
			return 0;
		}
		if(com>upos)
		{
			*newcom = (((int) com-upos)-umin);
			printf("conversion > succes\n");
			return 0;
		}
		else
		{
			printf("Invalid value from com\n");
			return 1;
		}
	}
	else{
		*newcom = com;
		return 1;
	}
	return 1;
}

