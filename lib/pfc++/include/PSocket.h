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


#ifndef _PSOCKET_H_
#define _PSOCKET_H_



// Classes defined in this document
// ---------------------------------
// 
class PSocket;
class PSocketException;



// Header includes
// ---------------------------------
// 
#include "PException.h"

#include <pfc++/socket.h>





// Class definition
// ---------------------------------
//
class PSocket
{
public:

	PSocket(int nAddressFamily, int nType);
	PSocket(PFC_SOCKET nExistingSocket);
	virtual ~PSocket();

	// TODO: ? virtual bool Listen(...) = 0;
	// TODO: ? virtual bool accept(...) = 0;
	virtual bool Bind(const char *pszIpAddress, pfc_inet_port nPort) = 0;
	virtual bool Bind(pfc_inet_addr nIpAddress, pfc_inet_port nPort) = 0;
	virtual bool Connect(pfc_inet_addr nIpAddress, pfc_inet_port nPort) = 0;
	// TODO: virtual void Disconnect() = 0;

	virtual int Send(const char *pcBuffer, int nBufferSize) = 0;
	virtual int Receive(char *pcBuffer, int nBufferSize) = 0;

	virtual pfc_inet_port GetLocalPort() = 0;
	virtual pfc_inet_addr GetLocalAddress() = 0;

	virtual pfc_inet_port GetForeignPort() = 0;
	virtual pfc_inet_addr GetForeignAddress() = 0;

	virtual PFC_FD GetDescriptor()
	{
		return m_nSocket;
	}

	// TODO: virtual int GetOptions(...);
	// TODO: virtual int SetOptions(...);

	// bool Wait(bool bInput = true, bool bOutput = true, bool bError = true);
	bool WaitForInput();
	bool WaitForOutput();
	bool WaitForError();
	void SetWaitTimeout(unsigned int nMilliseconds);
	int GetWaitTimeout() const { return (m_nTimeoutSeconds * 1000) + m_nTimeoutMilliseconds; }


protected:

	bool m_bIsBound;
	PFC_SOCKET m_nSocket;

	// stuff for the "Wait" functions
	fd_set   m_sDescriptorSet;

	int m_nTimeoutSeconds;
	int m_nTimeoutMilliseconds;

	timeval  m_sTimeout;
	timeval *m_pTimeout;
};



class PSocketException: public PException
{
public:
	PSocketException(int nErrorValue)
	:PException(nErrorValue)
	{
		switch(nErrorValue)
		{
		case PFC_ERROR_INET_ADDR_INUSE:
			SetDescription("The specified address is allready in use.");
			break;

		case PFC_ERROR_INET_ADDR_NOTAVAIL:
			SetDescription("The specified address is not available.");
			break;

#ifdef PFC_SYSTEM_TYPE_WINDOWS
		case PFC_ERROR_INET_DOWN:
			SetDescription("The network is down.");
			break;

		case PFC_ERROR_INET_NOTINITIALISED:
			SetDescription("The network is not initialized.");
			break;
#endif

		case PFC_ERROR_INET_NOSOCKET:
			SetDescription("The descriptor is no valid socket.");
			break;

		default:
			SetDescription("Unknown exception.");
			break;
		}
	}

	PSocketException(int nErrorValue, const char *pszDescription)
	:PException(nErrorValue, pszDescription)
	{
	}

	PSocketException()
	:PException()
	{
	}
};


#endif		// #ifndef _PSOCKET_H_
