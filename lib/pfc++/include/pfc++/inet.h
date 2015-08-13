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


#ifndef _PFC_INET_H_
#define _PFC_INET_H_

#include <pfc++/types.h>

typedef pfc_uint16 pfc_inet_port;
typedef pfc_uint32 pfc_inet_addr;

#define PFC_INET_ADDR_ANY               INADDR_ANY
#define PFC_INET_ADDR_NONE              INADDR_NONE
#define PFC_INET_ADDR_LOOPBACK          INADDR_LOOPBACK
#define PFC_INET_ADDR_BROADCAST         INADDR_BROADCAST
#define PFC_INET_PORT_ANY               0



#endif	// _PFC_INET_H_
