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


#ifndef _PFC_CSTRINGBUFFER_H_
#define _PFC_CSTRINGBUFFER_H_


// Header includes
// ======================================================================
#include "PString.h"
#include "CInetAddress.h"

// start namespace pfc
// ======================================================================

namespace pfc
{

	class CStringBuffer
	{
	public:

		CStringBuffer(int nBufferSize);
		~CStringBuffer();

		bool Append(char cValue);
		bool Append(int nValue);
		// DUMMY: bool Append(float fValue);
		// DUMMY: bool Append(double dValue);
		// DUMMY: bool Append(bool bValue);
		bool Append(CInetAddress *pInetAddr);
		bool Append(const char *pszValue);
		// DUMMY: bool Append(const char *pszValue, int nLength);

		// DUMMY: bool Insert(int nValue, bool bOverwrite = true);
		// DUMMY: bool Insert(const char *pszValue, bool bOverwrite = true);
		// DUMMY: bool Insert(const char *pszValue, int nLength, bool bOverwrite = true);


		// DUMMY: int CharAt(int nIndex) const;
		// DUMMY: bool SetCharAt(int nIndex, char nChar);

		void Clear() { SetLength(0); }
		int SetLength(int nLength);
		int GetLength() const { return m_nLength; }
		int GetBufferSize() const { return m_nBufferSize; }

		const char* GetCharPointer() const { return m_pszBuffer; }
		const char* GetValueBuffer() const { return m_pszBuffer; }


	private:

		int m_nLength;			// the number of bytes currently in use
		int m_nFree;			// the number of bytes currently available
		int m_nBufferSize;		// the total size of the buffer

		char *m_pszBuffer;		// the buffer
		char *m_pEnd;			// points to terminating '\0' of buffer
	};

// ======================================================================
// end namespace pfc
}

#endif		// #ifndef _PFC_CSTRINGBUFFER_H_
