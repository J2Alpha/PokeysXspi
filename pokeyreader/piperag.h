/*
 * piperag.h
 *
 *  Created on: 15 okt. 2014
 *      Author: J2Alpha
 */

#ifndef PIPERAG_H_
#define PIPERAG_H_

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <array>

//pipe stuff
#define BUFSIZE 512

int pipe_getbufsize(void);
int commencepiping(void);
int setpipemode(DWORD moderequest);
int pipe_stuff(LPTSTR message);
int pipe_smoke(TCHAR *buffer);
void pipe_extinguish(void);
#endif /* PIPERAG_H_ */
