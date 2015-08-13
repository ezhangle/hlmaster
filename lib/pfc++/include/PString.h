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


#ifndef _PSTRING_H_
#define _PSTRING_H_



// Classes defined in this document
// ---------------------------------
// 
class PString;



// Header includes
// ---------------------------------
// 
#include <string.h>
#include <ctype.h>

#if !defined NULL
#define NULL  0
#endif


// Class definition
// ---------------------------------
//
class PString
{
public:

	PString(int nMaxLength, const char *pszValue);
	PString(const char *pszValue);
	PString(const char *pValue, int nLength);
	PString(const PString &pString);
	~PString();

	int SetValue(const char *pszValue);
	int SetValue(const char *pszValue, int nLength);
	int SetValue(int nValue);
	int SetValue(float fValue);

	const char* GetValue() const;
	// DUMMY: int GetValueAsInt() const;
	// DUMMY: float GetValueAsFloat() const;
	// DUMMY: bool GetValueAsBool() const;

	// DUMMY: int GetCharAt(int nIndex) const;
	// DUMMY: int GetValue(char *pszBuffer, int nBufferSize) const;
	int GetLength() const { return m_nLength; }
	int GetBufferSize() const { return m_nBufferSize; }

	int  Compare(const char *pszString) const { return strcmp(m_pszString, pszString); }
	int  Compare(const PString *pString) const { return strcmp(m_pszString, pString->m_pszString); }

	bool Equals(const char *pszString) const { return Equals(m_pszString, pszString); }
	bool Equals(const PString *pString) const { return Equals(m_pszString, pString->m_pszString); }
	bool EqualsIgnoreCase(const char *pszString) const { return EqualsIgnoreCase(m_pszString, pszString); }
	bool EqualsIgnoreCase(const PString *pString) const { return EqualsIgnoreCase(m_pszString, pString->m_pszString); }

	void ToUpperCase() { ToUpperCase(m_pszString); }
	void ToLowerCase() { ToLowerCase(m_pszString); }

	// DUMMY: void Reverse() { Reverse(m_pszString); }
	void Trim(bool bTrimRight = true, bool bTrimLeft = true);
	// DUMMY: void Replace(const char cFromLetter, const char cToLetter);

	// DUMMY: int IndexOf(const char cLetter) const;
	// DUMMY: int LastIndexOf(const char cLetter) const;



	// ----------------------------------------
	// static functions

	static int GetLength(const char *pszString);

	static bool Equals(const char *pszString1, const char *pszString2);
	static bool EqualsIgnoreCase(const char *pszString1, const char *pszString2);
	static int Compare(const char *pszString1, const char *pszString2) { return strcmp(pszString1, pszString2); }

	static void ToUpperCase(char *pszString);
	static void ToLowerCase(char *pszString);
	// DUMMY: static void Reverse(char *pszString);
	static int Trim(char *pszString, bool bTrimRight = true, bool bTrimLeft = true);
	// DUMMY: static int Trim(char *pszString, const char cTrimChar, bool bTrimRight = true, bool bTrimLeft = true);
	// DUMMY: static int Replace(char *pszString, const char cFromLetter, const char cToLetter);

	// DUMMY: static int IndexOf(const char *pszString, const char cLetter);
	// DUMMY: static int LastIndexOf(const char *pszString, const char cLetter);

	// DUMMY: static int GetValueAsInt(const char *pszString);
	// DUMMY: static float GetValueAsFloat(const char *pszString);
	// DUMMY: static bool GetValueAsBool(const char *pszString);

	static int ValueOf(int nValue, char *pszBuffer, int nBufferSize);


protected:

	int m_nLength;
	int m_nBufferSize;

	char *m_pszString;
};





#endif		// #ifndef _PSTRING_H_
