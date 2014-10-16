/*
 * pokeysspi.h
 *
 *  Created on: 13 okt. 2014
 *      Author: J2Alpha
 */

#ifndef POKEYSSPI_H_
#define POKEYSSPI_H_

#define NAME "pokey spi module"
#define ID   "DC_consult.pokeytoqdecs0"
#define DESCRIPTION "to soft connect the qdecs to xplane using pokeys"

//on win, shit happens if this isn't here
#if IBM
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     );
//these 5 functions below must be implemented but can be empty
/*XPluginStart
 description:
 The XPluginStart function is called by X-Plane right after the plugin's DLL is loaded to initialize it
 parameters:
  - outName         - a pointer to null-terminated C strings of less than 256 chars. human-readable plugin name.
  - outSignature    - a pointer to null-terminated C strings of less than 256 chars. Fill in this buffer with the plug-in's unique signature.
  - outDescription  - a pointer to null-terminated C strings of less than 256 chars. Fill in this buffer with a pointer to a human-readable description of the plugin or a reason for not being able to load.
 Return Value:
  - 1 if the plug-in loaded successfully
  - 0 if it did not.
*/
#endif //IBM
void OpenComms(void);
//close port
void CloseComms(void);
//send chars
void SendToComPort(DWORD ResponseLength, unsigned char *Buffer);
//get chars
void ReceiveFromComPort(char *Buffer);
//push a message to xplane
void ShowMessage(char *pErrorString);
//comport callback


//standerd xplane things
PLUGIN_API int XPluginStart ( char * outName, char * outSignature, char * outDescription );
/*XPluginStop
description:
 The XPluginStop function is called by X-Plane right before the DLL is unloaded to let it finish up
*/
PLUGIN_API void XPluginStop ( void );
/*XPluginEnable
description:
 The XPluginEnable function is called by X-Plane right before the plugin is enabled.
 Until the plugin is enabled, it will not receive any other callbacks and its UI will be hidden and/or disabled.
*/
PLUGIN_API int XPluginEnable ( void );
/*XPluginDisable
description:
 The XPluginDisable  function is called by X-Plane right before the plugin is disabled.
 Until the plugin is disabled, it will not receive any other callbacks and its UI will be hidden and/or disabled.
*/
PLUGIN_API void XPluginDisable  ( void );
/*XPluginReceiveMessage
 description:
 The XPluginReceiveMessage function is called by the plugin manager when a message is sent to the plugin.
 It receives both messages that are specifically routed to the plugin and messages that are broadcast to all plugins.
 parameters:
  - inFrom - the ID of the plugin that sent the message.
  - inMessage - a long integer indicating the message sent.
  - inParam - a pointer to data that is specific to the message.
 Return Value:
  - 1 if the plug-in loaded successfully
  - 0 if it did not.
*/
PLUGIN_API void XPluginReceiveMessage ( XPLMPluginID inFrom, long inMessage, void * inParam );
#endif /* POKEYSSPI_H_ */
