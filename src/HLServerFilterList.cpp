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
   | Created.......: Thursday, 10. May 2001                             |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Half-Life master-server deamon.                    |
   +--------------------------------------------------------------------+
*/



#include "HLServerFilterList.h"
#include "PSocket.h"
#include "CStringBuffer.h"

#include <stdio.h>
#include <string.h>


// ======================================================================

HLServerFilterList::HLServerFilterList()
{
	m_nItems        = 0;
	m_pFirstItem    = NULL;
	m_bOrderAllow   = false;
	m_bDefaultAllow = false;
}

// ======================================================================

HLServerFilterList::~HLServerFilterList()
{
	RemoveAll();
}



// ======================================================================

int HLServerFilterList::Load(const char *pszFilename, PLogfile *pLogfile /* = NULL */)
{
	if(pszFilename == NULL)
		return -1;


	FILE *pFile;
	char szBuffer[64];
	char *pBuffer;
	bool bLastLineFinished;

	int nCount;
	int nLoaded;
	bool bAllow;
	char szIpAddress[16];
	char szNetmask[16];

	pfc_inet_addr nIpAddress;
	pfc_inet_addr nNetmask;


	pFile = fopen(pszFilename, "r");
	if(pFile != NULL)
	{
		// reset filter
		RemoveAll();

		SetOrderDeny();
		SetDefaultDeny();


		nLoaded = 0;
		bLastLineFinished = true;

		while(feof(pFile) == 0)
		{
			if(fgets(szBuffer, sizeof(szBuffer), pFile) == NULL)
				break;

			if(szBuffer[0] == '\0')
			{
				// blank line, this shouldnt happen
				bLastLineFinished = true;
				continue;
			}

			pBuffer = szBuffer + (strlen(szBuffer) - 1);
			if(*(pBuffer) == '\n')
			{
				// skip, we're continuing a long line...
				if(bLastLineFinished == false)
				{
					bLastLineFinished = true;
					continue;
				}
				else
					bLastLineFinished = true;
			}
			else
			{
				// skip, we're continuing a long line...
				if(bLastLineFinished == false)
					continue;
				else
					bLastLineFinished = false;
			}


			for(pBuffer = szBuffer; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r')); pBuffer++)
				; // do nothing, just skip whitespaces, newlines, carriage returns

			// skip comments
			if((*(pBuffer) == '\0') || (*(pBuffer) == '#'))
				continue;

			if(strncmp("deny", pBuffer, (sizeof("deny") - 1)) == 0)
			{
				bAllow = false;
				pBuffer += sizeof("deny");
			}
			else if(strncmp("allow", pBuffer, (sizeof("allow") - 1)) == 0)
			{
				bAllow = true;
				pBuffer += sizeof("allow");
			}
			else if(strncmp("order", pBuffer, (sizeof("order") - 1)) == 0)
			{
				pBuffer += sizeof("order");

				for( /* no init*/ ; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r')); pBuffer++)
					; // do nothing, just skip whitespaces, newlines, carriage returns

				if(strncmp("allow", pBuffer, (sizeof("allow") - 1)) == 0)
				{
					if(pLogfile != NULL)
						pLogfile->WriteLine("Loaded filter rule (order=allow)", PLogfile::c_LEVEL_DEBUG);

					SetOrderAllow();
				}
				else if(strncmp("deny", pBuffer, (sizeof("deny") - 1)) == 0)
				{
					if(pLogfile != NULL)
						pLogfile->WriteLine("Loaded filter rule (order=deny)", PLogfile::c_LEVEL_DEBUG);

					SetOrderDeny();
				}

				continue;
			}
			else if(strncmp("default", pBuffer, (sizeof("default") - 1)) == 0)
			{
				pBuffer += sizeof("default");

				for( /* no init*/ ; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r')); pBuffer++)
					; // do nothing, just skip whitespaces, newlines, carriage returns

				if(strncmp("allow", pBuffer, (sizeof("allow") - 1)) == 0)
				{
					if(pLogfile != NULL)
						pLogfile->WriteLine("Loaded filter rule (default=allow)", PLogfile::c_LEVEL_DEBUG);

					SetDefaultAllow();
				}
				else if(strncmp("deny", pBuffer, (sizeof("deny") - 1)) == 0)
				{
					if(pLogfile != NULL)
						pLogfile->WriteLine("Loaded filter rule (default=deny)", PLogfile::c_LEVEL_DEBUG);

					SetDefaultDeny();
				}

				continue;
			}
			else
			{
				// no default, just error
				continue;
			}


			for( /* no init*/ ; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r')); pBuffer++)
				; // do nothing, just skip whitespaces, newlines, carriage returns

			// skip comments
			if((*(pBuffer) == '\0') || (*(pBuffer) == '#'))
				continue;

			// read ip
			for(nCount = 0; nCount < 15; pBuffer++)
			{
				if(*(pBuffer) == '.' || isdigit(*(pBuffer)))
				{
					szIpAddress[nCount] = *(pBuffer);
					nCount++;
				}
				else
					break;
			}
			szIpAddress[nCount] = '\0';

			// skip bad ip's
			if(szIpAddress[0] == '\0')
				continue;

			for( /* no init*/ ; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r') || (*(pBuffer) == '/')); pBuffer++)
				; // do nothing, just skip whitespaces, newlines, carriage returns, ':'


			if((*(pBuffer) == '\0') || (*(pBuffer) == '#'))
			{
				// no netmask specified
				szNetmask[0] = '\0';
			}
			else
			{
				// read netmask
				for(nCount = 0; nCount < 15; pBuffer++)
				{
					if(*(pBuffer) == '.' || isdigit(*(pBuffer)))
					{
						szNetmask[nCount] = *(pBuffer);
						nCount++;
					}
					else
						break;
				}
				szNetmask[nCount] = '\0';
			}


			// use default netmask, if none supplied
			if(szNetmask[0] == '\0')
				strcpy(szNetmask, "255.255.255.255");

			nIpAddress = inet_addr(szIpAddress);
			nNetmask   = inet_addr(szNetmask);


			if(pLogfile != NULL && pLogfile->Begin(PLogfile::c_LEVEL_DEBUG) == true)
			{
				if(bAllow == true)
					pLogfile->Append("Loaded filter (type=allow) <");
				else
					pLogfile->Append("Loaded filter (type=deny) <");

				pLogfile->Append(szIpAddress);
				pLogfile->Append('/');
				pLogfile->Append(szNetmask);
				pLogfile->Append('>');

				pLogfile->End();
			}

			// add filter
			AddHost(bAllow, nIpAddress, nNetmask);

			// increment number of successfully loaded entries
			nLoaded++;
		}

		fclose(pFile);
		return nLoaded;
	}
	else
	{
		// could not open file
		return -1;
	}
}


// ======================================================================

bool HLServerFilterList::AddHost(bool bAllow, pfc_inet_addr nIpAddress)
{
	return AddHost(bAllow, nIpAddress, 0xFFFFFFFF);
}


// ======================================================================

bool HLServerFilterList::AddDefaultHost(bool bAllow)
{
	return AddHost(bAllow, (pfc_inet_addr) 0x00000000, (pfc_inet_addr) 0x00000000);
}


// ======================================================================

bool HLServerFilterList::AddHost(bool bAllow, const char *pszIpAddress)
{
	return AddHost(bAllow, inet_addr(pszIpAddress), 0xFFFFFFFF);
}


// ======================================================================

bool HLServerFilterList::AddHost(bool bAllow, const char *pszIpAddress, const char *pszNetMask)
{
	return AddHost(bAllow, inet_addr(pszIpAddress), inet_addr(pszNetMask));
}


// ======================================================================

bool HLServerFilterList::AddHost(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask)
{
	// do not allow two identical entries
	if(Exists(bAllow, nIpAddress, nNetMask) == true)
		return true;

	HLServerFilterList_Item *pNewItem = new HLServerFilterList_Item(bAllow, nIpAddress, nNetMask);

	if(pNewItem == NULL)
		return false;

	// add item to top of list
	pNewItem->m_pNextItem = m_pFirstItem;
	m_pFirstItem = pNewItem;

	// increment number of elements
	m_nItems++;

	// success
	return true;
}


// ======================================================================

bool HLServerFilterList::Exists(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask)
{
	// search element, begin at top of list
	HLServerFilterList_Item *pCurrent;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->m_bAllow == bAllow && pCurrent->m_nIpAddress == nIpAddress && pCurrent->m_nNetMask == nNetMask)
			return true;
	}

	// not found
	return false;
}


// ======================================================================

bool HLServerFilterList::RemoveHost(bool bAllow, pfc_inet_addr nIpAddress)
{
	return RemoveHost(bAllow, nIpAddress, 0xFFFFFFFF);
}


// ======================================================================

bool HLServerFilterList::RemoveDefaultHost(bool bAllow)
{
	return RemoveHost(bAllow, (pfc_inet_addr) 0xFFFFFFFF, (pfc_inet_addr) 0xFFFFFFFF);
}


// ======================================================================

bool HLServerFilterList::RemoveHost(bool bAllow, const char *pszIpAddress)
{
	return RemoveHost(bAllow, inet_addr(pszIpAddress), 0xFFFFFFFF);
}


// ======================================================================

bool HLServerFilterList::RemoveHost(bool bAllow, const char *pszIpAddress, const char *pszNetMask)
{
	return RemoveHost(bAllow, inet_addr(pszIpAddress), inet_addr(pszNetMask));
}


// ======================================================================

bool HLServerFilterList::RemoveHost(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask)
{
	HLServerFilterList_Item *pCurrent  = NULL;
	HLServerFilterList_Item *pPrevious = NULL;

	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->m_bAllow == bAllow && pCurrent->m_nIpAddress == nIpAddress && pCurrent->m_nNetMask == nNetMask)
		{
			if(pPrevious == NULL)
				m_pFirstItem = pCurrent->m_pNextItem;
			else
				pPrevious->m_pNextItem = pCurrent->m_pNextItem;

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

int HLServerFilterList::RemoveAll()
{
	int nCount;
	HLServerFilterList_Item *pCurrent;
	HLServerFilterList_Item *pNext;

	// remove all elements
	pNext = m_pFirstItem;
	for(nCount = 0; pNext != NULL; nCount++)
	{
		pCurrent = pNext;
		pNext = pNext->m_pNextItem;

		delete pCurrent;
	}

	// reset pointers & counter
	m_pFirstItem = NULL;
	m_nItems     = 0;

	// return number of items deleted
	return nCount;
}



// ======================================================================

bool HLServerFilterList::IsAllowed(const char *pszIpAddress)
{
	return IsAllowed(inet_addr(pszIpAddress));
}


// ======================================================================

bool HLServerFilterList::IsAllowed(pfc_inet_addr nIpAddress)
{
	// search element, begin at top of list
	HLServerFilterList_Item *pCurrent;
	bool bFound  = false;


	for(pCurrent = m_pFirstItem; pCurrent != NULL; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->m_nMaskedIpAddress == (nIpAddress & pCurrent->m_nNetMask))
		{
			if(pCurrent->m_bAllow == true)
			{
				if(m_bOrderAllow == true)	// order: allow, deny - found allow rule
					return true;
				else
					bFound = true;			// maybe there is a deny rule...
			}
			else
			{
				if(m_bOrderAllow == false)	// order: deny, allow - found deny rule
					return false;
				else
					bFound = true;			// maybe there is an allow rule...
			}
		}
	}


	if(bFound == true)
	{
		if(m_bOrderAllow == true)			// order: allow, deny - no allow rule found, but deny rule was found
			return false;
		else								// order: deny, allow - no deny rule found, but allow rule was found
			return true;
	}
	else
	{
		// no rule found, return default
		return m_bDefaultAllow;
	}
}







// ======================================================================

HLServerFilterList_Item::HLServerFilterList_Item(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask)
{
	m_bAllow     = bAllow;
	m_nIpAddress = nIpAddress;
	m_nNetMask   = nNetMask;

	m_nMaskedIpAddress = nIpAddress & nNetMask;
}




