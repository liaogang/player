/**
* lrcFch.h
* lyricsFetcher

*Created by liaogang on 7/7/14.
*Copyright (c) 2014 gang.liao. All rights reserved.
*/

#ifndef __HEADER_SOCKET_TOOL_H__
#define __HEADER_SOCKET_TOOL_H__


/**
 * is in unix or win-nt
 */
#ifndef _WINDOWS 
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>//sockaddr_in
typedef unsigned int UINT;
#define FALSE (0)
#define TRUE  (1)
#define BOOL INT
#define INT int
#define SOCKET int
#define HOSTENT hostent
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
#define SOCKADDR_IN sockaddr_in
#define closesocket(s) close(s)
#define _tcslen wcslen
#define _T(x) L##x
#else
#include "stdafx.h"
#endif

//int GetLastError();

BOOL CreateTcpSocketClient(const char *strHost , SOCKET *socketClient);

/// return bytes sended.
unsigned long sendDataToSocket(SOCKET socket , unsigned char *buffer , unsigned long bufLen);


/// return bytes received.
struct MemBuffer
{
    unsigned long length;
    unsigned char buffer[0];
};

MemBuffer *newMemBuffer(int len);

void deleteMemBuffer(MemBuffer *buffer);

MemBuffer* recvSocketData(SOCKET socket );

int writeHttpContent(SOCKET httpResponse , const char *savepath );

int writeHttpContent2(SOCKET socketDownload, FILE *pFile );

int curlUrlFile(const char *url , const char *savepath);


#endif