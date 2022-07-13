/* beagleMessageMgr.h -- -*- C -*-
 *
 * @(#)beagleMessageMgr.h	1.23	12/20/99 17:42:13
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 30 15:19:34 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Dec 20 17:42:02 1999
 * Update Count    : 419
 * Status          : 
 */

#ifndef	beagleMessageMgr_h
#define	beagleMessageMgr_h

#include "clnt.h"
#include "beagleMessage_Migr.h"
#include "beagleMessage_Ov.h"
#include "beagleMessage_Debug.h"
#include "beagleMessage_Info.h"
#include "beagleMessage_Var.h"
#include "beagleMessage_Misc.h"
#include "beagleMessage_TPingS.h"
#include "beagleMessage_TPingC.h"
#include "beagleMessage_File.h"
#include "beagleMessage_Connect.h"

/*- service functions -*/
extern I32s	BglMM_B_MenuBreak	P_((void));
extern I32s	BglMM_B_Cont		P_((void));  /* It's a non blocking */
extern void	BglMM_B_Esc		P_((void)); /* really need it? */
extern void	BglMM_B_OneCellCont	P_((void));
	/* continue from one cell start */


/***************************************************/
/***  BGL_REPLYMES INFORMATION access functions  ***/
extern I32s	BglMM_ReplyMes_modeReset	P_((void));
extern I32s	BglMM_HowManyReplyMes	P_((void));
extern BglReplyMesSt*	BglMM_PickReplyMes	P_((void));

/************************************************/
/***  BGL_STATS INFORMATION access functions  ***/
extern I32s	BglMM_Stats_modeReset	P_((void));
extern I32s	BglMM_HowManyStats	P_((void));
extern 	BglStatsSt*	BglMM_PickStats	P_((void));

/***********************************************/
/***  BGL_MESSAGE INFORMATION access functions  ***/
extern I32s	BglMM_Message_modeReset	P_((void));
extern I32s	BglMM_HowManyMessage	P_((void));
extern BglMessageSt*	BglMM_PickMessage	P_((void));

/***  about do_BGL_MC_STATE  ***/
extern I32s	BglMM_MCState_modeReset	P_((void));
extern I32s	BglMM_HowManyMCState	P_((void));
extern BglMCStateSt*	BglMM_PickMCState	P_((void));
extern I8s	BglMM_WhatIsMCStateMode	P_((void));
extern I8s	BglMM_WhatIsMCStateSlice	P_((void));
extern I8s	BglMM_WhatIsMCStateCpu	P_((void));

/******************************************/

/*---  oPt  ---*/
extern void	BglMM_oPt_w P_((I8s, double));


#endif	/* beagleMessageMgr_h */
