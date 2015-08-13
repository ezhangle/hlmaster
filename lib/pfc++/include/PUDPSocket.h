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


#ifndef _PUDPSOCKET_H_
#define _PUDPSOCKET_H_



// Classes defined in this document
// ---------------------------------
// 
class PUDPSocket;



// Header includes
// ---------------------------------
// 
#include "PSocket.h"

#include <string.h>


// Class definition
// ---------------------------------
//
class PUDPSocket: public PSocket
{
public:

	PUDPSocket();
	~PUDPSocket();

	virtual bool Bind(const char *pszIpAddress, pfc_inet_port nPort);
	virtual bool Bind(pfc_inet_addr nIpAddress, pfc_inet_port nPort);
	virtual bool Connect(pfc_inet_addr nIpAddress, pfc_inet_port nPort);

	virtual int Send(const char *pcBuffer, int nBufferSize);
	int SendTo(const char *pcBuffer, int nBufferSize, pfc_inet_addr nIpAddress, pfc_inet_port nPort);
	virtual int Receive(char *pcBuffer, int nBufferSize);

	virtual pfc_inet_port GetLocalPort()
	{
		return m_sLocalAddress.sin_port;
	}

	virtual pfc_inet_addr GetLocalAddress()
	{
		return m_sLocalAddress.sin_addr.s_addr;
	}

	virtual pfc_inet_port GetForeignPort()
	{
		return m_sForeignAddress.sin_port;
	}

	virtual pfc_inet_addr GetForeignAddress()
	{
		return m_sForeignAddress.sin_addr.s_addr;
	}

	// TODO: this is crap, remove
	const char* GetForeignAddressString()
	{
		return inet_ntoa(m_sForeignAddress.sin_addr);
	}
	const char* GetLocalAddressString()
	{
		return inet_ntoa(m_sLocalAddress.sin_addr);
	}


private:

	pfc_socklen_t m_nForeignAddressLength;

	struct sockaddr_in m_sLocalAddress;
	struct sockaddr_in m_sForeignAddress;
};




#endif		// #ifndef _PUDPSOCKET_H_
