/*
* $Logfile: /DescentIII/Main/Dmfc/idmfc.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003/08/26 03:57:21 $
* $Author: kevinb $
*
* DMFC COM Interface implementation
*
* $Log: idmfc.cpp,v $
* Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
* initial 1.5 import
*
 * 
 * 3     7/16/99 2:43p Jeff
 * changed CreateMenuItemWArgs function
 * 
 * 2     3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 * 
 * 1     3/17/99 11:54a Jeff
*
* $NoKeywords: $
*/

#include "idmfc.h"
#include "dmfcinternal.h"

/*
***********************************************
					IDMFC
***********************************************
*/
DMFCBase *basethis = NULL;
IDMFC DLLFUNCCALLPTR CreateDMFC(void)
{
	if(basethis)
	{
		return NULL;
	}

	basethis = new DMFCBase;
	IDMFC *pfsResult = basethis;
	if(pfsResult)
	{
		pfsResult->DuplicatePointer();
	}

	return pfsResult;
}

void DMFCBase::Delete(void)
{
	delete this;
	basethis = NULL;
}

void *DMFCBase::Dynamic_Cast(const char *pszType)
{
	void *pvResult = NULL;

	if(strcmp(pszType, "IDMFC" ) == 0 )
		pvResult = static_cast<IDMFC*>(this);
	else
		return NULL;

	((IObject *)pvResult)->DuplicatePointer();
	return pvResult;
}

void DMFCBase::DuplicatePointer(void)
{
	++m_cPtrs;
}

void DMFCBase::DestroyPointer(void)
{
	if(--m_cPtrs==0)
	{
		basethis = NULL;
		delete this;
	}
}

/*
***********************************************
				IMenuItem
***********************************************
*/

IMenuItem DLLFUNCCALLPTR CreateMenuItem(void)
{
	IMenuItem *pfsResult = new MenuItem;
	if(pfsResult)
		pfsResult->DuplicatePointer();
	return pfsResult;
}

IMenuItem DLLFUNCCALLPTR CreateMenuItemWArgs(char *title,char type,ubyte flags,void (*fp)(int),tCustomMenu *custom_menu)
{
	MenuItem *p;

	if(type!=MIT_CUSTOM && type!=MIT_STATE){
		p = new MenuItem(title,type,flags,fp);
	}else{
		if(type==MIT_CUSTOM){
			ASSERT(custom_menu!=NULL);
			if(custom_menu==NULL)
				return NULL;

			p = new MenuItem(title,type,flags,fp,custom_menu);
		}else{
			p = new MenuItem(title,type,flags,fp,0,0);	//set initially to 0 state items
		}
	}

	if(p)
		p->DuplicatePointer();

	return p;
}

void MenuItem::Delete(void)
{
	delete this;
}

void *MenuItem::Dynamic_Cast(const char *pszType)
{
	void *pvResult = NULL;

	if(strcmp(pszType, "IMenuItem" ) == 0 )
		pvResult = static_cast<IMenuItem*>(this);
	else
		return NULL;

	((IObject *)pvResult)->DuplicatePointer();
	return pvResult;
}

void MenuItem::DuplicatePointer(void)
{
	++m_cPtrs;
}

void MenuItem::DestroyPointer(void)
{
	if(--m_cPtrs==0)
		delete this;
}

/*
***********************************************
				IDmfcStats
***********************************************
*/
IDmfcStats DLLFUNCCALLPTR CreateDmfcStats(void)
{
	IDmfcStats *pfsResult = new CDmfcStats;
	if(pfsResult)
		pfsResult->DuplicatePointer();
	return pfsResult;
}


void CDmfcStats::Delete(void)
{
	delete this;
}

void *CDmfcStats::Dynamic_Cast(const char *pszType)
{
	void *pvResult = NULL;

	if(strcmp(pszType, "IDmfcStats" ) == 0 )
		pvResult = static_cast<IDmfcStats*>(this);
	else
		return NULL;

	((IObject *)pvResult)->DuplicatePointer();
	return pvResult;
}

void CDmfcStats::DuplicatePointer(void)
{
	++m_cPtrs;
}

void CDmfcStats::DestroyPointer(void)
{
	if(--m_cPtrs==0)
		delete this;
}
