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


#ifndef _PLOGFILE_H_
#define _PLOGFILE_H_



// Classes defined in this document
// ---------------------------------
// 
class PLogfile;



// Header includes
// ---------------------------------
// 
#include <pfc++/file.h>

#include "PFile.h"
#include "CStringBuffer.h"
#include "CInetAddress.h"

#include <string.h>
#include <time.h>




// Class definition
// ---------------------------------
//
class PLogfile
{
public:

	PLogfile(const char *pszFilename, int nLevel);
	~PLogfile();

	bool Open();
	bool ReOpen();
	void Close();

	const char* GetFilename() { return m_pFileName->GetValue(); }

	void SetLogLevel(int nLevel);
	int GetLogLevel() { return m_nLevel; };

	bool CheckLevel(int nLevel)
	{
		if(nLevel > 0 && nLevel <= m_nLevel)
			return true;
		else
			return false;
	}

	bool Begin(int nLevel);
	void Cancel();
	void End() { Flush(); }
	void Flush();

	bool Append(const char *pszMessage);
	bool Append(const char cValue);
	bool Append(int nValue);
	bool Append(pfc::CInetAddress *pInetAddr);
	bool WriteLine(const char *pszLine, int nLevel);


	static const int c_LEVEL_NONE;			// = 0;
	static const int c_LEVEL_CRITICAL;		// = 1;
	static const int c_LEVEL_ERROR;			// = 2;
	static const int c_LEVEL_WARNING;		// = 3;
	static const int c_LEVEL_INFO;			// = 4;
	static const int c_LEVEL_DEBUG;			// = 5;



private:

	bool m_bStarted;

	int m_nLevel;
	bool m_bIsOpen;
	FILE *m_pFile;

	char m_szDateBuffer[20];

	pfc::CStringBuffer *m_pLineBuffer;
	PString *m_pFileName;
};



#endif		// #ifndef _PLOGFILE_H_
