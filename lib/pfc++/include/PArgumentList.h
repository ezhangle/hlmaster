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


#ifndef _PARGUMENTLIST_H_
#define _PARGUMENTLIST_H_



// Classes defined in this document
// ---------------------------------
// 
class PArgumentList;
class PArgumentListItem;



// Header includes
// ---------------------------------
// 
#include "PStringList.h"

#include <ctype.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif


// Class definition
// ---------------------------------
//
class PArgumentList
{
public:

	PArgumentList();
	PArgumentList(bool bIgnoreCase);
	~PArgumentList();

	void SetIgnoreCase(bool bIgnore = true);
	bool GetIgnoreCase();

	/**
	format: "-a:--name:-b=:-b=default value"
	options are separated by ":"

	 o "-a"        -> short option, no value
	 o "--name"    -> long option, no value
     o a '=' marks -> this option requires a value. text after the '=' is treated as the default value (if no value found)

	 short options maximum length is 1 character! short options may be combined
	 on the command-line (eg. "program -abcf file.txt").
	*/
	// TODO: implement function
	void AddOptions(const char *pszOptionString);

	void AddOption(const char cShortOption, bool bRequireValue = false, const char *pszDefaultValue = NULL);
	void AddOption(const char *pszLongOption, bool bRequireValue = false, const char *pszDefaultValue = NULL);
	void AddOption(const char cShortOption, const char *pszLongOption, bool bRequireValue = false, const char *pszDefaultValue = NULL);

	// TODO: implement function
	bool RemoveOption(const char cShortOption, bool bRemoveLong = true);
	// TODO: implement function
	bool RemoveOption(const char *pszLongOption, bool bRemoveShort = true);
	int RemoveAllOptions();

	int ParseArguments(const char *pszArguments[], int nArguments);
	// TODO: implement function
	void ParseArgument(const char *pszArgument);


	// enable & disable options manually
	bool Enable(const char cShortOption, bool bEnable = true);
	bool Enable(const char *pszLongOption, bool bEnable = true);

	bool Disable(const char cShortOption) { return Enable(cShortOption, false); }
	bool Disable(const char *pszLongOption) { return Enable(pszLongOption, false); }

	// ask status
	bool IsEnabled(const char cShortOption);
	bool IsEnabled(const char *pszLongOption);

	// ask value
	bool HasValue(const char cShortOption);
	bool HasValue(const char *pszLongOption);
	const char* GetValue(const char cShortOption, bool bReturnDefault = true);
	const char* GetValue(const char *pszLongOption, bool bReturnDefault = true);

	const char* GetDefaultValue(const char cShortOption);
	const char* GetDefaultValue(const char *pszLongOption);


	int GetNumArguments();
	// TODO: implement function
	int GetNumGoodArguments();
	int GetNumUnknownArguments() { return m_pUnknownArgumentsList->GetNumItems(); }
	int GetNumUnknownValues()    { return m_pUnknownValuesList->GetNumItems(); }

	PStringList* GetUnknownArgumentsList() { return m_pUnknownArgumentsList; }
	PStringList* GetUnknownValuesList()    { return m_pUnknownValuesList; }


	// TODO: test
	bool PickCurrentFirst();
	bool PickCurrentLast();
	bool PickCurrentNext();
	bool PickCurrentPrevious();

	bool IsCurrentEqual(const char cShortOption);
	bool IsCurrentEqual(const char *pszLongOption);

	const char GetCurrentShortOption();
	const char* GetCurrentLongOption();

	bool EnableCurrent(bool bEnable = true);
	bool DisableCurrent() { return EnableCurrent(false); }

	bool IsCurrentValid();
	bool IsCurrentEnabled();
	bool HasCurrentValue();

	const char* GetCurrentValue(bool bReturnDefault = true);
	const char* GetCurrentDefaultValue();
	// end to test


private:

	PArgumentListItem* GetItem(const char cShortOption);
	PArgumentListItem* GetItem(const char *pszLongOption);
	PArgumentListItem* GetItem(const char cShortOption, const char *pszLongOption);


	bool m_bIgnoreCase;

	PStringList *m_pUnknownArgumentsList;
	PStringList *m_pUnknownValuesList;

	PArgumentListItem *m_pFirstItem;
	PArgumentListItem *m_pLastItem;

	PArgumentListItem *m_pCurrentItem;

	int m_nItems;
};




class PArgumentListItem
{
public:

	PArgumentListItem(const char cShortOption, const char *pszLongOption, bool bRequireValue, const char *pszDefaultValue);
	~PArgumentListItem();

	void SetName(const char cShortOption) { m_cShortName = cShortOption; };
	void SetName(const char *pszLongOption);

	void SetValue(const char *pszValue);
	void SetDefaultValue(const char *pszDefaultValue);

	void Enable(bool bEnable = true) { m_bIsEnabled = bEnable; }
	void Disable() { m_bIsEnabled = false; }

	bool Equals(const char cShortOption, bool bIgnoreCase = false)
	{
		if(m_cShortName == 0 || cShortOption == 0)
			return false;


		// TODO: improve performance
		if(bIgnoreCase == true)
		{
			if(toupper(m_cShortName) == toupper(cShortOption))
				return true;
			else
				return false;
		}
		else
		{
			if(m_cShortName == cShortOption)
				return true;
			else
				return false;
		}
	}

	bool Equals(const char *pszLongOption, bool bIgnoreCase = false)
	{
		if(m_pszLongName == NULL || pszLongOption == NULL)
			return false;

		int nCount;

		// TODO: improve performance
		if(bIgnoreCase == true)
		{
			for(nCount = 0; pszLongOption[nCount] != '\0' && m_pszLongName[nCount] != '\0'; nCount++)
			{
				if(tolower(pszLongOption[nCount] != tolower(m_pszLongName[nCount])))
					return false;
			}

			if(pszLongOption[nCount] == '\0' && m_pszLongName[nCount] == '\0')
				return true;
			else
				return false;
		}
		else
		{
			if(strcmp(m_pszLongName, pszLongOption) == 0)
				return true;
			else
				return false;
		}
	}

private:

	friend class PArgumentList;

	char  m_cShortName;
	char *m_pszLongName;

	static const char *m_pszVoidValue;
	char *m_pszValue;
	char *m_pszDefaultValue;

	bool  m_bRequireValue;
	bool  m_bIsEnabled;


	PArgumentListItem *m_pNext;
	PArgumentListItem *m_pPrevious;
};


#endif		// #ifndef _PARGUMENTLIST_H_
