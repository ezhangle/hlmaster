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
   | Created.......: Wednesday, 16. May 2001                            |
   | Author........: Silvan Minghetti <bullet@users.sourceforge.net>    |
   | Description...: Portable C++ framework classes for linux/windows.  |
   +--------------------------------------------------------------------+
*/

#include <pfc++/system.h>
#include <pfc++/file.h>

#include "PLogfile.h"
#include "CStringBuffer.h"



// ======================================================================

PLogfile::PLogfile(const char *pszFilename, int nLevel)
{
	if(pszFilename == NULL)
		m_pFileName = new PString(PFC_MAX_PATH, "unnamed.log");
	else
		m_pFileName = new PString(PFC_MAX_PATH, pszFilename);

	m_bIsOpen  = false;
	m_bStarted = false;

	if(Open() != true)
		throw PFileException();

	m_pLineBuffer = new pfc::CStringBuffer(256);

	SetLogLevel(nLevel);
}


// ======================================================================

PLogfile::~PLogfile()
{
	Close();

	delete m_pFileName;
	delete m_pLineBuffer;
}


// ======================================================================

void PLogfile::SetLogLevel(int nLevel)
{
	if(nLevel >= 0 && nLevel <= c_LEVEL_DEBUG)
		m_nLevel = nLevel;
	else
		m_nLevel = c_LEVEL_DEBUG;
}

// ======================================================================

const int PLogfile::c_LEVEL_NONE     = 0;
const int PLogfile::c_LEVEL_CRITICAL = 1;
const int PLogfile::c_LEVEL_ERROR    = 2;
const int PLogfile::c_LEVEL_WARNING  = 3;
const int PLogfile::c_LEVEL_INFO     = 4;
const int PLogfile::c_LEVEL_DEBUG    = 5;



// ======================================================================

bool PLogfile::Open()
{
	if(m_bIsOpen == true)
		return true;

	#if defined PFC_SYSTEM_TYPE_WINDOWS
	m_pFile = fopen(m_pFileName->GetValue(), "a");
	#elif defined PFC_SYSTEM_TYPE_UNIX

	mode_t mode_prev;
	// TODO: this is stoopid, no need to set/unset umask for that...
	mode_prev = umask(S_IWGRP | S_IWOTH);

	m_pFile = fopen(m_pFileName->GetValue(), "a");

	// reset umask
	umask(mode_prev);
	#else
	#error Unknown operating system. Only WIN32 / LINUX are recognized.
	#endif

	if(m_pFile == NULL)
		return false;

	m_bIsOpen = true;

	return true;
}


// ======================================================================

void PLogfile::Flush()
{
	if(m_bIsOpen == true && m_bStarted == true)
	{
		m_pLineBuffer->Append('\n');
		fwrite(m_pLineBuffer->GetCharPointer(), sizeof(char), m_pLineBuffer->GetLength(), m_pFile);

		fflush(m_pFile);
	}

	m_pLineBuffer->Clear();
	m_bStarted = false;
}


// ======================================================================

void PLogfile::Close()
{
	if(m_bIsOpen != true)
		return;

	Flush();
	fclose(m_pFile);

	m_bIsOpen = false;
}


// ======================================================================

bool PLogfile::ReOpen()
{
	this->Close();

	return this->Open();
}


// ======================================================================

bool PLogfile::Begin(int nLevel)
{
	if(CheckLevel(nLevel) != true)
		return false;

	if(m_bStarted == true)
		Flush();

	m_pLineBuffer->Append('[');
	m_pLineBuffer->Append(nLevel);
	m_pLineBuffer->Append(']');
	m_pLineBuffer->Append(' ');

	time_t nTimestamp = time(NULL);
	struct tm *pTime  = localtime(&nTimestamp);

	if(pTime != NULL)
		sprintf(m_szDateBuffer, "%04i-%02i-%02i %02i:%02i:%02i", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	else
		strcpy(m_szDateBuffer, "0000-00-00 00:00:00");

	m_pLineBuffer->Append(m_szDateBuffer);
	m_pLineBuffer->Append("> ");

	m_bStarted = true;
	return true;
}


// ======================================================================

void PLogfile::Cancel()
{
	m_bStarted = false;
	m_pLineBuffer->Clear();
}


// ======================================================================

bool PLogfile::WriteLine(const char *pszLine, int nLevel)
{
	if(Begin(nLevel) != true)
		return false;

	m_pLineBuffer->Append(pszLine);

	Flush();

	return true;
}


// ======================================================================

bool PLogfile::Append(const char *pszMessage)
{
	if(m_bStarted != true)
		return false;

	return m_pLineBuffer->Append(pszMessage);
}


// ======================================================================

bool PLogfile::Append(const char cValue)
{
	if(m_bStarted != true)
		return false;

	return m_pLineBuffer->Append(cValue);
}


// ======================================================================

bool PLogfile::Append(int nValue)
{
	if(m_bStarted != true)
		return false;

	return m_pLineBuffer->Append(nValue);
}


// ======================================================================

bool PLogfile::Append(pfc::CInetAddress *pInetAddr)
{
	if(m_bStarted != true)
		return false;

	return m_pLineBuffer->Append(pInetAddr);
}

