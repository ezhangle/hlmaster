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

#include "HLMaster.h"



#ifndef _HLSERVERLIST_H_
#define _HLSERVERLIST_H_



// Classes defined in this document
// ---------------------------------
// 
class HLServerList;
class HLServerListItem;



// Header includes
// ---------------------------------
// 
#include "HLServerFilterList.h"
#include "HLInfoList.h"
#include "PLogfile.h"
#include "CStringBuffer.h"

#include <time.h>



// Class definition
// ---------------------------------
//
class HLServerList
{
public:

	HLServerList(int nMaxItems, PLogfile *pInfoLog);
	~HLServerList();

	HLServerListItem* AddItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort);
	bool RemoveItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort);
	int RemoveAllItems();
	int RemoveAllStaticItems();
	int RemoveAllDynamicItems();

	HLServerListItem* Find(pfc_inet_addr nIpAddress, pfc_inet_port nPort);

	int FillPacket_Quakeworld(char *pBuffer, int nBufferSize);
	int FillPacket_Halflife(int nSkip, char *pBuffer, int nBufferSize, unsigned short *pnSkipped = NULL, bool *pbHasMore = NULL);


	int GetNumItems() const { return m_nItems; }
	HLServerListItem* GetFirstItem() const { return m_pFirstItem; }
	HLServerListItem* GetLastItem() const { return m_pLastItem; }

	int LoadHosts(const char *pszFilename);

	int CheckTimeout(time_t nTimestamp, HLMaster *pMaster);
	bool RequestStatus(time_t nTimestamp, HLMaster *pMaster);
	int GetRandomChallenge(int nSeed, int nSeed2);

	HLServerListItem* HeartBeat(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp);
	HLServerListItem* Challenge(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp, const int nChallengeId, const char nProtocol, const char nPlayers, const char nMaxPlayers, const char nOsType, const bool bDedicated, const bool bPassword, const bool bLan, const char *pszGameDir, const char *pszMapName);
	HLServerListItem* UpdateDetails(pfc_inet_addr nIpAddress, pfc_inet_port nPort, time_t nTimestamp, const char nProtocol, const char nPlayers, const char nMaxPlayers, const char nOsType, const bool bDedicated, const bool bPassword, const bool bLan, const char *pszGameDir, const char *pszMapName);
	bool Logoff(pfc_inet_addr nIpAddress, pfc_inet_port nPort);


#ifdef _DEBUG
	int LogSnapshot();
#endif	// _DEBUG

private:

	void SetGameDir(HLServerListItem *pItem, const char *pszGameDir);
	void SetMapName(HLServerListItem *pItem, const char *pszMapName);

	void RemoveItem(HLServerListItem *pItem);
	void ResortItem(HLServerListItem *pItem);


	int m_nMaxItems;
	int m_nItems;
	int m_nSkipRequests;

	int m_nChallengeSeed;

	PLogfile *m_pInfoLog;


	HLInfoList *m_pGameList;
	HLInfoList *m_pMapList;

	HLServerListItem *m_pHeadDummyItem;
	HLServerListItem *m_pTailDummyItem;

	HLServerListItem *m_pFirstItem;
	HLServerListItem *m_pLastItem;

	HLServerListItem *m_pUnknownIndex;
	HLServerListItem *m_pRequestIndex;
};




class HLServerListItem
{
public:

	HLServerListItem(pfc_inet_addr nIpAddress, pfc_inet_port nPort);
	~HLServerListItem();

//	int GetGameDir(char *pBuffer, const int nBufferSize);
//	int GetMapName(char *pBuffer, const int nBufferSize);


	void SetType(char nType)
	{
		if(nType >= c_TYPE_DYNAMIC && nType <= c_TYPE_STATIC_NOCHECK)
			m_nType = nType;
	}

	char GetType() const { return m_nType; }


	static const char c_TYPE_DYNAMIC;
	static const char c_TYPE_STATIC_CHECK;
	static const char c_TYPE_STATIC_NOCHECK;


private:

	friend class HLServerList;
	friend class HLMaster;

	char m_nType;							// dynamic, static or static_nocheck
	unsigned int m_nBadChecks;

	pfc_inet_addr m_nIpAddress;
	pfc_inet_port m_nPort;
	time_t m_nLastHeartbeat;

	// server infos
	int  m_nChallengeId;
	char m_nProtocol;
	char m_nPlayers;
	char m_nMaxPlayers;
	char m_nOsType;
	bool m_bDedicated;
	bool m_bPassword;
	bool m_bLan;

	HLInfoListItem *m_pGame;
	HLInfoListItem *m_pMap;


	HLServerListItem *m_pNextItem;
	HLServerListItem *m_pPreviousItem;
};



#endif		// #ifndef _HLSERVERLIST_H_
