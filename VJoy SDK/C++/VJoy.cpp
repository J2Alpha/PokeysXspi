// VJoy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "VJoy.h"

JOYSTICK_STATE m_joyState[2] = { 0 };

int main(int argc, char* argv[])
{
	VJoy_Initialize("DCcVstic", "1237942");
	  
	m_joyState[0].XAxis = 32767;
	m_joyState[0].YAxis = 32767; 
	m_joyState[0].ZAxis = 32767;
	m_joyState[0].Buttons = 0xAAAAAAAA;
	m_joyState[0].POV = (4 << 12) | (4 << 8) | (4 << 4) | 4;
	for (int i = 0; i < 10000; i++){
		m_joyState[0].XAxis++;
		Sleep(100);
		VJoy_UpdateJoyState(0, &m_joyState[0]);
	}
	VJoy_UpdateJoyState(0, &m_joyState[0]);

	VJoy_Shutdown();/*
	1>C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\V120\Microsoft.CppBuild.targets(1193,5): warning MSB8012: TargetPath(
	e:\Users\J2Alpha\code\VJoySDK\VJoy SDK\C++\x64\Release\VJoy.exe) does not match the Linker's OutputFile property value (
	e:\Users\J2Alpha\code\VJoySDK\VJoy SDK\C++\Release\VJoy.exe). This may cause your project to build incorrectly. To correct this, please make sure that $(OutDir), $(TargetName) and $(TargetExt) property values match the value specified in %(Link.OutputFile).
1>  VJoy.vcxproj -> e:\Users\J2Alpha\code\VJoySDK\VJoy SDK\C++\x64\Release\VJoy.exe
	
	*/
	return 0;
}
