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

#include <pfc++/system.h>

#include "HLMaster.h"
#include "HLServerList.h"
#include "PString.h"
#include "CStringBuffer.h"

#include <stdio.h>



// ======================================================================

HLMaster::HLMaster(const char *pszIpAddress, unsigned short nPort, int nMaxHosts, PLogfile *pInfoLog, PLogfile *pAccessLog)
{
	// server socket
	m_pServerSocket = new PUDPSocket();

	if(m_pServerSocket->Bind(pszIpAddress, nPort) != true)
	{
		delete m_pServerSocket;

#if defined PFC_SYSTEM_TYPE_WINDOWS
		throw PSocketException(WSAGetLastError());
#elif defined PFC_SYSTEM_TYPE_UNIX
		throw PSocketException(errno);
#else
#error Unknown operating system. Only WIN32 / LINUX are recognized.
#endif
	}

	// main server list
	m_pServerList = new HLServerList(nMaxHosts, pInfoLog);


	// server filter
	m_pHostFilter     = NULL;
	m_pHostFilterFile = NULL;

	m_pHostListFile   = NULL;

	// logfiles
	m_pInfoLog   = pInfoLog;
	m_pAccessLog = pAccessLog;


	m_nLastInterval     = 0;
	m_nLastRequest      = 0;
	m_nCurrentTimestamp = 0;

	m_bRequestQueueFinished = false;

	// initialize buffer stuff
	m_nBufferLength     = 0;
	m_nForeignAddress   = 0;
	m_nForeignPort      = 0;


	SetServerId(0);
	SetServerName(NULL);
}


// ======================================================================

HLMaster::~HLMaster()
{
	if(m_pHostFilter != NULL)
		delete m_pHostFilter;
	if(m_pHostFilterFile != NULL)
		delete m_pHostFilterFile;
	if(m_pHostListFile != NULL)
		delete m_pHostListFile;

	delete m_pServerList;
	delete m_pServerSocket;
}


// ======================================================================
// WARNING: these times are calculated. Change only if you know what you do!
//
// In 300 seconds, 3000 servers can be queried for status info with a request
// interval of 10 seconds. 3000 servers is the limit of static servers.
// 
const time_t HLMaster::c_CHECK_TIMEOUT		= 9 * 60;	// 9 minutes (540 seconds)
const time_t HLMaster::c_CHECK_INTERVAL		= 5 * 60;	// 5 minutes (300 seconds)
const time_t HLMaster::c_REQUEST_INTERVAL	= 10;		// 10 seconds


// ======================================================================

const char HLMaster::m_pPackage_Challenge[] = { '\xff', '\xff', '\xff', '\xff', 's', '\n' };
const int  HLMaster::m_nPkgSize_Challenge   = sizeof(HLMaster::m_pPackage_Challenge);

const char HLMaster::m_pPackage_Ping[]      = { '\xff', '\xff', '\xff', '\xff', 'j', '\r', '\n', '\0' };
const int  HLMaster::m_nPkgSize_Ping        = sizeof(HLMaster::m_pPackage_Ping);

const char HLMaster::m_pPackage_Servername[]= { '\xff', '\xff', '\xff', '\xff', 'h', '\r', '\n' };
const int  HLMaster::m_nPkgSize_Servername  = sizeof(HLMaster::m_pPackage_Servername);

const char HLMaster::m_pPackage_List_Type1[]= { '\xff', '\xff', '\xff', '\xff', 'd', '\n' };
const int  HLMaster::m_nPkgSize_List_Type1  = sizeof(HLMaster::m_pPackage_List_Type1);

const char HLMaster::m_pPackage_List_Type2[]= { '\xff', '\xff', '\xff', '\xff', 'f', '\r' };
const int  HLMaster::m_nPkgSize_List_Type2  = sizeof(HLMaster::m_pPackage_List_Type2);

const char HLMaster::m_pPackage_List_Auth[] = { '\xff', '\xff', '\xff', '\xff', 'w', '\r', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
const int  HLMaster::m_nPkgSize_List_Auth   = sizeof(HLMaster::m_pPackage_List_Auth);

const char HLMaster::m_pPackage_Req_Details[]= { '\xff', '\xff', '\xff', '\xff', 'd', 'e', 't', 'a', 'i', 'l', 's' };
const int  HLMaster::m_nPkgSize_Req_Details  = sizeof(HLMaster::m_pPackage_Req_Details);

// ======================================================================

const char HLMaster::c_OSTYPE_UNKNOWN = 0;
const char HLMaster::c_OSTYPE_WINDOWS = 1;
const char HLMaster::c_OSTYPE_LINUX   = 2;


// ======================================================================

void HLMaster::SetServerName(const char *pszName)
{
	if(pszName == NULL)
	{
		m_szServerName[0] = '\0';
	}
	else
	{
		int nCount;
		for(nCount = 0; nCount < ((int)sizeof(m_szServerName) - 1) && pszName[nCount] != '\0'; nCount++)
			m_szServerName[nCount] = pszName[nCount];

		m_szServerName[nCount] = '\0';
	}
}


// ======================================================================

int HLMaster::GetServerName(char *pszBuffer, int nBufferSize)
{
	if(nBufferSize < 1)
		return 0;

	int nCount;
	for(nCount = 0; nCount < (nBufferSize - 1) && m_szServerName[nCount] != '\0'; nCount++)
		pszBuffer[nCount] = m_szServerName[nCount];

	pszBuffer[nCount] = '\0';

	return nCount;
}


// ======================================================================

void HLMaster::EnableHostFilter()
{
	if(m_pHostFilter == NULL)
		m_pHostFilter = new HLServerFilterList();
}


// ======================================================================

void HLMaster::DisableHostFilter()
{
	if(m_pHostFilter != NULL)
	{
		delete m_pHostFilter;
		m_pHostFilter = NULL;
	}
}


// ======================================================================

bool HLMaster::LoadHostFilter(const char *pszFilename)
{
	int nLoaded;

	if(m_pHostFilter == NULL)
		m_pHostFilter = new HLServerFilterList();

	if(m_pHostFilterFile == NULL)
		m_pHostFilterFile = new PString(PFC_MAX_PATH, pszFilename);
	else
		m_pHostFilterFile->SetValue(pszFilename);

	nLoaded = m_pHostFilter->Load(m_pHostFilterFile->GetValue(), m_pInfoLog);
	if(nLoaded >= 0)
	{
		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
		{
			m_pInfoLog->Append("Successfully loaded ");
			m_pInfoLog->Append(nLoaded);
			m_pInfoLog->Append(" filters from file <");
			m_pInfoLog->Append(m_pHostFilterFile->GetValue());
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}

		return true;
	}
	else
	{
		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_WARNING) == true)
		{
			m_pInfoLog->Append("Failed to load filters from file <");
			m_pInfoLog->Append(m_pHostFilterFile->GetValue());
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}

		return false;
	}
}


// ======================================================================

bool HLMaster::LoadHosts(const char *pszFilename)
{
	int nLoaded = 0;

	if(m_pHostListFile == NULL)
		m_pHostListFile = new PString(PFC_MAX_PATH, pszFilename);
	else
		m_pHostListFile->SetValue(pszFilename);

	nLoaded = m_pServerList->LoadHosts(m_pHostListFile->GetValue());
	if(nLoaded >= 0)
	{
		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
		{
			m_pInfoLog->Append("Successfully loaded ");
			m_pInfoLog->Append(nLoaded);
			m_pInfoLog->Append(" servers from file <");
			m_pInfoLog->Append(m_pHostListFile->GetValue());
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}

		return true;
	}
	else
	{
		if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_WARNING) == true)
		{
			m_pInfoLog->Append("Failed to load servers from file <");
			m_pInfoLog->Append(m_pHostListFile->GetValue());
			m_pInfoLog->Append('>');

			m_pInfoLog->End();
		}

		return false;
	}
}


// ======================================================================

bool HLMaster::ReloadHostFilter()
{
	if(m_pHostFilter == NULL || m_pHostFilterFile == NULL)
		return false;

	if(m_pHostFilter->Load(m_pHostFilterFile->GetValue()) >= 0)
		return true;
	else
		return false;
}


// ======================================================================

bool HLMaster::ReloadHosts()
{
	if(m_pHostListFile == NULL)
		return false;

	if(m_pServerList->LoadHosts(m_pHostListFile->GetValue()) >= 0)
		return true;
	else
		return false;
}


// ======================================================================

time_t HLMaster::GetIntervalRemaining(time_t nTimestamp)
{
	if(nTimestamp == m_nLastInterval)
		return HLMaster::c_CHECK_INTERVAL;

	// check for error
	if(m_nLastInterval > nTimestamp)
		m_nLastInterval = nTimestamp;

	// calculate remaining time
	nTimestamp = (nTimestamp - m_nLastInterval);
	
	if(nTimestamp < HLMaster::c_CHECK_INTERVAL)
		return (HLMaster::c_CHECK_INTERVAL - nTimestamp);
	else
		return 0;
}


// ======================================================================

time_t HLMaster::GetRequestIntervalRemaining(time_t nTimestamp)
{
	if(nTimestamp == m_nLastRequest)
		return HLMaster::c_REQUEST_INTERVAL;

	// check for error
	if(m_nLastRequest > nTimestamp)
		m_nLastRequest = nTimestamp;

	// calculate remaining time
	nTimestamp = (nTimestamp - m_nLastRequest);
	
	if(nTimestamp < HLMaster::c_REQUEST_INTERVAL)
		return (HLMaster::c_REQUEST_INTERVAL - nTimestamp);
	else
		return 0;
}


// ======================================================================

void HLMaster::SendDetailsRequest(HLServerListItem *pItem)
{
	// send details request

	// create details request package
	memcpy(m_szBuffer, m_pPackage_Req_Details, m_nPkgSize_Req_Details);

	m_pServerSocket->SendTo(m_szBuffer, m_nPkgSize_Req_Details, pItem->m_nIpAddress, pItem->m_nPort);
}


// ======================================================================

bool HLMaster::WaitForInput()
{
	bool bReady;

	time_t nCurrentTime;
	time_t nTimeout;
	time_t nRequestTimeout = HLMaster::c_CHECK_INTERVAL;

	time(&nCurrentTime);

	nTimeout = GetIntervalRemaining(nCurrentTime);
	if(nTimeout == 0)
	{
		m_pServerList->CheckTimeout(nCurrentTime, this);

		m_nLastInterval = nCurrentTime;
		nTimeout        = HLMaster::c_CHECK_INTERVAL;

		m_bRequestQueueFinished = false;
	}


	if(m_pHostListFile != NULL && m_bRequestQueueFinished == false)
	{
		nRequestTimeout = GetRequestIntervalRemaining(nCurrentTime);
		if(nRequestTimeout == 0)
		{
			m_bRequestQueueFinished = m_pServerList->RequestStatus(nCurrentTime, this);
			if(m_bRequestQueueFinished == true)
			{
				// queue finished
				nRequestTimeout = HLMaster::c_CHECK_INTERVAL;
			}
			else
			{
				nRequestTimeout = HLMaster::c_REQUEST_INTERVAL;
			}

			m_nLastRequest  = nCurrentTime;
		}
	}


	if(m_pServerList->GetNumItems() > 0)
	{
		// wait until timeout
		if(nRequestTimeout < nTimeout)
			nTimeout = nRequestTimeout;

		m_pServerSocket->SetWaitTimeout(nTimeout * 1000);

		#ifdef _DEBUG
		printf("Sleep for %d seconds...(num_hosts=%d)\n", (int)nTimeout, (int)m_pServerList->GetNumItems());
		#endif
		bReady = m_pServerSocket->WaitForInput();
	}
	else
	{
		// wait 'forever'
		m_pServerSocket->SetWaitTimeout(PFC_WAIT_INFINITE);

		#ifdef _DEBUG
		printf("Sleep for undefined amount of time (list is empty)...\n");
		#endif
		bReady = m_pServerSocket->WaitForInput();

		// reset interval
		time(&m_nLastInterval);
		time(&m_nLastRequest);
	}


	return bReady;
}


// ======================================================================

bool HLMaster::ReadInput()
{
	// read input
	m_nBufferLength     = m_pServerSocket->Receive(m_szBuffer, sizeof(m_szBuffer));

	m_nForeignAddress   = m_pServerSocket->GetForeignAddress();
	m_nForeignPort      = m_pServerSocket->GetForeignPort();

	m_nCurrentTimestamp = time(NULL);

	// read error?
	if(m_nBufferLength < 0)
		m_nBufferLength = 0;

	if(m_nBufferLength > 0)
		return true;
	else
		return false;
}


// ======================================================================

void HLMaster::ProcessInput(time_t nTimestamp)
{
	if(m_nBufferLength <= 0)
		return;

	// ignore stupid packages
	if(m_nForeignAddress == PFC_INET_ADDR_ANY || m_nForeignAddress == PFC_INET_ADDR_BROADCAST)
	{
		// do not accept packages sent from a broadcast address
		return;
	}
	if(m_nForeignAddress == m_pServerSocket->GetLocalAddress() && m_nForeignPort == m_pServerSocket->GetLocalPort())
	{
		// do not accept packages sent from 'us'
		return;
	}


	// check request
	if(m_szBuffer[0] == 'q')
	{
		ProcessHeartbeat();
	}
	else if((m_nBufferLength > 2) && (m_szBuffer[0] == '0') && (m_szBuffer[1] == 10))
	{
		ProcessChallenge();
	}
	else if(m_szBuffer[0] == 'b')
	{
		ProcessLogoff();
	}
	else if(m_szBuffer[0] == 'c')
	{
		ProcessListRequest();
	}
	else if(m_szBuffer[0] == 'e' || m_szBuffer[0] == '1')
	{
		if(m_nBufferLength < 5)
			return;

		// NOTE: the '1' is used by a new protocol, which
		//       might send a filter request - but I'm not
		//       going to handle filgering right now.
		ProcessEnhancedListRequest();
	}
	else if(m_szBuffer[0] == 'v')
	{
		ProcessAuthServerListRequest();
	}
	else if((*((unsigned int*) &m_szBuffer[0]) == 0xFFFFFFFF) && (m_szBuffer[4] == 'm'))
	{
		ProcessDetailsReply();
	}
	else if(m_szBuffer[0] == 'g')
	{
		ProcessNameRequest();
	}
	else if(m_szBuffer[0] == 'i')
	{
		ProcessPingRequest();
	}
#ifdef _DEBUG
	else if(m_szBuffer[0] == 's')
	{
		// DEBUG ONLY - Log a snapshot of the
		// current list
		m_pServerList->LogSnapshot();
	}
#endif	// _DEBUG
	else
	{
		ProcessInvalidQuery();
	}
}



// ======================================================================

void HLMaster::ProcessHeartbeat()
{
	// halflife-server sent heartbeat
	if((m_pHostFilter != NULL) && (m_pHostFilter->IsAllowed(m_nForeignAddress) != true))
		return;

	HLServerListItem *pItem = m_pServerList->HeartBeat(m_nForeignAddress, m_nForeignPort, m_nCurrentTimestamp);

	if(pItem != NULL)
	{
		// send ack (challenge request), we will receive another packet after that...

		// create ack package
		memcpy(m_szBuffer, m_pPackage_Challenge, m_nPkgSize_Challenge);

		// copy m_nChallenge to m_szBuffer
		*((int*) (m_szBuffer + m_nPkgSize_Challenge)) = pItem->m_nChallengeId;

		m_pServerSocket->Send(m_szBuffer, m_nPkgSize_Challenge + sizeof(pItem->m_nChallengeId));
	}
}


// ======================================================================

void HLMaster::ProcessChallenge()
{
	if((m_pHostFilter != NULL) && (m_pHostFilter->IsAllowed(m_nForeignAddress) != true))
		return;

	// halflife-server sent challenge (after heartbeat)
	if(ParseChallengeString(m_szBuffer + 2, m_nBufferLength - 2) == true)
	{
		m_pServerList->Challenge(m_nForeignAddress, m_nForeignPort, m_nCurrentTimestamp, m_nChallenge_Id, m_nChallenge_Protocol, m_nChallenge_Players, m_nChallenge_MaxPlayers, m_nChallenge_OsType, m_bChallenge_Dedicated, m_bChallenge_Password, m_bChallenge_Lan, m_szChallenge_GameDir, m_szChallenge_MapName);
	}
}


// ======================================================================

bool HLMaster::ParseChallengeString(const char *pszChallenge, const int nChallengeSize)
{
	// reset temp challenge infos
	m_szChallenge_VarName[0] = '\0';
	m_szChallenge_VarValue[0]= '\0';

	m_nChallenge_Id          = 0;
	m_nChallenge_Protocol    = 0;

	m_nChallenge_Players     = 0;
	m_nChallenge_MaxPlayers  = 0;

	m_nChallenge_OsType      = HLMaster::c_OSTYPE_UNKNOWN;
	m_bChallenge_Dedicated   = false;
	m_bChallenge_Password    = false;
	m_bChallenge_Lan         = false;

	m_szChallenge_GameDir[0] = '\0';
	m_szChallenge_MapName[0] = '\0';


	// parse challenge string
	char *pcCurrent = (char*) pszChallenge;

	if(*(pcCurrent) == '\\')
	{
		// skip first '\'
		pcCurrent++;
		int nCount = 1;

		int nFilter      = 0;
		int nStringCount = 0;

		while(nCount < nChallengeSize)
		{
			// check for end of challenge string
			if((nCount == nChallengeSize - 1) || *(pcCurrent) == 10 || *(pcCurrent) == 13 || *(pcCurrent) == '\0')
			{
				if(nFilter != 1)
					break;

				// unexpected end of variable found
				m_szChallenge_VarValue[nStringCount] = '\0';

				nStringCount = 0;
				nFilter = 2;

				// increment counters
				nCount++;
				pcCurrent++;
			}

			if(nFilter == 0)
			{
				// name
				if(*(pcCurrent) == '\\')
				{
					if(nStringCount == 0)
						continue;

					// end of name found
					m_szChallenge_VarName[nStringCount] = '\0';
					m_szChallenge_VarValue[0]           = '\0';

					nStringCount = 0;
					nFilter++;
				}
				else if(nStringCount < ((int) sizeof(m_szChallenge_VarName) - 1))
				{
					// append char to variable name
					m_szChallenge_VarName[nStringCount] = *(pcCurrent);
					nStringCount++;
				}

				// increment counters
				nCount++;
				pcCurrent++;
			}
			else if(nFilter == 1)
			{
				// value
				if(*(pcCurrent) == '\\')
				{
					// end of value found
					m_szChallenge_VarValue[nStringCount] = '\0';

					nStringCount = 0;
					nFilter++;
				}
				else if(nStringCount < ((int) sizeof(m_szChallenge_VarValue) - 1))
				{
					// append char to variable value
					m_szChallenge_VarValue[nStringCount] = *(pcCurrent);
					nStringCount++;
				}

				// increment counters
				nCount++;
				pcCurrent++;
			}
			else if(nFilter == 2)
			{
				// name & value found, check and save it
				if(strcmp(m_szChallenge_VarName, "protocol") == 0)			// protocol
					m_nChallenge_Protocol = (char) atoi(m_szChallenge_VarValue);
				else if(strcmp(m_szChallenge_VarName, "challenge") == 0)	// challenge id
					m_nChallenge_Id = atoi(m_szChallenge_VarValue);
				else if(strcmp(m_szChallenge_VarName, "players") == 0)		// number of players
					m_nChallenge_Players = atoi(m_szChallenge_VarValue);
				else if(strcmp(m_szChallenge_VarName, "max") == 0)			// max players
					m_nChallenge_MaxPlayers = atoi(m_szChallenge_VarValue);
				else if(strcmp(m_szChallenge_VarName, "gamedir") == 0)		// gamedirectory
				{
					for(nStringCount = 0; nStringCount < ((int) sizeof(m_szChallenge_GameDir) - 1) && m_szChallenge_VarValue[nStringCount] != '\0'; nStringCount++)
						m_szChallenge_GameDir[nStringCount] = m_szChallenge_VarValue[nStringCount];

					m_szChallenge_GameDir[nStringCount] = '\0';
					nStringCount = 0;
				}
				else if(strcmp(m_szChallenge_VarName, "map") == 0)			// mapname
				{
					for(nStringCount = 0; nStringCount < ((int) sizeof(m_szChallenge_MapName) - 1) && m_szChallenge_VarValue[nStringCount] != '\0'; nStringCount++)
						m_szChallenge_MapName[nStringCount] = m_szChallenge_VarValue[nStringCount];

					m_szChallenge_MapName[nStringCount] = '\0';
					nStringCount = 0;
				}
				else if(strcmp(m_szChallenge_VarName, "type") == 0)			// type d=dedicated
				{
					if(*(m_szChallenge_VarValue) == 'd')
						m_bChallenge_Dedicated = true;
					else
						m_bChallenge_Dedicated = false;
				}
				else if(strcmp(m_szChallenge_VarName, "password") == 0)		// password protected?
				{
					if(*(m_szChallenge_VarValue) == '1')
						m_bChallenge_Password = true;
					else
						m_bChallenge_Password = false;
				}
				else if(strcmp(m_szChallenge_VarName, "os") == 0)			// w=windows, l=linux
				{
					if(*(m_szChallenge_VarValue) == 'w')
						m_nChallenge_OsType = HLMaster::c_OSTYPE_WINDOWS;
					else
						m_nChallenge_OsType = HLMaster::c_OSTYPE_LINUX;
				}
				else if(strcmp(m_szChallenge_VarName, "lan") == 0)			// lan?
				{
					if(*(m_szChallenge_VarValue) == '1')
						m_bChallenge_Lan = true;
					else
						m_bChallenge_Lan = false;
				}
				else
				{
					// unknown variable
				}

				
				nFilter = 0;
			}
		}

		// finished parsing
		return true;
	}
	else
	{
		// bad challenge string?
		// has to begin with '\'
		return false;
	}
}


// ======================================================================

bool HLMaster::ParseDetailsString(const char *pszDetails, const int nDetailsSize)
{
	// reset temp challenge infos
	m_nChallenge_Id          = 0;
	m_nChallenge_Protocol    = 0;

	m_nChallenge_Players     = 0;
	m_nChallenge_MaxPlayers  = 0;

	m_nChallenge_OsType      = HLMaster::c_OSTYPE_UNKNOWN;
	m_bChallenge_Dedicated   = false;
	m_bChallenge_Password    = false;
	m_bChallenge_Lan         = false;

	m_szChallenge_GameDir[0] = '\0';
	m_szChallenge_MapName[0] = '\0';


	int nCount;
	int nStringCount;
	char* pcCurrent = (char*) pszDetails;

	// server ip/port (not interesting)
	for(nCount = 0; (nCount < nDetailsSize) && (*(pcCurrent) != '\0'); nCount++, pcCurrent++)
		;	// do nothing, just skip
	pcCurrent++;
	nCount++;

	// server name (not interesting)
	for( ; (nCount < nDetailsSize) && (*(pcCurrent) != '\0'); nCount++, pcCurrent++)
		;	// do nothing, just skip
	pcCurrent++;
	nCount++;

	// map name
	for(nStringCount = 0; (nCount < nDetailsSize) && (*(pcCurrent) != '\0'); nCount++, pcCurrent++)
	{
		if(nStringCount < ((int)sizeof(m_szChallenge_MapName) - 1))
			m_szChallenge_MapName[nStringCount++] = *(pcCurrent);
	}
	m_szChallenge_MapName[nStringCount] = '\0';
	pcCurrent++;
	nCount++;

	// game directory
	for(nStringCount = 0; (nCount < nDetailsSize) && (*(pcCurrent) != '\0'); nCount++, pcCurrent++)
	{
		if(nStringCount < ((int)sizeof(m_szChallenge_GameDir) - 1))
			m_szChallenge_GameDir[nStringCount++] = *(pcCurrent);
	}
	m_szChallenge_GameDir[nStringCount] = '\0';
	pcCurrent++;
	nCount++;

	// game name (not interesting)
	for( ; (nCount < nDetailsSize) && (*(pcCurrent) != '\0'); nCount++, pcCurrent++)
		;	// do nothing, just skip
	pcCurrent++;
	nCount++;

	// number of players
	if(nCount < nDetailsSize)
	{
		m_nChallenge_Players = *(pcCurrent);
		pcCurrent++;
		nCount++;
	}

	// max number of players
	if(nCount < nDetailsSize)
	{
		m_nChallenge_MaxPlayers = *(pcCurrent);
		pcCurrent++;
		nCount++;
	}

	// protocol version
	if(nCount < nDetailsSize)
	{
		m_nChallenge_Protocol = *(pcCurrent);
		pcCurrent++;
		nCount++;
	}

	// type of server (l=listen, d=dedicated)
	if(nCount < nDetailsSize)
	{
		if(*(pcCurrent) == 'd')
			m_bChallenge_Dedicated = true;
		pcCurrent++;
		nCount++;
	}

	// os of server (l=linux, w=windows)
	if(nCount < nDetailsSize)
	{
		if(*(pcCurrent) == 'l')
			m_nChallenge_OsType = HLMaster::c_OSTYPE_LINUX;
		else if(*(pcCurrent) == 'w')
			m_nChallenge_OsType = HLMaster::c_OSTYPE_WINDOWS;
		pcCurrent++;
		nCount++;
	}

	// password on server (0=no, 1=yes)
	if(nCount < nDetailsSize)
	{
		if(*(pcCurrent) == 1)
			m_bChallenge_Password = true;
		pcCurrent++;
		nCount++;
	}


	// there is some more - not interesting - stuff
	// if we're not coming that far, there is an error...
	if(nCount < nDetailsSize)
		return true;
	else
		return false;
}


// ======================================================================

void HLMaster::ProcessDetailsReply()
{
	// server replied details packet
	/*
	if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_DEBUG) == true)
	{
		m_pInfoLog->Append("ServerInfo (type=details) <");
		m_pInfoLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pInfoLog->Append(':');
		m_pInfoLog->Append((int)ntohs(m_nForeignPort));
		m_pInfoLog->Append('>');

		m_pInfoLog->End();
	}
	*/

	if(m_nBufferLength <= 5)
		return;

	if(ParseDetailsString(&m_szBuffer[5], m_nBufferLength - 5) == true)
	{
		m_pServerList->UpdateDetails(m_nForeignAddress, m_nForeignPort, m_nCurrentTimestamp, m_nChallenge_Protocol, m_nChallenge_Players, m_nChallenge_MaxPlayers, m_nChallenge_OsType, m_bChallenge_Dedicated, m_bChallenge_Password, m_bChallenge_Lan, m_szChallenge_GameDir, m_szChallenge_MapName);
	}
}


// ======================================================================

void HLMaster::ProcessLogoff()
{
	// halflife-server sent logoff
	if((m_pHostFilter != NULL) && (m_pHostFilter->IsAllowed(m_nForeignAddress) != true))
		return;

	if(m_pServerList->Logoff(m_nForeignAddress, m_nForeignPort) != true)
		return;

	if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pInfoLog->Append("ServerInfo (type=logoff) <");
		m_pInfoLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pInfoLog->Append(':');
		m_pInfoLog->Append((int)ntohs(m_nForeignPort));
		m_pInfoLog->Append('>');

		m_pInfoLog->End();
	}
}


// ======================================================================

void HLMaster::ProcessListRequest()
{
	// client asks for: list (binary format, quakeworld protcol)
	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=list1) <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}

	int nSize = 0;

	memcpy(m_szBuffer, m_pPackage_List_Type1, m_nPkgSize_List_Type1);

	// get the list
	nSize = m_pServerList->FillPacket_Quakeworld(m_szBuffer + m_nPkgSize_List_Type1, sizeof(m_szBuffer) - m_nPkgSize_List_Type1);

	m_pServerSocket->Send(m_szBuffer, m_nPkgSize_List_Type1 + nSize);
}


void HLMaster::ProcessAuthServerListRequest()
{
	// the halflife ingame browser ask's for a list of
	// auth servers.
	/*
	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=authlist) <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}
	*/

	// normally, we would send a list of auth servers
	// but we actually don't know'em so we just send zero's
	memcpy(m_szBuffer, m_pPackage_List_Auth, m_nPkgSize_List_Auth);

	m_pServerSocket->Send(m_szBuffer, m_nPkgSize_List_Auth);
}

// ======================================================================

void HLMaster::ProcessEnhancedListRequest()
{
	// client asks for: list (binary format, halflife protocol, up to 65535 servers)
	int nSize = 0;
	int nSkip = 0;

	bool bHasMore = false;
	unsigned short nSkippedItems = 0;


	// check, how many items to skip (primitive, but works for now)
	if(m_nBufferLength >= 3)
		nSkip = ntohs(*((unsigned short*) &m_szBuffer[1]));

#ifdef _DEBUG
	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=list2 skip=");
		m_pAccessLog->Append(nSkip);
		m_pAccessLog->Append(") <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}
#else
	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=list2) <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}
#endif

	// create packet
	memcpy(m_szBuffer, m_pPackage_List_Type2, m_nPkgSize_List_Type2);

	// get the list
	// skip 4 bytes (required for num items, \0\0)
	nSize = m_pServerList->FillPacket_Halflife(nSkip, &m_szBuffer[(m_nPkgSize_List_Type2 + 4)], (sizeof(m_szBuffer) - m_nPkgSize_List_Type2 - 4), &nSkippedItems, &bHasMore);

	// write the 4 bytes skipped before
	if(bHasMore == true)
	{
		// write number of items to skip into packet
		*((unsigned short*) &m_szBuffer[m_nPkgSize_List_Type2]) = htons(nSkippedItems);
	}
	else
	{
		// 0 marks the end of the server list
		*((unsigned short*) &m_szBuffer[m_nPkgSize_List_Type2]) = 0;
	}
	m_szBuffer[m_nPkgSize_List_Type2 + 2] = 0;
	m_szBuffer[m_nPkgSize_List_Type2 + 3] = 0;


	m_pServerSocket->Send(m_szBuffer, m_nPkgSize_List_Type2 + nSize + 4);
}


// ======================================================================

void HLMaster::ProcessNameRequest()
{
	// client asks for: servername
	int nSize = 0;

	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=servername) <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}

	// create packet
	memcpy(m_szBuffer, m_pPackage_Servername, m_nPkgSize_Servername);

	// append name
	nSize = GetServerName(&m_szBuffer[m_nPkgSize_Servername], sizeof(m_szBuffer) - m_nPkgSize_Servername);
	// GetServerName() returns size without terminating '\0'
	nSize ++;

	// append "\r\n\0"
	memcpy(&m_szBuffer[m_nPkgSize_Servername + nSize], "\r\n", sizeof("\r\n"));

	m_pServerSocket->Send(m_szBuffer, (m_nPkgSize_Servername + nSize + sizeof("\r\n")));
}


// ======================================================================

void HLMaster::ProcessPingRequest()
{
	if(m_pAccessLog != NULL && m_pAccessLog->Begin(PLogfile::c_LEVEL_INFO) == true)
	{
		m_pAccessLog->Append("Request (type=ping) <");
		m_pAccessLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pAccessLog->Append(':');
		m_pAccessLog->Append((int)ntohs(m_nForeignPort));
		m_pAccessLog->Append('>');

		m_pAccessLog->End();
	}

	// client asks for: ping
	// NOTE: im not shure about that
	m_pServerSocket->Send(m_pPackage_Ping, m_nPkgSize_Ping);
}


// ======================================================================

void HLMaster::ProcessInvalidQuery()
{
	// invalid query, ignore
	if(m_pInfoLog != NULL && m_pInfoLog->Begin(PLogfile::c_LEVEL_DEBUG) == true)
	{
		m_pInfoLog->Append("Invalid query <");
		m_pInfoLog->Append(m_pServerSocket->GetForeignAddressString());
		m_pInfoLog->Append(':');
		m_pInfoLog->Append((int)ntohs(m_nForeignPort));
		m_pInfoLog->Append('>');

		m_pInfoLog->End();
	}
}



