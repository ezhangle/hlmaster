/*
   +--------------------------------------------------------------------+
   | ..........................:: HLMaster ::.......................... |
   +--------------------------------------------------------------------+
   | Copyright (C) 2001 Silvan Minghetti                                |
   |                                                                    |
   | This program is free software; you can redistribute it and/or      |
   | modify it under the terms of the GNU General Public License        |
   | as published by the Free Software Foundation; either version 2     |
   | of the License, or (at your option) any later version.             |
   |                                                                    |
   | This program is distributed in the hope that it will be useful,    |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of     |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      |
   | GNU General Public License for more details.                       |
   |                                                                    |
   | You should have received a copy of the GNU General Public License  |
   | along with this program; if not, write to the Free Software        |
   | Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA          |
   | 02111-1307, USA.                                                   |
   |                                                                    |
   +--------------------------------------------------------------------+
   | Created.......: Friday, 2. November 2001                           |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Half-Life master-server deamon.                    |
   +--------------------------------------------------------------------+
*/


#ifndef _HLINFOLIST_H_
#define _HLINFOLIST_H_


// Classes defined in this document
// ---------------------------------
// 
class HLInfoList;
class HLInfoListItem;



// Header includes
// ---------------------------------
//
#include "HLServerList.h"


// Class definition
// ---------------------------------
//
class HLInfoList
{
public:

	HLInfoList();
	~HLInfoList();

	int RemoveAllItems();
	int RemoveAllUnusedItems();

	int GetNumItems() const { return m_nItems; }

	HLInfoListItem* AddItem(const char *pszInfo);
	HLInfoListItem* GetItem(const char *pszInfo);


private:

	int m_nItems;

	HLInfoListItem *m_pFirstItem;
	HLInfoListItem *m_pLastItem;
};




class HLInfoListItem
{
public:

	HLInfoListItem(const char *pszInfo);

	int  GetUsage() const { return m_nReferenceCounter; }
	void IncrementUsage() { m_nReferenceCounter++; }
	void DecrementUsage()
	{
		if(m_nReferenceCounter == 0)
			return;

		m_nReferenceCounter--;

		if(m_nReferenceCounter == 0)
			m_pListIndex = NULL;
	}


	bool Equals(const char *pszInfo)
	{
		if(pszInfo == NULL)
			return false;

		// compare strings (case sensitive)
		char *pszString1 = m_szInfoString;
		char *pszString2 = (char*) pszInfo;

		while((*(pszString1) == *(pszString2)) && (*(pszString1) != '\0'))
		{
			pszString1++;
			pszString2++;
		}

		if(*(pszString1) == '\0' && *(pszString2) == '\0')
			return true;

		return false;
	}

private:

	friend class HLInfoList;
	friend class HLServerList;

	char m_szInfoString[33];
	int  m_nReferenceCounter;

	HLServerListItem *m_pListIndex;		// index to the first listitem with this info

	HLInfoListItem *m_pNextItem;
};




#endif		// #ifndef _HLINFOLIST_H_
