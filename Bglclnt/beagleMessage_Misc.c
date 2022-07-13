/*
 * beagleMessage_Misc.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Wed Oct 29 15:11:42 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:25:52 1999
 * Update Count    : 57
 * Status          : Unknown, Use with caution!
 */

#include "clnt.h"
#include "beagleProcCtrl.h"
#include "beagleMessageMgrP.h"
#include "beagleMessage_Misc.h"
#include "beagleUTIL_SockMgr.h"
#include "beagleUI.h"

/********************************************************************************
 * Static definitions
 */
static BglInjectGeneSt	BglInjectGeneBuf;

/***  about do_BGL_SIZEQUERY_O  ***/
#define	MM_SIZE_QUERY_MAX 50
MM_state_t MM_sizeQuery_s = MMS_NORMAL;
static I32s	MM_sizeQueryWi = 0;	/* Write Index */
static I32s	MM_sizeQueryRi = 0;	/* Read Index */
static BglSizQueryOutputSt	MM_sizeQueryData[MM_SIZE_QUERY_MAX];

/***  about Misc Hist Logging toggle functions  ***/
static MM_state_t MM_misc_h_s = MMS_NORMAL;

static I32s	BglMM_CheckInjectGene	P_((I8s*));

#ifdef NB_MECHANISM
/** functions for non blocking mechanism */
static MM_funcstate_t	BglMM_NB_Misc_h		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_h_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_SizeQuery_need	P_((void));
static MM_funcstate_t	BglMM_NB_SizeQuery_need_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_ReqSizeQuery_Need	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_i			P_((void));
static MM_funcstate_t	BglMM_NB_Misc_i_Wait		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_ie		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Suspend		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Suspend_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Suspend_Done	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Resume		P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Resume_Wait	P_((void));
static MM_funcstate_t	BglMM_NB_Misc_Resume_Done	P_((void));
#endif /* NB_MECHANISM */


/********************************************************************************
 * function implementation
 */

/******************************
 * size query buffer
 */
I32s	BglMM_HowManySizeQuery()
{
  I32s ret;
  ret = MM_sizeQueryWi - MM_sizeQueryRi;
  if ( ret < 0 ) ret += MM_SIZE_QUERY_MAX;
  return(ret);
}

void	BglMM_SizQuery_store(d)
     BglSizQueryOutputSt* d;
{
  /*DebugPrint("/==== BglMM_SizQuery_store ====/\n");*/
  if ( MM_sizeQuery_s == MMS_REQUESTED ||
       MM_sizeQuery_s == MMS_CHANGED ){

    MM_sizeQueryData[MM_sizeQueryWi].count = d->count;
    strcpy(MM_sizeQueryData[MM_sizeQueryWi].lbl, d->lbl);
    MM_sizeQueryData[MM_sizeQueryWi].moves = d->moves;
    MM_sizeQueryData[MM_sizeQueryWi].flags = d->flags;
    MM_sizeQueryData[MM_sizeQueryWi].bits = d->bits;
    MM_sizeQueryData[MM_sizeQueryWi].csS = d->csS;
    strcpy(MM_sizeQueryData[MM_sizeQueryWi].WatchBits, d->WatchBits);

    MM_sizeQuery_s = MMS_CHANGED;
    MM_sizeQueryWi++;
    if( MM_sizeQueryWi==MM_SIZE_QUERY_MAX )
      MM_sizeQueryWi=0;
  }
  else{
    DebugPrint("/==== BglMM_SizQuery_store ignored ====/\n");
  }
  return;
}

BglSizQueryOutputSt*	BglMM_PickSizeQuery()
{
  int ret_i;
  ret_i = MM_sizeQueryRi;
  MM_sizeQueryRi++;
  if( MM_sizeQueryRi==MM_SIZE_QUERY_MAX ) MM_sizeQueryRi=0;
  if( MM_sizeQueryRi==MM_sizeQueryWi ){
    MM_sizeQuery_s = MMS_NORMAL;
  }
  return &(MM_sizeQueryData[ret_i]);
}

/******************************
 * Size Query request
 */
void	BglMM_SizeQuery_Need(sizeStr)
     I8s* sizeStr;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[1];

  params[0] = sizeStr;
  
  MM_sizeQuery_s = MMS_REQUESTED;
  MM_sizeQueryRi = MM_sizeQueryWi = 0;

  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 params, 1 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_SizeQuery_need );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_SizeQuery_need_Wait ); sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_ReqSizeQuery_Need );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_SizeQuery(sizeStr);
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_SizeQuery(sizeStr)
     I8s* sizeStr;
{
  if ( BglMM_B_MenuBreak() ){
    if ( BglMM_B_SizeQuery_need() ){
      BglMM_B_ReqSizeQuery_Need(sizeStr);
    }
    if ( BglMM_B_Cont() ) return 1; /* Communication Error */
  }
  else{
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_SizeQuery was refused.");
  }
  return 0;
}

I32s	BglMM_B_SizeQuery_need()
{
  /*DebugPrint("/==== BglMM_B_SizeQuery_need ====/\n");*/
  if ( MM_sizeQuery_s == MMS_NORMAL ||
       MM_sizeQuery_s == MMS_REQUESTED ){

    if ( BglMM_ClntSend( &BglReqInfoZ ) ) return 0; /* Communication Error*/
    if ( BglMM_B_ReplyWait( BGL_INFO_Z_OK, BGL_INFO_Z_REF )
	 != BGL_REPLY_RET_OK )
      return 0;
    else
      return 1;
  }
  else return 0;
}

void	BglMM_B_ReqSizeQuery_Need(numStr)
     char* numStr;
{
  MM_sizeQuery_s = MMS_REQUESTED;
  MM_sizeQueryRi = MM_sizeQueryWi = 0;
  (void)BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd, numStr); 
  if ( ! BglMM_B_ClntRecv() ) return; /* Communication Error */
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_INFO_Z_BREAK);
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_SizeQuery_need()
{
  if ( !BglMM_ClntSend( &BglReqInfoZ ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_AGAIN;
}

static MM_funcstate_t	BglMM_NB_SizeQuery_need_Wait()
{
  I32s ret;

  ret = BglMM_NB_ReplyWait( BGL_INFO_Z_OK, BGL_INFO_Z_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static MM_funcstate_t	BglMM_NB_ReqSizeQuery_Need()
{
  (void)BglReplyMes(UTIL_Sock_GetCurrent_Connection()->fd,
		    BglMM_GetProcessParam(0)); 
  BglReplyAck(UTIL_Sock_GetCurrent_Connection()->fd, BGL_INFO_Z_BREAK);
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */

/******************************
 * hist logging
 */
I32s	BglMM_Misc_h_need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
#endif /* NB_MECHANISM */

  if ( MM_misc_h_s == MMS_NORMAL ){
#ifdef NB_MECHANISM
    MM_misc_h_s = MMS_NEED;

    BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			   NULL, NULL, BglMM_NB_Cont,
			   NULL, 0 );
    sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_h );		sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_h_Wait );	sP = sP->next;
    sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
      if ( !BglMM_B_MenuBreak() )
	return 0;

      if ( !BglMM_B_Misc_h())
	{
	  BglMM_B_Cont();
	  return 0;
	}
      
      if ( BglMM_B_Cont() )
	return 0;
#endif /* B_MECHANISM */

    return 1;
  }
  else return 0;
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_h()
{
  /*DebugPrint("/==== BglMM_B_Misc_h ====/\n");*/
  if (BglMM_ClntSend(&BglReqMiscHistLog))return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_MISC_HIST_OK, BGL_MISC_HIST_REF )
       != BGL_REPLY_RET_OK )
    /* NOP */
  if ( ! BglMM_B_ClntRecv() ) return 0; /* Communication Error */
  MM_misc_h_s =  MMS_NORMAL;
  return 1;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_h()
{
  /*DebugPrint("/==== BglMM_NB_Misc_h ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscHistLog ) ){
    /* Communication No Error */
    return MMFS_CONTINUE;
  }
  return MMFS_AGAIN;
}

static MM_funcstate_t	BglMM_NB_Misc_h_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_h_Wait ====/\n");*/
  ret = BglMM_NB_ReplyWait( BGL_MISC_HIST_OK, BGL_MISC_HIST_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    MM_misc_h_s =  MMS_NORMAL;
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  case BGL_REPLY_RET_TIMEOUT:
  case BGL_REPLY_RET_REF:
    MM_misc_h_s =  MMS_NORMAL;
    return MMFS_FAIL;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}
#endif /* NB_MECHANISM */

/******************************
 * gene injection
 */
void	BglMM_Inject_Need(nameStr, numStr, addrStr)
     I8s* nameStr;
     I8s* numStr;
     I8s* addrStr;
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  I8s *	params[3];

  params[0] = nameStr;
  params[1] = numStr;
  params[2] = addrStr;
  
  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, NULL, BglMM_NB_Cont,
			 params, 3 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_i );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_i_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_ie );		sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_Inject(nameStr, numStr, addrStr);
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_Inject(nameStr, numStr, addrStr)
     I8s* nameStr;
     I8s* numStr;
     I8s* addrStr;
{
  if ( BglMM_B_MenuBreak() ){
    if ( BglMM_B_Misc_i() ){
      BglMM_B_Misc_ie(nameStr, numStr, addrStr);
    }
    else{
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"Request Misc-Inject gene was refused.");
    }
    if ( BglMM_B_Cont() ) return 1;
  }
  else{
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Misc_Inject was refused.");
  }

  return 0;
}

I32s	BglMM_B_Misc_i()
{
  /*DebugPrint("/==== BglMM_B_Misc_i ====/\n");*/
  if ( BglMM_ClntSend( &BglReqMiscInject ) ) return 0;/*Communication Error*/
  if ( BglMM_B_ReplyWait( BGL_MISC_INJECT_OK, BGL_MISC_INJECT_REF )
       != BGL_REPLY_RET_OK )
    return 0;	/* Error return */
  else
    return 1;
}

I32s	BglMM_B_Misc_ie(nameStr, numStr, addrStr)
     I8s* nameStr;
     I8s* numStr;
     I8s* addrStr;
{
  I32s	cc;
  BglMesSt sendMes;
  I32s	len;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  /*DebugPrint("/==== BglMM_B_Misc_ie ====/\n");*/
  if( BglMM_CheckInjectGene(nameStr) ){
    if(nameStr[0] == '\n'){
      strcpy(BglInjectGeneBuf.gene, "ESC");
    }
    else{
      strcpy(BglInjectGeneBuf.gene, nameStr);
    }
    BglInjectGeneBuf.num  = atoi(numStr);
    BglInjectGeneBuf.addr = atoi(addrStr);
    
    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglInjectGeneSt(&msgHndl, &BglInjectGeneBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_B_Misc_ie()");
      return -1; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData( BGL_INJECT, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(UTIL_Sock_GetCurrent_Connection()->fd, sendMes.buf, sendMes.len );
    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
  }
  if ( ! (cc = BglMM_B_ClntRecv()) ) return 1; /* Communication Error */
  return cc;
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_i()
{
  /*DebugPrint("/==== BglMM_NB_Misc_i ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqMiscInject ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_AGAIN;
}

static MM_funcstate_t	BglMM_NB_Misc_i_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_i_Wait ====/\n");*/
  ret =  BglMM_NB_ReplyWait( BGL_MISC_INJECT_OK, BGL_MISC_INJECT_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static MM_funcstate_t	BglMM_NB_Misc_ie()
{
  I32s	len;
  BglMesSt sendMes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */
  
  strcpy(BglInjectGeneBuf.gene, BglMM_GetProcessParam(0));
  BglInjectGeneBuf.num  = atoi(BglMM_GetProcessParam(1));
  BglInjectGeneBuf.addr = atoi(BglMM_GetProcessParam(2));
  
  if( BglMM_CheckInjectGene(BglInjectGeneBuf.gene) ){
    if(BglInjectGeneBuf.gene[0] == '\n'){
      strcpy(BglInjectGeneBuf.gene, "ESC");
    }

    xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 0);
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglInjectGeneSt(&msgHndl, &BglInjectGeneBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
      BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
		"XDR encode error in the function BglMM_NB_Misc_ie()");
      return MMFS_FAIL;	/* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 1);
    len = txdr_getpos(&msgHndl);
    len = BglMakeMesData( BGL_INJECT, xdrMsgBuf, len, xdrMsgBuf2 );

    sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
    BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
    twrite(UTIL_Sock_GetCurrent_Connection()->fd, sendMes.buf, sendMes.len );
    tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 0);
    tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 1);
  }
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */

static I32s	BglMM_CheckInjectGene(buf)
     I8s* buf;
{
  I32s i;

  if (buf[0] == '\n') return 1;
  if (strlen(buf) != 7) return -1;
  for(i=0; i<4; i++){	if (!isdigit(buf[i])) return -1; }	
  for(i=5; i<7; i++){	if (!isalpha(buf[i])) return -1; }
  return 1;
}

/*********************************
 * Suspend for Tierra Life Loop 
 */
void	BglMM_Suspend_Need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  
  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, BglMM_NB_Misc_Suspend_Done, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_Suspend );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_Suspend_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_Suspend();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_Suspend()
{
  if ( BglMM_B_MenuBreak() ){
    if ( BglMM_ClntSend( &BglReqSuspendLife ) )return 1;/*Communication Error*/
    if ( BglMM_B_ReplyWait( BGL_SUSPENDLIFE_OK, BGL_SUSPENDLIFE_REF )
	!= BGL_REPLY_RET_OK )		      return 2;	/* Error return */
    BglUI_SuspendLifeLoop();
  }
  else{
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Misc_Suspend was refused.");
  }

  return 0; /* Normal return */
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_Suspend()
{
  /*DebugPrint("/==== BglMM_NB_Misc_Suspend ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqSuspendLife ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_AGAIN;
}

static MM_funcstate_t	BglMM_NB_Misc_Suspend_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_Suspend_Wait ====/\n");*/
  ret =  BglMM_NB_ReplyWait( BGL_SUSPENDLIFE_OK, BGL_SUSPENDLIFE_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static MM_funcstate_t	BglMM_NB_Misc_Suspend_Done()
{
  BglUI_SuspendLifeLoop();
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */

/*********************************
 * Resume for Tierra Life Loop 
 */
void	BglMM_Resume_Need()
{
#ifdef NB_MECHANISM
  MM_SequenceList_t* sP;
  
  BglMM_MakeProcessCell( sP = BglMM_MakeFuncCell( BglMM_NB_MenuBreak ),
			 NULL, BglMM_NB_Misc_Resume_Done, BglMM_NB_Cont,
			 NULL, 0 );
  sP->next = BglMM_MakeFuncCell( BglMM_NB_MenuBreak_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_Resume );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Misc_Resume_Wait );	sP = sP->next;
  sP->next = BglMM_MakeFuncCell( BglMM_NB_Cont );
#endif /* NB_MECHANISM */

#ifdef B_MECHANISM
  BglMM_B_Misc_Resume();
#endif /* B_MECHANISM */
}

#ifdef B_MECHANISM
I32s	BglMM_B_Misc_Resume()
{
  if ( BglMM_B_MenuBreak() ){
    if ( BglMM_ClntSend( &BglReqResumeLife ) )return 1;/*Communication Error*/
    if ( BglMM_B_ReplyWait( BGL_RESUMELIFE_OK, BGL_RESUMELIFE_REF )
	!= BGL_REPLY_RET_OK )		      return 2;	/* Error return */
    BglUI_ResumeLifeLoop();
  }
  else{
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Request BglMM_B_Misc_Resume was refused.");
  }

  return 0; /* Normal return */
}
#endif /* B_MECHANISM */

#ifdef NB_MECHANISM
static MM_funcstate_t	BglMM_NB_Misc_Resume()
{
  /*DebugPrint("/==== BglMM_NB_Misc_Resume ====/\n");*/
  if ( !BglMM_ClntSend( &BglReqResumeLife ) ){
    /* Communication NO Error */
    return MMFS_CONTINUE;
  }
  return MMFS_AGAIN;
}

static MM_funcstate_t	BglMM_NB_Misc_Resume_Wait()
{
  I32s ret;
  /*DebugPrint("/==== BglMM_NB_Misc_Resume_Wait ====/\n");*/
  ret =  BglMM_NB_ReplyWait( BGL_RESUMELIFE_OK, BGL_RESUMELIFE_REF );
  switch( ret ){
  case BGL_REPLY_RET_OK:
    return MMFS_CONTINUE;
    break;

  case BGL_REPLY_RET_NONE:	/* No reply data come from Tierra */
    return MMFS_AGAIN;
    break;

  default:
    return MMFS_FAIL;
    break;
  }
}

static MM_funcstate_t	BglMM_NB_Misc_Resume_Done()
{
  BglUI_ResumeLifeLoop();
  return MMFS_CONTINUE;
}
#endif /* NB_MECHANISM */
