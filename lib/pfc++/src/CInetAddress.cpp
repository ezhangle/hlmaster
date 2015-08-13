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


#include "CInetAddress.h"


// start namespace pfc
// ======================================================================

namespace pfc
{
	// ==================================================================

	CInetAddress::CInetAddress()
	{
		m_nIpAddress = PFC_INET_ADDR_ANY;
	}


	// ==================================================================

	CInetAddress::CInetAddress(const char *pszIpAddress)
	{
		SetAddress(pszIpAddress);
	}


	// ==================================================================

	CInetAddress::~CInetAddress()
	{
	}


	// ==================================================================

	void CInetAddress::SetAddress(pfc_inet_addr nIpAddress)
	{
		m_nIpAddress = nIpAddress;
	}


	// ==================================================================

	void CInetAddress::SetAddress(const char *pszIpAddress)
	{
		m_nIpAddress = ::inet_addr(pszIpAddress);
	}


	// ==================================================================

	bool CInetAddress::IsValid()
	{
		if(m_nIpAddress == PFC_INET_ADDR_ANY)
			return false;
		else
			return true;
	}


	// ==================================================================
	// NOTE: this function is just 'a quick & dirty hack'

	int CInetAddress::GetAddress(char *pszBuffer, int nBufferSize)
	{
		if(pszBuffer == NULL || nBufferSize < 16)
			return 0;

		int nLength     = 0;
		int nPartLength = 0;
		pfc_inet_addr nPart;

		nPart = (m_nIpAddress)       & 0x000000FF;
		nPartLength = ::PString::ValueOf((int) nPart, pszBuffer + nLength, nBufferSize - nLength);
		nLength += nPartLength;
		pszBuffer[nLength++] = '.';

		nPart = (m_nIpAddress >> 8)  & 0x000000FF;
		nPartLength = ::PString::ValueOf((int) nPart, pszBuffer + nLength, nBufferSize - nLength);
		nLength += nPartLength;
		pszBuffer[nLength++] = '.';

		nPart = (m_nIpAddress >> 16) & 0x000000FF;
		nPartLength = ::PString::ValueOf((int) nPart, pszBuffer + nLength, nBufferSize - nLength);
		nLength += nPartLength;
		pszBuffer[nLength++] = '.';

		nPart = (m_nIpAddress >> 24) & 0x000000FF;
		nPartLength = ::PString::ValueOf((int) nPart, pszBuffer + nLength, nBufferSize - nLength);
		nLength += nPartLength;

		return nLength;
	}



// ======================================================================
// end namespace pfc
}


