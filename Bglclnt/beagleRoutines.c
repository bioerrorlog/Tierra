/* beagleRoutines.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 19 17:35:01 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri Jun  4 14:12:51 1999
 * Update Count    : 396
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleRoutines.c	1.29	06/04/99 14:13:31";
#endif /* lint */

#include "clnt.h"
#include "beagleRoutines.h"
#include "beagleMessage_Connect.h"
#include "beagleProcCtrl.h"
#include "beagleUTIL_MigrMgr.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUI.h"


/*
 *- SPECIAL VALIABLE NAME
 *- ====================
 *-
 */

/* None */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- These function are in the file "beagleMessageMgr.c".
 *-      and "beagleUTIL_SockMgr.c"
 */
extern void	BglMM_MakeClntReq	P_((BglSockSt*));
extern I32s	BglMM_MigrationDataPortOpen	P_((void));
extern void	BglMM_MigrationDataCheckAndDo	P_((void));
extern void	BglMM_SendRequestCheckAndDo	P_((BglSockSt*));
extern void	BglMM_StandardCommunication	P_((BglSockSt*));
extern void	BglMM_ReceivedDataCheckAndDo	P_((BglSockSt*));


/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/* None */

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/* None */


/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILES.
 *- ==================================================================
 */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void
BglClntSetup(s, script)
     BglSockSt* s;
     char *script;
{
  BglSockSt* sP;
  int err = 0;

  if (fsmbuild() != 0){		/* Make FSM state machine for command */
    err = 1;
    BglExit(1);
  }
  if (!err && ddbuild() != 0){	/* Make FSM state machine for data */
    err = 1;
    BglExit(2);
  }
  if (!err) BglMM_MakeClntReq(s);	/* Make request commands */
  if (!err && (BglMM_Connect_Open(script, 1) != 0)){
    err = 1;
    BglExit(3);
  }
  if (!err && ((sP = UTIL_Sock_GetCurrent_Connection()) == NULL)){
    err = 1;
    BglExit(4);
  }
  if (!err){
    *s = *sP;
    BglMM_ProcessListReset();	/* initialize process manager */
    if (BglMM_MigrationDataPortOpen() != 0){
      err = 1;
      BglExit(5);
    }
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void 
BglOneAction(s)
     BglSockSt* s;
{
  static int	i = 0;

  BglMM_MigrationDataCheckAndDo();

  if ( UTIL_Sock_IsConnection_Changed() ){
    /* DebugPrint("/=== UTIL_Sock_IsConnection_Changed true ===/\n"); */
    BglUI_Connection_Changed();
    i = 0;
  }

  switch( i ){
  case  0:
    /* DebugPrint("/------- BglOneAction -----(1)----------/\n"); */
    BglMM_SendRequestCheckAndDo(s);
    i = 1;
    break;

  case  1:
    /* DebugPrint("/------- BglOneAction -----(2)----------/\n"); */
    BglMM_StandardCommunication(s);
    i = 2;
    break;

  case  2:
    /* DebugPrint("/------- BglOneAction -----(3)----------/\n"); */
    BglMM_ReceivedDataCheckAndDo(s);
    i = 0;
    break; 

  default:
    DebugPrint("beagleRoutines: something program bug!!!\n");
    break;
  }

  return;
}

/*
 * BglExit();
 */
void
BglExit(eno)
     I32s eno;
{
  if (eno != -10610)    /* check if it is not a binding error
			     for migration control */
    {
      UTIL_MigrMgr_Close();
      if (eno)
	BglPrintf(BGL_MENU_SCR, BGL_MENU_X, BGL_MENU_Y, 0, 0,
		  "Beagle is going down");
      BglUI_Exit(eno);
    }
}


/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/* None */
