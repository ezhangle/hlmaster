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
   | Created.......: Monday, 21. May 2001                               |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/


#ifndef _PSTRINGLIST_H_
#define _PSTRINGLIST_H_



// Classes defined in this document
// ---------------------------------
// 
class PStringListItem;
class PStringList;



// Header includes
// ---------------------------------
// 

#include <ctype.h>
#include <string.h>

#if !defined NULL
#define NULL  0
#endif


// Class definition
// ---------------------------------
//
class PStringList
{
public:

	PStringList();
	PStringList(bool bIgnoreCase, bool bUniqueItems);
	~PStringList();

	bool AddItem(const char *pszString);
	bool RemoveItem(const char *pszString);
	int  RemoveAllItems();

	bool FindItem(const char *pszString);
	bool FindItem(const char *pszString, bool bIgnoreCase);

	bool IsCurrentEqual(const char *pszString);
	bool IsCurrentValid()
	{
		if(m_pCurrentItem != NULL)
			return true;
		else
			return false;
	}

	bool PickFirst();
	bool PickNext();

	// TODO: implement function bool RemoveCurrent();

	int GetNumItems() { return m_nItems; }
	const char* GetCurrent();

protected:

	PStringListItem *m_pFirstItem;
	PStringListItem *m_pLastItem;

	PStringListItem *m_pCurrentItem;

	bool m_bIgnoreCase;
	bool m_bUniqueItems;

	int m_nItems;
};



class PStringListItem
{
public:

	PStringListItem(const char *pszString);
	~PStringListItem();

	bool Equals(const char *pszString, bool bIgnoreCase);

	int SetValue(const char *pszString);
	const char* GetValue() { return m_pszString; }


protected:

	friend class PStringList;

	int m_nBufferSize;
	char *m_pszString;

	PStringListItem *m_pNextItem;
};


#endif		// #ifndef _PSTRINGLIST_H_
