/* beagleUTIL_SockMgr.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Thu Apr  3 17:26:19 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri Jun  4 14:14:25 1999
 * Update Count    : 132
 * Status          : 
 */

#include "clnt.h"
#include "beagleUI.h"
#include "beagleUTIL_SockMgr.h"


/*
 *  This file is include utility functions for Beagle
 *  about BglSockSt data management.
 *
 *      [data]
 *       sockStRoot                                      List END
 *      +---------+                                    +-----------+
 *      |  next   | ----> point to nex data...  ---->  | next = 0  |
 *      +---------+                                    +-----------+
 *      |BglSockSt|                                    |BglSockSt=0|
 *      +---------+                                    +-----------+
 *
 *  ( sockStRoot.next == NULL ) means this Beagle does not hove CONNECTION.
 *
 */
struct SockStList_T {
  struct SockStList_T*	next;
  BglSockSt		sock;
};
typedef struct SockStList_T	SockStList_t;

/*
 *- SPECIAL VALIABLE NAME/TYPE
 *- ===========================
 */


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 */
extern I32s	BglMM_ChangeMigrCtrlPort P_((void));
extern int	BglMM_IsOvSession_stop	P_((void));
extern I32s	BglMM_OvSession_modeReset	P_((void));
extern I32s	BglMM_OvSession_need	P_((void));
extern int	BglMM_IsInfoSize_stop	P_((void));
extern I32s	BglMM_InfoSize_modeReset	P_((void));
extern I32s	BglMM_Info_s_need	P_((void));
extern int	BglMM_IsInfoGene_stop	P_((void));
extern I32s	BglMM_InfoGene_modeReset	P_((void));
extern I32s	BglMM_Info_g_need	P_((void));
extern int	BglMM_IsInfoMem_stop	P_((void));
extern I32s	BglMM_InfoMem_modeReset	P_((void));
extern I32s	BglMM_Info_m_need	P_((void));
extern int	BglMM_IsInfoES_stop	P_((void));
extern I32s	BglMM_InfoES_modeReset	P_((void));
extern I32s	BglMM_Info_es_need	P_((void));
extern int	BglMM_IsInfoEG_stop	P_((void));
extern I32s	BglMM_InfoEG_modeReset	P_((void));
extern I32s	BglMM_Info_eg_need	P_((void));
extern int	BglMM_IsPlan_stop	P_((void));
extern I32s	BglMM_Plan_modeReset	P_((void));
extern I32s	BglMM_Plan_need		P_((void));
extern int	BglMM_IsVar_stop	P_((void));
extern I32s	BglMM_Var_modeReset	P_((void));
extern I32s	BglMM_Var_need		P_((void));/* default 'NEED' */
extern int	BglMM_IsTPingS_stop	P_((void));
extern int	BglMM_IsTPingC_stop	P_((void));
extern I32s	BglMM_TPingS_modeReset	P_((void));
extern I32s	BglMM_TPingC_modeReset	P_((void));
extern I32s	BglMM_Misc_tSOn_need	P_((void));
extern I32s	BglMM_Misc_tCOn_need	P_((void));


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
/* sockStRoot has a current connection information */
static SockStList_t	sockStRoot = { (SockStList_t*)0,  0, };
static BglSockSt	preconnectionSocket;
static int	connectionState = 0; /* 1: changed */
static int	overviewSession = 0;
static int	sizeGraphSession = 0;
static int	geneGraphSession = 0;
static int	memGraphSession  = 0;
static int	efficSizeGraphSession = 0;
static int	efficGeneGraphSession = 0;
static int	planSession = 0;
static int	varSession = 0;
static int	tPingSSession = 0;
static int	tPingCSession = 0;


/*
 *- HERE ARE SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 *- I wrote prototype define in 'beagleUTIL_SockMgr.h'.
 */


/**********************************************************************
 * Connection Buffer
 */
void		UTIL_Sock_Push_Connection(sP)
     BglSockSt* sP;
{
  SockStList_t*	newP;

  newP = (SockStList_t*)tcalloc(1,sizeof(SockStList_t),52100);
  newP->sock = sockStRoot.sock;
  newP->next = sockStRoot.next;
  sockStRoot.next = newP;
  sockStRoot.sock = *sP;
  BglMM_ChangeMigrCtrlPort();/* Current Connection had Changed then call it */
}

void		UTIL_Sock_Pop_Connection()
{
  SockStList_t*	p;

  p = sockStRoot.next;
  sockStRoot.sock = sockStRoot.next->sock;
  sockStRoot.next = sockStRoot.next->next;
  tfree( p, sizeof(SockStList_t), 52100 );
  BglMM_ChangeMigrCtrlPort();/* Current Connection had Changed then call it */
}

void		UTIL_Sock_Swap_Connection(connection, fd)
     int connection;
     I32s fd;
{
  int	num;
  SockStList_t*	targetP;
  SockStList_t*	beforeP;
  SockStList_t	tmpS;

  if ( connection == 0 ) return; /* Not need Swap */
  beforeP = &sockStRoot;
  for (num = 1 ; beforeP->next->next && (num < connection) ; num++){
    beforeP = beforeP->next;
  }
  targetP = beforeP->next;
  if ( !targetP->next )	return; /* could not found "connection" data */

  beforeP->next = targetP->next; /* extract target data from list */

  tmpS	= sockStRoot;
  sockStRoot.sock = targetP->sock;
  sockStRoot.next = targetP;
  sockStRoot.sock.fd = fd;
  *targetP = tmpS;
  BglMM_ChangeMigrCtrlPort();/* Current Connection had Changed then call it */
}

BglSockSt*	UTIL_Sock_GetCurrent_Connection()
{
  if (sockStRoot.next)
    return &(sockStRoot.sock);
  else
    return NULL;
}

BglSockSt*	UTIL_Sock_Get_Connection(connection)
     int connection;
{
  int	num = 0;
  SockStList_t*	p;
  
  p = sockStRoot.next;
  while(p)
    {
      num++;
      if (( num == connection ) && (p->next))
	return &(p->sock);
      p = p->next;
    }
  return NULL;
}

/**********************************************************************
 * Pre-connection Buffer
 */
void		UTIL_Sock_Push_PreConnection(sP)
     BglSockSt* sP;
{
  preconnectionSocket = *sP;
}

BglSockSt*	UTIL_Sock_Pop_PreConnection()
{
  return &preconnectionSocket;
}

/**********************************************************************
 * Virtual Connection
 */
int	UTIL_Sock_IsConnection_Changed()
{
  if ( connectionState ){
    connectionState = 0;
    return 1;
  }
  else return 0;
}

int	UTIL_Sock_Close_VirtualConnection()
{
  if ( BglMM_IsOvSession_stop() ){
    overviewSession = 0;
  }
  else{
    BglMM_OvSession_modeReset();
    overviewSession = 1;
  }

  if ( BglMM_IsInfoSize_stop() ){
    sizeGraphSession = 0;
  }
  else{
    BglMM_InfoSize_modeReset();
    sizeGraphSession = 1;
  }

  if ( BglMM_IsInfoGene_stop() ){
    geneGraphSession = 0;
  }
  else{
    BglMM_InfoGene_modeReset();
    geneGraphSession = 1;
  }

  if ( BglMM_IsInfoMem_stop() ){
    memGraphSession = 0;
  }
  else{
    BglMM_InfoMem_modeReset();
    memGraphSession = 1;
  }

  if ( BglMM_IsInfoES_stop() ){
    efficSizeGraphSession = 0;
  }
  else{
    BglMM_InfoES_modeReset();
    efficSizeGraphSession = 1;
  }

  if ( BglMM_IsInfoEG_stop() ){
    efficGeneGraphSession = 0;
  }
  else{
    BglMM_InfoEG_modeReset();
    efficGeneGraphSession = 1;
  }

  if ( BglMM_IsPlan_stop() ){
    planSession = 0;
  }
  else{
    BglMM_Plan_modeReset();
    planSession = 1;
  }

  if ( BglMM_IsVar_stop() ){
    varSession = 0;
  }
  else{
    BglMM_Var_modeReset();
    varSession = 1;
  }

  if ( BglMM_IsTPingS_stop() ){
    tPingSSession = 0;
  }
  else{
    BglMM_TPingS_modeReset();
    tPingSSession = 1;
  }

  if ( BglMM_IsTPingC_stop() ){
    tPingCSession = 0;
  }
  else{
    BglMM_TPingC_modeReset();
    tPingCSession = 1;
  }


  if ( Bgl_step == 1 ){/* == MICRO Debugger Keypress-mode == */
    Bgl_step = -1;
    BglUI_MicroDebuggerToOffMode();
  }
  else if ( Bgl_step == 0 ){ /* == MICRO Debugger Delay-mode == */
    Bgl_step = -1;
    BglUI_MicroDebuggerToOffMode();
  }
  else{ /* == MICRO Debugger Off == */
    /* NOP */
  }

  return 0;
}


int	UTIL_Sock_Open_VirtualConnection()
{
  connectionState = 1; /* Connection changed */
  if ( overviewSession )		BglMM_OvSession_need();
  if (      sizeGraphSession )		BglMM_Info_s_need();
  else if ( geneGraphSession )		BglMM_Info_g_need();
  else if ( memGraphSession )		BglMM_Info_m_need();
  else if ( efficSizeGraphSession )	BglMM_Info_es_need();
  else if ( efficGeneGraphSession )	BglMM_Info_eg_need();
  else if ( planSession )		BglMM_Plan_need();
  if ( varSession )	BglMM_Var_need();
  if ( tPingSSession )	BglMM_Misc_tSOn_need();
  if ( tPingCSession )	BglMM_Misc_tCOn_need();

  return 0;
}
