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


#include "PUDPSocket.h"




// ======================================================================

PUDPSocket::PUDPSocket()
:PSocket(AF_INET, SOCK_DGRAM)
{
	// initialize local socket infos
	m_sLocalAddress.sin_family      = AF_INET;
	m_sLocalAddress.sin_port        = PFC_INET_PORT_ANY;
	m_sLocalAddress.sin_addr.s_addr = PFC_INET_ADDR_ANY;
	memset(&m_sLocalAddress.sin_zero, 0, sizeof(m_sLocalAddress.sin_zero));

	// initialize foreign socket infos
	m_sForeignAddress.sin_family      = AF_INET;
	m_sForeignAddress.sin_port        = 0;
	m_sForeignAddress.sin_addr.s_addr = 0;
	memset(&m_sForeignAddress.sin_zero, 0, sizeof(m_sForeignAddress.sin_zero));
}


// ======================================================================

PUDPSocket::~PUDPSocket()
{
}


// ======================================================================

bool PUDPSocket::Bind(const char *pszIpAddress, pfc_inet_port nPort)
{
	return Bind(inet_addr(pszIpAddress), htons(nPort));
}


// ======================================================================

bool PUDPSocket::Bind(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	if(m_nSocket != PFC_SOCKET_INVALID)
	{
		m_sLocalAddress.sin_port        = nPort;
		m_sLocalAddress.sin_addr.s_addr = nIpAddress;

		if(bind(m_nSocket, (struct sockaddr*) &m_sLocalAddress, sizeof(m_sLocalAddress)) == 0)
		{
			// successfully bound
			m_bIsBound = true;

			return true;
		}
		else
		{
			// error
			m_sLocalAddress.sin_addr.s_addr = 0;
			m_sLocalAddress.sin_port        = 0;

			return false;
		}
	}

	return false;
}


// ======================================================================

// TODO: implement bool PUDPSocket::Connect(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
bool PUDPSocket::Connect(pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	if(m_bIsBound != true)
	{
		if(Bind((pfc_inet_addr)PFC_INET_ADDR_ANY, (pfc_inet_port)PFC_INET_PORT_ANY) != true)
			return false;
	}

	return false;
}


// ======================================================================

int PUDPSocket::Send(const char *pcBuffer, int nBufferSize)
{
	if(m_bIsBound != true)
	{
		if(Bind((pfc_inet_addr)PFC_INET_ADDR_ANY, (pfc_inet_port)PFC_INET_PORT_ANY) != true)
			return false;
	}

	if(m_sForeignAddress.sin_addr.s_addr != 0 && m_sForeignAddress.sin_port != 0)
		return sendto(m_nSocket, pcBuffer, nBufferSize, 0, (struct sockaddr*) &m_sForeignAddress, sizeof(m_sForeignAddress));
	else
		return 0;
}


// ======================================================================

int PUDPSocket::SendTo(const char *pcBuffer, int nBufferSize, pfc_inet_addr nIpAddress, pfc_inet_port nPort)
{
	if(m_bIsBound != true)
	{
		if(Bind((pfc_inet_addr)PFC_INET_ADDR_ANY, (pfc_inet_port)PFC_INET_PORT_ANY) != true)
			return false;
	}

	if(nIpAddress != 0 && nPort != 0)
	{
		m_sForeignAddress.sin_addr.s_addr = nIpAddress;
		m_sForeignAddress.sin_port        = nPort;

		return sendto(m_nSocket, pcBuffer, nBufferSize, 0, (struct sockaddr*) &m_sForeignAddress, sizeof(m_sForeignAddress));
	}
	else
		return 0;
}


// ======================================================================

int PUDPSocket::Receive(char *pcBuffer, int nBufferSize)
{
	if(m_bIsBound != true)
		return false;

	m_nForeignAddressLength = sizeof(m_sForeignAddress);

	return recvfrom(m_nSocket, pcBuffer, nBufferSize, 0, (struct sockaddr*) &m_sForeignAddress, &m_nForeignAddressLength);
}



