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
   | Created.......: Monday, 27. June 2001                              |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#ifndef _PFC_CINETADDRESS_H_
#define _PFC_CINETADDRESS_H_


// Header includes
// ======================================================================
#include "PSocket.h"
#include "PString.h"


// start namespace pfc
// ======================================================================

namespace pfc
{
	class CInetAddress
	{
	public:

		CInetAddress();
		CInetAddress(pfc_inet_addr nIpAddress);
		CInetAddress(const char *pszIpAddress);
		~CInetAddress();

		bool IsValid();

		void SetAddress(const char *pszString);
		void SetAddress(pfc_inet_addr nIpAddress);

//		pfc_inet_addr GetAddress() { return m_nIpAddress; }
		int GetAddress(char *pszBuffer, int nBufferSize);


		// TODO: a lot. this class is far away from usefull...

	private:

		pfc_inet_addr m_nIpAddress;
	};

// ======================================================================
// end namespace pfc
}

#endif		// #ifndef _PFC_CINETADDRESS_H_
