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


#include "HLServerList.h"
#include "HLMaster.h"
#include "CInetAddress.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>



// ======================================================================

HLServerList::HLServerList(int nMaxItems, PLogfile *pInfoLog)
{
	if(nMaxItems > 0 && nMaxItems <= 60000)
		m_nMaxItems  = nMaxItems;
	else
		m_nMaxItems = 200;

	m_pInfoLog = pInfoLog;

	// TODO: if memory allocation fails, this will lead to a very nice error
	m_pGameList = new HLInfoList();
	m_pMapList  = new HLInfoList();

	m_pHeadDummyItem = new HLServerListItem(0xFFFFFFFF, 0xFFFF);
	m_pTailDummyItem = new HLServerListItem(0xFFFFFFFF, 0xFFFF);

	m_pHeadDummyItem->m_pNextItem     = m_pTailDummyItem;
	m_pHeadDummyItem->m_pPreviousItem = m_pTailDummyItem;

	m_pTailDummyItem->m_pNextItem     = m_pHeadDummyItem;
	m_pTailDummyItem->m_pPreviousItem = m_pHeadDummyItem;

	m_pFirstItem    = m_pHeadDummyItem;
	m_pLastItem     = m_pTailDummyItem;

	m_pRequestIndex = m_pHeadDummyItem;
	m_pUnknownIndex = m_pTailDummyItem;

	m_nItems        = 0;
	m_nSkipRequests = 0;

	// initialize challenge number (should be random)
	m_nChallengeSeed    = (int) &m_pHeadDummyItem;
}


// ======================================================================

HLServerList::~HLServerList()
{
	RemoveAllItems();

	delete m_pGameList;
	delete m_pMapList;

	delete m_pHeadDummyItem;
	delete m_pTailDummyItem;
}



// ======================================================================

HLServerListItem* HLServerList::AddItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	HLServerListItem *pNewItem;

	if(m_nItems >= m_nMaxItems)
		return NULL;

	if(nIpAddress != 0 && nPort != 0)
	{
		pNewItem = new HLServerListItem(nIpAddress, nPort);

		if(pNewItem == NULL)
			return NULL;

		// check index
		if(m_pRequestIndex == m_pUnknownIndex)
			m_pRequestIndex = pNewItem;

		// add item as first unknown item
		pNewItem->m_pPreviousItem = m_pUnknownIndex->m_pPreviousItem;
		pNewItem->m_pNextItem     = m_pUnknownIndex;

		m_pUnknownIndex->m_pPreviousItem->m_pNextItem = pNewItem;
		m_pUnknownIndex->m_pPreviousItem              = pNewItem;

		m_pUnknownIndex = pNewItem;

		// increment number of elements
		m_nItems++;

		// return the new item
		return pNewItem;
	}

	return NULL;
}


// ======================================================================

bool HLServerList::RemoveItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	HLServerListItem *pItem = Find(nIpAddress, nPort);

	if(pItem == NULL)
		return false;

	RemoveItem(pItem);
	return true;
}


// ======================================================================

int HLServerList::RemoveAllItems()
{
	int nDeleted = 0;
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	// remove all elements
	for(nDeleted = 0; pCurrent != m_pLastItem; nDeleted++)
	{
		// dont let the destructor decrement usage counters
		// we will remove all items anyway
		pCurrent->m_pGame = NULL;
		pCurrent->m_pMap  = NULL;

		// delete current
		pCurrent = pCurrent->m_pNextItem;
		delete pCurrent->m_pPreviousItem;
	}

	m_pRequestIndex = m_pFirstItem;
	m_pUnknownIndex = m_pLastItem;
	m_nItems = 0;

	m_pGameList->RemoveAllItems();
	m_pMapList->RemoveAllItems();

	// return number of items deleted
	// this should match initial m_nItems!
	return nDeleted;
}


// ======================================================================

int HLServerList::RemoveAllStaticItems()
{
	int nDeleted = 0;
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	// check all items in list
	while(pCurrent != m_pLastItem)
	{
		if(pCurrent->m_nType == HLServerListItem::c_TYPE_DYNAMIC)
		{
			pCurrent = pCurrent->m_pNextItem;
			continue;
		}

		// delete current
		pCurrent = pCurrent->m_pNextItem;

		RemoveItem(pCurrent->m_pPreviousItem);
		nDeleted++;
	}


	return nDeleted;
}



// ======================================================================

int HLServerList::RemoveAllDynamicItems()
{
	int nDeleted = 0;
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	// check all items in list
	while(pCurrent != m_pLastItem)
	{
		if(pCurrent->m_nType != HLServerListItem::c_TYPE_DYNAMIC)
		{
			pCurrent = pCurrent->m_pNextItem;
			continue;
		}

		// delete current
		pCurrent = pCurrent->m_pNextItem;

		RemoveItem(pCurrent->m_pPreviousItem);
		nDeleted++;
	}


	return nDeleted;
}


// ======================================================================

void HLServerList::RemoveItem(HLServerListItem *pItem)
{
	// unlink item
	pItem->m_pPreviousItem->m_pNextItem = pItem->m_pNextItem;
	pItem->m_pNextItem->m_pPreviousItem = pItem->m_pPreviousItem;

	// check gametype index
	if(pItem->m_pGame != NULL && pItem->m_pGame->m_pListIndex == pItem && pItem->m_pGame == pItem->m_pNextItem->m_pGame)
	{
		// update the index, if the next item is of the same type
		pItem->m_pGame->m_pListIndex = pItem->m_pNextItem;
	}
	// NOTE: if there is no next item and the item is on the index,
	//       we would actually have to set the index to NULL - but
	//       since the destructor resets the index to NULL, if there
	//       are no more reference, everything should be ok. (unless there
	//       is a bug of course :-)

	// NOTE: maps are currently not indexed

	// check first unknown item index
	if(pItem == m_pUnknownIndex)
		m_pUnknownIndex = pItem->m_pNextItem;

	// check request index
	if(pItem == m_pRequestIndex)
		m_pRequestIndex = pItem->m_pNextItem;

	// delete
	delete pItem;

	m_nItems--;
}


// ======================================================================

void HLServerList::ResortItem(HLServerListItem *pItem)
{
	// check first unknown item index
	if(m_pUnknownIndex == pItem)
		m_pUnknownIndex = pItem->m_pNextItem;

	// check request index
	if(m_pRequestIndex == pItem)
		m_pRequestIndex = pItem->m_pNextItem;

	// unlink
	pItem->m_pPreviousItem->m_pNextItem = pItem->m_pNextItem;
	pItem->m_pNextItem->m_pPreviousItem = pItem->m_pPreviousItem;

	if(pItem->m_pGame->m_pListIndex == NULL)
	{
		// relink at top of the list
		pItem->m_pNextItem     = m_pFirstItem->m_pNextItem;
		pItem->m_pPreviousItem = m_pFirstItem;
	}
	else
	{
		// relink as first indexed item
		pItem->m_pNextItem     = pItem->m_pGame->m_pListIndex;
		pItem->m_pPreviousItem = pItem->m_pGame->m_pListIndex->m_pPreviousItem;
	}

	// update surrounding items
	pItem->m_pNextItem->m_pPreviousItem = pItem;
	pItem->m_pPreviousItem->m_pNextItem = pItem;

	// set index to current item
	pItem->m_pGame->m_pListIndex = pItem;
}


// ======================================================================

HLServerListItem* HLServerList::Find(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	while(pCurrent != m_pLastItem)
	{
		if(pCurrent->m_nIpAddress == nIpAddress && pCurrent->m_nPort == nPort)
			return pCurrent;

		pCurrent = pCurrent->m_pNextItem;
	}

	return NULL;
}


// ======================================================================

int HLServerList::GetRandomChallenge(int nSeed1, int nSeed2)
{
	m_nChallengeSeed  = ((m_nChallengeSeed + 123456) ^ (nSeed1 + nSeed2)) + m_nItems;
	int nNewChallenge = m_nChallengeSeed;

	nNewChallenge <<= 8;
	nNewChallenge  ^= nSeed1;
	nNewChallenge  ^= nSeed2;

	return nNewChallenge;
}


// ======================================================================

int HLServerList::LoadHosts(const char *pszFilename)
{
	if(pszFilename == NULL)
		return -1;

	HLServerListItem *pItem = NULL;

	FILE *pFile;
	char szBuffer[64];
	char *pBuffer;
	bool bLastLineFinished;

	int nCount;
	int nLoaded;
	char nType;
	char szIpAddress[16];
	char szPort[6];

	time_t nTimestamp = time(NULL) - HLMaster::c_CHECK_INTERVAL - 5;


	pfc_inet_addr nIpAddress;
	pfc_inet_port nPort;

	pFile = fopen(pszFilename, "r");
	if(pFile != NULL)
	{
		// remove all static hosts from list, before adding new ones
		RemoveAllStaticItems();


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

			if(strncmp("nocheck", pBuffer, (sizeof("nocheck") - 1)) == 0)
			{
				nType = HLServerListItem::c_TYPE_STATIC_NOCHECK;
				pBuffer += sizeof("nocheck");
			}
			else if(strncmp("check", pBuffer, (sizeof("check") - 1)) == 0)
			{
				nType = HLServerListItem::c_TYPE_STATIC_CHECK;
				pBuffer += sizeof("check");
			}
			else
			{
				// default to "check"
				nType = HLServerListItem::c_TYPE_STATIC_CHECK;
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

			for( /* no init*/ ; (*(pBuffer) != '\0') && ((*(pBuffer) == ' ') || (*(pBuffer) == '\t') || (*(pBuffer) == '\n') || (*(pBuffer) == '\r') || (*(pBuffer) == ':')); pBuffer++)
				; // do nothing, just skip whitespaces, newlines, carriage returns, ':'


			if((*(pBuffer) == '\0') || (*(pBuffer) == '#'))
			{
				// no port specified
				szPort[0] = '\0';
			}
			else
			{
				// read port
				for(nCount = 0; nCount < 5; pBuffer++)
				{
					if(isdigit(*(pBuffer)))
					{
						szPort[nCount] = *(pBuffer);
						nCount++;
					}
					else
						break;
				}
				szPort[nCount] = '\0';
			}

			if(szPort[0] == '\0')
			{
				// no port specified, default to 27015
				strcpy(szPort, "27015");
			}

			// ok, got em all, now check em...
			nIpAddress = inet_addr(szIpAddress);
			nPort      = htons(atoi(szPort));


			if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_DEBUG) == true)
			{
				if(nType == HLServerListItem::c_TYPE_STATIC_CHECK)
					m_pInfoLog->Append("Loaded server (type=check) <");
				else
					m_pInfoLog->Append("Loaded server (type=nocheck) <");

				m_pInfoLog->Append(szIpAddress);
				m_pInfoLog->Append(':');
				m_pInfoLog->Append(szPort);
				m_pInfoLog->Append('>');

				m_pInfoLog->End();
			}

			pItem = Find(nIpAddress, nPort);
			if(pItem == NULL)
			{
				// not found, add
				pItem = AddItem(nIpAddress, nPort);
			}
			else
			{
				if(pItem->m_nType == HLServerListItem::c_TYPE_DYNAMIC && m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
				{
					if(nType == HLServerListItem::c_TYPE_STATIC_CHECK)
						m_pInfoLog->Append("Convert (event=load from=dynamic to=check) <");
					else
						m_pInfoLog->Append("Convert (event=load from=dynamic to=nocheck) <");

					m_pInfoLog->Append(szIpAddress);
					m_pInfoLog->Append(':');
					m_pInfoLog->Append(szPort);
					m_pInfoLog->Append('>');

					m_pInfoLog->End();
				}

				// NOTE: if it's not a dynamic host, but it looks like
				//       it's appearing twice in the config-file.
			}

			if(pItem != NULL)
			{
				pItem->SetType(nType);

				pItem->m_nBadChecks     = 2;
				pItem->m_nLastHeartbeat = nTimestamp;
			}

			// increment number of successfully loaded entries
			nLoaded++;

			// check limit
			if(nLoaded == 3000)
			{
				if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_WARNING) == true)
				{
					m_pInfoLog->Append("Maximum number of loadable servers reached (3000).");
					m_pInfoLog->End();
				}

				break;
			}
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

HLServerListItem* HLServerList::HeartBeat(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp)
{
	HLServerListItem *pItem;

	pItem = Find(nIpAddress, nPort);
	if(pItem == NULL)
	{
		// not found, add
		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
		{
			pfc::CInetAddress tempInetAddr;
			tempInetAddr.SetAddress(nIpAddress);

			m_pInfoLog->Append("ServerInfo (type=newheartbeat) <");
			m_pInfoLog->Append(&tempInetAddr);
			m_pInfoLog->Append(':');
			m_pInfoLog->Append((int)ntohs(nPort));
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}

		pItem = AddItem(nIpAddress, nPort);


		if(pItem != NULL)
		{
			// get a fresh, random id
			pItem->m_nChallengeId = GetRandomChallenge((int)&pItem, (int)nTimestamp);
		}
	}

	if(pItem != NULL)
	{
		// found, update timestamp
		pItem->m_nLastHeartbeat = nTimestamp;

		// increment badchecks
		// if a host does not send back the correct challenge after
		// sending a heartbeat, it's a sux0r host
		pItem->m_nBadChecks++;
	}

	return pItem;
}


// ======================================================================

bool HLServerList::Logoff(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	HLServerListItem *pItem = Find(nIpAddress, nPort);

	if(pItem == NULL || pItem->m_nType != HLServerListItem::c_TYPE_DYNAMIC)
		return false;

	RemoveItem(pItem);

	return true;
}


// ======================================================================

HLServerListItem* HLServerList::Challenge(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp, const int nChallengeId, const char nProtocol, const char nPlayers, const char nMaxPlayers, const char nOsType, const bool bDedicated, const bool bPassword, const bool bLan, const char *pszGameDir, const char *pszMapName)
{
	HLServerListItem *pItem;
	pfc::CInetAddress tempInetAddr;

	pItem = Find(nIpAddress, nPort);
	if(pItem == NULL)
	{
		// not found, invalid host, reject
		return NULL;
	}

	if(pItem->m_nChallengeId != nChallengeId)
	{
		// aiii, bad challenge id, reject
		// NOTE: we could remove the host right now
		return NULL;
	}

	// update host infos
	pItem->m_nLastHeartbeat	= nTimestamp;
	pItem->m_nBadChecks		= 0;

	pItem->m_nProtocol		= nProtocol;
	pItem->m_nPlayers		= nPlayers;
	pItem->m_nMaxPlayers	= nMaxPlayers;
	pItem->m_nOsType		= nOsType;
	pItem->m_bDedicated		= bDedicated;
	pItem->m_bPassword		= bPassword;
	pItem->m_bLan			= bLan;

	SetGameDir(pItem, pszGameDir);
	SetMapName(pItem, pszMapName);


	if(pItem->m_nType == HLServerListItem::c_TYPE_STATIC_CHECK)
	{
		// convert to dynamic host
		pItem->SetType(HLServerListItem::c_TYPE_DYNAMIC);

		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
		{
			m_pInfoLog->Append("Convert (event=challenge from=check to=dynamic) <");

			tempInetAddr.SetAddress(pItem->m_nIpAddress);
			m_pInfoLog->Append(&tempInetAddr);
			m_pInfoLog->Append(':');
			m_pInfoLog->Append((int)ntohs(pItem->m_nPort));
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}
	}

	// successfully updated infos
	return pItem;
}


// ======================================================================

HLServerListItem* HLServerList::UpdateDetails(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp, const char nProtocol, const char nPlayers, const char nMaxPlayers, const char nOsType, const bool bDedicated, const bool bPassword, const bool bLan, const char *pszGameDir, const char *pszMapName)
{
	HLServerListItem *pItem;

	pItem = Find(nIpAddress, nPort);
	if(pItem == NULL)
	{
		// not found, invalid host, reject
		return NULL;
	}

	if(pItem->m_nType != HLServerListItem::c_TYPE_STATIC_CHECK)
	{
		// invalid host type, reject
		return NULL;
	}

	// update host infos
	pItem->m_nLastHeartbeat	= nTimestamp;
	pItem->m_nBadChecks		= 0;

	pItem->m_nProtocol		= nProtocol;
	pItem->m_nPlayers		= nPlayers;
	pItem->m_nMaxPlayers	= nMaxPlayers;
	pItem->m_nOsType		= nOsType;
	pItem->m_bDedicated		= bDedicated;
	pItem->m_bPassword		= bPassword;
	pItem->m_bLan			= bLan;

	SetGameDir(pItem, pszGameDir);
	SetMapName(pItem, pszMapName);


	// successfully updated infos
	return pItem;
}


// ======================================================================

void HLServerList::SetGameDir(HLServerListItem *pItem, const char *pszGameDir)
{
	if(pszGameDir != NULL && pszGameDir[0] != '\0')
	{
		if(pItem->m_pGame != NULL)
		{
			if(pItem->m_pGame->Equals(pszGameDir) == true)
				return;

			// gametype changed
			pItem->m_pGame->DecrementUsage();

			// check index
			if(pItem->m_pGame->m_pListIndex != NULL && pItem->m_pGame == pItem->m_pNextItem->m_pGame)
				pItem->m_pGame->m_pListIndex = pItem->m_pNextItem;
		}

		// update gamedirectory
		pItem->m_pGame = m_pGameList->GetItem(pszGameDir);

		if(pItem->m_pGame == NULL)
			pItem->m_pGame = m_pGameList->AddItem(pszGameDir);

		if(pItem != NULL)
			pItem->m_pGame->IncrementUsage();

		// resort item
		ResortItem(pItem);
	}
	else if(pItem->m_pGame != NULL)
	{
		pItem->m_pGame->DecrementUsage();

		// check index
		if(pItem->m_pGame->m_pListIndex != NULL && pItem->m_pGame == pItem->m_pNextItem->m_pGame)
			pItem->m_pGame->m_pListIndex = pItem->m_pNextItem;

		pItem->m_pGame = NULL;

		// resort list
		ResortItem(pItem);
	}
}


// ======================================================================

void HLServerList::SetMapName(HLServerListItem *pItem, const char *pszMapName)
{
	if(pszMapName != NULL && pszMapName[0] != '\0')
	{
		if(pItem->m_pMap != NULL)
		{
			if(pItem->m_pMap->Equals(pszMapName) == true)
				return;

			// map changed
			pItem->m_pMap->DecrementUsage();
		}

		// update map
		pItem->m_pMap = m_pMapList->GetItem(pszMapName);

		if(pItem->m_pMap == NULL)
			pItem->m_pMap = m_pMapList->AddItem(pszMapName);

		if(pItem != NULL)
			pItem->m_pMap->IncrementUsage();
	}
	else if(pItem->m_pMap != NULL)
	{
		pItem->m_pMap->DecrementUsage();
		pItem->m_pMap = NULL;
	}
}


// ======================================================================

bool HLServerList::RequestStatus(time_t nTimestamp, HLMaster *pMaster)
{
	int nCount;
	const int nLimit = 100;
	HLServerListItem *pCurrent = m_pRequestIndex->m_pNextItem;


	// check at most 100 items
	for(nCount = 0; pCurrent != m_pLastItem && nCount < nLimit; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->m_nType != HLServerListItem::c_TYPE_STATIC_CHECK)
			continue;

		// increment counter only for static items
		nCount++;

		// check timestamp
		if(pCurrent->m_nLastHeartbeat == 0 || pCurrent->m_nLastHeartbeat > nTimestamp)
			pCurrent->m_nLastHeartbeat = nTimestamp;

		if((nTimestamp - pCurrent->m_nLastHeartbeat) < HLMaster::c_CHECK_INTERVAL)
			continue;

		if(pCurrent->m_nBadChecks > 288)		// 1 Day
		{
			// check every 4 hours
			if((pCurrent->m_nBadChecks % 48) == 0)
			{
				pMaster->SendDetailsRequest(pCurrent);
			}
		}
		else if(pCurrent->m_nBadChecks > 6)		// 30 Minutes
		{
			// check every 20 minutes
			if((pCurrent->m_nBadChecks % 4) == 0)
			{
				pMaster->SendDetailsRequest(pCurrent);
			}
		}
		else
		{
			// check now
			pMaster->SendDetailsRequest(pCurrent);
		}

		pCurrent->m_nBadChecks++;
	}


	if(nCount == nLimit && pCurrent != m_pLastItem)
	{
		// limit reached and there are some more
		m_pRequestIndex = pCurrent;
		return false;
	}
	else
	{
		// finished the queue
		m_pRequestIndex = m_pFirstItem;
		return true;
	}
}

// ======================================================================

int HLServerList::CheckTimeout(time_t nTimestamp, HLMaster *pMaster)
{
	int nDeleted  = 0;
	pfc::CInetAddress tempInetAddr;
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	// check all items in list
	while(pCurrent != m_pLastItem)
	{
		// do not check static, nocheck hosts
		// keep static hosts 1 week before deleting them
		if((pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_NOCHECK) || (pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_CHECK && pCurrent->m_nBadChecks < 2016))
		{
			pCurrent = pCurrent->m_pNextItem;
			continue;
		}

		// check timestamp
		if(pCurrent->m_nLastHeartbeat == 0 || pCurrent->m_nLastHeartbeat > nTimestamp)
			pCurrent->m_nLastHeartbeat = nTimestamp;


		// check if we should delete that bastard
		if((nTimestamp - pCurrent->m_nLastHeartbeat) >= HLMaster::c_CHECK_TIMEOUT)
		{
			// remove server (timed out)
			if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
			{
				m_pInfoLog->Append("Timeout (type=");
				if(pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_CHECK)
					m_pInfoLog->Append("check) <");
				else
					m_pInfoLog->Append("dynamic) <");

				tempInetAddr.SetAddress(pCurrent->m_nIpAddress);
				m_pInfoLog->Append(&tempInetAddr);

				m_pInfoLog->Append(':');
				m_pInfoLog->Append(ntohs(pCurrent->m_nPort));
				m_pInfoLog->Append('>');

				m_pInfoLog->End();
			}

			// delete current
			pCurrent = pCurrent->m_pNextItem;

			RemoveItem(pCurrent->m_pPreviousItem);
			nDeleted++;
		}
		else
		{
			pCurrent = pCurrent->m_pNextItem;
		}
	}


	// clean up game/map list
	m_pGameList->RemoveAllUnusedItems();
	m_pMapList->RemoveAllUnusedItems();

	return nDeleted;
}


// ======================================================================

int HLServerList::FillPacket_Quakeworld(char *pBuffer, int nBufferSize)
{
	int nCount;
	int nSize;
	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	unsigned int   *pInt  = (unsigned int*)  pBuffer;
	unsigned short *pShort= (unsigned short*) (pBuffer + 4);


	if(nBufferSize < 1 || m_nItems == 0)
		return 0;

	// create packet
	// send max 230 ip's (gamespy limitation)
	for(nCount = 0, nSize = 0; pCurrent != m_pLastItem && (nSize + 6) < nBufferSize && nCount < 230; pCurrent = pCurrent->m_pNextItem)
	{
		// skip timed out servers
		if(pCurrent->m_nBadChecks > 1 && pCurrent->m_nType != HLServerListItem::c_TYPE_STATIC_NOCHECK)
			continue;

		// copy host to buffer
		*(pInt)   = pCurrent->m_nIpAddress;
		*(pShort) = pCurrent->m_nPort;

		// adjust buffer pointers
		pBuffer += 6;
		pInt   = (unsigned int*)  pBuffer;
		pShort = (unsigned short*) (pBuffer + 4);

		// increment counters
		nCount++;
		nSize += 6;
	}

	return nSize;
}


// ======================================================================

int HLServerList::FillPacket_Halflife(int nSkip, char *pBuffer, int nBufferSize, unsigned short *pnSkipped /* = NULL */, bool *pbHasMore /* = NULL */)
{
	int nSize;
	int nCount;
	int nTotalSkipped;

	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	unsigned int   *pInt  = (unsigned int*)  pBuffer;
	unsigned short *pShort= (unsigned short*) (pBuffer + 4);


	if(nBufferSize < 1 || m_nItems == 0 || nSkip >= m_nItems)
		return 0;

	// skip if necessary
	for(nCount = 0; pCurrent != m_pLastItem && nCount < nSkip; pCurrent = pCurrent->m_pNextItem)
	{
		// do not count timed out servers
		if(pCurrent->m_nBadChecks < 2 || pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_NOCHECK)
			nCount++;
	}
	nTotalSkipped = nCount;


	// copy to buffer
	for(nCount = 0, nSize = 0; pCurrent != m_pLastItem && (nSize + 6) < nBufferSize; pCurrent = pCurrent->m_pNextItem)
	{
		// skip timed out servers
		if(pCurrent->m_nBadChecks > 1 && pCurrent->m_nType != HLServerListItem::c_TYPE_STATIC_NOCHECK)
			continue;

		// copy host to buffer
		*(pInt)   = pCurrent->m_nIpAddress;
		*(pShort) = pCurrent->m_nPort;

		// adjust buffer pointers
		pBuffer += 6;
		pInt   = (unsigned int*)  pBuffer;
		pShort = (unsigned short*) (pBuffer + 4);

		// increment counters
		nCount++;
		nSize += 6;
	}

	// tell, if it was the last item
	if(pbHasMore != NULL)
	{
		if(pCurrent != m_pLastItem && nCount > 0)
			*(pbHasMore) = true;
		else
			*(pbHasMore) = false;
	}

	// tell, how many items got skipped
	if(pnSkipped != NULL)
		*(pnSkipped) = nCount + nTotalSkipped;

	return nSize;
}


// ======================================================================
#ifdef _DEBUG
int HLServerList::LogSnapshot()
{
	int nCount;
	FILE *pFile;

	HLServerListItem *pCurrent = m_pFirstItem->m_pNextItem;

	// open snapshot.log
	pFile = fopen("snapshot.log", "w");

	if(pFile == 0)
		return -1;

	// write header
	fputs("Type               BadChecks  Gametype\tMap\tChallenge-Id\tPlayers\tMay-Players\n", pFile);
	fputs("----------------------------------------------------------------------\n\n", pFile);

	// parse list
	for(nCount = 0; pCurrent != m_pLastItem; pCurrent = pCurrent->m_pNextItem)
	{
		if(pCurrent->m_nType == HLServerListItem::c_TYPE_DYNAMIC)
			fputs("Dynamic            ", pFile);
		else if(pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_CHECK)
			fputs("Static (check)     ", pFile);
		else if(pCurrent->m_nType == HLServerListItem::c_TYPE_STATIC_NOCHECK)
			fputs("Static (nocheck)   ", pFile);
		else
			fputs("<unknown>          ", pFile);

		fprintf(pFile, "%04d       ", pCurrent->m_nBadChecks);

		if(pCurrent->m_pGame != NULL)
			fputs(pCurrent->m_pGame->m_szInfoString, pFile);
		else
			fputs("<unknown>", pFile);

		fputs("\t", pFile);
		if(pCurrent->m_pMap != NULL)
			fputs(pCurrent->m_pMap->m_szInfoString, pFile);
		else
			fputs("<unknown>", pFile);

		fprintf(pFile, "\t%010d\t%d\t%d\n", pCurrent->m_nChallengeId, pCurrent->m_nPlayers, pCurrent->m_nMaxPlayers);
	}


	fclose(pFile);

	return nCount;
}
#endif // _DEBUG




// ======================================================================

HLServerListItem::HLServerListItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	m_nIpAddress     = nIpAddress;
	m_nPort          = nPort;

	m_nChallengeId   = 0;
	m_nLastHeartbeat = 0;
	m_nProtocol      = 0;
	m_nPlayers       = 0;
	m_nMaxPlayers    = 0;

	m_nOsType        = 0;
	m_bDedicated     = false;
	m_bPassword      = false;
	m_bLan           = false;

	m_nType          = c_TYPE_DYNAMIC;
	m_nBadChecks     = 1;

	m_pMap           = NULL;
	m_pGame          = NULL;

	m_pNextItem      = NULL;
	m_pPreviousItem  = NULL;
}

const char HLServerListItem::c_TYPE_DYNAMIC        = 0;
const char HLServerListItem::c_TYPE_STATIC_CHECK   = 1;
const char HLServerListItem::c_TYPE_STATIC_NOCHECK = 2;


// ======================================================================

HLServerListItem::~HLServerListItem()
{
	if(m_pMap != NULL)
		m_pMap->DecrementUsage();
	if(m_pGame != NULL)
		m_pGame->DecrementUsage();
}


// ======================================================================

// TODO: update to use HLInfoList
/*
int HLServerListItem::GetMapName(char *pBuffer, const int nBufferSize)
{
	if(nBufferSize < 1 || m_pMap == NULL)
		return 0;

	int nCount;

	// copy string
	for(nCount = 0; nCount < (nBufferSize - 1) && m_szMapName[nCount] != '\0'; nCount++)
		pBuffer[nCount] = m_szMapName[nCount];

	pBuffer[nCount] = '\0';

	return nCount;
}

// ======================================================================

// TODO: update to use HLInfoList
int HLServerListItem::GetGameDir(char *pBuffer, const int nBufferSize)
{
	if(nBufferSize < 1 || m_pGame == NULL)
		return 0;

	int nCount;

	// copy string
	for(nCount = 0; nCount < (nBufferSize - 1) && m_szGameDir[nCount] != '\0'; nCount++)
		pBuffer[nCount] = m_szGameDir[nCount];

	pBuffer[nCount] = '\0';

	return nCount;
}

*/

