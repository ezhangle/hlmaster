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
   | Created.......: Sunday, 27. May 2001                               |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#include "PString.h"



// ======================================================================

PString::PString(const char *pszValue)
{
	m_nBufferSize = PString::GetLength(pszValue);
	m_nBufferSize += 1;	// for terminating '\0'
	m_pszString = new char[ m_nBufferSize ];

	// TODO: check for malloc errors

	SetValue(pszValue, m_nBufferSize);
}


// ======================================================================

PString::PString(int nMaxLength, const char *pszValue)
{
	m_nBufferSize = PString::GetLength(pszValue);

	if(m_nBufferSize > nMaxLength)
		m_nBufferSize = nMaxLength;

	m_nBufferSize += 1;	// for terminating '\0'
	m_pszString = new char[ m_nBufferSize ];

	// TODO: check for malloc errors

	SetValue(pszValue, m_nBufferSize);
}


// ======================================================================

PString::PString(const char *pszValue, int nLength)
{
	if(nLength >= 0)
		m_nBufferSize = nLength;
	else
		m_nBufferSize = 0;

	m_nBufferSize += 1;	// for terminating '\0'
	m_pszString = new char[ m_nBufferSize ];

	// TODO: check for malloc errors

	SetValue(pszValue, m_nBufferSize);
}


// ======================================================================

PString::PString(const PString &pString)
{
	m_nBufferSize = pString.m_nBufferSize;
	m_pszString = new char[ m_nBufferSize ];

	SetValue(pString.m_pszString, pString.m_nLength);
}


// ======================================================================

PString::~PString()
{
	delete[] m_pszString;
}



// ======================================================================

int PString::SetValue(const char *pszValue)
{
	return SetValue(pszValue, PString::GetLength(pszValue));
}


// ======================================================================

const char* PString::GetValue() const
{
	return m_pszString;
}


// ======================================================================

int PString::SetValue(const char *pszValue, int nLength)
{
	if(pszValue == NULL || nLength <= 0)
	{
		m_pszString[0] = '\0';
		return 0;
	}
	else
	{
		int nCount;

		// copy string
		for(nCount = 0; nCount < nLength && nCount < (m_nBufferSize - 1); nCount++)
			m_pszString[nCount] = pszValue[nCount];

		m_pszString[nCount] = '\0';

		m_nLength = nCount;
		return m_nLength;
	}
}


// ======================================================================
// static function

int PString::GetLength(const char *pszString)
{
	if(pszString == NULL)
		return 0;

	return strlen(pszString);
}


// ======================================================================
// static function

bool PString::Equals(const char *pszString1, const char *pszString2)
{
	if(pszString1 == NULL || pszString2 == NULL)
		return false;

	if(strcmp(pszString1, pszString2) == 0)
		return true;
	else
		return false;
}


// ======================================================================
// static function

bool PString::EqualsIgnoreCase(const char *pszString1, const char *pszString2)
{
	if(pszString1 == NULL || pszString2 == NULL)
		return false;

	// TODO: improve performance
	int nCount;
	for(nCount = 0; pszString1[nCount] != '\0' && pszString2[nCount] != '\0'; nCount++)
	{
		if(tolower(pszString1[nCount]) != tolower(pszString2[nCount]))
			return false;
	}

	if(pszString1[nCount] == '\0' && pszString2[nCount] == '\0')
		return true;
	else
		return false;
}


// ======================================================================
// static function

void PString::ToUpperCase(char *pszString)
{
	if(pszString == NULL)
		return;

	while(*(pszString) != '\0')
	{
		*(pszString) = (char)toupper(*(pszString));

		pszString++;
	}
}


// ======================================================================
// static function

void PString::ToLowerCase(char *pszString)
{
	if(pszString == NULL)
		return;

	while(*(pszString) != '\0')
	{
		*(pszString) = (char)tolower(*(pszString));

		pszString++;
	}
}


// ======================================================================
// static function

int PString::Trim(char *pszString, bool bTrimRight /* = true */, bool bTrimLeft /* = true */)
{
	if(pszString == NULL)
		return 0;

	int nCount = 0;
	char *pszTempOne;
	char *pszTempTwo;

	if(bTrimRight == true)
	{
		// check for spaces & tabs
		pszTempTwo = pszString;
		for(nCount = 0; (*(pszTempTwo) == ' ') || (*(pszTempTwo) == '\t'); nCount++, pszTempTwo++)
			;	/* do nothing */

		if(nCount > 0)
		{
			if(*(pszTempTwo) == '\0')
			{
				// string contains only spaces
				*(pszString) = '\0';
				return nCount;
			}
			else
			{
				// pszTempTwo is allready set by preceding loop
				pszTempOne = pszString;

				// strip spaces & tabs
				while(*(pszTempTwo) != '\0')
				{
					*(pszTempOne) = *(pszTempTwo);

					pszTempOne++;
					pszTempTwo++;
				}
			}
		}
	}

	if(bTrimLeft == true)
	{
		pszTempTwo = pszString;

		if(*(pszTempTwo) != '\0')
		{
			// go to end
			while(*(pszTempTwo) != '\0')
				pszTempTwo++;
			pszTempTwo--;

			// strip all spaces & tabs
			while((*(pszTempTwo) == ' ') || (*(pszTempTwo) == '\t'))
			{
				*(pszTempTwo) = '\0';
				nCount++;

				if(pszTempTwo == pszString)
					break;

				pszTempTwo--;
			}
		}
	}

	return nCount;
}


// ======================================================================
// static function

int PString::ValueOf(int nValue, char *pszBuffer, int nBufferSize)
{
	if(pszBuffer == NULL || nBufferSize < 2)
		return 0;

	int nCount   = 0;
	int nCurrent = 0;

	// for reversing
	char cTemp;
	char *pStart;
	char *pEnd;

	if(nValue == 0)
	{
		pszBuffer[0] = '0';
		pszBuffer[1] = '\0';

		return 1;
	}

	if(nValue < 0)
	{
		pszBuffer[0] = '-';
		nCount++;

		// turn value to positive
		nValue = -(int)nValue;
	}

	// set start after the '-'
	pStart = &pszBuffer[nCount];


	for(/* no init */; (nCount < (nBufferSize - 1)) && (nValue > 0); nCount++)
	{
		nCurrent = nValue % 10;		// get current digit
		nValue  /= 10;				// for next digit

		// convert to ascii char and append to string
		pszBuffer[nCount] = (char) (nCurrent + '0');
	}

	if(nValue != 0)
	{
		// buffer to small
		pszBuffer[0] = '\0';
		return 0;
	}
	else
	{
		// terminate string
		pszBuffer[nCount] = '\0';
	}


	// string is in reverse order, we might need to reverse it
	if(nCount > 1)
	{
		pEnd = &pszBuffer[(nCount - 1)];

		while(pStart < pEnd)
		{
			// swap chars
			cTemp = *(pStart);
			*(pStart) = *(pEnd);
			*(pEnd)   = cTemp;

			pStart++;
			pEnd--;
		}
	}

	return nCount;
}


