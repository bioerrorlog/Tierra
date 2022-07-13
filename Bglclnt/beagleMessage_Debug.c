/* beagleMessage_Debug.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Sep 26 12:15:52 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:19:43 1999
 * Update Count    : 58
 * Status          : Unknown, Use with caution!
 */

#ifndef lint
static char* sccsProgramId="@(#)beagleMessage_Debug.c	1.19	11/22/99 16:20:03";
#endif /* lint */

#include "clnt.h"
#include "beagleUI.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_Debug.h"

static BrkPointSt	BglBrkPointBuf;

#define MM_MC_SPY_MAX	10
MM_state_t MM_mCSpy_s = MMS_NEED;
static I32s	MM_mCSpyWi = 0;	/* Write Index */
static I32s	MM_mCSpyRi = 0;	/* Read Index */
static BglMCSpySt	MM_mCSpyData[MM_MC_SPY_MAX];


#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_Debug_Toggle_abnormtermP	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_m		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_m_Wait	P_((void));

static MM_funcstate_t	BglMM_NB_Misc_mn	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_mT	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_mU	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_mN	P_((void));
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
static void		BglMM_B_MiscBrk_Put	P_((void));
static void		BglMM_B_MiscBrk_Repeat	P_((void));
static void		BglMM_B_MiscBrk_Clear	P_((void));
#endif /* B_MECHANISM */

static void		BglMM_Debug_BrkPut	P_((void));
#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_MiscBrk_Put	P_((void));
static MM_funcstate_t	BglMM_NB_MiscBrk_Put_Wait P_((void));
static MM_funcstate_t	BglMM_Debug_BrkPut_normtermP P_((void));
static MM_funcstate_t	BglMM_NB_MiscBrk_Clear	P_((void));
static MM_funcstate_t	BglMM_NB_MiscBrk_Clear_Wait P_((void));
static MM_funcstate_t	BglMM_NB_MiscBrk_Repeat	P_((void));
static MM_funcstate_t	BglMM_NB_MiscBrk_Repeat_Wait P_((void));
#endif /* NB_MECHANISM */


/*********************************************************************************
 * Commands for debugger control
 */
I32s	BglMM_Debug_Toggle()	/* public function */
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_Debug_Toggle_abnormtermP,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_m );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_m_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
  return 0;
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_m_Toggle();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_m_Toggle()	/* public function */
{
  if ( BglMM_B_MenuBreak() ){
    if ( BglMM_B_Misc_m() ){
      /* NOP */
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request Micro-Toggle was refused.");
    }
    if ( BglMM_B_Cont() ) return 1;
  }
  else{
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Misc_m_Toggle was refused.");
  }

  return 0;
}

I32s	BglMM_B_Misc_m()	/* public function */
{
  I32s ret;

  /*DebugPrint("/==== BglMM_B_Misc_m ====/\n");*/
  if ( BglMM_ClntSend( &BglReqMiscMicroToggle ) )
    return 0;/* Communication Error*/
  while(1){
    ret = BglMM_B_ReplyWait( BGL_MC_TOGGLE_OK, BGL_MC_TOGGLE_REF );
    if ( ret == BGL_REPLY_RET_OK ){ /* OK return */
      DebugPrint("BglMM_Misc_m : BGL_MC_TOGGLE_OK\n"); 
      if(BglMCStateBuf.MC_step == 'k'){
	Bgl_step = 1L;
	BglUI_MicroDebuggerToKeypressMode();
      } 
      else if(BglMCStateBuf.MC_step == 'd'){
	Bgl_step = 0L;
	BglUI_MicroDebuggerToDelayMode();
      }
      else{
	Bgl_step = -1L;
	BglUI_MicroDebuggerToOffMode();
      }
      return 1;
    }
    else if ( ret == BGL_REPLY_RET_REF )     return 0; /* refused */
    else if ( ret == BGL_REPLY_RET_TIMEOUT ) continue;/* time out, try again */
    else	return 0; /* Other ERROR */
  }
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_Debug_Toggle_abnormtermP()
{
  BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	    "Request Micro-Toggle was refused.");
  BglMM_NB_Cont();
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_Misc_m()
{
  /*DebugPrint("/==== BglMM_NB_Misc_m ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscMicroToggle ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}

static MM_funcstate_t	BglMM_NB_Misc_m_Wait()
{
  I32s ret;

  /*DebugPrint("/==== BglMM_NB_Misc_m_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_MC_TOGGLE_OK, BGL_MC_TOGGLE_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK: /* OK return */
    DebugPrint("BglMM_Misc_m : BGL_MC_TOGGLE_OK\n"); 
    if(BglMCStateBuf.MC_step == 'k'){
      Bgl_step = 1L;
      BglUI_MicroDebuggerToKeypressMode();
    } 
    else if(BglMCStateBuf.MC_step == 'd'){
      Bgl_step = 0L;
      BglUI_MicroDebuggerToDelayMode();
    }
    else{
      Bgl_step = -1L;
      BglUI_MicroDebuggerToOffMode();
    }
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */





void	BglMM_Misc_mq()		/* public function */
{
  /*DebugPrint("/==== BglMM_Misc_mq ====/\n");*/
  BglUI_MainMenu();
  return;	
}




void	BglMM_Debug_Next()	/* public function */
{  
#ifdef NB_MECHANISM
  BglMM_MakeProcessCell(BglMM_MakeFuncCell( BglMM_NB_Misc_mn ),
			NULL, NULL, NULL,
			NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_mn();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
void	BglMM_B_Misc_mn()	/* public function */
{
  /*DebugPrint("/==== BglMM_B_Misc_mn ====/\n");*/
  if ( BglMM_ClntSend(&BglReqDoMicroStepn) ) return; /*Communication Eror*/
  if ( ! BglMM_B_ClntRecv() ) return; /* Communication Error */
  return;	
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_mn()
{
  /*DebugPrint("/==== BglMM_NB_Misc_mn ====/\n");*/
  if ( !BglMM_ClntSend(&BglReqDoMicroStepn) ){
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}
#endif /* NB_MECHANISM */




void	BglMM_Debug_Slice_Toggle()
{
#ifdef NB_MECHANISM
  BglMM_MakeProcessCell(BglMM_MakeFuncCell( BglMM_NB_Misc_mT ),
			NULL, NULL, NULL,
			NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_mT();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
void	BglMM_B_Misc_mT()	/* public function */
{
  /*DebugPrint("/==== BglMM_B_Misc_mT ====/\n");*/
  if ( BglMM_ClntSend(&BglReqDoMicroStepSlice))return;/*Communication Error*/
  if ( ! BglMM_B_ClntRecv() ) return; /* Communication Error */
  return;	
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_mT()
{
  /*DebugPrint("/==== BglMM_NB_Misc_mT ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqDoMicroStepSlice ) ){
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;
}
#endif /* NB_MECHANISM */




void	BglMM_Debug_Cpu_Toggle()
{
#ifdef NB_MECHANISM
  BglMM_MakeProcessCell(BglMM_MakeFuncCell( BglMM_NB_Misc_mU ),
			NULL, NULL, NULL,
			NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_mU();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
void	BglMM_B_Misc_mU()	/* public function */
{
  /*DebugPrint("/==== BglMM_B_Misc_mU ====/\n");*/
  if ( BglMM_ClntSend(&BglReqDoMicroStepCpu)) return;/*Communication Error*/
  if ( ! BglMM_B_ClntRecv() ) return; /* Communication Error */
  return;	
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_mU()
{
  /*DebugPrint("/==== BglMM_NB_Misc_mU ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqDoMicroStepCpu ) ){
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;	
}
#endif /* NB_MECHANISM */




void	BglMM_Debug_New_Cpu()
{
#ifdef NB_MECHANISM
  BglMM_MakeProcessCell(BglMM_MakeFuncCell( BglMM_NB_Misc_mN ),
			NULL, NULL, NULL,
			NULL, 0 );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_mN();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
void	BglMM_B_Misc_mN()	/* public function */
{
  /*DebugPrint("/==== BglMM_B_Misc_mU ====/\n");*/
  if ( BglMM_ClntSend(&BglReqDoMicroStepNewCpu)) return;/*Communication Error*/
  if ( ! BglMM_B_ClntRecv() ) return; /* Communication Error */
  return;	
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_mN()
{
  /*DebugPrint("/==== BglMM_NB_Misc_mU ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqDoMicroStepNewCpu ) ){
    return MMFS_CONTINUE;
  }
  return MMFS_FAIL;	
}
#endif /* NB_MECHANISM */


/**********************************************************************************
 *				Breakpoint manager
 */
void	BglMM_Debug_Brk_Cell_off(namecell, offset)
     I8s* namecell;
     I32s* offset;
{
  BglBrkPointBuf.type = BGL_BRK_CELL_OFF;
  strcpy(BglBrkPointBuf.gene, namecell);
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = *offset;
  BglBrkPointBuf.time.m = 0;
  BglBrkPointBuf.time.i = 0;

  BglMM_Debug_BrkPut();
}

void	BglMM_Debug_Brk_CurrCell_off(offset)
     I32s* offset;
{
  BglBrkPointBuf.type = BGL_BRK_CURCELL_OFF;
  strcpy(BglBrkPointBuf.gene, "");
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = *offset;
  BglBrkPointBuf.time.m = 0;
  BglBrkPointBuf.time.i = 0;
 
  BglMM_Debug_BrkPut();
}

void	BglMM_Debug_Brk_AbsTime(time)
     BglEventSt* time;
{
  BglBrkPointBuf.type = BGL_BRK_ABSINST;
  strcpy(BglBrkPointBuf.gene, "");
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = 0;
  BglBrkPointBuf.time = *time;
 
  BglMM_Debug_BrkPut();
}

void	BglMM_Debug_Brk_RelTime(time)
     BglEventSt* time;
{
  BglBrkPointBuf.type = BGL_BRK_RELINST;
  strcpy(BglBrkPointBuf.gene, "");
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = 0;
  BglBrkPointBuf.time = *time;
 
  BglMM_Debug_BrkPut();
}

void	BglMM_Debug_Brk_ThrdSize(size)
     I8s* size;
{
  BglBrkPointBuf.type = BGL_BRK_THRDSIZE;
  strcpy(BglBrkPointBuf.gene, size);
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = 0;
  BglBrkPointBuf.time.m = 0;
  BglBrkPointBuf.time.i = 0;
 
  BglMM_Debug_BrkPut();
}

void	BglMM_Debug_Brk_ThrdGene(gene)
     I8s* gene;
{
  BglBrkPointBuf.type = BGL_BRK_THRDCELL;
  strcpy(BglBrkPointBuf.gene, gene);
  strcpy(BglBrkPointBuf.parasite, "");
  BglBrkPointBuf.offset = 0;
  BglBrkPointBuf.time.m = 0;
  BglBrkPointBuf.time.i = 0;
 
  BglMM_Debug_BrkPut();
}

/*
 * BglMM_Debug_BrkPut
 *	Send message to tierra for puting a breakpoint. Sequence is :
 * MCDO MCFEMENU MCDOFEM MCMICRO_BRK_PUT Data MCSELECT
 * non blocking mechanism
 */
static void	BglMM_Debug_BrkPut()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, BglMM_Debug_BrkPut_normtermP, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Put );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Put_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_MiscBrk_Put();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
/*
 * BglMM_B_MiscBrk_Put
 *	Send message to tierra for puting a breakpoint. Sequence is :
 * MCDO MCFEMENU MCDOFEM MCMICRO_BRK_PUT Data MCSELECT
 */
static void	BglMM_B_MiscBrk_Put()
{
  int len;
  BglMesSt sendmes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  if (!BglMM_B_MenuBreak()){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request MenuBreak was refused.");
    return;
  }
  
  if ( BglMM_ClntSend( &BglReqDoMicroBrkPut ) ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for command breakpoint refused.");
    return;
  }

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BrkPointSt(&msgHndl, &BglBrkPointBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR encode error in the function BglMM_B_MiscBrk_Put()");
    return; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_BRK, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11602);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  if ( BglMM_ClntSend(&sendmes) ){
    tfree(sendmes.buf,BGL_MAX_MES_LEN, 11602);
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
    return; /* Communication Error */
  }
  tfree(sendmes.buf,BGL_MAX_MES_LEN, 11602);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);

  if (BglMM_B_ReplyWait(BGL_MC_BRK_OK, BGL_MC_BRK_REF) == BGL_REPLY_RET_OK)
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted.");
  else
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused, breakpoint put");

  BglMM_B_Cont();
  BglMM_Polling_need();
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_Debug_BrkPut_normtermP()
{
  BglMM_Polling_need();
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_MiscBrk_Put()
{
  int len;
  BglMesSt sendmes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  if ( BglMM_ClntSend( &BglReqDoMicroBrkPut ) ){
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for command breakpoint refused.");
    return MMFS_FAIL;
  }

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BrkPointSt(&msgHndl, &BglBrkPointBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR encode error in the function BglMM_NB_MiscBrk_Put()");
    return MMFS_FAIL;	/* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData( BGL_BRK, xdrMsgBuf, len, xdrMsgBuf2 );

  sendmes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11602);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendmes);
  if ( BglMM_ClntSend(&sendmes) ){
    tfree(sendmes.buf,BGL_MAX_MES_LEN, 11602);
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
    return MMFS_FAIL; /* Communication Error */
  }
  tfree(sendmes.buf,BGL_MAX_MES_LEN, 11602);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_MiscBrk_Put_Wait()
{
  I32s ret;

  ret = BglMM_NB_ReplyWait( BGL_MC_BRK_OK, BGL_MC_BRK_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted.");
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused, breakpoint put");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */




void	BglMM_Debug_BrkClear()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Clear );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Clear_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_MiscBrk_Clear();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
static void	BglMM_B_MiscBrk_Clear()
{
  if (!BglMM_B_MenuBreak())
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request MenuBreak was refused.");
      return;
    }
  
  if ( BglMM_ClntSend( &BglReqDoMicroBrkClear ) )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request for command breakpoint refused.");
      return;
    }

  if (BglMM_B_ReplyWait(BGL_MC_BRK_OK, BGL_MC_BRK_REF) == BGL_REPLY_RET_OK)
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted, breakpoint cleared.");
  else
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused.");
  
  BglMM_B_Cont();
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_MiscBrk_Clear()
{
  if ( BglMM_ClntSend( &BglReqDoMicroBrkClear ) )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request for command breakpoint refused.");
      return MMFS_FAIL;
    }
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_MiscBrk_Clear_Wait()
{
  I32s ret;

  ret = BglMM_NB_ReplyWait( BGL_MC_BRK_OK, BGL_MC_BRK_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted, breakpoint cleared.");
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused, breakpoint clear.");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */




void	BglMM_Debug_BrkRepeat()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, BglMM_Debug_BrkPut_normtermP, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Repeat );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MiscBrk_Repeat_Wait ); sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_MiscBrk_Repeat();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
static void	BglMM_B_MiscBrk_Repeat()
{
  if (!BglMM_B_MenuBreak())
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request MenuBreak was refused.");
      return;
    }
  
  if ( BglMM_ClntSend( &BglReqDoMicroBrkRepeat ) )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request for command breakpoint refused.");
      return;
    }

  if (BglMM_B_ReplyWait(BGL_MC_BRK_OK, BGL_MC_BRK_REF) == BGL_REPLY_RET_OK)
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted, breakpoint repeated.");
  else
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused.");
  
  BglMM_B_Cont();
  BglMM_Polling_need();
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_MiscBrk_Repeat()
{
  if ( BglMM_ClntSend( &BglReqDoMicroBrkRepeat ) )
    {
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request for command breakpoint refused.");
      return MMFS_FAIL;
    }
  return MMFS_CONTINUE;
}

static MM_funcstate_t	BglMM_NB_MiscBrk_Repeat_Wait()
{
  I32s ret;

  ret = BglMM_NB_ReplyWait( BGL_MC_BRK_OK, BGL_MC_BRK_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint accepted, breakpoint repeated.");
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request for breakpoint refused, breakpoint repeat.");
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
    return MMFS_FAIL;
    break;

  default:
    /* NOP */
    break;
  }
  return MMFS_AGAIN;
}
#endif /* NB_MECHANISM */


/*******************************************************************
 *Commands for spy informations
 */
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_MCSpy_modeReset()
{
  MM_mCSpy_s = MMS_REQUESTED;
  MM_mCSpyRi = MM_mCSpyWi = 0;
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s	BglMM_HowManyMCSpy()
{
  I32s ret;
  ret = MM_mCSpyWi - MM_mCSpyRi;
  if ( ret < 0 ) ret += MM_MC_SPY_MAX;
  return(ret);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void	BglMM_MCSpy_store(d)
     BglMCSpySt* d;
{
  I32s i;
  /*DebugPrint("/==== BglMM_MCSpy_store ====/\n");*/
  MM_mCSpyData[MM_mCSpyWi].Ploidy = d->Ploidy;			/* #0 */
  MM_mCSpyData[MM_mCSpyWi].MinCellSize = d->MinCellSize;	/* #1 */
  MM_mCSpyData[MM_mCSpyWi].Stack_size = d->Stack_size;		/* #2 */
  MM_mCSpyData[MM_mCSpyWi].Numreg = d->Numreg;  		/* #3 */
  MM_mCSpyData[MM_mCSpyWi].cp_md_p = d->cp_md_p;        	/* #4 */
  MM_mCSpyData[MM_mCSpyWi].cp_md_s = d->cp_md_s;        	/* #5 */
  MM_mCSpyData[MM_mCSpyWi].cp_q_this_a = d->cp_q_this_a;    	/* #6 */
  MM_mCSpyData[MM_mCSpyWi].cp_q_this_i = d->cp_q_this_i;    	/* #7 */
  MM_mCSpyData[MM_mCSpyWi].cp_mm_s = d->cp_mm_s;		/* #8 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].lbl, d->lbl);			/* #9 */
  MM_mCSpyData[MM_mCSpyWi].soup = d->soup;		     	/* #10 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].id, d->id );			/* #11 */
  MM_mCSpyData[MM_mCSpyWi].cp_mm_p = d->cp_mm_p;		/* #12 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_ib = d->cp_c_ib;		/* #13 */
  for (i=0; i<BGL_MICRO_SPY_ST_N; i++){
    MM_mCSpyData[MM_mCSpyWi].cp_c_c_st[i] = d->cp_c_c_st[i]; /* #14 - 23 */
  }
  for (i=0; i<BGL_MICRO_SPY_RE_N; i++){
    MM_mCSpyData[MM_mCSpyWi].cp_c_c_re[i] = d->cp_c_c_re[i]; /* #24 - 35 */
  }
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_sp = d->cp_c_c_sp;		/* #36 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_ip = d->cp_c_c_ip;		/* #37 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_threadid = d->cp_c_c_threadid;/* #38 */
  MM_mCSpyData[MM_mCSpyWi].off_char = d->off_char;		/* #39 */
  MM_mCSpyData[MM_mCSpyWi].off_int = d->off_int;		/* #40 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_fl_E = d->cp_c_c_fl_E;	/* #41 */
  strncpy(MM_mCSpyData[MM_mCSpyWi].d_data, d->d_data, strlen(d->d_data));/* #42 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_ex = d->cp_c_c_ex;		/* #43 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_so = d->cp_c_c_so;		/* #44 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_c_de = d->cp_c_c_de;		/* #45 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_ac = d->cp_c_ac;		/* #46 */
  MM_mCSpyData[MM_mCSpyWi].cp_c_n = d->cp_c_n;			/* #47 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol0, d->regcol0);		/* #48 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol1, d->regcol1);		/* #49 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol2, d->regcol2);		/* #50 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol3, d->regcol3);		/* #51 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol4, d->regcol4);		/* #52 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].regcol5, d->regcol5);		/* #53 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf1, d->hostbuf1);	/* #54 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf2, d->hostbuf2);	/* #55 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf3, d->hostbuf3);	/* #56 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf4, d->hostbuf4);	/* #57 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf5, d->hostbuf5);	/* #58 */
  strcpy(MM_mCSpyData[MM_mCSpyWi].hostbuf6, d->hostbuf6);	/* #59 */

  MM_mCSpy_s = MMS_CHANGED;
  MM_mCSpyWi++; if( MM_mCSpyWi==MM_MC_SPY_MAX ) MM_mCSpyWi=0;
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
BglMCSpySt*	BglMM_PickMCSpy()
{
  int ret_i;
  ret_i = MM_mCSpyRi;
  MM_mCSpyRi++;
  if( MM_mCSpyRi==MM_MC_SPY_MAX ) MM_mCSpyRi=0;
  if( MM_mCSpyRi==MM_mCSpyWi ) MM_mCSpy_s = MMS_REQUESTED;
  return &(MM_mCSpyData[ret_i]);
}
