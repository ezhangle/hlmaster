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

#include <pfc++/system.h>

#include "PSocket.h"

#include <stdio.h>


// ======================================================================

PSocket::PSocket(int nAddressFamily, int nType)
{
	m_nSocket = socket(nAddressFamily, nType, 0);

	if(m_nSocket == PFC_SOCKET_INVALID)
	{
#ifdef PFC_SYSTEM_TYPE_WINDOWS
		throw PSocketException(WSAGetLastError());
#else
		throw PSocketException(errno);
#endif
	}

	m_bIsBound = false;

	SetWaitTimeout(PFC_WAIT_INFINITE);
}


// ======================================================================

PSocket::PSocket(PFC_SOCKET nExistingSocket)
{
	// TODO: implement PSocket::PSocket(PFC_SOCKET nExistingSocket)
}


// ======================================================================

PSocket::~PSocket()
{
	if(m_nSocket != PFC_SOCKET_INVALID)
	{
	#ifdef PFC_SYSTEM_TYPE_WINDOWS
		closesocket(m_nSocket);
	#else
		close(m_nSocket);
	#endif
	}
}


// ======================================================================

bool PSocket::WaitForInput()
{
	fd_set sReadSet;

	FD_ZERO(&sReadSet);
	FD_SET(m_nSocket, &sReadSet);

	if(m_pTimeout != NULL)
	{
		m_sTimeout.tv_sec  = m_nTimeoutSeconds;
		m_sTimeout.tv_usec = m_nTimeoutMilliseconds;
	}

	if(select(m_nSocket + 1, &sReadSet, NULL, NULL, m_pTimeout) == 1)
		return true;
	else
		return false;
}


// ======================================================================

bool PSocket::WaitForOutput()
{
	fd_set sWriteSet;

	FD_ZERO(&sWriteSet);
	FD_SET(m_nSocket, &sWriteSet);

	if(m_pTimeout != NULL)
	{
		m_sTimeout.tv_sec  = m_nTimeoutSeconds;
		m_sTimeout.tv_usec = m_nTimeoutMilliseconds;
	}

	if(select(m_nSocket + 1, NULL, &sWriteSet, NULL, m_pTimeout) == 1)
		return true;
	else
		return false;
}


// ======================================================================

bool PSocket::WaitForError()
{
	fd_set sErrorSet;

	FD_ZERO(&sErrorSet);
	FD_SET(m_nSocket, &sErrorSet);

	if(m_pTimeout != NULL)
	{
		m_sTimeout.tv_sec  = m_nTimeoutSeconds;
		m_sTimeout.tv_usec = m_nTimeoutMilliseconds;
	}

	if(select(m_nSocket + 1, NULL, NULL, &sErrorSet, m_pTimeout) == 1)
		return true;
	else
		return false;
}


// ======================================================================

void PSocket::SetWaitTimeout(unsigned int nMilliseconds)
{
	if(nMilliseconds == PFC_WAIT_INFINITE)
	{
		m_nTimeoutSeconds      = PFC_WAIT_INFINITE;
		m_nTimeoutMilliseconds = 0;

		m_pTimeout = NULL;
	}
	else if(nMilliseconds <= 0)
	{
		m_nTimeoutSeconds      = 0;
		m_nTimeoutMilliseconds = 0;
		m_pTimeout = &m_sTimeout;
	}
	else
	{
		m_nTimeoutSeconds      = nMilliseconds / 1000;
		m_nTimeoutMilliseconds = (nMilliseconds % 1000) * 1000;
		m_pTimeout = &m_sTimeout;
	}
}




