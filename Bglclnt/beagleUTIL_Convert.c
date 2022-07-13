/* beagleUTIL_Convert.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Nov 21 15:29:35 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Jan 25 13:31:57 2000
 * Update Count    : 36
 * Status          : 
 */

#include "clnt.h"
#include "beagleUTIL_Convert.h"


/*
 *  This file is include utility functions for Beagle
 *  about Overviews data management.
 *
 *                             [data]
 *
 *                            ipName_t
 *                         +-------------+
 *                         |    next     | ----> point to nex data
 *                         +-------------+
 *                         |    ip_t     |
 *                         +-------------+
 *                         | char* nameP |
 *                         +-------------+
 *
 *                  typedef struct in_addr    ip_t;
 *
 */


/*
 *- SPECIAL VALIABLE NAME/TYPE
 *- ===========================
 */
struct IPNAME_T{
  struct IPNAME_T*	next;
  ip_t	ip;
  char*	nameP;
};
typedef struct IPNAME_T	ipName_t;



/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 */


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static ipName_t		ipNameListSentinel;
static ipName_t*	ipNameRootP = &ipNameListSentinel;

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static char*		GetNameByIp	P_((ip_t*));
static char*		IpToName	P_((ip_t*));
static ip_t*		NameToIp	P_((char*));
static ipName_t*	CreateIpNameData	P_((ip_t*,char*));
static void		DestroyIpNameData	P_((ipName_t*));
static ipName_t*	AddIpNameData	P_((ipName_t*));


/*
 *- HERE ARE SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleUTIL_SockMgr.h'.
 */
int	UTIL_Convert_Initialize()
{
  ipName_t*	dP;
  while( (dP=ipNameRootP) != &ipNameListSentinel){
    ipNameRootP = dP->next;
    DestroyIpNameData(dP);
  }
  return 0;	/* NO ERROR */
}


ip_t*	UTIL_Convert_NameToIp(nameP)
     char* nameP;
{
  ip_t*	ipP;
  ipP = NameToIp(nameP);
  if ( GetNameByIp(ipP) == NULL ){	/* This is not entry */
    (void)AddIpNameData( CreateIpNameData(ipP, nameP) );
  }
  return ipP;
}


char*	UTIL_Convert_IpToName(ipP)
     ip_t* ipP;
{
  char* nameP;
  ipName_t*	ipNameDataP;
  if ( (nameP=GetNameByIp(ipP)) != NULL )	return nameP; /* Found! */
  else{
    ipNameDataP = AddIpNameData( CreateIpNameData(ipP, IpToName(ipP) ) );
    return ipNameDataP->nameP;
  }
}

/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static char*	GetNameByIp(ipP)
     ip_t* ipP;
{
  ipName_t*	dP = ipNameRootP;
  ipNameListSentinel.ip.s_addr = ipP->s_addr;
  while(dP->ip.s_addr != ipP->s_addr)    dP = dP->next;
  if ( dP == &ipNameListSentinel )	return NULL;	/* Not found */
  else					return dP->nameP;
}


static char*	IpToName(ipP)
     ip_t* ipP;
{
  struct hostent* hP;
  if ( (hP = gethostbyaddr((char*)ipP, sizeof(ip_t), AF_INET) ) != NULL )
    return hP->h_name;	/* No ERROR, machine name return */
  else
    return inet_ntoa(*(struct in_addr*)ipP);/* dotted address string return */
}


static ip_t*	NameToIp(nameP)
     char* nameP;
{
  struct hostent* hP;
  static ip_t	nullIp;
  if ( (hP = tgethostbyname(nameP)) != NULL ) return (ip_t*)hP->h_addr_list[0];
  else{
    nullIp.s_addr = 0;
    return &nullIp;
  }
}


static ipName_t*	CreateIpNameData(ipP, nameP)
     ip_t* ipP;
     char* nameP;
{
  ipName_t*	retP;
  retP = (ipName_t*)tcalloc(1, sizeof(ipName_t), 60000);
  retP->ip = *ipP;
  retP->nameP = (char*)tcalloc(1, strlen(nameP)+1, 60001);
  strcpy(retP->nameP, nameP);
  return retP;
}


static void	DestroyIpNameData(ipNameP)
     ipName_t* ipNameP;
{
  tfree(ipNameP->nameP, strlen(ipNameP->nameP), 60001);
  tfree(ipNameP, sizeof(ipName_t), 60000);
  return;
}


static ipName_t*	AddIpNameData(ipNameP)
     ipName_t* ipNameP;
{
  ipNameP->next = ipNameRootP;
  ipNameRootP = ipNameP;
  return ipNameP;
}
