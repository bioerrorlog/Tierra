/* beagleMessage_TPingC.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 09:33:00 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:51:16 1999
 * Update Count    : 35
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleMessage_TPingC.c	1.4	10/18/99 09:51:29";
#endif /* lint */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_TPingC.h"
#include "beagleUI.h"

/********************************************************************************
 * Static definitions
 */

/***  about TPing access functions  ***/
MM_state_t		MM_tPingC_s	= MMS_NORMAL;
static MM_state_t	MM_tPingC_On_s	= MMS_NORMAL;
static MM_state_t	MM_tPingC_Off_s	= MMS_NORMAL;
struct MM_BglTPingCList_T {
  struct MM_BglTPingCList_T*	next;
  BglTPingSt			d;
};
typedef struct MM_BglTPingCList_T MM_BglTPingCList_t;
static MM_BglTPingCList_t	MM_tPingCDataRoot;
static MM_BglTPingCList_t*	MM_tPingCWp = &MM_tPingCDataRoot; /* write p */
static MM_BglTPingCList_t*	MM_tPingCRp = &MM_tPingCDataRoot; /* read p */
static I32s			MM_tPingCData_n = 0;

static I32s	BglMM_TPingC_need	P_((void));


#ifdef NB_MECHANISM
/** functions for non blocking mechanism */
static MM_funcstate_t	BglMM_NB_Misc_tCOn		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tCOn_Wait		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tCOff		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tCOff_Wait	P_((void));
#endif /* NB_MECHANISM */

/******************************************************************
 * function implementation
 */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int	BglMM_IsTPingC_stop()
{
  return (MM_tPingC_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_TPingC_modeReset()
{
  MM_tPingC_s     = MMS_NORMAL;
  MM_tPingC_On_s  = MMS_NORMAL;
  MM_tPingC_Off_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/******************************
 * TPingC buffer
 */

/*
 * BglMM_TPingC_need
 *	put tping buffer into good state for starting a tping session.
 */
static I32s	BglMM_TPingC_need()
{
  MM_tPingC_s = MMS_REQUESTED;
  MM_tPingCWp = MM_tPingCRp;
  MM_tPingCData_n = 0;
  if ( !(MM_tPingCDataRoot.next) ) MM_tPingCDataRoot.next = &MM_tPingCDataRoot;
  return 1;
}

I32s	BglMM_HowManyTPingC()
{
  return MM_tPingCData_n;
}

void	BglMM_TPingC_store(d)
     BglTPingSt* d;
{
  /*DebugPrint("/==== BglMM_TPingC_store ====/\n");*/
  if ( MM_tPingC_s == MMS_REQUESTED ||
       MM_tPingC_s == MMS_CHANGED ){
    MM_tPingCWp->d.mode = d->mode;
    MM_tPingCWp->d.id = d->id;
    MM_tPingCWp->d.node = d->node;
    MM_tPingCWp->d.portnb = d->portnb;
    MM_tPingCWp->d.FecundityAvg = d->FecundityAvg;
    MM_tPingCWp->d.Speed = d->Speed;
    MM_tPingCWp->d.NumCells = d->NumCells;
    MM_tPingCWp->d.AgeAvg = d->AgeAvg;
    MM_tPingCWp->d.SoupSize = d->SoupSize;
    MM_tPingCWp->d.TransitTime = d->TransitTime;
    MM_tPingCWp->d.Time = d->Time;
    MM_tPingCWp->d.InstExec = d->InstExec;
    MM_tPingCWp->d.InstExecConnect = d->InstExecConnect;
    MM_tPingCWp->d.OS = d->OS;
    if ( MM_tPingCWp->next == MM_tPingCRp ){
      MM_tPingCWp->next
	= (MM_BglTPingCList_t*)tcalloc(1,sizeof(MM_BglTPingCList_t),52070);
      MM_tPingCWp->next->next = MM_tPingCRp;
    }
    MM_tPingCWp = MM_tPingCWp->next;
    MM_tPingCData_n++;
    MM_tPingC_s = MMS_CHANGED;
  }

  return;
}

BglTPingSt*	BglMM_PickTPingC()
{
  BglTPingSt* retP = &(MM_tPingCRp->d);
  MM_tPingCRp = MM_tPingCRp->next;
  if ( MM_tPingCRp == MM_tPingCWp )MM_tPingC_s = MMS_REQUESTED;
  MM_tPingCData_n--;
  return retP;
}


/******************************
 * TPingC session start
 */
I32s	BglMM_Misc_tCOn_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if ( MM_tPingC_On_s == MMS_NORMAL ){
#ifdef NB_MECHANISM
    MM_tPingC_On_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tCOn );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tCOn_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() )	return 0;

    if ( !BglMM_B_Misc_tCOn()){
      BglMM_B_Cont();
      return 0;
    }
    if ( BglMM_B_Cont() )	return 0;
#endif /* B_MECHANISM */

    return 1;
  }
  else return 0;
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_tCOn()
{
  /*DebugPrint("/==== BglMM_B_Misc_tCOn ====/\n");*/
  if ( BglMM_ClntSend(&BglReqMiscTpingCOn) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_TPING_C_OK, BGL_TPING_C_REF )
      != BGL_REPLY_RET_OK ){
    BglUI_TPingCStatsToOff();
    return 0;
  }
  else{
    MM_tPingC_On_s = MMS_REQUESTED;
    MM_tPingC_Off_s = MMS_NORMAL;
    BglMM_TPingC_need();
    BglUI_TPingCStatsToOn();
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static 	MM_funcstate_t BglMM_NB_Misc_tCOn()
{
  /*DebugPrint("/==== BglMM_NB_Misc_tCOn ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscTpingCOn ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static 	MM_funcstate_t BglMM_NB_Misc_tCOn_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_tCOn_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_TPING_C_OK, BGL_TPING_C_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_tPingC_On_s = MMS_REQUESTED;
    MM_tPingC_Off_s = MMS_NORMAL;
    BglMM_TPingC_need();
    BglUI_TPingCStatsToOn();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    MM_tPingC_On_s = MMS_NORMAL;
    MM_tPingC_Off_s = MMS_NORMAL;
    BglUI_TPingCStatsToOff();
    return MMFS_FAIL;
    break;

  default:
    if(BglReplyBuf.ack == BGL_TPING_C_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      MM_tPingC_On_s = MMS_NORMAL;
      MM_tPingC_Off_s = MMS_NORMAL;
      BglUI_TPingCStatsToOff();
      return  MMFS_FAIL;
    }
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */

/******************************
 * TPingC session stop
 */
I32s	BglMM_Misc_tCOff_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if ( MM_tPingC_Off_s == MMS_NORMAL ){
#ifdef NB_MECHANISM
    MM_tPingC_Off_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tCOff );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tCOff_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() )	return 0;

    if ( !BglMM_B_Misc_tCOff()){
      BglMM_B_Cont();
      return 0;
    }
    if ( BglMM_B_Cont() )	return 0;
#endif /* B_MECHANISM */

    return 1;
  }
  else return 0;
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_tCOff()
{
  /*DebugPrint("/==== BglMM_B_Misc_tCOff ====/\n");*/
  if ( BglMM_ClntSend(&BglReqMiscTpingCOff) ) return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_TPING_C_OK, BGL_TPING_C_REF )
      != BGL_REPLY_RET_OK ){
    BglUI_TPingCStatsToOn();
    return 0;
  }
  else{
    MM_tPingC_Off_s = MMS_REQUESTED;
    MM_tPingC_On_s = MMS_NORMAL;
    MM_tPingC_s = MMS_NORMAL;
    BglUI_TPingCStatsToOff();
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_tCOff()
{
  /*DebugPrint("/==== BglMM_NB_Misc_tCOff ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscTpingCOff ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Misc_tCOff_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_tCOff_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_TPING_C_OK, BGL_TPING_C_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_tPingC_Off_s = MMS_REQUESTED;
    MM_tPingC_On_s = MMS_NORMAL;
    MM_tPingC_s = MMS_NORMAL;
    BglUI_TPingCStatsToOff();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    MM_tPingC_On_s = MMS_NORMAL;
    MM_tPingC_Off_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;

  default:
    if(BglReplyBuf.ack == BGL_TPING_C_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      return MMFS_FAIL;
    }
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */
