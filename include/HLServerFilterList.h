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


#ifndef _HLSERVERFILTERLIST_H_
#define _HLSERVERFILTERLIST_H_


#ifndef NULL
#define NULL 0
#endif



// Classes defined in this document
// ---------------------------------
// 
class HLServerFilterList;
class HLServerFilterList_Item;



// Header includes
// ---------------------------------
// 
#include "PLogfile.h"


// Class definition
// ---------------------------------
//
class HLServerFilterList
{
public:

	HLServerFilterList();
	~HLServerFilterList();

	bool AddDefaultHost(bool bAllow);
	bool AddHost(bool bAllow, pfc_inet_addr nIpAddress);
	bool AddHost(bool bAllow, const char *pszIpAddress);
	bool AddHost(bool bAllow, const char *pszIpAddress, const char *pszNetMask);
	bool AddHost(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask);

	bool Exists(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask);
	bool IsAllowed(pfc_inet_addr nIpAddress);
	bool IsAllowed(const char *pszIpAddress);

	void SetOrderAllow()  { m_bOrderAllow = true; }
	void SetOrderDeny()   { m_bOrderAllow = false; }
	bool IsOrderAllow()   { return m_bOrderAllow; }

	void SetDefaultAllow(){ m_bDefaultAllow = true; }
	void SetDefaultDeny() { m_bDefaultAllow = false; }
	bool IsDefaultAllow() { return m_bDefaultAllow; }

	bool RemoveDefaultHost(bool bAllow);
	bool RemoveHost(bool bAllow, pfc_inet_addr nIpAddress);
	bool RemoveHost(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask);
	bool RemoveHost(bool bAllow, const char *pszIpAddress);
	bool RemoveHost(bool bAllow, const char *pszIpAddress, const char *pszNetMask);

	int  RemoveAll();

	int Load(const char *pszFilename, PLogfile *pLogfile = NULL);


private:

	int m_nItems;

	HLServerFilterList_Item* m_pFirstItem;
	HLServerFilterList_Item* m_pLastItem;

	bool m_bOrderAllow;
	bool m_bDefaultAllow;
};




class HLServerFilterList_Item
{
public:

	HLServerFilterList_Item(bool bAllow, pfc_inet_addr nIpAddress, pfc_inet_addr nNetMask);


private:

	friend class HLServerFilterList;

	bool m_bAllow;
	pfc_inet_addr m_nIpAddress;
	pfc_inet_addr m_nMaskedIpAddress;
	pfc_inet_addr m_nNetMask;


	HLServerFilterList_Item* m_pNextItem;
};




#endif		// #ifndef _HLSERVERFILTERLIST_H_
