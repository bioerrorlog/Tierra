/* beagleMessage_TPingS.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 09:31:06 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:52:30 1999
 * Update Count    : 33
 * Status          : 
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleMessage_TPingS.c	1.4	10/18/99 09:52:40";
#endif /* lint */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_TPingS.h"
#include "beagleUI.h"

/********************************************************************************
 * Static definitions
 */

/***  about TPing access functions  ***/
MM_state_t		MM_tPingS_s	= MMS_NORMAL;
static MM_state_t	MM_tPingS_On_s	= MMS_NORMAL;
static MM_state_t	MM_tPingS_Off_s	= MMS_NORMAL;
struct MM_BglTPingSList_T {
  struct MM_BglTPingSList_T*	next;
  BglTPingSt			d;
};
typedef struct MM_BglTPingSList_T MM_BglTPingSList_t;
static MM_BglTPingSList_t	MM_tPingSDataRoot;
static MM_BglTPingSList_t*	MM_tPingSWp = &MM_tPingSDataRoot; /* write p */
static MM_BglTPingSList_t*	MM_tPingSRp = &MM_tPingSDataRoot; /* read p */
static I32s			MM_tPingSData_n = 0;

static I32s	BglMM_TPingS_need	P_((void));


#ifdef NB_MECHANISM
/** functions for non blocking mechanism */
static MM_funcstate_t	BglMM_NB_Misc_tSOn		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tSOn_Wait		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tSOff		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_tSOff_Wait	P_((void));
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
int	BglMM_IsTPingS_stop()
{
  return (MM_tPingS_s == MMS_NORMAL);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_TPingS_modeReset()
{
  MM_tPingS_s     = MMS_NORMAL;
  MM_tPingS_On_s  = MMS_NORMAL;
  MM_tPingS_Off_s = MMS_NORMAL;
  return 0; /* Normal return */
}

/******************************
 * TPingS buffer
 */

/*
 * BglMM_TPingS_need
 *	put tping buffer into good state for starting a tping session.
 */
static I32s	BglMM_TPingS_need()
{
  MM_tPingS_s = MMS_REQUESTED;
  MM_tPingSWp = MM_tPingSRp;
  MM_tPingSData_n = 0;
  if ( !(MM_tPingSDataRoot.next) ) MM_tPingSDataRoot.next = &MM_tPingSDataRoot;
  return 1;
}

I32s	BglMM_HowManyTPingS()
{
  return MM_tPingSData_n;
}

void	BglMM_TPingS_store(d)
     BglTPingSt* d;
{
  /*DebugPrint("/==== BglMM_TPingS_store ====/\n");*/
  if ( MM_tPingS_s == MMS_REQUESTED ||
       MM_tPingS_s == MMS_CHANGED ){
    MM_tPingSWp->d.mode = d->mode;
    MM_tPingSWp->d.id = d->id;
    MM_tPingSWp->d.node = d->node;
    MM_tPingSWp->d.portnb = d->portnb;
    MM_tPingSWp->d.FecundityAvg = d->FecundityAvg;
    MM_tPingSWp->d.Speed = d->Speed;
    MM_tPingSWp->d.NumCells = d->NumCells;
    MM_tPingSWp->d.AgeAvg = d->AgeAvg;
    MM_tPingSWp->d.SoupSize = d->SoupSize;
    MM_tPingSWp->d.TransitTime = d->TransitTime;
    MM_tPingSWp->d.Time = d->Time;
    MM_tPingSWp->d.InstExec = d->InstExec;
    MM_tPingSWp->d.InstExecConnect = d->InstExecConnect;
    MM_tPingSWp->d.OS = d->OS;
    if ( MM_tPingSWp->next == MM_tPingSRp ){
      MM_tPingSWp->next
	= (MM_BglTPingSList_t*)tcalloc(1,sizeof(MM_BglTPingSList_t),52070);
      MM_tPingSWp->next->next = MM_tPingSRp;
    }
    MM_tPingSWp = MM_tPingSWp->next;
    MM_tPingSData_n++;
    MM_tPingS_s = MMS_CHANGED;
  }

  return;
}

BglTPingSt*	BglMM_PickTPingS()
{
  BglTPingSt* retP = &(MM_tPingSRp->d);
  MM_tPingSRp = MM_tPingSRp->next;
  if ( MM_tPingSRp == MM_tPingSWp )MM_tPingS_s = MMS_REQUESTED;
  MM_tPingSData_n--;
  return retP;
}


/******************************
 * TPingS session start
 */
I32s	BglMM_Misc_tSOn_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if ( MM_tPingS_On_s == MMS_NORMAL ){
#ifdef NB_MECHANISM
    MM_tPingS_On_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tSOn );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tSOn_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() )	return 0;

    if ( !BglMM_B_Misc_tSOn()){
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
I32s	BglMM_B_Misc_tSOn()
{
  /*DebugPrint("/==== BglMM_B_Misc_tSOn ====/\n");*/
  if ( BglMM_ClntSend(&BglReqMiscTpingSOn) ) return 0; /*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_TPING_S_OK, BGL_TPING_S_REF )
      != BGL_REPLY_RET_OK ){
    BglUI_TPingSStatsToOff();
    return 0;
  }
  else{
    MM_tPingS_On_s = MMS_REQUESTED;
    MM_tPingS_Off_s = MMS_NORMAL;
    BglMM_TPingS_need();
    BglUI_TPingSStatsToOn();
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static 	MM_funcstate_t BglMM_NB_Misc_tSOn()
{
  /*DebugPrint("/==== BglMM_NB_Misc_tSOn ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscTpingSOn ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static 	MM_funcstate_t BglMM_NB_Misc_tSOn_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_tSOn_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_TPING_S_OK, BGL_TPING_S_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_tPingS_On_s = MMS_REQUESTED;
    MM_tPingS_Off_s = MMS_NORMAL;
    BglMM_TPingS_need();
    BglUI_TPingSStatsToOn();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    MM_tPingS_On_s = MMS_NORMAL;
    MM_tPingS_Off_s = MMS_NORMAL;
    BglUI_TPingSStatsToOff();
    return MMFS_FAIL;
    break;

  default:
    if(BglReplyBuf.ack == BGL_TPING_S_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      MM_tPingS_On_s = MMS_NORMAL;
      MM_tPingS_Off_s = MMS_NORMAL;
      BglUI_TPingSStatsToOff();
      return  MMFS_FAIL;
    }
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */

/******************************
 * TPingS session stop
 */
I32s	BglMM_Misc_tSOff_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if ( MM_tPingS_Off_s == MMS_NORMAL ){
#ifdef NB_MECHANISM
    MM_tPingS_Off_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			  NULL, NULL, BglMM_NB_Cont,
			  NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tSOff );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_tSOff_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
    if ( !BglMM_B_MenuBreak() )	return 0;

    if ( !BglMM_B_Misc_tSOff()){
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
I32s	BglMM_B_Misc_tSOff()
{
  /*DebugPrint("/==== BglMM_B_Misc_tSOff ====/\n");*/
  if ( BglMM_ClntSend(&BglReqMiscTpingSOff) ) return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_TPING_S_OK, BGL_TPING_S_REF )
      != BGL_REPLY_RET_OK ){
    BglUI_TPingSStatsToOn();
    return 0;
  }
  else{
    MM_tPingS_Off_s = MMS_REQUESTED;
    MM_tPingS_On_s = MMS_NORMAL;
    MM_tPingS_s = MMS_NORMAL;
    BglUI_TPingSStatsToOff();
    return 1;
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_tSOff()
{
  /*DebugPrint("/==== BglMM_NB_Misc_tSOff ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscTpingSOff ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Misc_tSOff_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_tSOff_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_TPING_S_OK, BGL_TPING_S_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_tPingS_Off_s = MMS_REQUESTED;
    MM_tPingS_On_s = MMS_NORMAL;
    MM_tPingS_s = MMS_NORMAL;
    BglUI_TPingSStatsToOff();
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    MM_tPingS_On_s = MMS_NORMAL;
    MM_tPingS_Off_s = MMS_NORMAL;
    return MMFS_FAIL;
    break;

  default:
    if(BglReplyBuf.ack == BGL_TPING_S_REF_NONET){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Tierra is not a network version.");
      return MMFS_FAIL;
    }
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */
