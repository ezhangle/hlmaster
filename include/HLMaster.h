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


#ifndef _HLMASTER_H_
#define _HLMASTER_H_



// Classes defined in this document
// ---------------------------------
// 
class HLMaster;



// Header includes
// ---------------------------------
//
#include "PUDPSocket.h"
#include "HLServerList.h"
#include "HLServerFilterList.h"
#include "PLogfile.h"
#include "PString.h"
#include "CStringBuffer.h"


#include <time.h>


// Class definition
// ---------------------------------
//
class HLMaster
{
public:

	HLMaster(const char *pszIpAddress, unsigned short nPort, int nMaxHosts, PLogfile *pInfoLog, PLogfile *pAccessLog);
	~HLMaster();


	void SetServerId(int nId) { m_nServerId = nId; }
	int  GetServerId() { return m_nServerId; }

	void SetServerName(const char *pszName);
	int  GetServerName(char *pszBuffer, int nBufferSize);

	bool WaitForInput();
	bool ReadInput();
	void ProcessInput()
	{
		time_t nTimeout = time(NULL);
		ProcessInput(nTimeout);
	}
	void ProcessInput(time_t nTimestamp);


	inline time_t GetIntervalRemaining(time_t nTimestamp);
	inline time_t GetRequestIntervalRemaining(time_t nTimestamp);

	void SendDetailsRequest(HLServerListItem *pItem);

	void EnableHostFilter();
	void DisableHostFilter();
	bool LoadHostFilter(const char *pszFilename);
	bool ReloadHostFilter();
	HLServerFilterList* GetHostFilter() const { return m_pHostFilter; }

	bool LoadHosts(const char *pszFilename);
	bool ReloadHosts();


	const PFC_FD GetSocketDescriptor() const { return m_pServerSocket->GetDescriptor(); }
	const int GetNumHosts() const { return m_pServerList->GetNumItems(); }


	static const time_t c_CHECK_TIMEOUT;
	static const time_t c_CHECK_INTERVAL;
	static const time_t c_REQUEST_INTERVAL;


protected:

	inline void ProcessHeartbeat();
	inline void ProcessChallenge();
	inline void ProcessLogoff();

	inline void ProcessDetailsReply();

	inline void ProcessAuthServerListRequest();
	inline void ProcessListRequest();
	inline void ProcessEnhancedListRequest();
	inline void ProcessPingRequest();
	inline void ProcessNameRequest();

	inline void ProcessInvalidQuery();

	bool ParseChallengeString(const char *pszChallenge, const int nChallengeSize);
	bool ParseDetailsString(const char *pszDetails, const int nDetailsSize);

private:

	int m_nServerId;
	char m_szServerName[65];

	PLogfile *m_pInfoLog;
	PLogfile *m_pAccessLog;

	PUDPSocket *m_pServerSocket;
	HLServerList *m_pServerList;
	HLServerFilterList *m_pHostFilter;

	PString *m_pHostFilterFile;
	PString *m_pHostListFile;


	time_t m_nCurrentTimestamp;
	time_t m_nLastInterval;
	time_t m_nLastRequest;

	bool m_bRequestQueueFinished;

	pfc_inet_addr m_nForeignAddress;
	pfc_inet_port m_nForeignPort;

	
	// send & receive buffer
	int  m_nBufferLength;
	char m_szBuffer[2048];


	// temporary buffer for the server-infos
	char m_szChallenge_VarName[33];
	char m_szChallenge_VarValue[33];

	int  m_nChallenge_Id;
	char m_nChallenge_Protocol;
	char m_nChallenge_Players;
	char m_nChallenge_MaxPlayers;
	char m_nChallenge_OsType;
	bool m_bChallenge_Dedicated;
	bool m_bChallenge_Password;
	bool m_bChallenge_Lan;

	char m_szChallenge_GameDir[33];
	char m_szChallenge_MapName[33];

	// m_nChallenge_Os values
	static const char c_OSTYPE_UNKNOWN;
	static const char c_OSTYPE_WINDOWS;
	static const char c_OSTYPE_LINUX;

	// predefined packages
	static const char m_pPackage_Challenge[];
	static const int  m_nPkgSize_Challenge;

	static const char m_pPackage_Ping[];
	static const int  m_nPkgSize_Ping;

	static const char m_pPackage_Servername[];
	static const int  m_nPkgSize_Servername;

	static const char m_pPackage_List_Type1[];
	static const int  m_nPkgSize_List_Type1;

	static const char m_pPackage_List_Type2[];
	static const int  m_nPkgSize_List_Type2;

	static const char m_pPackage_List_Auth[];
	static const int  m_nPkgSize_List_Auth;

	static const char m_pPackage_Req_Details[];
	static const int  m_nPkgSize_Req_Details;
};




#endif		// #ifndef _HLMASTER_H_
