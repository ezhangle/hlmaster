/*
   +--------------------------------------------------------------------+
   | .............:: pastabrothers foundation classes ::............... |
   +--------------------------------------------------------------------+
   | Copyright (C) 2001 Silvan Minghetti                                |
   |                                                                    |
   | This library is free software; you can redistribute it and/or      |
   | modify it under the terms of the GNU Lesser General Public         |
   | License as published by the Free Software Foundation; either       |
   | version 2.1 of the License, or (at your option) any later version. |
   |                                                                    |
   | This library is distributed in the hope that it will be useful,    |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of     |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  |
   | Lesser General Public License for more details.                    |
   |                                                                    |
   | You should have received a copy of the GNU Lesser General Public   |
   | License along with this library; if not, write to the              |
   | Free Software Foundation, Inc., 59 Temple Place - Suite 330,       |
   | Boston, MA 02111-1307, USA.                                        |
   |                                                                    |
   +--------------------------------------------------------------------+
   | Created.......: Monday, 25. June 2001                              |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#ifndef _PFC_SOCKET_H_
#define _PFC_SOCKET_H_

#include <pfc++/types.h>
#include <pfc++/inet.h>




// begin windows specific
// ======================================================================
#if defined PFC_SYSTEM_TYPE_WINDOWS

#include <winsock2.h>


#define PFC_ERROR_INET_ADDR_INUSE       WSAEADDRINUSE
#define PFC_ERROR_INET_ADDR_NOTAVAIL	WSAEADDRNOTAVAIL
#define PFC_ERROR_INET_DOWN             WSAENETDOWN
#define PFC_ERROR_INET_NOTINITIALISED   WSANOTINITIALISED
#define PFC_ERROR_INET_NOSOCKET         WSAENOTSOCK

typedef SOCKET PFC_SOCKET;
typedef SOCKET PFC_FD;

typedef SOCKET pfc_socket_t;
typedef int pfc_socklen_t;


// begin unix specific
// ======================================================================
#elif defined PFC_SYSTEM_TYPE_UNIX

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>


#define PFC_ERROR_INET_ADDR_INUSE       EINVAL
#define PFC_ERROR_INET_ADDR_NOTAVAIL	EACCES
#define PFC_ERROR_INET_DOWN             0
#define PFC_ERROR_INET_NOTINITIALISED   0
#define PFC_ERROR_INET_NOSOCKET         ENOTSOCK


typedef int PFC_SOCKET;
typedef int PFC_FD;


typedef int pfc_socket_t;
typedef socklen_t pfc_socklen_t;


// end unix specific
// ======================================================================
#endif




#define PFC_FD_INVALID                  -1
#define PFC_SOCKET_INVALID              -1
#define PFC_SOCKET_ERROR				-1


#define PFC_WAIT_INFINITE				0xFFFFFFFF




#endif	// _PFC_SOCKET_H_
