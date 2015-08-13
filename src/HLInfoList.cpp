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


#include "HLInfoList.h"



// ======================================================================

HLInfoList::HLInfoList()
{
	m_pFirstItem = NULL;
	m_pLastItem  = NULL;

	m_nItems = 0;
}


// ======================================================================

HLInfoList::~HLInfoList()
{
	RemoveAllItems();
}


// ======================================================================

int HLInfoList::RemoveAllItems()
{
	int nCount;

	HLInfoListItem *pCurrent = NULL;
	HLInfoListItem *pNext    = NULL;

	// remove all elements
	pNext = m_pFirstItem;
	for(nCount = 0; pNext != NULL; nCount++)
	{
		pCurrent = pNext;
		pNext = pNext->m_pNextItem;

		delete pCurrent;
	}

	// update pointers & counter
	m_pFirstItem = NULL;
	m_pLastItem  = NULL;
	m_nItems     = 0;

	// return number of items deleted
	return nCount;
}


// ======================================================================

int HLInfoList::RemoveAllUnusedItems()
{
	HLInfoListItem *pNext     = NULL;
	HLInfoListItem *pCurrent  = NULL;
	HLInfoListItem *pPrevious = NULL;

	// check all items in list
	int nDeleted = 0;
	pNext = m_pFirstItem;

	while(pNext != NULL)
	{
		pCurrent = pNext;
		pNext = pNext->m_pNextItem;

		if(pCurrent->m_nReferenceCounter == 0)
		{
			// remove unused item
			if(pCurrent->m_pNextItem == NULL)
				m_pLastItem = pPrevious;

			if(pPrevious == NULL)
				m_pFirstItem = pCurrent->m_pNextItem;
			else
				pPrevious->m_pNextItem = pCurrent->m_pNextItem;

			// delete current
			delete pCurrent;

			m_nItems--;
			nDeleted++;
		}
		else
		{
			// only change pPrevious, if we did not delete the current item
			pPrevious = pCurrent;
		}
	}

	return nDeleted;
}


// ======================================================================

HLInfoListItem* HLInfoList::AddItem(const char *pszInfo)
{
	// TODO: insert alphanumerically sorted (speeds up searching)
	HLInfoListItem *pNewItem;

	if(pszInfo == NULL)
		return NULL;

	pNewItem = new HLInfoListItem(pszInfo);

	if(pNewItem == NULL)
		return NULL;

	// add item to end of list
	if(m_pFirstItem == NULL)
		m_pFirstItem = pNewItem;
	else if(m_pLastItem != NULL)
		m_pLastItem->m_pNextItem = pNewItem;

	m_pLastItem  = pNewItem;

	// increment number of elements
	m_nItems++;

	// return the new item
	return pNewItem;
}


// ======================================================================

HLInfoListItem* HLInfoList::GetItem(const char *pszInfo)
{
	HLInfoListItem *pTemp;

	for(pTemp = m_pFirstItem; pTemp != NULL; pTemp = pTemp->m_pNextItem)
	{
		if(pTemp->Equals(pszInfo) == true)
			return pTemp;
	}

	return NULL;
}






// ======================================================================

HLInfoListItem::HLInfoListItem(const char *pszInfo)
{
	if(pszInfo != NULL)
	{
		// copy string
		int nCount;

		for(nCount = 0; (nCount < (int) sizeof(m_szInfoString) - 1) && pszInfo[nCount] != '\0'; nCount++)
			m_szInfoString[nCount] = pszInfo[nCount];

		m_szInfoString[nCount] = '\0';
	}
	else
	{
		m_szInfoString[0] = 'n';
		m_szInfoString[1] = 'u';
		m_szInfoString[2] = 'l';
		m_szInfoString[3] = 'l';
		m_szInfoString[4] = '\0';
	}

	// initialize
	m_nReferenceCounter = 0;
	m_pListIndex        = NULL;
	m_pNextItem         = NULL;
}
