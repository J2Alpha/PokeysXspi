/*
 * bagpipes.h
 *
 * nearly identical to http://msdn.microsoft.com/en-us/library/aa365588.aspx
 *
 *  Created on: 14 okt. 2014
 *      Author: J2Alpha
 */

#ifndef BAGPIPES_H_
#define BAGPIPES_H_

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
//#include <strsafe.h>// not used!
#define BUFSIZE 512

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

#endif /* BAGPIPES_H_ */
