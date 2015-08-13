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


#include "PStringList.h"




// ======================================================================

PStringList::PStringList()
{
	m_bIgnoreCase  = false;
	m_bUniqueItems = true;

	m_nItems       = 0;
	m_pFirstItem   = NULL;
	m_pLastItem    = NULL;
	m_pCurrentItem = NULL;
}


// ======================================================================

PStringList::PStringList(bool bIgnoreCase, bool bUniqueItems)
{
	m_bIgnoreCase  = bIgnoreCase;
	m_bUniqueItems = bUniqueItems;

	m_nItems       = 0;
	m_pFirstItem   = NULL;
	m_pLastItem    = NULL;
	m_pCurrentItem = NULL;
}


// ======================================================================

PStringList::~PStringList()
{
	RemoveAllItems();
}


// ======================================================================

bool PStringList::AddItem(const char *pszString)
{
	if(pszString == NULL)
		return false;

	if(m_bUniqueItems == true && FindItem(pszString) == true)
		return true;

	PStringListItem *pNewItem = new PStringListItem(pszString);

	// add item to end of list
	if(m_pFirstItem == NULL)
		m_pFirstItem = pNewItem;
	else if(m_pLastItem != NULL)
		m_pLastItem->m_pNextItem = pNewItem;

	m_pLastItem  = pNewItem;

	if(m_nItems == 0)
		m_pCurrentItem = pNewItem;

	// increment number of elements
	m_nItems++;

	return true;
}


// ======================================================================

bool PStringList::RemoveItem(const char *pszString)
{
	PStringListItem *pCurrent  = NULL;
	PStringListItem *pPrevious = NULL;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->Equals(pszString, m_bIgnoreCase) == true)
		{
			if(pCurrent->m_pNextItem == NULL)
				m_pLastItem = pPrevious;

			if(pPrevious == NULL)
				m_pFirstItem = pCurrent->m_pNextItem;
			else
				pPrevious->m_pNextItem = pCurrent->m_pNextItem;

			if(m_pCurrentItem == pCurrent)
				m_pCurrentItem = pCurrent->m_pNextItem;

			delete pCurrent;

			m_nItems--;

			// success
			return true;
		}

		pPrevious = pCurrent;
	}

	return false;
}


// ======================================================================

int PStringList::RemoveAllItems()
{
	int nCount;

	PStringListItem *pCurrent = NULL;
	PStringListItem *pNext    = NULL;

	// remove all elements
	pNext = m_pFirstItem;
	for(nCount = 0; pNext != NULL; nCount++)
	{
		pCurrent = pNext;
		pNext = pNext->m_pNextItem;

		delete pCurrent;
	}

	// update pointers & counter
	m_pFirstItem   = NULL;
	m_pLastItem    = NULL;
	m_pCurrentItem = NULL;
	m_nItems       = 0;

	// return number of items deleted
	return nCount;
}


// ======================================================================

bool PStringList::FindItem(const char *pszString)
{
	PStringListItem *pItem  = NULL;

	for(pItem = m_pFirstItem; pItem != NULL; pItem = pItem->m_pNextItem)
	{
		if(pItem->Equals(pszString, m_bIgnoreCase) == true)
			return true;
	}

	// not found
	return false;
}


// ======================================================================

bool PStringList::FindItem(const char *pszString, bool bIgnoreCase)
{
	PStringListItem *pItem  = NULL;

	for(pItem = m_pFirstItem; pItem != NULL; pItem = pItem->m_pNextItem)
	{
		if(pItem->Equals(pszString, bIgnoreCase) == true)
			return true;
	}

	// not found
	return false;
}


// ======================================================================

bool PStringList::PickFirst()
{
	if(m_pCurrentItem == NULL)
		return false;

	m_pCurrentItem = m_pFirstItem;

	if(m_pCurrentItem != NULL)
		return true;
	else
		return false;
}


// ======================================================================

bool PStringList::PickNext()
{
	if(m_pCurrentItem == NULL)
		return false;

	m_pCurrentItem = m_pCurrentItem->m_pNextItem;

	if(m_pCurrentItem != NULL)
		return true;
	else
		return false;
}


// ======================================================================

bool PStringList::IsCurrentEqual(const char *pszString)
{
	if(m_pCurrentItem == NULL)
		return false;
	else
		return m_pCurrentItem->Equals(pszString, m_bIgnoreCase);
}


// ======================================================================

const char* PStringList::GetCurrent()
{
	if(m_pCurrentItem != NULL)
		return m_pCurrentItem->m_pszString;
	else
		return NULL;
}











// ======================================================================

PStringListItem::PStringListItem(const char *pszString)
{
	m_pNextItem   = NULL;
	m_nBufferSize = 0;

	SetValue(pszString);
}


// ======================================================================

PStringListItem::~PStringListItem()
{
	delete[] m_pszString;
}





// ======================================================================


int PStringListItem::SetValue(const char *pszString)
{
	int nCount;

	// get string length
	for(nCount = 0; pszString[nCount] != '\0'; nCount++)
		; /* do nothing */

	if((nCount + 1) > m_nBufferSize)
	{
		if(m_nBufferSize != 0)
			delete[] m_pszString;

		m_nBufferSize = nCount + 1;

		// allocate some fresh'n fruity memory
		m_pszString = new char[ m_nBufferSize ];
	}

	// copy string
	for(nCount = 0; pszString[nCount] != '\0'; nCount++)
		m_pszString[nCount] = pszString[nCount];
	m_pszString[nCount] = '\0';

	return nCount;
}


// ======================================================================

bool PStringListItem::Equals(const char *pszString, bool bIgnoreCase)
{
	if(pszString == NULL)
		return false;

	int nCount;

	// TODO: improve performance
	if(bIgnoreCase == true)
	{
		for(nCount = 0; pszString[nCount] != '\0' && m_pszString[nCount] != '\0'; nCount++)
		{
			if(tolower(pszString[nCount] != tolower(m_pszString[nCount])))
				return false;
		}

		if(pszString[nCount] == '\0' && m_pszString[nCount] == '\0')
			return true;
		else
			return false;
	}
	else
	{
		if(strcmp(m_pszString, pszString) == 0)
			return true;
		else
			return false;
	}
}




