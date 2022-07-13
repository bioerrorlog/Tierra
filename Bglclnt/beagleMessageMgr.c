/* beagleMessageMgr.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Aug 30 15:20:10 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Dec 20 17:40:35 1999
 * Update Count    : 2314
 * Status          : 
 */

#include "clnt.h"
#include "beagleUI.h"
#include "beagleMessageMgr.h"
#include "beagleMessageMgrP.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUTIL_MigrMgr.h"


/*
 *- HERE ARE GLOBAL VARIABLE/TYPE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

fd_set	BglAfds, BglRfds;
I32s	BglNfds;

I32s		MM_isReceived = 1;

BglMesSt	BglReqPreConn;		/* Pre-Connect to tierra */
BglMesSt	BglReqConnect;		/* Connect to tierra */
static BglMesSt BglReqMessage;		/* Frontend message */
static BglMesSt BglReqFEMenu;		/* FEMenu */
static BglMesSt	BglReqOneCellCont;	/* continue from one cell start */
BglMesSt	BglReqInfoPlan;		/* FEMenu-info-plan */
BglMesSt	BglReqInfoSize;		/* FEMenu-info-sizehisto */
BglMesSt	BglReqInfoGen;		/* FEMenu-info-genhisto */
BglMesSt	BglReqInfoMem;		/* FEMenu-info-memhisto */
BglMesSt	BglReqInfoEfficS;	/* FEMenu-info(size)-reprod_effhisto */
BglMesSt	BglReqInfoEfficG;	/* FEMenu-info(gene)-reprod_effhisto */
BglMesSt	BglReqSave;		/* FEMenu-save */
BglMesSt	BglReqMiscMicroToggle;	/* Micro debugger Toggle swtich */
BglMesSt	BglReqMiscHistLog;	/* FEMenu-misc-histolog */
BglMesSt	BglReqMiscTpingSOn;	/* FEMenu-misc-tping-server-on */
BglMesSt	BglReqMiscTpingSOff;	/* FEMenu-misc-tping-server-off */
BglMesSt	BglReqMiscTpingCOn;	/* FEMenu-misc-tping-cluster-on */
BglMesSt	BglReqMiscTpingCOff;	/* FEMenu-misc-tping-cluster-off */
BglMesSt	BglReqMiscInject;	/* FEMenu-misc-inject */
BglMesSt	BglReqValExam;		/* FEMenu-value-examine */
BglMesSt	BglReqValAlt;		/* FEMenu-value-alter */
BglMesSt	BglReqValExamAll;	/* FEMenu-value-examine-all */
BglMesSt	BglReqInfoZ;		/* FEMenu-info-size_query */
static BglMesSt BglReqEsc;		/* Escape */
BglMesSt	BglReqQuit;		/* Quit */
BglMesSt	BglReqSaveQuit;		/* Save & Quit */
BglMesSt	BglReqSuspendLife;	/* Suspend Tierra Life Loop */
BglMesSt	BglReqResumeLife;	/* resume Tierra Life Loop */
BglMesSt	BglReqSelect;		/* Back to MSSELECT */
BglMesSt	BglReqMicroStepn;	/* Micro debugger input step next*/
BglMesSt	BglReqMicroStepSlice;	/* Micro debugger track cell */
BglMesSt	BglReqMicroStepCpu;	/* Micro debugger track cpu */
BglMesSt	BglReqMicroStepNewCpu;	/* Micro debugger track new cpu */
BglMesSt	BglReqMicroStepQ;	/* Micro debugger quit */
BglMesSt	BglReqDoMicroStepn;	/* Micro debugger input step next*/
BglMesSt	BglReqDoMicroStepSlice;	/* Micro debugger track cell */
BglMesSt	BglReqDoMicroStepCpu;	/* Micro debugger track cpu */
BglMesSt	BglReqDoMicroStepNewCpu;/* Micro debugger track new cpu */
BglMesSt	BglReqDoMicroStepQ;	/* Micro debugger quit */
BglMesSt	BglReqDoMicroBrkPut;	/* Micro debugger put breakpoint */
BglMesSt	BglReqDoMicroBrkClear;	/* Micro debugger clear breakpoint */
BglMesSt	BglReqDoMicroBrkRepeat;	/* Micro debugger repeat breakpoint */
static BglMesSt BglReqBackToFEMenu;	/* Back to MSFEMENU */
BglMesSt	BglReqOV_S;		/* OV start */
BglMesSt	BglReqOV_S_I;		/* OV start */
BglMesSt	BglReqOV_Q;		/* OV quit */
BglMesSt	BglReqOV_GA_I;		/* OV gene Place */
BglMesSt	BglReqOV_GA;		/* OV gene Place */
BglMesSt	BglReqOV_GS_I;		/* OV gene size class*/
BglMesSt	BglReqOV_GS;		/* OV gene size class*/
BglMesSt	BglReqOV_Save;		/* OV gene size class*/
BglMesSt	BglReqMigTrackOn;	/* Migration Tracking On */
BglMesSt	BglReqMigTrackOff;	/* Migration tracking Off */


#define	MM_REPLY_MAX	10
static MM_state_t MM_reply_s = MMS_NEED;
static I32s	MM_replyWi = 0;	/* Write Index */
static I32s	MM_replyRi = 0;	/* Read Index */
static BglReplySt	MM_replyData[MM_REPLY_MAX];


#define MM_REPLY_MES_MAX 10
static MM_state_t MM_replyMes_s = MMS_NEED;
static I32s	MM_replyMesWi = 0;	/* Write Index */
static I32s	MM_replyMesRi = 0;	/* Read Index */
static BglReplyMesSt	MM_replyMesData[MM_REPLY_MES_MAX];

#define MM_STATS_MAX 10
static MM_state_t MM_stats_s = MMS_NEED;
static I32s	MM_statsWi = 0;	/* Write Index */
static I32s	MM_statsRi = 0;	/* Read Index */
static BglStatsSt	MM_statsData[MM_STATS_MAX];

static MM_state_t MM_message_s = MMS_REQUESTED;
struct MM_BglMessageList_T {
  struct MM_BglMessageList_T*	next;
  BglMessageSt			d;
};
typedef struct MM_BglMessageList_T	MM_BglMessageList_t;
static MM_BglMessageList_t	MM_messageDataRoot;
static MM_BglMessageList_t*	MM_messageWp = &MM_messageDataRoot;/*write p*/
static MM_BglMessageList_t*	MM_messageRp = &MM_messageDataRoot;/*read p*/
static I32s			MM_messageData_n = 0;

/**  about do_BGL_MC_STATE  ***/
#define MM_MC_STATE_MAX	10
static MM_state_t MM_mCState_s = MMS_REQUESTED;
static I32s	MM_mCStateWi = 0;	/* Write Index */
static I32s	MM_mCStateRi = 0;	/* Read Index */
static BglMCStateSt	MM_mCStateData[MM_MC_STATE_MAX];
static I8s	MM_lastMCStateMode = ' ';
static I8s	MM_lastMCStateSlice = ' ';
static I8s	MM_lastMCStateCpu = ' ';


/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

static MM_funcstate_t	BglMM_NB_OneCellCont	P_((void));
static MM_funcstate_t	BglMM_NB_Polling	P_((void));

#ifdef HAVE_STDARG_H
static void	BglMM_MakeReqComm	P_((BglMesSt*, I32s, ...));
#else
static void	BglMM_MakeReqComm	P_(());
#endif
static void	BglWaitTimer		P_((double));
static int	BglSkipTimer		P_((long *,struct timeval*,double));


/*
 *- HERE ARE SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILEs.
 *- =====================================================================
 */

void	BglMM_MakeClntReq(s)
     BglSockSt* s;
{
  BglNfds = bgl_open_max();	/* initialize for communication
				   into message manager */

  BglMM_MakeReqComm(&BglReqPreConn,		2, MCDOFEM, MCPRECONN);
  BglMM_MakeReqComm(&BglReqConnect,		2, MCDO, MCCONNECT);
  BglMM_MakeReqComm(&BglReqSelect,		1, MCSELECT);
  BglMM_MakeReqComm(&BglReqMessage,		2, MCDO, MCMESSAGE);
  BglMM_MakeReqComm(&BglReqFEMenu,		2, MCDO, MCFEMENU);
  BglMM_MakeReqComm(&BglReqOneCellCont,		2, MCDO, MCONECELLCONT);
  BglMM_MakeReqComm(&BglReqInfoPlan,		2, MCDOFEM, MCINFO_PLAN);
  BglMM_MakeReqComm(&BglReqInfoSize,		2, MCDOFEM, MCINFO_SIZE);
  BglMM_MakeReqComm(&BglReqInfoGen,		2, MCDOFEM, MCINFO_GEN);
  BglMM_MakeReqComm(&BglReqInfoMem,		2, MCDOFEM, MCINFO_MEM);
  BglMM_MakeReqComm(&BglReqInfoEfficS,		2, MCDOFEM, MCINFO_EFFICS);
  BglMM_MakeReqComm(&BglReqInfoEfficG,		2, MCDOFEM, MCINFO_EFFICG);
  BglMM_MakeReqComm(&BglReqInfoZ,		2, MCDOFEM, MCINFO_Z);
  BglMM_MakeReqComm(&BglReqMiscMicroToggle,  2,MCDOFEM, MCMISC_MICRO_TOGGLE);
  BglMM_MakeReqComm(&BglReqMiscHistLog,	     2, MCDOFEM, MCMISC_HISTO);
  BglMM_MakeReqComm(&BglReqMiscTpingSOn,     2, MCDOFEM, MCMISC_TPING_S_ON);
  BglMM_MakeReqComm(&BglReqMiscTpingSOff,    2, MCDOFEM, MCMISC_TPING_S_OFF);
  BglMM_MakeReqComm(&BglReqMiscTpingCOn,     2, MCDOFEM, MCMISC_TPING_C_ON);
  BglMM_MakeReqComm(&BglReqMiscTpingCOff,    2, MCDOFEM, MCMISC_TPING_C_OFF);
  BglMM_MakeReqComm(&BglReqMiscInject,		2, MCDOFEM, MCMISC_INJECT);
  BglMM_MakeReqComm(&BglReqValExam,		2, MCDOFEM, MCVAL_EXAM);
  BglMM_MakeReqComm(&BglReqValExamAll,		2, MCDOFEM, MCVAL_EXAM_ALL);
  BglMM_MakeReqComm(&BglReqValAlt,		2, MCDOFEM, MCVAL_ALT);
  BglMM_MakeReqComm(&BglReqOV_S_I,		2, MCDOFEM, MCOV_S_I);
  BglMM_MakeReqComm(&BglReqOV_S,		2, MCDOFEM, MCOV_S);
  BglMM_MakeReqComm(&BglReqOV_Q,		2, MCDOFEM, MCOV_Q);
  BglMM_MakeReqComm(&BglReqOV_GA_I,		2, MCDOFEM, MCOV_GA_I);
  BglMM_MakeReqComm(&BglReqOV_GA,		2, MCDOFEM, MCOV_GA);
  BglMM_MakeReqComm(&BglReqOV_GS_I,		2, MCDOFEM, MCOV_GS_I);
  BglMM_MakeReqComm(&BglReqOV_GS,		2, MCDOFEM, MCOV_GS);
  BglMM_MakeReqComm(&BglReqOV_Save,		2, MCDOFEM, MCOV_SAVE_CELL);
  BglMM_MakeReqComm(&BglReqEsc,			1, MCESC);
  BglMM_MakeReqComm(&BglReqSave,		2, MCDOFEM, MCSAVE);
  BglMM_MakeReqComm(&BglReqQuit,		2, MCDOFEM, MCQUIT);
  BglMM_MakeReqComm(&BglReqSaveQuit,		2, MCDOFEM, MCSAVEQUIT);
  BglMM_MakeReqComm(&BglReqSuspendLife,		2, MCDOFEM, MCSUSPENDLIFE);
  BglMM_MakeReqComm(&BglReqResumeLife,		2, MCDOFEM, MCRESUMELIFE);
  BglMM_MakeReqComm(&BglReqMicroStepn,		1, MCMICRO_STEP_n);
  BglMM_MakeReqComm(&BglReqMicroStepSlice,	1, MCMICRO_STEP_SLICE);
  BglMM_MakeReqComm(&BglReqMicroStepCpu,	1, MCMICRO_STEP_CPU);
  BglMM_MakeReqComm(&BglReqMicroStepNewCpu,	1, MCMICRO_STEP_NEWCPU);
  BglMM_MakeReqComm(&BglReqMicroStepQ,		1, MCMICRO_STEP_q);
  BglMM_MakeReqComm(&BglReqDoMicroStepn,	2, MCDO, MCMICRO_STEP_n);
  BglMM_MakeReqComm(&BglReqDoMicroStepSlice,	2, MCDO, MCMICRO_STEP_SLICE);
  BglMM_MakeReqComm(&BglReqDoMicroStepCpu,	2, MCDO, MCMICRO_STEP_CPU);
  BglMM_MakeReqComm(&BglReqDoMicroStepNewCpu,	2, MCDO, MCMICRO_STEP_NEWCPU);
  BglMM_MakeReqComm(&BglReqDoMicroStepQ,	2, MCDO, MCMICRO_STEP_q);
  BglMM_MakeReqComm(&BglReqDoMicroBrkPut,	2, MCDOFEM, MCMICRO_BRK_PUT);
  BglMM_MakeReqComm(&BglReqDoMicroBrkClear,	2, MCDOFEM, MCMICRO_BRK_CLEAR);
  BglMM_MakeReqComm(&BglReqDoMicroBrkRepeat,	2, MCDOFEM, MCMICRO_BRK_REPEAT);
  BglMM_MakeReqComm(&BglReqBackToFEMenu,	1, MCFEMENU);
  BglMM_MakeReqComm(&BglReqMigTrackOn,		2, MCDOFEM, MCMIG_TRACK_ON);
  BglMM_MakeReqComm(&BglReqMigTrackOff,		2, MCDOFEM, MCMIG_TRACK_OFF);
}


void	BglMM_SendRequestCheckAndDo(s)
     BglSockSt* s;
{
  BglMM_ProcessOneAction();

  return;
}

void	BglMM_StandardCommunication(s)
     BglSockSt* s;
{
  I32s cc = 1;
  static long   skipCounter=-1; /* -1 is mean first time call */
  static struct timeval lastTimeValue = { 0, 0 };
  
  memcpy((I8s *)&BglRfds, (I8s *)&BglAfds,  sizeof(BglRfds));	
  if(bgl_select(BglNfds, &BglRfds, 0) > 0){
    cc = BglMM_B_ClntRecv();
  }

  if (!cc)			/* if communication error */
    return;
  
  if( Bgl_step  == 1L){/* == MICRO Debugger Keypress-mode == */
    /* DebugPrint("/===  MICRO Debugger Keypress-mode  ===/\n"); */
    BglUI_MicroDebuggerKeyWait();
  }
  else if( Bgl_step  == 0L){ /* == MICRO Debugger Delay-mode == */
    /* DebugPrint("/===  MICRO Debugger Delay-mode  ===/\n"); */
    if ( BglSkipTimer(&skipCounter,&lastTimeValue,Bgl_wait1) ){
      /* == Automatic send debug Next message == */
      BglMM_Debug_Next();
    }
  }
  else{ /* == MICRO Debugger Off == */
    /* DebugPrint("/===  MICRO Debugger Off  ===/\n"); */
    if ( BglSkipTimer(&skipCounter,&lastTimeValue,Bgl_wait1) ){
      if ( MM_isReceived ){
	/* Send next times data request  */
	BglMM_Polling_need();
	BglUI_CommunicationCondition(".");
      }
      else{
	BglUI_CommunicationCondition("?");
      }
    }
  }

  return;
}

void	BglMM_ReceivedDataCheckAndDo(s)
     BglSockSt* s;
{
  BglMM_CheckCatch();
  
  if ( MM_histClear_s	== MMS_CHANGED ){
    BglUI_HistClear_Changed();
    MM_histClear_s = MMS_REQUESTED;
  }
  if ( MM_infoSize_s	== MMS_CHANGED ) BglUI_SizeHist_Changed();
  if ( MM_infoGene_s	== MMS_CHANGED ) BglUI_GeneHist_Changed();
  if ( MM_infoMem_s	== MMS_CHANGED ) BglUI_MemHist_Changed();
  if ( MM_infoEfficS_s	== MMS_CHANGED ) BglUI_ESHist_Changed();
  if ( MM_infoEfficG_s	== MMS_CHANGED ) BglUI_EGHist_Changed();
  if ( MM_replyMes_s	== MMS_CHANGED ) BglUI_ReplyMes_Changed();
  if ( MM_stats_s	== MMS_CHANGED ) BglUI_Stats_Changed();
  if ( MM_plan_s	== MMS_CHANGED ) BglUI_Plan_Changed();
  if ( MM_message_s	== MMS_CHANGED ) BglUI_Message_Changed();
  if ( MM_tPingS_s	== MMS_CHANGED ) BglUI_TPingS_Changed();
  if ( MM_tPingC_s	== MMS_CHANGED ) BglUI_TPingC_Changed();
  if ( MM_ovFrame_s	== MMS_CHANGED ) BglUI_OvFrame_Changed();
  if ( MM_ov_s		== MMS_CHANGED ) BglUI_Ov_Changed();
  if ( MM_ovGInst_s	== MMS_CHANGED ) BglUI_OvGInst_Changed();
  if ( MM_ovG_s		== MMS_CHANGED ) BglUI_OvG_Changed();
  if ( MM_sizeQuery_s	== MMS_CHANGED ) BglUI_SizeQuery_Changed();
  if ( MM_mCState_s	== MMS_CHANGED ) BglUI_MCState_Changed();
  if ( MM_mCSpy_s	== MMS_CHANGED ) BglUI_MCSpy_Changed();
  if ( MM_var_s		== MMS_CHANGED ) BglUI_Var_Changed();
  if ( MM_migration_s	== MMS_CHANGED ) BglUI_Migration_Changed();

  return;
}


I32s	BglMM_B_MenuBreak()
{
  /*DebugPrint("/==== BglMM_B_MenuBreak ====/\n");*/
  if ( BglMM_ClntSend( &BglReqFEMenu ) ) return 0;/* Communication Error */
  if ( BglMM_B_ReplyWait( BGL_FEMENU_OK, BGL_FEMENU_REF )!= BGL_REPLY_RET_OK){
    return 0; /* Eerror Return */
  }
  else{
    return 1;
  }
}

MM_funcstate_t	BglMM_NB_MenuBreak()
{
  /*DebugPrint("/==== BglMM_NB_MenuBreak ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqFEMenu ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

MM_funcstate_t	BglMM_NB_MenuBreak_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_MenuBreak_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_FEMENU_OK, BGL_FEMENU_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
  case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  case BGL_REPLY_RET_TIMEOUT:
    (void)BglMM_NB_Cont();
    /* Don't put BREAK here */
  default:
    return MMFS_FAIL;
    break;
  }
}

I32s	BglMM_B_Cont()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_B_Cont ====/\n");*/
  ret = BglMM_ClntSend(&BglReqSelect);
  MM_isReceived = 1;
  return ret;
}

MM_funcstate_t	BglMM_NB_Cont()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Cont ====/\n");*/
  ret = BglMM_ClntSend(&BglReqSelect);
  MM_isReceived = 1;
  if ( ret )	return MMFS_FAIL;	/* Communication Error */
  else		return MMFS_CONTINUE;	/* Communication NO Error */
}

void	BglMM_B_Esc()
{
  /*DebugPrint("/==== BglMM_B_Esc ====/\n");*/
}

void	BglMM_B_OneCellCont()
{
  /*DebugPrint("/==== BglMM_B_OneCellCont ====/\n");*/
  BglMM_ClntSend(&BglReqOneCellCont); 	
  MM_isReceived = 1;
}

static MM_funcstate_t	BglMM_NB_OneCellCont()
{
  /*DebugPrint("/==== BglMM_NB_OneCellCont ====/\n");*/
  if ( !BglMM_ClntSend(&BglReqOneCellCont) ){
    /* Communication NO Error */
    MM_isReceived = 1;
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

void	BglMM_Polling_need()
{
  BglMM_MakeProcessCell( BglMM_MakeFuncCell( BglMM_NB_Polling ),
			 NULL, NULL, NULL,
			 NULL, 0);
  MM_isReceived = 0;
}

static MM_funcstate_t	BglMM_NB_Polling()
{
  (void)BglMM_ClntSend( &BglReqMessage );
  MM_isReceived = 0;
  return MMFS_CONTINUE;
}

/***************************************/
/*********  for BGL_REPLY Data *********/
/***************************************/
I32s	BglMM_Reply_modeReset()
{
  MM_reply_s = MMS_REQUESTED;
  MM_replyRi = MM_replyWi = 0;
  return 0; /* Normal return */
}

I32s	BglMM_HowManyReply()
{
  I32s ret;
  ret = MM_replyWi - MM_replyRi;
  if ( ret < 0 ) ret += MM_REPLY_MAX;
  return(ret);
}

void	BglMM_Reply_store(d)
     BglReplySt* d;
{
  /*DebugPrint("/==== BglMM_Reply_store ====/\n");*/
  MM_replyData[MM_replyWi].ack = d->ack;
  MM_reply_s = MMS_CHANGED;
  MM_replyWi++; if( MM_replyWi==MM_REPLY_MAX ) MM_replyWi=0;
  return;
}

BglReplySt*	BglMM_PickReply()
{
  int ret_i;
  ret_i = MM_replyRi;
  MM_replyRi++;
  if( MM_replyRi==MM_REPLY_MAX ) MM_replyRi=0;
  if( MM_replyRi==MM_replyWi ) MM_reply_s = MMS_REQUESTED;
  return &(MM_replyData[ret_i]);
}


/******************************************/
/*********  for BGL_REPLYMES Data *********/
/******************************************/
/***  BGL_REPLYMES INFORMATION access functions  ***/
I32s	BglMM_ReplyMes_modeReset()
{
  MM_replyMes_s = MMS_REQUESTED;
  MM_replyMesRi = MM_replyMesWi = 0;
  return 0; /* Normal return */
}

I32s	BglMM_HowManyReplyMes()
{
  I32s ret;
  ret = MM_replyMesWi - MM_replyMesRi;
  if ( ret < 0 ) ret += MM_REPLY_MES_MAX;
  return(ret);
}

void	BglMM_ReplyMes_store(d)
     BglReplyMesSt* d;
{
  /*DebugPrint("/==== BglMM_ReplyMes_store ====/\n");*/
  strcpy(MM_replyMesData[MM_replyMesWi].mes, d->mes);
  MM_replyMes_s = MMS_CHANGED;
  MM_replyMesWi++; if( MM_replyMesWi==MM_REPLY_MES_MAX ) MM_replyMesWi=0;
  return;
}


BglReplyMesSt*	BglMM_PickReplyMes()
{
  int ret_i;
  ret_i = MM_replyMesRi;
  MM_replyMesRi++;
  if( MM_replyMesRi==MM_REPLY_MES_MAX ) MM_replyMesRi=0;
  if( MM_replyMesRi==MM_replyMesWi ) MM_replyMes_s = MMS_REQUESTED;
  return &(MM_replyMesData[ret_i]);
}

/******************************************/
/*********  for BGL_STATS Data ************/
/******************************************/
/***  BGL_STATS INFORMATION access functions  ***/
I32s	BglMM_Stats_modeReset()
{
  MM_stats_s = MMS_REQUESTED;
  MM_statsRi = MM_statsWi = 0;
  MM_isReceived = 1;
  return 0; /* Normal return */
}

I32s	BglMM_HowManyStats()
{
  I32s ret;
  ret = MM_statsWi - MM_statsRi;
  if ( ret < 0 ) ret += MM_STATS_MAX;
  return(ret);
}

void	BglMM_Stats_store(d)
     BglStatsSt* d;
{
  /*DebugPrint("/==== BglMM_Stats_store ====/\n");*/
  MM_statsData[MM_statsWi].GeneBnker = d->GeneBnker;
  MM_statsData[MM_statsWi].InstExe_m = d->InstExe_m;
  MM_statsData[MM_statsWi].InstExe_i = d->InstExe_i;
  MM_statsData[MM_statsWi].NumCells = d->NumCells;
  MM_statsData[MM_statsWi].NumGenotypes = d->NumGenotypes;
  MM_statsData[MM_statsWi].NumSizes = d->NumSizes;
  strcpy(MM_statsData[MM_statsWi].ExtrG_gen , d->ExtrG_gen);
  MM_stats_s = MMS_CHANGED;
  MM_statsWi++; if( MM_statsWi==MM_STATS_MAX ) MM_statsWi=0;
  MM_isReceived = 1;
  return;
}

BglStatsSt*	BglMM_PickStats()
{
  int ret_i;
  ret_i = MM_statsRi;
  MM_statsRi++;
  if( MM_statsRi==MM_STATS_MAX ) MM_statsRi=0;
  if( MM_statsRi==MM_statsWi ) MM_stats_s = MMS_REQUESTED;
  return &(MM_statsData[ret_i]);
}

/*********************************************/
/*********  for BGL_MESSAGE Data *************/
/*********************************************/
/***  BGL_MESSAGE INFORMATION access functions  ***/
I32s	BglMM_Message_modeReset()
{
  MM_message_s = MMS_REQUESTED;
  MM_messageWp = MM_messageRp;
  MM_messageData_n = 0;
  return 0;
}

I32s	BglMM_HowManyMessage(){ return MM_messageData_n; }

void	BglMM_Message_store(d)
     BglMessageSt* d;
{
  /*DebugPrint("/==== BglMM_Message_store ====/\n");*/
  if ( !(MM_messageDataRoot.next) )
    MM_messageDataRoot.next = &MM_messageDataRoot;
  strcpy(MM_messageWp->d.message , d->message);
  if ( MM_messageWp->next == MM_messageRp ){
    MM_messageWp->next
      = (MM_BglMessageList_t*)tcalloc(1,sizeof(MM_BglMessageList_t),52060);
    MM_messageWp->next->next = MM_messageRp;
  }
  MM_messageWp = MM_messageWp->next;
  MM_messageData_n++;
  MM_message_s = MMS_CHANGED;
  return;
}

BglMessageSt*	BglMM_PickMessage()
{
  BglMessageSt* retP = &(MM_messageRp->d);
  MM_messageRp = MM_messageRp->next;
  if( MM_messageRp == MM_messageWp ) MM_message_s = MMS_REQUESTED;
  MM_messageData_n--;
  return retP;
}

/***********************************************/
/*********  about do_BGL_MC_STATE  *************/
/***********************************************/
I32s	BglMM_MCState_modeReset()
{
  MM_mCState_s = MMS_REQUESTED;
  MM_mCStateRi = MM_mCStateWi = 0;
  return 1;
}

I32s	BglMM_HowManyMCState()
{
  I32s ret;
  ret = MM_mCStateWi - MM_mCStateRi;
  if ( ret < 0 ) ret += MM_MC_STATE_MAX;
  return(ret);
}

void	BglMM_MCState_store(d)
     BglMCStateSt* d;
{
  /*DebugPrint("/==== BglMM_MCState_store ====/\n");*/
  MM_mCStateData[MM_mCStateWi].MC_step = d->MC_step;
  MM_mCStateData[MM_mCStateWi].Slice = d->Slice;
  MM_mCStateData[MM_mCStateWi].Cpu = d->Cpu;
  MM_mCState_s = MMS_CHANGED;
  MM_mCStateWi++; if( MM_mCStateWi==MM_MC_STATE_MAX ) MM_mCStateWi=0;
  return;
}


BglMCStateSt*	BglMM_PickMCState()
{
  int ret_i;
  ret_i = MM_mCStateRi;
  MM_lastMCStateMode = MM_mCStateData[ret_i].MC_step;
  MM_lastMCStateSlice = MM_mCStateData[ret_i].Slice;
  MM_lastMCStateCpu = MM_mCStateData[ret_i].Cpu;
  MM_mCStateRi++;
  if( MM_mCStateRi==MM_MC_STATE_MAX ) MM_mCStateRi=0;
  if( MM_mCStateRi==MM_mCStateWi ) MM_mCState_s = MMS_REQUESTED;
  return &(MM_mCStateData[ret_i]);
}

I8s	BglMM_WhatIsMCStateMode(){ return MM_lastMCStateMode; }

I8s	BglMM_WhatIsMCStateSlice(){ return  MM_lastMCStateSlice; }

I8s	BglMM_WhatIsMCStateCpu(){ return MM_lastMCStateCpu; }


/**----------------------**/
/**---  oPt  ------------**/
/**----------------------**/
void	BglMM_oPt_w(a, t)
     I8s a;
     double t;
{
  /*DebugPrint("/==== BglMM_oPt_w ====/\n");*/
  switch(a) {
  case '0':	Bgl_wait0 = t;	break;
  case '1':	Bgl_wait1 = t;	break;
  case '2':	Bgl_wait2 = t;	break;
  default :	break;
  }
}


/*
 *- HERE ARE STATIC FUNCTIONs DEFINITION FOR CALL FROM IN THIS FILE ONLY.
 *- =====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 *- -- First part are creating window style.
 *- -- Second part are window actions mechanism.
 */
#ifdef HAVE_STDARG_H
/*
 * Make Requset command message
 *
 * Message Format:
 *             +----+---+---------------------+
 * com->buf    |LEN |tag|com1|com2|......|comn|
 *             +----+---+---------------------+
 *              I32s I8s I32s I32s ...... I32s
 */
static void	BglMM_MakeReqComm(BglMesSt* com, I32s n, ...)
{
  va_list ap;
  I32s i;
/*  I8s *p1, *p2;*/
  I32s md, nmd;
  I32s len, nlen;

  len = n * 4;
  com->len = BGL_MES_HEADER_LEN + len ;
  com->buf = (I8s*)tcalloc(1, com->len , 11701);

  nlen = htonl(len);
  memcpy(&com->buf[0], &nlen, 4);
  com->buf[4] = BGL_TAG_COM;

  va_start(ap, n);
/*  p1 = va_arg (ap, I8s *);*/
/*  p2 = va_arg (ap, I8s *);*/
  for (i = 0; i < n; i++) {
    md = va_arg (ap, int);
    DebugPrint1( "md=%d ",  md); 
    nmd = htonl(md);
    memcpy(&com->buf[BGL_MES_HEADER_LEN+i*4], &nmd, 4);
  }
  DebugPrint( "\n" );
  va_end(ap);

}

#else /* HAVE_STDARG_H */
/*
 * Make Requset command message
 */
static void	BglMM_MakeReqComm(com, n, va_alist)
     BglMesSt* com;
     I32s n;
     va_dcl
{
  va_list ap;
  I32s i;
  I32s md, nmd;
  I32s len, nlen;

  len = n * 4 ;
  com->len = BGL_MES_HEADER_LEN + len;
  com->buf = (I8s*)tcalloc(1, com->len , 11702);

  nlen = htonl(len);
  memcpy(&com->buf[0], &nlen, 4);
  com->buf[4] = BGL_TAG_COM;

  va_start(ap);
  for (i = 0; i < n; i++) {
    md = va_arg (ap, int);
    DebugPrint1( "md = %d ",md ); 
    nmd = htonl(md);
    memcpy(&com->buf[BGL_MES_HEADER_LEN+i*4], &nmd, 4);
  }
  DebugPrint( "\n" );
  va_end(ap);
}
#endif /* HAVE_STDARG_H */



static void BglWaitTimer(t)
     double t;
{
  struct timeval w;

  if (t > (double)0 ){
    w.tv_sec = (I32s)t;
    w.tv_usec = (I32s)((t - (float)w.tv_sec)*1000000);
    tselect(0,(fd_set *)0,(fd_set *)0,(fd_set *)0, &w);
  }
  return;
}

static int BglSkipTimer(counter, lastTimeValue, t)
     long * counter;
     struct timeval* lastTimeValue;
     double t;
{
  static struct timeval w = { 0L, 1L };
  struct timeval timeValue;
  long delta_sec, delta_usec;
  static long pass = 0;

  tgettimeofday( &timeValue, 0 );
  delta_sec = timeValue.tv_sec - lastTimeValue->tv_sec;
  delta_usec = timeValue.tv_usec - lastTimeValue->tv_usec;
  lastTimeValue->tv_sec  = timeValue.tv_sec;
  lastTimeValue->tv_usec = timeValue.tv_usec;

  if ( *counter < 0 ){
    /* Now first times call. */
    tselect(0,(fd_set *)0,(fd_set *)0,(fd_set *)0, &w);
    pass++;
    (*counter) = 1;
  }
  else if ( t >= 1.0 ){
    /* this case I use time data only. */
    (*counter) = (*counter) + delta_sec * 1000000 + delta_usec;
  }
  else if ( ( delta_sec != 0 )||( delta_usec != 0 ) ){
    /* There is something happen then entering this routine is later. */
    (*counter) = (*counter) + delta_sec * 1000000 + delta_usec - pass * 1;
    pass = 0;
  }
  else{
    /* Usual Micro sec counter use. */
    tselect(0,(fd_set *)0,(fd_set *)0,(fd_set *)0, &w);
    pass++;
    (*counter)+= 1;
  }
  
  if ( *counter > (t * 1000000) ){
    *counter = 0;
    return 100;
  }
  else	return 0;
}


I32s	BglMM_ClntSend(sendMsg)
     BglMesSt* sendMsg;
{
  return BglMM_Send(UTIL_Sock_GetCurrent_Connection()->fd, sendMsg);
}

I32s	BglMM_Send(fd, msg)
     I32s fd;
     BglMesSt* msg;
{
  I32s cc;

  cc = BglSend(fd, msg);
  if (cc <= 0){
    tsprintf(bglStrBuf, "Error in BglMM_Send. cc = %d", (int)cc);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    if (UTIL_Sock_GetCurrent_Connection() &&
	fd == UTIL_Sock_GetCurrent_Connection()->fd)
      BglMM_Connect_Close();
    else			/* if we send migration data */
      BglMM_CloseBglSockFD(fd);
    return 1;	/* Re-Connect another Tierra */
  }

  return 0;
}

I32s	BglMM_B_ClntRecv()
{
  return BglMM_B_Recv(UTIL_Sock_GetCurrent_Connection()->fd);
}

I32s	BglMM_B_Recv(fd)
     I32s fd;
{
  I32s cc;

  cc = BglRecv(fd);
  if (cc < 0){
    tsprintf(bglStrBuf,"Error in BglMM_B_Recv. cc = %d", (int)cc);	
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
    if (UTIL_Sock_GetCurrent_Connection() &&
	fd == UTIL_Sock_GetCurrent_Connection()->fd)
      BglMM_Connect_Close();
    else			/* if we send migration data */
      BglMM_CloseBglSockFD(fd);
    return 0;	/* Re-Connect another Tierra */
  }

  return cc;
}

I32s BglMM_B_ReplyWait(repOk, repRef)
     I32s repOk, repRef;
{
  I32s ret;

  while(1){
    ret = BglReplyWait( UTIL_Sock_GetCurrent_Connection()->fd,
			BglNfds, &BglAfds, repOk, repRef, Bgl_wait0 );

    switch( ret ){
    case BGL_REPLY_RET_OK:	/* GOOD! (Normal return) */
      (void)BglMM_PickReply();
      return ret;
      break;
    case BGL_REPLY_RET_REF:	/* Request refused by Tierra */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      return ret;
      break;
    case BGL_REPLY_RET_NONE:
      continue;  /* Read again */
      break;
    case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected reply data come from Tierra. (mistrust PROGRAM)");
      return ret;
      break;
    case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected FD. (mistrust PROGRAM)");
      continue;
      /*return BGL_REPLY_RET_ILL_FD;*/
      break;
    case BGL_REPLY_RET_TIMEOUT:	/* message TIME OUT */
      /*BglMM_B_Cont( s );*/
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"reply message could not reach on time. ignore this request.");
      return ret;
      break;
    case BGL_REPLY_COM_ERR:	/* Communication Error */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Error in BglMM_B_ReplyWait");
      BglMM_Connect_Close();
      return ret;
      break;
    case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
      /*BglMM_B_Cont( s );*/
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"something happened during bgl_select. (mistrust PROGRAM)");
      return ret;
      break;
    }
  }/* END of whilw Loop */
}


I32s	BglMM_NB_ReplyWait(repOk, repRef)
     I32s repOk, repRef;
{
  I32s ret;
  static I32s timeCount = 0;
  BglReplySt* rP;

  if ( BglMM_HowManyReply() ){
    rP = BglMM_PickReply();
    if ( rP->ack == repOk ){
      timeCount = 0;
      return BGL_REPLY_RET_OK;
    }
    else if ( rP->ack == repRef ){
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      timeCount = 0;
      return BGL_REPLY_RET_REF;
    }
    else{
      tsprintf(bglStrBuf,
	       "unexpected data received. (NB 1) ok=%d ref=%d recv=%d",
	       repOk, repRef, rP->ack );
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
      timeCount = 0;
      return BGL_REPLY_RET_UNEXPECT;
    }
  }
  else{
    ret = BglReplyWait( UTIL_Sock_GetCurrent_Connection()->fd,
			BglNfds, &BglAfds, repOk, repRef, (double)0.001 );

    switch( ret ){
    case BGL_REPLY_RET_OK:	/* GOOD! (Normal return) */
      (void)BglMM_PickReply();
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_RET_REF:	/* Request refused by Tierra */
      (void)BglMM_PickReply();
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request was refused by Tierra.");
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
      rP = BglMM_PickReply();
      tsprintf(bglStrBuf,
	       "unexpected data received. (NB 2) ok=%d ref=%d recv=%d",
	       repOk, repRef, rP->ack );
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_RET_TIMEOUT:	/* message TIME OUT */
      timeCount++;
      if ( timeCount > ( Bgl_wait0 * 1000 ) ){
	BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		  "reply message could not reach on time. ignore this request.");
	timeCount = 0;
	return ret;		/* Time out */
      }
      else{
	return BGL_REPLY_RET_NONE;	/* There is no reply yet */
      }
      break;
    case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"unexpected FD. (mistrust PROGRAM)");
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_COM_ERR:	/* Communication Error */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Error in BglMM_NB_ReplyWait");
      BglMM_Connect_Close();
      timeCount = 0;
      return ret;
      break;
    case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"something happened during bgl_select. (mistrust PROGRAM)");
      timeCount = 0;
      return ret;
      break;
    default:
      timeCount = 0;
      return ret;
      break;
    }
  }
}
