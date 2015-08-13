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
   | Created.......: Thursday, 10. May 2001                             |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#ifndef _PEXCEPTION_H_
#define _PEXCEPTION_H_



// Classes defined in this document
// ---------------------------------
// 
class PException;



// Header includes
// ---------------------------------
// 



// Class definition
// ---------------------------------
//
class PException
{
public:

	PException()
	{
		m_nErrorValue = 0;
		m_szDescription[0] = '\0';
	}

	PException(int nErrorValue)
	{
		m_nErrorValue = nErrorValue;
		m_szDescription[0] = '\0';
	}

	PException(const char *pszDescription)
	{
		m_nErrorValue = 0;
		SetDescription(pszDescription);
	}

	PException(int nErrorValue, const char *pszDescription)
	{
		m_nErrorValue = nErrorValue;
		SetDescription(pszDescription);
	}



	const char* GetDescription()
	{
		return m_szDescription;
	}

	const int GetValue()
	{
		return m_nErrorValue;
	}

	const int IsEqual(const int nErrorValue)
	{
		if(nErrorValue == m_nErrorValue)
			return true;
		else
			return false;
	}


protected:

	void SetDescription(const char *pszDescription)
	{
		int nCount;
		int nBufferSize = sizeof(m_szDescription) - 1;

		// copy string, check for buffer overflow
		for(nCount = 0; nCount < nBufferSize && *(pszDescription) != '\0'; nCount++, pszDescription++)
			m_szDescription[nCount] = *(pszDescription);

		m_szDescription[nCount] = '\0';
	}

	int  m_nErrorValue;
	char m_szDescription[64];
};



#endif		// #ifndef _PEXCEPTION_H_
