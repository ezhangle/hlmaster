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


#include "CStringBuffer.h"
#include "CInetAddress.h"


// start namespace pfc
// ======================================================================

namespace pfc
{
	// ==================================================================

	CStringBuffer::CStringBuffer(int nBufferSize)
	{
		if(nBufferSize < 1)
			m_nBufferSize = 33;
		else
			m_nBufferSize = (nBufferSize + 1);

		// TODO: check for malloc errors
		m_pszBuffer    = new char[ m_nBufferSize ];

		SetLength(0);
	}


	// ==================================================================

	CStringBuffer::~CStringBuffer()
	{
		if(m_pszBuffer != NULL)
			delete[] m_pszBuffer;
	}


	// ==================================================================

	int CStringBuffer::SetLength(int nLength)
	{
		if(nLength < 0)
			m_nLength = 0;
		else if(nLength >= m_nBufferSize)
			m_nLength = (m_nBufferSize - 1);
		else
			m_nLength = nLength;

		m_nFree   = (m_nBufferSize - m_nLength - 1);
		m_pEnd    = &m_pszBuffer[ m_nLength ];
		*(m_pEnd) = '\0';

		return m_nLength;
	}


	// ==================================================================

	bool CStringBuffer::Append(char cValue)
	{
		if(m_nFree != 0)
		{
			*(m_pEnd) = cValue;

			m_pEnd++;
			m_nFree--;
			m_nLength++;

			*(m_pEnd) = '\0';

			return true;
		}
		else
			return false;
	}


	// ==================================================================

	bool CStringBuffer::Append(int nValue)
	{
		int nLength = ::PString::ValueOf(nValue, m_pEnd, m_nFree);
		if(nLength > 0)
		{
			m_pEnd    += nLength;
			m_nLength += nLength;
			m_nFree   -= nLength;

			return true;
		}
		else
			return false;
	}


	// ==================================================================

	bool CStringBuffer::Append(CInetAddress *pInetAddr)
	{
		int nLength = pInetAddr->GetAddress(m_pEnd, m_nFree);
		if(nLength > 0)
		{
			m_pEnd    += nLength;
			m_nLength += nLength;
			m_nFree   -= nLength;

			return true;
		}
		else
			return false;
	}


	// ==================================================================

	bool CStringBuffer::Append(const char *pszValue)
	{
		// buffer full?
		if(m_nFree == 0)
			return false;

		if(pszValue == NULL)
			pszValue = "(null)";

		for(/* no init*/; (m_nFree > 0) && (*(pszValue) != '\0'); m_nLength++, m_nFree--)
		{
			// copy string
			*(m_pEnd) = *(pszValue);

			m_pEnd++;
			pszValue++;
		}

		// terminate buffer, recalculate free space
		*(m_pEnd) = '\0';

		// check, if string copied completly
		if(*(pszValue) == '\0')
			return true;
		else
			return false;
	}



// ======================================================================
// end namespace pfc
}


