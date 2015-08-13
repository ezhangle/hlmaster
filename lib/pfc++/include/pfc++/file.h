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


#ifndef _PFC_FILE_H_
#define _PFC_FILE_H_

#include <pfc++/system.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


// begin windows specific
// ======================================================================
#if defined PFC_SYSTEM_TYPE_WINDOWS

#define PFC_MAX_PATH		_MAX_PATH
#define FFC_MAX_FILENAME	_MAX_FNAME


// begin unix specific
// ======================================================================
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define PFC_MAX_PATH		_POSIX_PATH_MAX
#define PFC_MAX_FILENAME	_POSIX_NAME_MAX


// end unix specific
// ======================================================================
#endif



#endif	// _PFC_FILE_H_
