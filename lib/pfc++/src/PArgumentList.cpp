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


#include "PArgumentList.h"

#include <stdio.h>
#include <string.h>




// ======================================================================

PArgumentList::PArgumentList()
{
	m_pFirstItem  = NULL;
	m_pLastItem   = NULL;
	m_pCurrentItem= NULL;
	m_nItems      = 0;
	m_bIgnoreCase = false;

	m_pUnknownArgumentsList  = new PStringList(m_bIgnoreCase, false);
	m_pUnknownValuesList = new PStringList(m_bIgnoreCase, false);
}


// ======================================================================

PArgumentList::PArgumentList(bool bIgnoreCase)
{
	m_pFirstItem  = NULL;
	m_pLastItem   = NULL;
	m_pCurrentItem= NULL;
	m_nItems      = 0;
	m_bIgnoreCase = bIgnoreCase;

	m_pUnknownArgumentsList  = new PStringList(m_bIgnoreCase, false);
	m_pUnknownValuesList = new PStringList(m_bIgnoreCase, false);
}



// ======================================================================

PArgumentList::~PArgumentList()
{
	RemoveAllOptions();

	delete m_pUnknownArgumentsList;
	delete m_pUnknownValuesList;
}



// ======================================================================

int PArgumentList::RemoveAllOptions()
{
	int nCount = 0;
	PArgumentListItem *pNext    = NULL;
	PArgumentListItem *pCurrent = NULL;

	// remove all items, begin at end
	for(pNext = m_pLastItem; pNext != NULL; nCount++)
	{
		pCurrent = pNext;
		pNext = pNext->m_pPrevious;

		delete pCurrent;
	}

	m_pFirstItem   = NULL;
	m_pLastItem    = NULL;
	m_pCurrentItem = NULL;
	m_nItems       = 0;

	// return number of items deleted
	return nCount;
}



// ======================================================================

bool PArgumentList::HasValue(const char cShortOption)
{
	PArgumentListItem *pItem = GetItem(cShortOption);

	if(pItem == NULL)
		return false;
	else if(pItem->m_pszValue != NULL)
		return true;
	else
		return false;
}


// ======================================================================

bool PArgumentList::HasValue(const char *pszLongOption)
{
	PArgumentListItem *pItem = GetItem(pszLongOption);

	if(pItem == NULL)
		return false;
	else if(pItem->m_pszValue != NULL)
		return true;
	else
		return false;
}



// ======================================================================

void PArgumentList::AddOption(const char cShortOption, bool bRequireValue /* = false */, const char *pszDefaultValue /* = NULL */)
{
	AddOption(cShortOption, NULL, bRequireValue, pszDefaultValue);
}


// ======================================================================

void PArgumentList::AddOption(const char *pszLongOption, bool bRequireValue /* = false */, const char *pszDefaultValue /* = NULL */)
{
	AddOption((char) 0, pszLongOption, bRequireValue, pszDefaultValue);
}

// ======================================================================

void PArgumentList::AddOption(const char cShortOption, const char *pszLongOption, bool bRequireValue /* = false */, const char *pszDefaultValue /* = NULL */)
{
	if(cShortOption == 0 && pszLongOption == NULL)
		return;

	PArgumentListItem *pItem = GetItem(cShortOption, pszLongOption);
	if(pItem != NULL)
	{
		// item allready exists
		pItem->SetName(cShortOption);
		pItem->SetName(pszLongOption);

		pItem->m_bRequireValue = bRequireValue;
		pItem->SetDefaultValue(pszDefaultValue);
	}
	else
	{
		// add new item
		pItem = new PArgumentListItem(cShortOption, pszLongOption, bRequireValue, pszDefaultValue);

		// add item to end of list
		if(m_pFirstItem == NULL)
			m_pFirstItem = pItem;

		pItem->m_pPrevious = m_pLastItem;
		pItem->m_pNext     = NULL;

		if(m_pLastItem != NULL)
			m_pLastItem->m_pNext = pItem;

		m_pLastItem = pItem;

		if(m_nItems == 0)
			m_pCurrentItem = pItem;

		// increment number of items
		m_nItems++;
	}
}


// ======================================================================

PArgumentListItem* PArgumentList::GetItem(const char cShortOption)
{
	if(cShortOption == 0)
		return NULL;

	PArgumentListItem *pCurrent;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNext)
	{
		if(pCurrent->Equals(cShortOption, m_bIgnoreCase) == true)
		{
			// item found
			return pCurrent;
		}
	}

	// not found
	return NULL;
}


// ======================================================================

PArgumentListItem* PArgumentList::GetItem(const char *pszLongOption)
{
	if(pszLongOption == NULL)
		return NULL;

	PArgumentListItem *pCurrent;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNext)
	{
		if(pCurrent->Equals(pszLongOption, m_bIgnoreCase) == true)
		{
			// item found
			return pCurrent;
		}
	}

	// not found
	return NULL;
}


// ======================================================================

PArgumentListItem* PArgumentList::GetItem(const char cShortOption, const char *pszLongOption)
{
	if(cShortOption == 0 && pszLongOption == NULL)
		return NULL;

	PArgumentListItem *pCurrent;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNext)
	{
		if((pCurrent->Equals(cShortOption, m_bIgnoreCase) == true) || (pCurrent->Equals(pszLongOption, m_bIgnoreCase) == true))
		{
			// item found
			return pCurrent;
		}
	}

	// not found
	return NULL;
}


// ======================================================================

bool PArgumentList::Enable(const char cShortOption, bool bEnable /* = true */)
{
	PArgumentListItem *pItem = GetItem(cShortOption);

	if(pItem == NULL)
		return false;

	pItem->Enable(bEnable);

	return true;
}


// ======================================================================

bool PArgumentList::Enable(const char *pszLongOption, bool bEnable /* = true */)
{
	PArgumentListItem *pItem = GetItem(pszLongOption);

	if(pItem == NULL)
		return false;

	pItem->Enable(bEnable);

	return true;
}


// ======================================================================

const char* PArgumentList::GetValue(const char cShortOption, bool bReturnDefault /* = true */)
{
	PArgumentListItem *pItem = GetItem(cShortOption);

	if(pItem == NULL)
		return NULL;

	if(pItem->m_pszValue != NULL)
		return pItem->m_pszValue;
	else if(bReturnDefault == true)
		return pItem->m_pszDefaultValue;
	else
		return NULL;
}


// ======================================================================

const char* PArgumentList::GetDefaultValue(const char cShortOption)
{
	PArgumentListItem *pItem = GetItem(cShortOption);

	if(pItem == NULL)
		return NULL;

	return pItem->m_pszDefaultValue;
}


// ======================================================================

const char* PArgumentList::GetDefaultValue(const char *pszLongOption)
{
	PArgumentListItem *pItem = GetItem(*pszLongOption);

	if(pItem == NULL)
		return NULL;

	return pItem->m_pszDefaultValue;
}


// ======================================================================

const char* PArgumentList::GetValue(const char *pszLongOption, bool bReturnDefault /* = true */)
{
	PArgumentListItem *pItem = GetItem(pszLongOption);

	if(pItem == NULL)
		return NULL;

	if(pItem->m_pszValue != NULL)
		return pItem->m_pszValue;
	else if(bReturnDefault == true)
		return pItem->m_pszDefaultValue;
	else
		return NULL;
}


// ======================================================================

bool PArgumentList::IsEnabled(const char cShortOption)
{
	PArgumentListItem *pItem = GetItem(cShortOption);

	if(pItem == NULL)
		return false;
	else
		return pItem->m_bIsEnabled;
}


// ======================================================================

bool PArgumentList::IsEnabled(const char *pszLongOption)
{
	PArgumentListItem *pItem = GetItem(pszLongOption);

	if(pItem == NULL)
		return false;
	else
		return pItem->m_bIsEnabled;
}


// ======================================================================

int PArgumentList::ParseArguments(const char *pszArguments[], int nArguments)
{
	if(nArguments <= 0)
		return 0;

	int nCount;
	int nCharCount;
	int nGoodArguments = 0;
	char *pszValue = NULL;
	char szSmallArgBuffer[3] = { '\0' };

	PArgumentListItem *pLastRequireArg = NULL;
	PArgumentListItem *pItem           = NULL;

	m_pCurrentItem = m_pFirstItem;

	// TODO: improve performance, use pointers instead of array counters...
	// TODO: strip '"' from values

	for(nCount = 0; nCount < nArguments; nCount++)
	{
		if(pszArguments[nCount] == NULL)
			break;

		if(pszArguments[nCount][0] == '-' && pszArguments[nCount][1] == '-')
		{
			// long type argument
			pLastRequireArg = NULL;

			// check for '='
			for(pszValue = (char*)&pszArguments[nCount][2]; (*(pszValue) != '\0') && (*(pszValue) != '='); pszValue++)
				;	/* do nothing */

			// found a value separated by '='?
			if(*(pszValue) == '\0')
			{
				// no value found
				pszValue = NULL;
			}
			else if(*(pszValue) == '=')
			{
				// split argument/value
				*(pszValue) = '\0';
				pszValue++;
			}


			pItem = GetItem(&pszArguments[nCount][2]);

			if(pItem != NULL)
			{
				if(pszValue != NULL)
				{
					// found a value, save it
					pItem->Enable();

					if(pItem->m_bRequireValue == true)
						pItem->SetValue(pszValue);
					else
					{
						// this argument does not require a value
						m_pUnknownValuesList->AddItem(pszValue);
					}

					// restore our splitted argument/value
					*(pszValue - 1) = '=';
				}
				else
				{
					// no value, enable it, if it does not require a value
					if(pItem->m_bRequireValue == true)
						pLastRequireArg = pItem;
					else
						pItem->Enable();
				}
			}
			else
			{
				// argument not found, add to list: unknown argument
				// TODO: add to list
				m_pUnknownArgumentsList->AddItem(pszArguments[nCount]);

				// restore our splitted argument/value
				if(pszValue != NULL)
					*(pszValue - 1) = '=';
			}
		}
		else if(pszArguments[nCount][0] == '-')
		{
			// short type argument(s)
			pLastRequireArg = NULL;

			// parse string
			for(nCharCount = 1; pszArguments[nCount][nCharCount] != '\0'; nCharCount++)
			{
				if(pszArguments[nCount][nCharCount] == '=')
				{
					// found a value, save it
					if(pLastRequireArg != NULL)
					{
						if(pLastRequireArg->m_bRequireValue == true)
							pLastRequireArg->SetValue(&pszArguments[nCount][nCharCount + 1]);
						else
						{
							// this argument does not require a value
							m_pUnknownValuesList->AddItem(&pszArguments[nCount][nCharCount + 1]);
						}

						pLastRequireArg = NULL;
					}
					else
					{
						// no argument takes this value
						m_pUnknownValuesList->AddItem(&pszArguments[nCount][nCharCount + 1]);
					}

					// break out, string parsed
					break;
				}
				else
				{
					pItem = GetItem(pszArguments[nCount][nCharCount]);
					if(pItem == NULL)
					{
						// add to list of unknown arguments
						pLastRequireArg = NULL;

						szSmallArgBuffer[0] = '-';
						szSmallArgBuffer[1] = pszArguments[nCount][nCharCount];
						szSmallArgBuffer[2] = '\0';

						m_pUnknownArgumentsList->AddItem(szSmallArgBuffer);
					}
					else
					{
						if(pItem->m_bRequireValue != true)
						{
							pLastRequireArg = NULL;
							pItem->Enable();
						}
						else
							pLastRequireArg = pItem;
					}
				}
			}
		}
		else
		{
			if(pLastRequireArg != NULL)
			{
				// found a value for an argument
				pLastRequireArg->Enable();
				pLastRequireArg->SetValue(pszArguments[nCount]);
			}
			else
			{
				// does not belong to any argument
				m_pUnknownValuesList->AddItem(pszArguments[nCount]);
			}

			pLastRequireArg = NULL;
		}
	}


	// return number of good arguments found
	return nGoodArguments;
}


// ======================================================================

bool PArgumentList::PickCurrentFirst()
{
	m_pCurrentItem = m_pFirstItem;

	if(m_pCurrentItem != NULL)
		return true;
	else
		return false;
}


// ======================================================================

bool PArgumentList::PickCurrentLast()
{
	m_pCurrentItem = m_pLastItem;

	if(m_pCurrentItem != NULL)
		return true;
	else
		return false;
}


// ======================================================================

bool PArgumentList::PickCurrentNext()
{
	if(m_pCurrentItem != NULL)
	{
		m_pCurrentItem = m_pCurrentItem->m_pNext;
		return true;
	}
	else
		return false;
}


// ======================================================================

bool PArgumentList::PickCurrentPrevious()
{
	if(m_pCurrentItem != NULL)
	{
		m_pCurrentItem = m_pCurrentItem->m_pPrevious;
		return true;
	}
	else
		return false;
}


// ======================================================================
bool PArgumentList::IsCurrentEqual(const char cShortOption)
{
	if(m_pCurrentItem == NULL)
		return false;
	else
		return m_pCurrentItem->Equals(cShortOption, m_bIgnoreCase);
}


// ======================================================================
bool PArgumentList::IsCurrentEqual(const char *pszLongOption)
{
	if(m_pCurrentItem == NULL)
		return false;
	else
		return m_pCurrentItem->Equals(pszLongOption, m_bIgnoreCase);
}


// ======================================================================
bool PArgumentList::EnableCurrent(bool bEnable /* = true */)
{
	if(m_pCurrentItem == NULL)
		return false;

	m_pCurrentItem->Enable(bEnable);

	return true;
}


// ======================================================================
bool PArgumentList::IsCurrentValid()
{
	if(m_pCurrentItem == NULL)
		return false;
	else
		return true;
}


// ======================================================================
bool PArgumentList::IsCurrentEnabled()
{
	if(m_pCurrentItem == NULL)
		return false;
	else
		return m_pCurrentItem->m_bIsEnabled;
}


// ======================================================================
bool PArgumentList::HasCurrentValue()
{
	if(m_pCurrentItem == NULL)
		return false;

	if(m_pCurrentItem->m_pszValue == NULL)
		return false;
	else
		return true;
}


// ======================================================================
const char* PArgumentList::GetCurrentValue(bool bReturnDefault /* = true */)
{
	if(m_pCurrentItem == NULL)
		return NULL;

	if(m_pCurrentItem->m_pszValue != NULL)
		return m_pCurrentItem->m_pszValue;
	else if(bReturnDefault == true)
		return m_pCurrentItem->m_pszDefaultValue;
	else
		return NULL;
}


// ======================================================================
const char* PArgumentList::GetCurrentDefaultValue()
{
	if(m_pCurrentItem == NULL)
		return NULL;

	return m_pCurrentItem->m_pszDefaultValue;
}











// ======================================================================

PArgumentListItem::PArgumentListItem(const char cShortOption, const char *pszLongOption, bool bRequireValue, const char *pszDefaultValue)
{
	m_bRequireValue   = bRequireValue;
	m_cShortName      = cShortOption;
	m_pszLongName     = NULL;
	m_pszValue        = NULL;
	m_pszDefaultValue = NULL;
	m_bIsEnabled      = false;

	SetName(pszLongOption);
	SetDefaultValue(pszDefaultValue);

}

const char* PArgumentListItem::m_pszVoidValue = "";



// ======================================================================

PArgumentListItem::~PArgumentListItem()
{
	if(m_pszLongName != NULL)
		delete[] m_pszLongName;

	if(m_pszValue != NULL)
		delete[] m_pszValue;

	if(m_pszDefaultValue != NULL && m_pszDefaultValue != m_pszVoidValue)
		delete[] m_pszDefaultValue;
}


// ======================================================================

void PArgumentListItem::SetName(const char *pszLongOption)
{
	// copy long option name
	if(pszLongOption == NULL)
	{
		// free memory, if necessary
		if(m_pszLongName != NULL)
			delete[] m_pszLongName;

		m_pszLongName = NULL;
	}
	else
	{
		int nCount;

		// get string length
		for(nCount = 0; pszLongOption[nCount] != '\0'; nCount++)
			; /* do nothing */

		// allocate memory
		m_pszLongName = new char[ (nCount + 1) ];

		// copy string
		for(nCount = 0; pszLongOption[nCount] != '\0'; nCount++)
			m_pszLongName[nCount] = pszLongOption[nCount];
		m_pszLongName[nCount] = '\0';
	}
}


// ======================================================================

void PArgumentListItem::SetValue(const char *pszValue)
{
	if(pszValue == NULL)
	{
		// free memory, if necessary
		if(m_pszValue != NULL)
			delete[] m_pszValue;

		m_pszValue = NULL;
	}
	else
	{
		int nCount;

		// get string length
		for(nCount = 0; pszValue[nCount] != '\0'; nCount++)
			; /* do nothing */

		// allocate memory
		m_pszValue = new char[ (nCount + 1) ];

		// copy string
		for(nCount = 0; pszValue[nCount] != '\0'; nCount++)
			m_pszValue[nCount] = pszValue[nCount];
		m_pszValue[nCount] = '\0';
	}
}


// ======================================================================

void PArgumentListItem::SetDefaultValue(const char *pszDefaultValue)
{
	if(pszDefaultValue == NULL)
	{
		// free memory, if necessary
		if(m_pszDefaultValue != NULL && m_pszDefaultValue != m_pszVoidValue)
			delete[] m_pszDefaultValue;

		// the defaul value may not be NULL
		// now it points to a value of ""
		m_pszDefaultValue = (char*)m_pszVoidValue;
	}
	else
	{
		int nCount;

		// get string length
		for(nCount = 0; pszDefaultValue[nCount] != '\0'; nCount++)
			; /* do nothing */

		// allocate memory
		m_pszDefaultValue = new char[ (nCount + 1) ];

		// copy string
		for(nCount = 0; pszDefaultValue[nCount] != '\0'; nCount++)
			m_pszDefaultValue[nCount] = pszDefaultValue[nCount];
		m_pszDefaultValue[nCount] = '\0';
	}
}




