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
   | Created.......: Wednesday, 16. May 2001                            |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#ifndef _PFILE_H_
#define _PFILE_H_



// Classes defined in this document
// ---------------------------------
// 
class PFile;



// Header includes
// ---------------------------------
// 
#include "PException.h"



// Class definition
// ---------------------------------
//

class PFileException: public PException
{
public:

	PFileException(int nErrorValue)
	:PException(nErrorValue)
	{
	}

	PFileException(int nErrorValue, const char *pszDescription)
	:PException(nErrorValue, pszDescription)
	{
	}

	PFileException()
	:PException()
	{
	}
};


#endif		// #ifndef _PFILE_H_
