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


#ifndef _PFC_TYPES_H_
#define _PFC_TYPES_H_

#include <pfc++/system.h>


// begin windows specific types
// ======================================================================
#if defined PFC_SYSTEM_TYPE_WINDOWS

typedef signed __int8    pfc_int8;
typedef signed __int16   pfc_int16;
typedef signed __int32   pfc_int32;

typedef unsigned __int8  pfc_uint8;
typedef unsigned __int16 pfc_uint16;
typedef unsigned __int32 pfc_uint32;


// begin linux specific types
// ======================================================================
#else

#include <sys/types.h>

typedef int8_t           pfc_int8;
typedef int16_t          pfc_int16;
typedef int32_t          pfc_int32;

typedef u_int8_t         pfc_uint8;
typedef u_int16_t        pfc_uint16;
typedef u_int32_t        pfc_uint32;


// end linux specific types
// ======================================================================
#endif


#endif	// _PFC_TYPES_H_
