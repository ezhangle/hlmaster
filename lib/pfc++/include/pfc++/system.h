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


#ifndef _PFC_SYSTEM_H_
#define _PFC_SYSTEM_H_


#if defined WIN32
#define PFC_SYSTEM_WINDOWS
#define PFC_SYSTEM_TYPE_WINDOWS

#elif defined __linux__
#define PFC_SYSTEM_LINUX
#define PFC_SYSTEM_TYPE_UNIX

#elif defined __FreeBSD__
#define PFC_SYSTEM_FREEBSD
#define PFC_SYSTEM_TYPE_UNIX

#elif defined __NetBSD__
#define PFC_SYSTEM_NETBSD
#define PFC_SYSTEM_TYPE_UNIX

#else
#error Unknown operating system. Please check <pfc++/systems.h>.
#endif


#endif	// _PFC_SYSTEM_H_
