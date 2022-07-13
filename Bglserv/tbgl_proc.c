/* tbgl_proc.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:45:16 1999
 * Update Count    : 600
 * Status          : 
 */

/* Miscellaneous functions for hooking the beagle code                     */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf labels: #123XX, in use:   12301, 12302, 12310 - 12326 */
/* FEError labels: #123XX, in use:   12301 - 12306 */


#include "license.h"
#include "tierra.h"
#include "globals.h"

#include "tbgl.h"

static I32s BglSockTCP;		/* Tcp/Ip Socket Descriptor for Beagle */
static I32s BglQLEN  = 5;
static I32s BglNfds;		/* Number of descriptors */
static fd_set BglAfds, BglRfds;
static fd_set BglMigrationAfds;
static I32u TBglReqAppTbl[2][NCHRS]; /*Table for request command is approvable*/
static I8s BglMesBuf[BGL_MAX_MES][BGL_MAX_MES_LEN-1];
					/*Buffer of messages to be sent */ 
static I32s BglMP = 0;		/* BglMesBuf's pointer */
static I8s BglTblMes[BGL_MAX_MES];	/* Table of read or unread message */
static I8s BglTblPlan;			/* Table of read or unread Plan */
static BglOvMesSt	BglOvMesBuf;
static I32s BglMigrCtrlSockTCP;         /* socket which control migration */
static int  BglMigrCtrlQLEN = 10;  /* 10 clients can do a request to tierra */

static I32s TBglAccept		P_((void));
static void TBglRequestRecv	P_((void));
static void TBglShutdown	P_((I32s));
static void TBglMakeReq		P_((void));
static I32s TBglMakeBufOvG	P_((BglMesSt*, I32s, I32s));
static I32s TBglOvSend		P_((void));
static void TBglSetReqAppTbl	P_((void));
static I32s TBglGetReqApp	P_((const I8s*));
static int TBgl_bc_compare P_((const void *i, const void *j));
static int TBgl_bg_compare P_((const void *i, const void *j));

static I8s*	soupInVariableNameList[] = 
{
  (I8s *)"AliveGen",
#ifdef NET
  (I8s *)"ApocalypseFreq", (I8s *)"ApocFixFrac", (I8s *)"ApocRanFrac",
#endif /* NET */
  (I8s *)"alive",

#ifdef NET
  (I8s *)"BasSubNetChgFrq",
#ifndef SIGBLOCK
  (I8s *)"BklogProcFreq",
#endif /* SIGBLOCK */
#endif /* NET */

  (I8s *)"BrkupSiz",

  (I8s *)"CumGeneBnk",

  (I8s *)"DeconstructOn", (I8s *)"DiskBank", (I8s *)"DiskOut",
  (I8s *)"DistFreq", (I8s *)"DistNext.i",
  (I8s *)"DistNext.m", (I8s *)"DistProp", (I8s *)"DivSameGen",
  (I8s *)"DivSameSiz", (I8s *)"DropDead", (I8s *)"debug",

  (I8s *)"EjectRate",

  (I8s *)"FindTimeI", (I8s *)"FindTimeM",

  (I8s *)"GeneBnker", (I8s *)"GenebankPath", (I8s *)"GenPerBkgMut",
  (I8s *)"GenPerCroIns",
  (I8s *)"GenPerCroInsSamSiz", (I8s *)"GenPerCroSeg",
  (I8s *)"GenPerDelIns", (I8s *)"GenPerDelSeg",
  (I8s *)"GenPerDivMut", (I8s *)"GenPerFlaw", (I8s *)"GenPerInsIns",
  (I8s *)"GenPerInsSeg", (I8s *)"GenPerMovMut",

  (I8s *)"hangup",

  (I8s *)"IMapFile",

#if FRONTEND == BASIC
  (I8s *)"InstExeUpdFreq",
#endif /* FRONTEND == BASIC */

  (I8s *)"JmpSouTra", (I8s *)"JumpTrackProb",

  (I8s *)"LazyTol",
#ifdef NET
  (I8s *)"LocalPort",
#endif /* NET */
  (I8s *)"M_tvWait_sec",
  (I8s *)"M_OV_threshold",
  (I8s *)"MaxCpuPerCell", (I8s *)"MaxFreeBlocks", (I8s *)"MaxGetBufSiz",
  (I8s *)"MaxIOBufSiz", (I8s *)"MaxMalMult", (I8s *)"MaxPutBufSiz",
  (I8s *)"MaxSoupSize",

#ifdef NET
#ifdef __MWERKS__
  (I8s *)"MacHost", (I8s *)"MacSite",
#endif /* __MWERKS__ */
#endif /* NET */

  (I8s *)"MalMode", (I8s *)"MalReapTol", (I8s *)"MalSamSiz",
  (I8s *)"MalTol", (I8s *)"MateSizeEp",
#ifdef NET
  (I8s *)"map_fnC",
  (I8s *)"map_fnS",
#endif /* NET */
  (I8s *)"MemModeFree", (I8s *)"MemModeMine", (I8s *)"MemModeProt",
  (I8s *)"MigrCtrlPort",
  (I8s *)"MinCellSize", (I8s *)"MinGenMemSiz", (I8s *)"MinSoupSize",
  (I8s *)"MinTemplSize",
  (I8s *)"MonPort",
  (I8s *)"MovPropThrDiv", (I8s *)"MutBitProp",

  (I8s *)"new_soup", (I8s *)"NumCells", (I8s *)"NumCellsMin",

#ifdef NET
  (I8s *)"OnLineStat",
#endif /* NET */

  (I8s *)"PendReqMax", (I8s *)"PendReqTime", (I8s *)"PhotonPow",
  (I8s *)"PhotonWidth", (I8s *)"PhotonWord",
#ifdef NET
  (I8s *)"PingDatLog",
#endif /* NET */
  (I8s *)"PutLimit",

  (I8s *)"ReapRndProp",

  (I8s *)"SaveFreq", (I8s *)"SavMinNum", (I8s *)"SavRenewMem",
  (I8s *)"SavThrMem", (I8s *)"SavThrPop",
  (I8s *)"SearchLimit", (I8s *)"seed", (I8s *)"SizDepSlice",
  (I8s *)"SlicePow", (I8s *)"SliceSize", (I8s *)"SliceStyle",
  (I8s *)"SlicFixFrac", (I8s *)"SlicRanFrac", (I8s *)"SpeedUpdate",

#ifdef NET
  (I8s *)"SpdZeroTime", (I8s *)"SrvrApocSleep",
#endif

 (I8s *)"StrictIP",

#ifdef NET
  (I8s *)"SubNetChgApocProb",
  (I8s *)"SubNetChgFrqFixFrac",
  (I8s *)"SubNetChgFrqRanFrac",
  (I8s *)"SubNetCnt",

#if NETTYPE == TCP
  (I8s *)"TCPLocSelTmeSec", (I8s *)"TCPLocSelTmeUSec",
  (I8s *)"TCPRemSelTmeSec", (I8s *)"TCPRemSelTmeUSec",
#endif /* NETTYPE == TCP */
#endif /* NET */
  (I8s *)"ThrdAnaMaxFec", (I8s *)"ThrdAnaPartSave",
  (I8s *)"ThreadAnalysis", (I8s *)"TierraLog",
  (I8s *)"TierraNice",
  (I8s *)"TierraSleep",
#ifdef NET
  (I8s *)"TieMsgBkLog", (I8s *)"TieMsgMaxAge",
#if NETTYPE == UDP || NETTYPE == UDPASM
  (I8s *)"TieMTU",
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */
  (I8s *)"TieSubNet",
  (I8s *)"TrackNormLike",
#endif /* NET */

  (I8s *)"WatchExe", (I8s *)"WatchMov", (I8s *)"WatchTem",

  (I8s *)"XDRBufMaxSize",
  (I8s *)NULL
};

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglExit(ENO)
     I32s ENO;
{
  FEExit(ENO);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglTierraSet()
{
  I32s i;
  I8s BglPortStr[10];	/*Port number for Beagle*/

  tsprintf((char *)(&(BglPortStr[0])), "%d", MonPort);
  BglSock.user.priv = BGL_PRIV_F;
  BglSock.fd = 1;
  BglSockTCP = bgl_passiveTCP(BglPortStr, BglQLEN);
  SockBufSize = 0;
  DebugPrint2("BglSockTCP = %d port = %s\n", BglSockTCP, BglPortStr);
  FD_ZERO(&BglAfds);
  FD_SET(BglSockTCP, &BglAfds);
  BglNfds = bgl_open_max();
  fsmbuild ();
  ddbuild (); 
  TBglMakeReq();
  TBglSetReqAppTbl();

  /* Initialize the Masks of message buffer */
  FD_ZERO(&BglMask);
  FD_SET(BGL_MASK_MES, &BglMask);
  BglTblPlan = BGL_READ;
  BglTblHistClear = BGL_READ;
  for (i = 0; i < BGL_MAX_MES; i++)  BglTblMes[i] = BGL_READ;

  BglMCStateBuf.MC_step = 'o';
  BglMCStateBuf.Slice = 'T';
  BglMCStateBuf.Cpu = 'U';

  TBglDebugDisplayClr();

  TBglVarNumMax = 0; 
  while(soupInVariableNameList[TBglVarNumMax] != NULL) TBglVarNumMax++;
  BglVarBuf = (BglVarSt*)tcalloc(TBglVarNumMax, sizeof(BglVarSt), 12310);

  BglMessageBuf 
    = (BglMessageSt*)tcalloc(BGL_MAX_MES, sizeof(BglMessageSt), 12311);

#ifdef NET
  /* for Migration tracking data */
  BglMigrationDataBuf.node = LocalNetAddr.node;
  /* Init for sending nodes to tierra */
#endif /* NET */

  tsprintf((char *)(&(BglPortStr[0])), "%d", MigrCtrlPort);
  BglMigrCtrlSockTCP
    = bgl_passiveTCP( BglPortStr, BglMigrCtrlQLEN );

  FD_SET( BglMigrCtrlSockTCP, &BglAfds );
  BglMigrCtrlSockFd = -1;

}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglChangeCommunicationPort(newport)
     I8s *newport;
{
  I16u portnum;

  if ((!sscanf((const char *)newport,
    (const char *)"%hu", &portnum)) ||
    (portnum == MonPort))
    return 1;

  FD_CLR((I32u)BglSockTCP, &BglAfds);
  tclosesocket(BglSockTCP);
  BglSockTCP = bgl_passiveTCP(newport, BglQLEN);
  FD_SET(BglSockTCP, &BglAfds);
  MonPort = portnum;

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglRecvFunc(blk)
     I32s blk;
{
  static int	level = 0;
  struct sockaddr_in fsin;
  int		alen = sizeof(fsin);
  I32s		nselect;
  int		debugStay = 0;

#ifdef SIGBLOCK
  sigset_t SigSaveSet;
  SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

  if ( (level == 0) && (MC_step != -1L)){/* MICRO Debugger On */
    do_send_message();
    TBglNextFlg = 0;
    TBglNextBrkFlg = 0;
    debugStay = 1;
  }
  
  do{
    memcpy((I8s*) &BglRfds,(I8s*) &BglAfds, sizeof(BglRfds));
    if ((nselect = bgl_select(BglNfds, &BglRfds, 0)) < 0){
      FEError(-12301, NOEXIT, NOWRITE, "Select Error in TBglRecvFunc()");
    }
    else if (nselect > 0){
      if (FD_ISSET(BglSockTCP, &BglRfds)){
	if (TBglAccept()){
#ifdef SIGBLOCK
	  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
	  return; /* Accept Error */
	}
	else{
	  TBglRecv(BglSockBuf[SockBufSize - 1].fd);
	}
      }
      if (FD_ISSET(BglMigrCtrlSockTCP, &BglRfds)){
	if ( ( BglMigrCtrlSockFd
	       = accept( BglMigrCtrlSockTCP, (struct sockaddr*)&fsin, &alen ) )
	     < 0 ){
	  FEError(-12302, NOEXIT, NOWRITE, "Accept Error\n");
#ifdef SIGBLOCK
	  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
	  return;
	}
	else{
          FD_CLR((I32u)BglMigrCtrlSockTCP, &BglAfds );
	  FD_SET( BglMigrCtrlSockFd, &BglAfds );
	  BglRecv(BglMigrCtrlSockFd);
	}
      }

      if ( (level > 0) || (MC_step == -1L) ){ /* MICRO Debugger Off */
	level++;
	TBglRequestRecv();
	level--;
      }
      else{		 /* MICRO Debugger On */
	level++;
	TBglRequestRecv();
	level--;
	if( TBglNextFlg || TBglNextBrkFlg || MC_step == -1L ){
	  debugStay = 0;
	}
      } 
    }
  } while (( (blk != BGL_NONBLOCK) && (nselect == 0) )
	   || (debugStay && !level)
	   || (TBglSuspendLifeFlg && !level) );

#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  return;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s TBglAccept()
{
  struct sockaddr_in fsin;
  int alen = sizeof(fsin);
  I32s ret;

  DebugPrint("/**** TBglAccept() ***/\n"); 
  BglSockBuf[SockBufSize].fd =
    accept(BglSockTCP, (struct sockaddr *) &fsin, &alen);
  if (BglSockBuf[SockBufSize].fd < 0){
    tsprintf((char *)(&(bglStrBuf[0])), "Accept Error\n");
    FEError(-12302, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
    ret = 1; /* Accept Error */
  }
  else{	
    FD_SET(BglSockBuf[SockBufSize].fd, &BglAfds);
    tsprintf((char *)(&(bglStrBuf[0])),"TBglAccept(): Connected : s.fd = %d\n",
	     BglSockBuf[SockBufSize].fd);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, -1, bglStrBuf);
    SockBufSize++;
    ret = 0;
  }

  return ret;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void TBglRequestRecv()
{
  int i;
  
  for (i = 0 ; i < SockBufSize ; i++){
    if ((BglSockBuf[i].fd >= 0) && (FD_ISSET(BglSockBuf[i].fd, &BglRfds))){
      if(BglSockBuf[i].user.priv != BGL_PRIV_F){
	TBglRecv(BglSockBuf[i].fd);
      }
    }
  }
  if ((BglSock.fd >= 0) && FD_ISSET(BglSock.fd, &BglRfds)){
    if(BglSock.user.priv != BGL_PRIV_F){
      TBglRecv(BglSock.fd);
    }
  }
  if ((BglMigrCtrlSockFd >= 0) && FD_ISSET(BglMigrCtrlSockFd, &BglRfds)){
    if (BglRecv(BglMigrCtrlSockFd) <= 0){
      shutdown(BglMigrCtrlSockFd, 2);
      FD_CLR((I32u)BglMigrCtrlSockFd, &BglAfds);
      tclosesocket(BglMigrCtrlSockFd);
      BglMigrCtrlSockFd = -1;
      FD_SET(BglMigrCtrlSockTCP, &BglAfds);
    }
  }
  return;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglSend(fd, sendmes)
     I32s fd;
     BglMesSt* sendmes;
{
  I32s cc;

  if ((cc = BglSend(fd, sendmes)) <= 0){
    tsprintf((char *)(&(bglStrBuf[0])), "Connection Closed at TBglSend.");
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, -1, bglStrBuf);
    TBglShutdown(fd);
  }		
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglRecv(fd)
     I32s fd;
{
  I32s cc;

  if ((cc = BglRecv(fd)) <= 0){
    tsprintf((char *)(&(bglStrBuf[0])), "Connection Closed at TBglRecv.");
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, -1, bglStrBuf);
    TBglShutdown(fd);
  }
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglQuit(fd)
     I32s fd;
{
  BglMesSt tmp, sendmes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  BglQuitBuf.state = 1;	/* Now tentative */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);

#ifdef SIGBLOCK
  SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BglQuitSt(&msgHndl, &BglQuitBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR encode error in the function TBglQuit()");
    TBgl_make_femessages(bglStrBuf,strlen(
        (const char *)(&(bglStrBuf[0]))));
    return; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);
  tmp.len = txdr_getpos(&msgHndl);
  tmp.len = BglMakeMesData(BGL_QUIT, xdrMsgBuf, tmp.len, xdrMsgBuf2);
  tmp.buf = (I8s*)tcalloc(1, tmp.len, 12301);
  memcpy(tmp.buf, xdrMsgBuf2, tmp.len);
  TBglSendDat(fd, &sendmes, &tmp, tmp.len, 1);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);

  TBglShutdown(fd);
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void TBglShutdown(fd)
     I32s fd;
{
  I32s i;

  /* close TCP connection */
  FD_CLR((I32u)fd, &BglAfds);
  shutdown(fd, 2);
  (void)tclosesocket(fd);

  /* if, by error it is migration control port */
  if(fd == BglMigrCtrlSockFd)
    {
      BglMigrCtrlSockFd = -1;
      FD_SET(BglMigrCtrlSockTCP, &BglAfds);
      return;
    }

  /* if there is no beagle connected */
  if ( BglConnectFlg == 0)
    return;

  /* if this connection is into buffer, then erase it */
  for (i = 0 ; (i < SockBufSize) && (BglSockBuf[i].fd - fd) ; i++);
  if(i<SockBufSize)
  {   SockBufSize--;
      for(;i<SockBufSize;i++)
          BglSockBuf[i] = BglSockBuf[i+1];
  }
  
  /* if there are several connections, we keep virtual connection */
  if ( BglConnectFlg >= 2)
    {	
      BglConnectFlg--;
      return;
    }

  /* At this state, we close virtual connection with beagle, session is finished */ 
  BglConnectFlg = 0;	
  TBglSuspendLifeFlg = 0;	/* Turn off the Suspend life loop flag */
  FD_CLR(BGL_MASK_MICRO, &BglMask); /* add T.Yoshikawa '96-12-03*/

  BglMCStateBuf.MC_step = 'o';
  BglMCStateBuf.Slice = 'T';
  BglMCStateBuf.Cpu = 'U';

  MC_step = -1L;/* MICRO Debugger Off */

  BglSock.user.priv = BGL_PRIV_F;
  BglSock.fd = -1;

  if (IMode != PLN_STATS){
    CloseGeneBanker();
    if (Hist){
      thfree((void*) Hist, HistAlocSiz, 2101);
      Hist = (HistType*) NULL;
      HistAlocSiz = 0;
    }
    IMode = PLN_STATS;
    FEVidInit(0);
    FEPlan(0);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "Now in Plan Display mode, updated every million time steps");
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(bglStrBuf[0])));
    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);
  }
  FD_CLR(BGL_MASK_OV, &BglMask);

#ifdef NET
  if (TBgl_TPingS_Mode == 1 ){
    TBgl_TPingS_Mode = 0;
    if(BglTPingSBuf != NULL){
      tfree(BglTPingSBuf, BglTPingSAllocSiz*sizeof(BglTPingSt), 12106);
      tfree(BglTblTPingS, BglTPingSAllocSiz*sizeof(I8s), 12106);
      BglTPingSBuf = NULL;
      BglTPingSAllocSiz = BglNumIPMapSDat = 0;
    }
  }
  if (TBgl_TPingC_Mode == 1 ){
    TBgl_TPingC_Mode = 0;
    if(BglTPingCBuf != NULL){
      tfree(BglTPingCBuf, BglTPingCAllocSiz*sizeof(BglTPingSt), 12107);
      tfree(BglTblTPingC, BglTPingCAllocSiz*sizeof(I8s), 12107);
      BglTPingCBuf = NULL;
      BglTPingCAllocSiz = BglNumIPMapCDat = 0;
    }
  }
#endif /* NET */

  ttstate = MSSELECT;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglPlan(tp)
     time_t tp;
{
  BglPlanBuf.GeneBnker = GeneBnker;
  BglPlanBuf.InstExe_m = InstExe.m;
  BglPlanBuf.Generations = Generations;
  BglPlanBuf.tp = (I32s) tp;
  BglPlanBuf.NumCells = NumCells;
  BglPlanBuf.Speed = Speed;
  BglPlanBuf.RateMut = RateMut;
  BglPlanBuf.RateMovMut = RateMovMut;
  BglPlanBuf.RateFlaw = RateFlaw;
  BglPlanBuf.AverageSize = AverageSize;
  BglPlanBuf.AvgPop = AvgPop;
  BglPlanBuf.FecundityAvg = FecundityAvg;
  BglPlanBuf.RepInstEff = RepInstEff;
  BglPlanBuf.FEBirthNum = FEBirthNum;
  BglPlanBuf.FEDeathNum = FEDeathNum;
  if (GeneBnker){
    BglPlanBuf.NumGenotypes = NumGenotypes;
    BglPlanBuf.NumSizes = NumSizes;
    BglPlanBuf.MaxPop = MaxPop;
    BglPlanBuf.MaxGenPop_size = MaxGenPop.size;
    tsprintf((char *)(&(BglPlanBuf.MaxGenPop_label[0])),
        "%3.3s", MaxGenPop.label);
    BglPlanBuf.MaxMemry = MaxMemry;
    BglPlanBuf.MaxGenMem_size = MaxGenMem.size;
    tsprintf((char *)(&(BglPlanBuf.MaxGenMem_label[0])),
        "%3.3s", MaxGenMem.label);
    BglPlanBuf.NumGenDG = NumGenDG;
  }
  else{ /* (GeneBnker) */
    BglPlanBuf.NumGenotypes = 0;
    BglPlanBuf.NumSizes = 0;
    BglPlanBuf.MaxPop = 0;
    BglPlanBuf.MaxGenPop_size = 0;
    tsprintf((char *)(&(BglPlanBuf.MaxGenPop_label[0])), "000");
    BglPlanBuf.MaxMemry = 0;
    BglPlanBuf.MaxGenMem_size = 0;
    tsprintf((char *)(&(BglPlanBuf.MaxGenMem_label[0])), "000");
    BglPlanBuf.NumGenDG = 0;
  }
  BglPlanBuf.FEEjectToSelf = FEEjectToSelf;
#ifdef NET
  BglPlanBuf.NetFlg = BGL_FLG_TRUE;
  BglPlanBuf.NumNodes = NumNodesC;
  BglPlanBuf.FESImmigraNum = FEMonCom[EMIGRATETAG].SNumMess;
  BglPlanBuf.FESPingReqNum = FEMonCom[TPINGREQTAG].SNumMess;
  BglPlanBuf.FESPingRepNum = FEMonCom[TPINGREPTAG].SNumMess;
  BglPlanBuf.FESSendAddNum = FEMonCom[SENDADDRTAG].SNumMess;
  BglPlanBuf.FESByeNum     = FEMonCom[BYETAG].SNumMess;
  BglPlanBuf.FERImmigraNum = FEMonCom[EMIGRATETAG].RNumMess;
  BglPlanBuf.FERPingReqNum = FEMonCom[TPINGREQTAG].RNumMess;
  BglPlanBuf.FERPingRepNum = FEMonCom[TPINGREPTAG].RNumMess;
  BglPlanBuf.FERSendAddNum = FEMonCom[SENDADDRTAG].RNumMess;
  BglPlanBuf.FERByeNum     = FEMonCom[BYETAG].RNumMess;
  BglPlanBuf.FEBirthLocal  = FEBirthLocal;
  BglPlanBuf.FEBirthInject = FEBirthInject;
  BglPlanBuf.FEBirthEject  = FEBirthEject;
  BglPlanBuf.FEFecundYesSum = FEFecundYesSum;
  BglPlanBuf.FEFecunZeroSum = FEFecunZeroSum;
  BglPlanBuf.FEFecunEjecSum = FEFecunEjecSum;
#else /* NET */
  BglPlanBuf.NetFlg = BGL_FLG_FALSE;
  BglPlanBuf.NumNodes = 0;
  BglPlanBuf.FESImmigraNum = 0;
  BglPlanBuf.FESPingReqNum = 0;
  BglPlanBuf.FESPingRepNum = 0;
  BglPlanBuf.FESSendAddNum = 0;
  BglPlanBuf.FESByeNum = 0;
  BglPlanBuf.FERImmigraNum = 0;
  BglPlanBuf.FERPingReqNum = 0;
  BglPlanBuf.FERPingRepNum = 0;
  BglPlanBuf.FERSendAddNum = 0;
  BglPlanBuf.FERByeNum = 0;
  BglPlanBuf.FEBirthLocal = 0;
  BglPlanBuf.FEBirthInject = 0;
  BglPlanBuf.FEBirthEject = 0;
  BglPlanBuf.FEFecundYesSum = 0;
  BglPlanBuf.FEFecunZeroSum = 0;
  BglPlanBuf.FEFecunEjecSum = 0;
#endif /* NET */
#ifdef MEM_PROF
  BglPlanBuf.MemProFlg = BGL_FLG_TRUE;
  BglPlanBuf.DynMemUse = MemStat.DynMemUse;
  BglPlanBuf.TotNumAloc = MemStat.TotNumAloc;
#else /* MEM_PROF */
  BglPlanBuf.MemProFlg = BGL_FLG_FALSE;
  BglPlanBuf.DynMemUse = 0;
  BglPlanBuf.TotNumAloc = 0;
#endif /* MEM_PROF */
  BglTblPlan = BGL_UNREAD;	
}

#ifdef NET
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int TBglMigration()
{
  I32s migrationFd;
  BglMesSt	sendMes;
  I32s		len;
  I8s		service[20];
  I32s		ret;
  int		returnvalue = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
  SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

  /*   1. complete MigrationData structure.*/
  /* NOP */

  /*   2. open Beagle Migration data port.*/
  tsprintf((char *)service, "%d", BglMigrationConnectBuf.portnb );
  if ( ( migrationFd
	 = bgl_connectTCP((I8s *)inet_ntoa(*((struct in_addr*)
				      &(BglMigrationConnectBuf.node))),
			  service ) )
       < 0 ){
    returnvalue = 1;
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return returnvalue;
  }

  FD_ZERO(&BglMigrationAfds);
  FD_SET(migrationFd, &BglMigrationAfds);

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
  if (!xdr_BglMigrationDataSt(&msgHndl, &BglMigrationDataBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
    returnvalue = 1;
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR encode error in the function TBglMigration()");
    TBgl_make_femessages(bglStrBuf,strlen(
        (const char *)(&(bglStrBuf[0]))));
    return returnvalue; /* Communication (Encode) Error */
  }
    
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);
  len = txdr_getpos(&msgHndl);
  len = BglMakeMesData(BGL_MIGRATION, xdrMsgBuf, len, xdrMsgBuf2 );

  sendMes.buf = (I8s*)tcalloc(1, BGL_MAX_MES_LEN, 11502);
  BglMakeMes(BGL_TAG_DAT, xdrMsgBuf2, len, &sendMes);
  txdr_destroy(&msgHndl);
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);

  /*   3. send MigrationData to Beagle.*/
  twrite(migrationFd, (char *)(&(sendMes.buf[0])), sendMes.len );

  /*   4. wait Ack/Nack from Beagle via Migration data port.*/
  /*   5. if Ack received then do nothing
   *      else if Nack received then "MigrationTrackStop=MigrationTrackNew".
   */

  ret = BglReplyWait( migrationFd,
		      BglNfds, &BglMigrationAfds,
		      BGL_MIG_DATA_OK, BGL_MIG_DATA_REF, 60.0 );

  switch( ret ){
  case BGL_REPLY_RET_REF:	/* Request refused by Tierra */
    returnvalue = 2;
    break;
    
  case BGL_REPLY_RET_TIMEOUT:	/* message TIME OUT */
    returnvalue = 1;
    break;

  case BGL_REPLY_RET_OK:	/* GOOD! (Normal return) */
  case BGL_REPLY_RET_NONE:
  case BGL_REPLY_RET_UNEXPECT:  /* unexpected reply data come from Tierra */
  case BGL_REPLY_RET_ILL_FD:	/* unexpected FD */
  case BGL_REPLY_COM_ERR:	/* Communication Error */
  case BGL_REPLY_SELECT_ERR:	/* bgl_select return with -1 */
  default:
    /* NOP */
    break;
  }

  /*   6. close Beagle Migration data port.  */
  shutdown( migrationFd, 2 );
  tclosesocket( migrationFd );
  FD_CLR( (I32u)migrationFd, &BglMigrationAfds );

  tfree( sendMes.buf, BGL_MAX_MES_LEN, 11502 );

#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
  return returnvalue;
}
#endif /* NET */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void TBglMakeReq()
{
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int TBglNewestNotCurrentCpu()
{
  return (ce->c.ac != (ce->c.n) - 1);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBgl_Micro_Spy(cp)
     Pcells cp;
{
  int dbgentry = 0;

  if(BrkTrapCondBuf.curbrkdat.BrkSet){
    if(!ChkBrk(cp))
      dbgentry=1;
  }
  else
    if(BrkTrapCondBuf.curbrkdat.MicroSlice)
    {   if(cp==BrkTrapCondBuf.curbrkdat.MicroSlice)
        {   if(BrkTrapCondBuf.curbrkdat.MicroCpuI>=0)
            {   if(cp->c.ac == BrkTrapCondBuf.curbrkdat.MicroCpuI)
                    dbgentry=1;
	    }
        }
	else
	  dbgentry=1;
    }
    else
      if((MC_step==0)||(MC_step==1))
	dbgentry=1;


  if ( dbgentry ){
    TBglRecvFunc(BGL_BLOCK);

    if (!TBglNextBrkFlg)
      GoBrkClr();			/* delete all tracking */

    if( TBglNextFlg && !TBglNextBrkFlg)          
      switch (BglMCStateBuf.Cpu)
	{
	case 'U':
	  /* track cpu */
	  TrkCPU(cp,0,-1);
	  break;
	  
	case 'N':
	  /* track new cpu */
	  TrkCPU(cp,1,-1);
	  BglMCStateBuf.Cpu = 'U';
	  break;

	default:
	  /* No Cpu tracking */
	  switch (BglMCStateBuf.Slice)
	    {
	    case 'T':
	      /* track cell */
	      TrkCell(cp);
	    break;
	    
	    default:
	      /* No cell tracking */
	      break;
	  }
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
I32s BglSuAuthentCheck()
{
  FILE* fd_passwd;
  I8s BufFile[42], BufName[32], BufPasswd[16];
#ifdef unix
  I8s salt[3];
#else
  I32s i;
#endif

  if ((fd_passwd = tfopen((I8s *)"etc/passwd", (I8s *)"r")) == NULL){
    tsprintf((char *)(&(bglStrBuf[0])),
        "Error open etc/passwd in BglSuAuthentCheck()\n");
    FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
  }
  else{
    while (tfgets(BufFile, 42, fd_passwd) != NULL){
      BufFile[strlen((const char *)(&(BufFile[0])))-1]='\0';
      tsprintf((char *)(&(BufName[0])),
          (char *)"%s", strtok((char *)(
          &(BufFile[0])), (const char *)":"));
      tsprintf((char *)(&(BufPasswd[0])), "%s", strtok(NULL, ":"));
#ifdef unix
      salt[0]=BufPasswd[0];
      salt[1]=BufPasswd[1];
      salt[2]='\0';
      strcpy((char *)(&((BglSockBuf[0].user.passwd)[0])),
          (const char *)crypt((char *)(&((BglSockBuf[0].
          user.passwd)[0])), (const char *)(&(salt[0]))));
#else
      for(i=0;i<((int)(strlen((const char *)
          (&((BglSockBuf[0].user.passwd)[0])))));i++)
          (BglSockBuf[0].user.passwd)[i]=
          (I8s)(((I32s)((BglSockBuf[0].user.passwd)[i]))+
          ((I32s)(((BglSockBuf[0].user.name)[i])%10)));
      (BglSockBuf[0].user.passwd)[i]='\0';

#endif
      if((!strncmp((const char *)(&(BufName[0])),
          (const char *)(&(BglSockBuf[0].user.name[0])),
          strlen((const char *)(&(BufName[0])))))&&
	    (!strncmp((const char *)(&(BufPasswd[0])),
            (const char *)(&(BglSockBuf[0].user.passwd[0])),
            strlen((const char *)(&(BufPasswd[0])))))){
	tsprintf((char *)(&(bglStrBuf[0])), "BglSuAuthentCheck is OK.\n");
	FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
	tfclose(fd_passwd);
	return 1;
      }
    }
    tfclose(fd_passwd);
  }
  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntStats()
{
  return (1);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufStats(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  BglStatsSt tmpStats;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){
    tmpStats.GeneBnker    = GeneBnker;
    tmpStats.InstExe_m    = InstExe.m;
    tmpStats.InstExe_i    = InstExe.i;
    tmpStats.NumCells     = NumCells;
    tmpStats.NumGenotypes = NumGenotypes;
    if (GeneBnker){
      tmpStats.NumSizes   = NumSizes;
      strncpy((char *)(&(tmpStats.ExtrG_gen[0])),
          (const char *)(&(ExtrG[0])), BGL_EXTR_G_LEN);
    }
    else{
      tmpStats.NumSizes = -1;
      strncpy((char *)(&(tmpStats.ExtrG_gen[0])),
          (const char *)"----", BGL_EXTR_G_LEN);
    }

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglStatsSt(&msgHndl, &tmpStats)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufStats()");
      TBgl_make_femessages(bglStrBuf,strlen(
          (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    
    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_STATS,xdrMsgBuf,tmp[c+i].len,xdrMsgBuf2);
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntPlan()
{
  if (BglTblPlan == BGL_UNREAD)	return (1);
  else				return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufPlan(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  
    txdrmem_create(&msgHndl, xdrMsgBuf,	XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglPlanSt(&msgHndl, &BglPlanBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufPlan()");
      TBgl_make_femessages(bglStrBuf,strlen(
          (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_PLAN, xdrMsgBuf,tmp[c+i].len,xdrMsgBuf2);
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
    BglTblPlan = BGL_READ;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBgl_make_femessages(fbuf, flen)
     I8s* fbuf;
     I32s flen;
{
  memset(&BglMesBuf[BglMP][0], 0, BGL_MAX_MES_LEN-1);
  flen = flen > BGL_MAX_MES_LEN ? BGL_MAX_MES_LEN : flen;
  strncpy((char *)(&BglMesBuf[BglMP][0]), (const char *)fbuf, flen);
  if (BglMesBuf[BglMP][flen - 1] == '\n') BglMesBuf[BglMP][flen - 1] = '\0';
  BglTblMes[BglMP] = BGL_UNREAD;
  BglMP++;
  if (BglMP >= BGL_MAX_MES - 1)     BglMP = 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntMessage()
{
  I32s i, n = 0;

  for (i = 0; i < BGL_MAX_MES; i++)
    if (BglTblMes[i] == BGL_UNREAD){
      strcpy((char *)(&(BglMessageBuf[n].message[0])),
          (const char *)(BglMesBuf[i]));
      BglTblMes[i] = BGL_READ;
      n++;
    }
  return (n);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufMessage(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMessageSt(&msgHndl, &BglMessageBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufMessage()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_MESSAGE, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntMCState()
{
  return (1);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufMCState(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  /* update MC_step variable because tierra can change this value
   * if one breakpoint is encountered */
  switch(MC_step){
  case 1L:
    BglMCStateBuf.MC_step = 'k';
    break;
  case 0:
    BglMCStateBuf.MC_step = 'd';
    break;
  case -1L:
    BglMCStateBuf.MC_step = 'o';
    break;
  default:
    break;
  }

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  
    txdrmem_create(&msgHndl, xdrMsgBuf,	XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMCStateSt(&msgHndl, &BglMCStateBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufMCState()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_MC_STATE, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c + i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntMCSpy()
{
  if (FD_ISSET(BGL_MASK_MICRO, &BglMask))	return (1);
  else						return (0);
}



/*
 * TBgl_Brk_GivenGen_Offset
 *	create breakpoint for one given gene and offset from the start of this
 * gene.
 */
int TBgl_Brk_GivenGen_Offset()
{
  I32s  gidx;
  I32s	size;
  char	label[4];
  
  GoBrkClr();

  /* translate names for gene */
  if (sscanf((const char *)(&(BglBrkPointBuf.gene[0])),
    (const char *)"%d%3s", &size, label) !=2)
    return 0;
  OpenGeneBanker();
  gidx = Lbl2Int((I8s *)(&(label[0])));
  
  if ((!((size < 1) || (size > (siz_sl - 1)) || (!(sl[size]))))
      && (gidx < sl[size]->a_num) && (sl[size]->g[gidx]))
    {
      /* set trap parameters. */
      BrkTrapCondBuf.curbrkdat.BrkSet = 1;
      BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_GIVENGEN_OFFSET;
      BrkTrapCondBuf.curbrkdat.prim_genome.gi = gidx;
      BrkTrapCondBuf.curbrkdat.prim_genome.si = size;
      BrkTrapCondBuf.curbrkdat.genoffset = BglBrkPointBuf.offset;
      return 1;
    }
  CloseGeneBanker();
  GoBrkClr();
  return 0;
}

/*
 * TBgl_Brk_CurCell_Offset
 *	put a breakpoint for stoping when the current cell reach the offset given.
 */
int TBgl_Brk_CurCell_Offset()
{
  GoBrkClr();
  
  /* set trap parameters. */
  BrkTrapCondBuf.curbrkdat.BrkSet = 1;
  BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_CURCELL_OFFSET;
  if(BglMCStateBuf.Cpu == 'u')
    TrkCell(ce);
  else
    TrkCPU(ce,0,-1);
  BrkTrapCondBuf.curbrkdat.genoffset = BglBrkPointBuf.offset;
  return 1;
}

/*
 * TBgl_Brk_AbsTimeInst
 *	put breakpoint for stoping at a specific time.
 */
int TBgl_Brk_AbsTimeInst()
{
  GoBrkClr();  
  /* set trap parameters. */
  BrkTrapCondBuf.curbrkdat.BrkSet = 1;
  BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ABSTIMEINST;
  BrkTrapCondBuf.curbrkdat.AbsTime.m = BglBrkPointBuf.time.m;
  BrkTrapCondBuf.curbrkdat.AbsTime.i = BglBrkPointBuf.time.i;
  return 1;
}

/*
 * TBgl_Brk_IntrvlTimeInst
 *	put breakpoint at a specific time starting from current time.
 */
int TBgl_Brk_IntrvlTimeInst()
{
  BglEventSt	stoptime;
  
  GoBrkClr();
  stoptime.m  = InstExe.m;
  stoptime.i  = InstExe.i;
  stoptime.i += BglBrkPointBuf.time.i;
  if (stoptime.i > 999999){
    stoptime.m += 1;
    stoptime.i -= 1000000;
  }
  stoptime.m += BglBrkPointBuf.time.m;
  
  /* set trap parameters. */
  BrkTrapCondBuf.curbrkdat.BrkSet = 1;
  BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_INTRVLTIMEINST;
  BrkTrapCondBuf.curbrkdat.AbsTime.m = stoptime.m;
  BrkTrapCondBuf.curbrkdat.AbsTime.i = stoptime.i;
  BrkTrapCondBuf.curbrkdat.IntervalTime.m = BglBrkPointBuf.time.m;
  BrkTrapCondBuf.curbrkdat.IntervalTime.i = BglBrkPointBuf.time.i;
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int TBgl_Brk_ThrdAnaDatSizeRdy()
{
  I32s	size;
  
  GoBrkClr();

  /* translate names for gene */
  if (sscanf((const char *)(&(BglBrkPointBuf.gene[0])),
        (const char *)"%d", &size) != 1)
    return 0;
  OpenGeneBanker();

  if (!((size < 1) || (size > (siz_sl - 1)) || (!(sl[size]))))
  {
      GoBrkClr();
      /* set trap parameters. */
      BrkTrapCondBuf.curbrkdat.BrkSet	= 1;
      BrkTrapCondBuf.curbrkdat.TrapType	= BRKTYP_THRDANADATRDY;
      BrkTrapCondBuf.curbrkdat.szclsgn	= 's';
      BrkTrapCondBuf.curbrkdat.sizecls	= size;
      return 1;
  }
  CloseGeneBanker();
  GoBrkClr();
  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int TBgl_Brk_ThrdAnaDatGeneRdy()
{
  I32s  gidx;
  I32s	size;
  char	label[4];
  
  GoBrkClr();

  /* translate names for gene */
  if (sscanf((const char *)(&(BglBrkPointBuf.gene[0])),
      (const char *)"%d%3s", &size, label) !=2)
    return 0;
  OpenGeneBanker();
  gidx = Lbl2Int((I8s *)(&(label[0])));

  if ((!((size < 1) || (size > (siz_sl - 1)) || (!(sl[size]))))
      && (gidx < sl[size]->a_num) && (sl[size]->g[gidx]))
    {
      /* set trap parameters. */
      BrkTrapCondBuf.curbrkdat.BrkSet		= 1;
      BrkTrapCondBuf.curbrkdat.TrapType		= BRKTYP_THRDANADATRDY;
      BrkTrapCondBuf.curbrkdat.szclsgn		= 'g';
      BrkTrapCondBuf.curbrkdat.prim_genome.gi	= gidx;
      BrkTrapCondBuf.curbrkdat.prim_genome.si	= size;
      return 1;
    }
  CloseGeneBanker();
  GoBrkClr();
  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglDebugDisplayClr()
{
  memset(BglMCSpyBuf.regcol0, 0, BGL_MICRO_SPY_NUMSPECREG+1);
  memset(BglMCSpyBuf.regcol1, 0, BGL_MICRO_SPY_NUMSPECREG+1);
  memset(BglMCSpyBuf.regcol2, 0, BGL_MICRO_SPY_NUMSPECREG+1);
  memset(BglMCSpyBuf.regcol3, 0, BGL_MICRO_SPY_NUMSPECREG+1);
  memset(BglMCSpyBuf.regcol4, 0, BGL_MICRO_SPY_NUMSPECREG+1);
  memset(BglMCSpyBuf.regcol5, 0, BGL_MICRO_SPY_NUMSPECREG+1);

  memset(BglMCSpyBuf.hostbuf1, 0, 30);
  memset(BglMCSpyBuf.hostbuf2, 0, 30);
  memset(BglMCSpyBuf.hostbuf3, 0, 30);
  memset(BglMCSpyBuf.hostbuf4, 0, 30);
  memset(BglMCSpyBuf.hostbuf5, 0, 30);
  memset(BglMCSpyBuf.hostbuf6, 0, 30);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglDebugDisplay (cp, d_data, off_char, off_int,
		       hostbuf1, hostbuf2, hostbuf3,
		       hostbuf4, hostbuf5, hostbuf6)
     Pcells cp;
     I8s *d_data;
     I32s off_char;
     I32s off_int;
     I8s *hostbuf1, *hostbuf2, *hostbuf3, *hostbuf4, *hostbuf5, *hostbuf6;
{
  I32s i;

  BglMCSpyBuf.Ploidy = PLOIDY;
  BglMCSpyBuf.MinCellSize = MinCellSize;
  BglMCSpyBuf.Stack_size = STACK_SIZE;
  BglMCSpyBuf.Numreg = NUMREG;

  BglMCSpyBuf.cp_md_p = cp->md.p;
  BglMCSpyBuf.cp_md_s = cp->md.s;
  BglMCSpyBuf.cp_q_this_a = cp->q.htis.a;
  BglMCSpyBuf.cp_q_this_i = cp->q.htis.i;
  BglMCSpyBuf.cp_mm_s = cp->mm.s;
  strncpy((char *)(&(BglMCSpyBuf.lbl[0])),
      (const char *)Int2Lbl(cp->d.gi), 3);
#if PLOIDY == 1
  BglMCSpyBuf.soup = soup[ad(cp->c.c->ip)];
  strcpy((char *)(&(BglMCSpyBuf.id[0])),
      (const char *)(&(id[soup[ad(cp->c.c->ip)]].mn[0])));
#else /* PLOIDY == 1 */
  BglMCSpyBuf.soup = soup[ad(cp->c.c->ip)][cp->c.c->ex];
  strcpy(BglMCSpyBuf.id, id[soup[ad(cp->c.c->ip)][cp->c.c->ex]].mn);
#endif /* PLOIDY == 1 */
  BglMCSpyBuf.cp_mm_p = cp->mm.p;
  BglMCSpyBuf.cp_c_ib = cp->c.ib;
  for (i = 0; i < BGL_MICRO_SPY_ST_N; i++){
    BglMCSpyBuf.cp_c_c_st[i] = cp->c.c->st[i];
  }
  for (i = 0; i < BGL_MICRO_SPY_RE_N; i++){
    if( i > 8 )	BglMCSpyBuf.cp_c_c_re[i] = 0;
    else	BglMCSpyBuf.cp_c_c_re[i] = cp->c.c->re[i];
  }
  BglMCSpyBuf.cp_c_c_sp = cp->c.c->sp;
  BglMCSpyBuf.cp_c_c_ip = cp->c.c->ip;
  BglMCSpyBuf.cp_c_c_threadid = cp->c.c->threadid;
  BglMCSpyBuf.off_char = (I8s)off_char;
  BglMCSpyBuf.off_int = cp->c.c->ip - cp->mm.p;
  BglMCSpyBuf.cp_c_c_fl_E = (int) cp->c.c->fl.E;
  if (cp->md.s >= MinCellSize)
    tsprintf((char *)(&(d_data[0])), "Daughter @ %-6d + %-6d",
	     cp->md.p, cp->md.s);
  else
    tsprintf((char *)(&(d_data[0])), "NO Daughter");
  strcpy((char *)(&(BglMCSpyBuf.d_data[0])), (const char *)d_data);
#if PLOIDY == 1
  BglMCSpyBuf.cp_c_c_ex = 0;
  BglMCSpyBuf.cp_c_c_so = 0;
  BglMCSpyBuf.cp_c_c_de = 0;
#else /* PLOIDY == 1 */
  BglMCSpyBuf.cp_c_c_ex = (I32s) cp->c.c->ex;
  BglMCSpyBuf.cp_c_c_so = (I32s) cp->c.c->so;
  BglMCSpyBuf.cp_c_c_de = (I32s) cp->c.c->de;
#endif /* PLOIDY == 1 */
  BglMCSpyBuf.cp_c_ac = (I32s) cp->c.ac;
  BglMCSpyBuf.cp_c_n = (I32s) cp->c.n;
  strcpy((char *)(&(BglMCSpyBuf.hostbuf1[0])),
     (const char *)(&(hostbuf1[0])));
  strcpy((char *)(&(BglMCSpyBuf.hostbuf2[0])),
     (const char *)(&(hostbuf2[0])));
  strcpy((char *)(&(BglMCSpyBuf.hostbuf3[0])),
     (const char *)(&(hostbuf3[0])));
  strcpy((char *)(&(BglMCSpyBuf.hostbuf4[0])),
     (const char *)(&(hostbuf4[0])));
  strcpy((char *)(&(BglMCSpyBuf.hostbuf5[0])),
     (const char *)(&(hostbuf5[0])));
  strcpy((char *)(&(BglMCSpyBuf.hostbuf6[0])),
     (const char *)(&(hostbuf6[0])));

  FD_SET(BGL_MASK_MICRO, &BglMask);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufMCSpy(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len=0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMCSpySt(&msgHndl, &BglMCSpyBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufMCSpy()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_MC_SPY, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
    FD_CLR(BGL_MASK_MICRO, &BglMask);
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoHistClr()
{
  if (BglTblHistClear == BGL_UNREAD)	return (1);
  else					return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoHistClr(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len=0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglHistClearSt(&msgHndl, &BglHistClearBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoHistClr()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_HISTCLEAR, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
    BglTblHistClear = BGL_READ;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoSiz()
{
  I32s t, num = 0;

  if (FD_ISSET(BGL_MASK_INFO_S, &BglMask)){
    for (t = 0; t < Bgl_nnn; t++){
      if ( BglTblSizHist[t] == BGL_UNREAD ) num++;
    }
    return (num);
  }
  else
    return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoSiz(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, num = 0, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for ( i = 0; i < Bgl_nnn; i++ ){
    if ( BglTblSizHist[i] == BGL_READ ) continue;
    if ( num >= n ) break;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglSizHistSt(&msgHndl, &BglSizHistBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoSiz()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(BGL_SIZHIST, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    BglTblSizHist[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoGen()
{
  I32s t, num = 0;

  if (FD_ISSET(BGL_MASK_INFO_G, &BglMask)){
    for (t = 0; t < Bgl_nnn; t++){
      if ( BglTblGenHist[t] == BGL_UNREAD ) num++;
    }
    return (num);
  }
  else
    return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoGen(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, num = 0, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for ( i = 0; i < Bgl_nnn; i++ ){
    if ( BglTblGenHist[i] == BGL_READ ) continue;
    if ( num >= n ) break;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglGenHistSt(&msgHndl, &BglGenHistBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoGen()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(BGL_GENHIST, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    BglTblGenHist[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoMem()
{
  I32s t, num = 0;

  if (FD_ISSET(BGL_MASK_INFO_M, &BglMask)){
    for (t = 0; t < Bgl_nnn; t++){
      if ( BglTblMemHist[t] == BGL_UNREAD ) num++;
    }
    return (num);
  }
  else
    return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoMem(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, num = 0, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for ( i = 0; i < Bgl_nnn; i++ ){
    if ( BglTblMemHist[i] == BGL_READ ) continue;
    if ( num >= n ) break;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglMemHistSt(&msgHndl, &BglMemHistBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoMem()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(BGL_MEMHIST, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    BglTblMemHist[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoEfficS()
{
  I32s t, num = 0;

  if (FD_ISSET(BGL_MASK_INFO_ES, &BglMask)){
    for (t = 0; t < Bgl_nnn; t++){
      if ( BglTblEfficSHist[t] == BGL_UNREAD ) num++;
    }
    return (num);
  }
  else
    return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoEfficS(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, num = 0, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for ( i = 0; i < Bgl_nnn; i++ ){
    if ( BglTblEfficSHist[i] == BGL_READ ) continue;
    if ( num >= n ) break;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglEfficSHistSt(&msgHndl, &BglEfficSHistBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoEfficS()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(BGL_ESHIST, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    BglTblEfficSHist[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntInfoEfficG()
{
  I32s t, num = 0;

  if (FD_ISSET(BGL_MASK_INFO_EG, &BglMask)){
    for (t = 0; t < Bgl_nnn; t++){
      if ( BglTblEfficGHist[t] == BGL_UNREAD ) num++;
    }
    return (num);
  }
  else
    return (0);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoEfficG(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, num = 0, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for ( i = 0; i < Bgl_nnn; i++ ){
    if ( BglTblEfficGHist[i] == BGL_READ ) continue;
    if ( num >= n ) break;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglEfficGHistSt(&msgHndl, &BglEfficGHistBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoEfficG()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(BGL_EGHIST, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    BglTblEfficGHist[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

#ifdef NET
/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglAlterIPMap(n, whichmap)
     I32s n, whichmap;
{
  TBglCpyIPMap(n, n, 'a', whichmap);
  if (whichmap){	/* Server side map */
    BglTblTPingS[n] = BGL_UNREAD;
  }
  else{			/* Cluster side map */
    BglTblTPingC[n] = BGL_UNREAD;
  }
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglAddIPMap(n, whichmap)
     I32s n, whichmap;
{
  I32s siz;

  siz = sizeof(BglTPingSt);
  if (whichmap){	/* Server side map */
    if (BglTPingSBuf == NULL){
      BglTPingSBuf = (BglTPingSt*)tcalloc(1, siz, 12106);
      BglTblTPingS = (I8s*)tcalloc(1, sizeof(I8s), 12106);
      BglTPingSAllocSiz = 1;
      BglNumIPMapSDat = 0;
    }
    else{
      if (BglTPingSAllocSiz == BglNumIPMapSDat){
	BglTPingSBuf = (BglTPingSt*)trecalloc((void*)BglTPingSBuf,
					      (BglTPingSAllocSiz + 1) * siz,
					      BglTPingSAllocSiz * siz, 12106);
	BglTblTPingS = (I8s*)trecalloc((void*)BglTblTPingS,
				       (BglTPingSAllocSiz + 1) * sizeof(I8s),
				       BglTPingSAllocSiz * sizeof(I8s), 12106);
	BglTPingSAllocSiz++;
      }
    }
    TBglCpyIPMap(n, n, 'b', whichmap);
    BglTblTPingS[n] = BGL_UNREAD;
    BglNumIPMapSDat++;
  }
  else{			/* Cluster side map */
    if (BglTPingCBuf == NULL){
      BglTPingCBuf = (BglTPingSt*)tcalloc(1, siz, 12107);
      BglTblTPingC = (I8s*)tcalloc(1, sizeof(I8s), 12107);
      BglTPingCAllocSiz = 1;
      BglNumIPMapCDat = 0;
    }
    else{
      if (BglTPingCAllocSiz == BglNumIPMapCDat){
	BglTPingCBuf = (BglTPingSt*)trecalloc((void*)BglTPingCBuf,
					      (BglTPingCAllocSiz + 1) * siz,
					      BglTPingCAllocSiz * siz, 12107);
	BglTblTPingC = (I8s*)trecalloc((void*)BglTblTPingC,
				       (BglTPingCAllocSiz + 1) * sizeof(I8s),
				       BglTPingCAllocSiz * sizeof(I8s), 12107);
	BglTPingCAllocSiz++;
      }
    }
    TBglCpyIPMap(n, n, 'b', whichmap);
    BglTblTPingC[n] = BGL_UNREAD;
    BglNumIPMapCDat++;
  }

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglRemoveIPMap(n, whichmap)
     I32s n, whichmap;
{
  BglTPingSt	*newMap;
  I8s*		newTbl;
  I32s siz;
  I32s i, j = 0;

  siz = sizeof(BglTPingSt);
  if (whichmap){	/* Server side map */
    newMap = (BglTPingSt*)tcalloc(BglNumIPMapSDat, siz, 12106);
    newTbl = (I8s*)tcalloc(BglNumIPMapSDat, sizeof(I8s), 12106);
    for (i = 0; i < BglNumIPMapSDat; i++){
      if (i != n){
	newMap[j] = BglTPingSBuf[i];
	if (j < n){
	  newTbl[j] = BglTblTPingS[i];
	}
	else{
	  newMap[j].id--;
	  newTbl[j] = BGL_UNREAD;
	}
	j++;
      }
    }
    tfree(BglTPingSBuf, BglTPingSAllocSiz * siz, 12106);
    BglTPingSBuf = newMap;
    tfree(BglTblTPingS, BglTPingSAllocSiz * sizeof(I8s), 12106);
    BglTblTPingS = newTbl;
    BglTPingSAllocSiz = BglNumIPMapSDat;
    BglNumIPMapSDat--;
  }
  else{			/* Cluster side map */
    newMap = (BglTPingSt*)tcalloc(BglNumIPMapCDat, siz, 12107);
    newTbl = (I8s*)tcalloc(BglNumIPMapCDat, sizeof(I8s), 12107);
    for (i = 0; i < BglNumIPMapCDat; i++){
      if (i != n){
	newMap[j] = BglTPingCBuf[i];
	if (j < n){
	  newTbl[j] = BglTblTPingC[i];
	}
	else{
	  newMap[j].id--;
	  newTbl[j] = BGL_UNREAD;
	}
	j++;
      }
    }
    tfree(BglTPingCBuf, BglTPingCAllocSiz * siz, 12107);
    BglTPingCBuf = newMap;
    tfree(BglTblTPingC, BglTPingCAllocSiz * sizeof(I8s), 12107);
    BglTblTPingC = newTbl;
    BglTPingCAllocSiz = BglNumIPMapCDat;
    BglNumIPMapCDat--;
  }
  TBglCpyIPMap(j, j, 'd', whichmap);
  newTbl[j] = BGL_UNREAD;

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglCpyIPMap(i, n, mode, whichmap)
     I32s i, n;
     I32s mode;
     I32s whichmap;
{
  MapNode  *IPMap;
  BglTPingSt *mapP;

  if (whichmap){
    IPMap = IPMapS;
    mapP = BglTPingSBuf;	/* Server side map  */
  }
  else{
    IPMap = IPMapC;
    mapP = BglTPingCBuf;	/* Cluster side map */
  }

  if( mode != 'd'){
    mapP[i].mode = (I8s)mode;
    mapP[i].id = n;
    mapP[i].node = IPMap[n].address.node;
    mapP[i].portnb = IPMap[n].address.portnb;
    mapP[i].FecundityAvg = IPMap[n].data.FecundityAvg;
    mapP[i].Speed = IPMap[n].data.Speed;
    mapP[i].NumCells = IPMap[n].data.NumCells;
    mapP[i].AgeAvg = IPMap[n].data.AgeAvg;
    mapP[i].SoupSize = IPMap[n].data.SoupSize;
    mapP[i].TransitTime = IPMap[n].data.TransitTime;
    mapP[i].Time = IPMap[n].data.Time;
    mapP[i].InstExec = IPMap[n].data.InstExec;
    mapP[i].InstExecConnect = IPMap[n].data.InstExecConnect;
    mapP[i].OS = IPMap[n].data.OS;
  }
  else{
    mapP[i].mode = (I8s)mode;
    mapP[i].id = n;
    mapP[i].node =  0;
    mapP[i].portnb = 0;
    mapP[i].FecundityAvg = 0;
    mapP[i].Speed = 0;
    mapP[i].NumCells = 0;
    mapP[i].AgeAvg = 0;
    mapP[i].SoupSize = 0;
    mapP[i].TransitTime = 0;
    mapP[i].Time = 0;
    mapP[i].InstExec = 0;
    mapP[i].InstExecConnect = 0;
    mapP[i].OS = 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglCntTPing(whichmap)
     I32s whichmap;
{
  I32s i, n = 0;

  if (whichmap){	/* Server side map */
    if (TBgl_TPingS_Mode != 1)	return 0;
    else{
      for (i = 0; i < BglTPingSAllocSiz; i++){
	if (BglTblTPingS[i] == BGL_UNREAD) n++;
      }
    }
  }
  else{			/* Cluster side map */
    if (TBgl_TPingC_Mode != 1)	return 0;
    else{
      for (i = 0; i < BglTPingCAllocSiz; i++){
	if (BglTblTPingC[i] == BGL_UNREAD) n++;
      }
    }
  }

  return n;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufTPing(tmp, c, n, whichmap)
     BglMesSt* tmp;
     I32s c, n, whichmap;
{
  I32s i, num = 0, max, len=0, code;
  BglTPingSt *mapP;
  I8s* tableP;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  if (whichmap){		/* Server side map  */
    mapP = BglTPingSBuf;
    tableP = BglTblTPingS;
    max = BglTPingSAllocSiz;
    code = BGL_TPING_S;
  }
  else{				/* Cluster side map */
    mapP = BglTPingCBuf;
    tableP = BglTblTPingC;
    max = BglTPingCAllocSiz;
    code = BGL_TPING_C;
  }

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i = 0; i < max; i++){  
    if ( tableP[i] == BGL_READ ) continue;

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglTPingSt(&msgHndl, &mapP[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufTPing()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    
    tmp[c+num].len = txdr_getpos(&msgHndl);
    tmp[c+num].len = BglMakeMesData(code, xdrMsgBuf,
				    tmp[c+num].len, xdrMsgBuf2 );
    tmp[c+num].buf = (I8s*)tcalloc(1, tmp[c+num].len, 12301);
    memcpy(tmp[c+num].buf, xdrMsgBuf2, tmp[c+num].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+num].len;
    tableP[i] = BGL_READ;
    num++;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}
#endif /* NET */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglSendDat (fd, sendmes, tmp, total_len, cnt)
     I32s fd;
     BglMesSt* sendmes;
     BglMesSt* tmp;
     I32s total_len, cnt;
{
  I32s nlen, loc, i, cc;

  sendmes->len = total_len + BGL_MES_HEADER_LEN;
  sendmes->buf = (I8s*)tcalloc(1, sendmes->len, 12302);
  nlen =  htonl(total_len);
  memcpy(&sendmes->buf[0], &nlen, BGL_MES_HEADER_LEN-1);
  sendmes->buf[BGL_MES_HEADER_LEN-1] = BGL_TAG_DAT;
  loc = BGL_MES_HEADER_LEN;
  for(i=0; i<cnt; i++){
    memcpy(&sendmes->buf[loc], &tmp[i].buf[0], tmp[i].len);
    loc += tmp[i].len;
  }
  cc = TBglSend(fd, sendmes);
  for(i=0; i<cnt; i++){
    tfree(tmp[i].buf, tmp[i].len, 12301);
  }
  tfree(sendmes->buf, sendmes->len, 12302);
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufInfoZ(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){
    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglSizQueryOutputSt(&msgHndl, &BglSizQueryOutputBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufInfoZ()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_SIZQUERY_O, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufVar(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i,len=0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){  

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglVarSt(&msgHndl, &BglVarBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufVar()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_VAR, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12302);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglGetAVar()
{
  I32s i, id = 0, ret;

  DebugPrint1("TBglVarNumMax = %d\n", TBglVarNumMax);
  for (i = 0; i < TBglVarNumMax; i++){
    ret = GetAVar(soupInVariableNameList[i], 0, 1,
        sizeof(soupInVariableNameList[i]));
    if (ret == 0){
      tsprintf((char *)(&(bglStrBuf[0])),
	       "Tierra TBglGetAVar() bad soup_in variable name: %s\n",
	       soupInVariableNameList[i]);
      FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(bglStrBuf[0])));
    }
    else{
      BglVarBuf[id].index = id;
      BglVarBuf[id].type = vqu.type;
      strcpy((char *)(&(BglVarBuf[id].name[0])),
          (const char *)(&(vqu.name[0])));
      strcpy((char *)(&(BglVarBuf[id].value[0])),
          (const char *)(&(vqu.value[0])));
      DebugPrint3("BglVarBuf[%d].name : %s = %s\n",
		  id, BglVarBuf[id].name, BglVarBuf[id].value);
      id++;
    }
  }

  DebugPrint1("TBglGetAVar() id = %d\n", id);
  return id;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglOvFrame()
{
  BglOvFrameBuf.InstNum = InstNum;
  BglOvFrameBuf.SoupSize = SoupSize;
  BglOvFrameBuf.NumCells = NumCells;
  BglOvFrameBuf.NumSizes = NumSizes;
  BglOvFrameBuf.NumGenotypes = NumGenotypes;

  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufOvFrame(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglOvFrameSt(&msgHndl, &BglOvFrameBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufOvFrame()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_OVFRAME, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c + i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglOvInst()
{
  I32s i;

  if (BglOvGInstBuf == NULL)
    BglOvGInstBuf = (BglOvGInstSt*)tcalloc(InstNum,
					   sizeof(BglOvGInstSt), 12302);
  for (i=0; i<InstNum; i++){
    BglOvGInstBuf[i].op = (I32u) id[i].op;
    strcpy((char *)(&(BglOvGInstBuf[i].mn[0])),
        (const char *)(&(id[i].mn[0])));
    DebugPrint4("id[%d].op = %x BglOvGInstBuf[i].op = %x  %s\n",
		i, id[i].op, BglOvGInstBuf[i].op, BglOvGInstBuf[i].mn);
  }
  return InstNum;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglMakeBufOvInst(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglOvGInstSt(&msgHndl, &BglOvGInstBuf[i])){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufOvInst()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    
    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_OV_G_INST, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s*)tcalloc(1, tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglOvInit ()
{
  I32s n=0, ar, ci;
  Pcells cp;

  BglOvMesBuf.event = 's';
  BglOvMesBuf.start = 0;
  BglOvMesBuf.length = 0;
  TBglOvSend();
  n++;

  for (ar = 0; ar < NumCelAr; ar++){
    for (ci = 0; ci < CelArSiz; ci++){
      if (ar == 0 && ci < 2)                continue;
      cp = &cells[ar][ci];
      if (cp->ld){
	if (cp->mm.s){
	  TBglBirth( cp->mm.p, cp->mm.s );
	  n++;
	}
	if (cp->md.s){
	  TBglBirth( cp->md.p, cp->md.s );
	  n++;
	}
      }
    }
  }
  return n;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s TBglMakeBufOvG(tmp, c, n)
     BglMesSt* tmp;
     I32s c, n;
{
  I32s i, len = 0;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);

  for(i=0; i<n; i++){

    txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    if (!xdr_BglOvGOutputSt(&msgHndl, &BglOvGOutputBuf)){
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
      tsprintf((char *)(&(bglStrBuf[0])),
	       "XDR encode error in the function TBglMakeBufOvG()");
      TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
      return 0; /* Communication (Encode) Error */
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    
    tmp[c+i].len = txdr_getpos(&msgHndl);
    tmp[c+i].len = BglMakeMesData(BGL_OV_G, xdrMsgBuf,
				  tmp[c+i].len, xdrMsgBuf2 );
    tmp[c+i].buf = (I8s *)tcalloc(1,tmp[c+i].len, 12301);
    memcpy(tmp[c+i].buf, xdrMsgBuf2, tmp[c+i].len);
    txdr_destroy(&msgHndl);

    len += tmp[c+i].len;
  }
  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);
  return len;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglGetGeneFromAddr(fd, addr)
     I32s fd;
     I32s addr;
{
  I32s cc, len, ar, ci, flg;
  FpInst p;
  Pcells mcp;
  BglMesSt* tmp;
  BglMesSt sendmes;

  for (ar = 0; ar < NumCelAr; ar++){
    for (ci = 0; ci < CelArSiz; ci++){
      if (ar == 0 && ci < 2)             continue;
      flg = 0;
      mcp = &cells[ar][ci];
      p = soup + mcp->mm.p;
      if (mcp->mm.p == addr && mcp->mm.s != 0){
	tsprintf((char *)(&(BglOvGOutputBuf.comment[0])), "Mother cell");
	BglOvGOutputBuf.type = 'm';
	BglOvGOutputBuf.start = addr;
	BglOvGOutputBuf.genome_siz =  mcp->mm.s;
	BglOvGOutputBuf.genome.genome_len = (mcp->mm.s-1)/sizeof(xdrwrd)+1;
	if (GeneBnker){
	  strcpy((char *)(&(BglOvGOutputBuf.label[0])),
              (const char *)(&(mcp->d.gen.label[0])));
	  BglOvGOutputBuf.difforig = (mcp->d.gi != mcp->d.origgi) ? 'o' : 'c';
	  BglLastCellViewed.size   = mcp->d.gen.size;
	  BglLastCellViewed.gi     = mcp->d.gi;
	  BglLastCellViewed.origgi = mcp->d.origgi;
	}
	else{
	  strcpy((char *)(&(BglOvGOutputBuf.
              label[0])), (const char *)"---");
	  BglOvGOutputBuf.difforig	= 'c';
	}
	
	BglOvGOutputBuf.genome.genome_val =
	  (xdrwrd*)thcalloc(BglOvGOutputBuf.genome.genome_len,
			    sizeof(xdrwrd), 12315);
	memcpy(BglOvGOutputBuf.genome.genome_val, p, mcp->mm.s);

	tmp = (BglMesSt*)tcalloc(1 , sizeof(BglMesSt), 12316);

	len = TBglMakeBufOvG(tmp, 0, 1);
	cc = TBglSendDat(fd, &sendmes, tmp, len, 1);

	tfree(tmp,  sizeof(BglMesSt), 12316);
	tfree(BglOvGOutputBuf.genome.genome_val,
	      BglOvGOutputBuf.genome.genome_len*sizeof(xdrwrd), 12315);
	return (cc);
      }

      if (mcp->md.p == addr && mcp->md.s != 0){
	tsprintf((char *)(&(BglOvGOutputBuf.comment[0])),
            "Requested cell is still daughter");
	BglOvGOutputBuf.type = 'd';
	BglOvGOutputBuf.start = mcp->md.p;
	BglOvGOutputBuf.genome_siz = mcp->md.s;
	BglOvGOutputBuf.genome.genome_len = (mcp->md.s-1)/sizeof(xdrwrd)+1;
	if (GeneBnker){
	  strcpy((char *)(&(BglOvGOutputBuf.label[0])),
              (const char *)(&(mcp->d.gen.label[0])));
	  BglOvGOutputBuf.difforig = (mcp->d.gi != mcp->d.origgi) ? 'o' : 'c';
	}
	else{
	  strcpy((char *)(&(BglOvGOutputBuf.label[0])),
              (const char *)"---");
	  BglOvGOutputBuf.difforig	= 'c';
	}
	BglOvGOutputBuf.genome.genome_val =
	  (xdrwrd*)thcalloc(BglOvGOutputBuf.genome.genome_len,
			    sizeof(xdrwrd), 12316);
	memcpy(BglOvGOutputBuf.genome.genome_val, p, mcp->md.s);

	tmp = (BglMesSt*)tcalloc( 1, sizeof(BglMesSt), 12317);

	len = TBglMakeBufOvG(tmp, 0, 1);
	cc = TBglSendDat(fd, &sendmes, tmp, len, 1);

	tfree(tmp,  sizeof(BglMesSt), 12317);
	tfree(BglOvGOutputBuf.genome.genome_val,
	      BglOvGOutputBuf.genome.genome_len*sizeof(xdrwrd), 12316);
	return (cc);
      }
    }
  }
  tsprintf((char *)(&(bglStrBuf[0])), "Requested cell(%d) is missing ", addr);
  cc = BglReplyMes(fd, bglStrBuf);
  return (cc);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglGetGeneFromSiz(fd, siz, lbl)
     I32s fd;
     I32s siz;
     I8s lbl[4];
{
  I32s cc, len, ar, ci, flg;
  FpInst p;
  Pcells mcp;
  BglMesSt* tmp;
  BglMesSt sendmes;

  for (ar = 0; ar < NumCelAr; ar++){
    for (ci = 0; ci < CelArSiz; ci++){
      if (ar == 0 && ci < 2)	continue;
      flg = 0;
      mcp = &cells[ar][ci];
      p = soup + mcp->mm.p;
      if(mcp->mm.s==siz&&strncmp((const char *)
        (&(mcp->d.gen.label[0])),
        (const char *)(&(lbl[0])), 3) == 0 ){
	BglLastCellViewed.size	= mcp->d.gen.size;
	BglLastCellViewed.gi	= mcp->d.gi;
	BglLastCellViewed.origgi= mcp->d.origgi;
      	tsprintf((char *)(&(BglOvGOutputBuf.comment[0])), "Mother cell");
	BglOvGOutputBuf.type = 'm';
	BglOvGOutputBuf.start = mcp->mm.p;
	BglOvGOutputBuf.genome_siz = mcp->mm.s;
	BglOvGOutputBuf.genome.genome_len = (mcp->mm.s-1)/sizeof(xdrwrd)+1;
	strcpy((char *)(&(BglOvGOutputBuf.label[0])),
            (const char *)(&(mcp->d.gen.label[0])));
	BglOvGOutputBuf.difforig = (mcp->d.gi != mcp->d.origgi) ? 'o' : 'c';
	BglOvGOutputBuf.genome.genome_val =
	  (xdrwrd*)thcalloc(BglOvGOutputBuf.genome.genome_len,
			    sizeof(xdrwrd), 12318);
	memcpy(BglOvGOutputBuf.genome.genome_val, p, mcp->mm.s);

	tmp = (BglMesSt*)tcalloc( 1, sizeof(BglMesSt), 12319);

	len = TBglMakeBufOvG(tmp, 0, 1);
	cc = TBglSendDat(fd, &sendmes, tmp, len, 1);
	tfree(tmp,  sizeof(BglMesSt), 12319);
	tfree(BglOvGOutputBuf.genome.genome_val,
	      BglOvGOutputBuf.genome.genome_len*sizeof(xdrwrd), 12318);
	return (cc);
      }
      
      if (mcp->md.s==siz&&strncmp((const char *)
        (&(mcp->d.gen.label[0])),
        (const char *)(&(lbl[0])), 3) == 0 ){
	BglLastCellViewed.size	= mcp->d.gen.size;
	BglLastCellViewed.gi	= mcp->d.gi;
	BglLastCellViewed.origgi= mcp->d.origgi;
	tsprintf((char *)(&(BglOvGOutputBuf.comment[0])),
            "Requested cell is still daughter");
	BglOvGOutputBuf.difforig = (mcp->d.gi != mcp->d.origgi) ? 'o' : 'c';
	BglOvGOutputBuf.type = 'd';
	BglOvGOutputBuf.start = mcp->md.p;
	BglOvGOutputBuf.genome_siz = mcp->md.s;
	BglOvGOutputBuf.genome.genome_len = (mcp->md.s-1)/sizeof(xdrwrd)+1;
	strcpy((char *)(&(BglOvGOutputBuf.label[0])),
            (const char *)(&(mcp->d.gen.label[0])));
	BglOvGOutputBuf.genome.genome_val =
	  (xdrwrd*)thcalloc(BglOvGOutputBuf.genome.genome_len,
			    sizeof(xdrwrd), 12320);
	memcpy(BglOvGOutputBuf.genome.genome_val, p, mcp->md.s);

	tmp = (BglMesSt*)tcalloc( 1, sizeof(BglMesSt), 12321);

	len = TBglMakeBufOvG(tmp, 0, 1);
	cc = TBglSendDat(fd, &sendmes, tmp, len, 1);
	tfree(tmp,  sizeof(BglMesSt), 12321);
	tfree(BglOvGOutputBuf.genome.genome_val,
	      BglOvGOutputBuf.genome.genome_len*sizeof(xdrwrd), 12320);
	return (cc);
      }
    }
  }
  tsprintf((char *)(&(bglStrBuf[0])),
      "Requested cell(%4.4d%s) is missing ", siz, lbl);
  cc = BglReplyMes(fd, bglStrBuf);
  return (cc);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static int TBgl_bc_compare(i, j)
const void *i,*j;
{   return((((size_buf *)j)->count)-(((size_buf *)i)->count));
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static int TBgl_bg_compare(i, j)
const void *i,*j;
{   return((((size_buf *)i)->lbl)-(((size_buf *)j)->lbl));
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBgl_query_size(size_class)
     I32s size_class;
{
  I32s num_spec, tc, t;

  if ((size_class < 1) || (size_class > siz_sl) || (!(sl[size_class])))
    return (-1);
  num_spec = sl[size_class]->num_g;

  BglSizQueryOutputBuf
    = (BglSizQueryOutputSt*)thcalloc((num_spec + 1),
				     sizeof(BglSizQueryOutputSt), 12322);

  for (tc = 0, t = 0; t < sl[size_class]->a_num; t++){
    if ((TNULL(sl[size_class]->g[t])) || (!sl[size_class]->g[t]->pop))
      continue;

    strncpy((char *)(&(BglSizQueryOutputBuf[tc].lbl[0])),
        (const char *)Int2Lbl(tc), 3);
    BglSizQueryOutputBuf[tc].count = sl[size_class]->g[t]->pop;
    BglSizQueryOutputBuf[tc].moves = sl[size_class]->g[t]->d1.mov_daught;
    BglSizQueryOutputBuf[tc].flags = sl[size_class]->g[t]->d1.flags;
    BglSizQueryOutputBuf[tc].bits = sl[size_class]->g[t]->bits;
    BglSizQueryOutputBuf[tc].csS =
      (I32s) (100.0 * BglSizQueryOutputBuf[tc].count * size_class /
	      SoupSize);
    tc++;
  }
  qsort(BglSizQueryOutputBuf, num_spec - 1, sizeof(BglSizQueryOutputSt),
        TBgl_bc_compare);  /*all, by count*/
  qsort(BglSizQueryOutputBuf, tc, sizeof(BglSizQueryOutputSt),
        TBgl_bg_compare);/*top num, by size*/
  for (t = 0; t < tc; t++){
    if (BglSizQueryOutputBuf[t].count < 1)
      break;
    WritEcoB(BglSizQueryOutputBuf[t].bits, BglSizQueryOutputBuf[t].WatchBits);
  }
  return tc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglBirth(start, length)
     I32u start, length;
{
  BglOvMesBuf.event = 'b';
  BglOvMesBuf.start = start;
  BglOvMesBuf.length = length;
  TBglOvSend();
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void TBglDeath(start, length)
     I32u start, length;
{
  BglOvMesBuf.event = 'd';
  BglOvMesBuf.start = start;
  BglOvMesBuf.length = length;
  TBglOvSend();
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s TBglOvSend()
{
  BglMesSt tmp, sendmes;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
  txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);
#ifdef SIGBLOCK
  SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
  if (!xdr_BglOvMesSt(&msgHndl, &BglOvMesBuf)){
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    txdr_destroy(&msgHndl);
    tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "XDR encode error in the function TBglMakeBufOvSend()");
    TBgl_make_femessages(bglStrBuf,strlen(
         (const char *)(&(bglStrBuf[0]))));
    return 0; /* Communication (Encode) Error */
  }
#ifdef SIGBLOCK
  TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    
  xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);
  tmp.len = txdr_getpos(&msgHndl);
  tmp.len = BglMakeMesData(BGL_OV, xdrMsgBuf,
			   tmp.len, xdrMsgBuf2 );
  tmp.buf = (I8s*)tcalloc(1, tmp.len, 12301);
  memcpy(tmp.buf, xdrMsgBuf2, tmp.len);
  TBglSendDat(BglSock.fd, &sendmes, &tmp, tmp.len, 1);
  txdr_destroy(&msgHndl);

  tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
  tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);

  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void  TBglOneCellStart()
{
  I32s cc;
#ifdef SIGBLOCK
  sigset_t BglSigSaveSet;

  BglSigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

  tsprintf((char *)(&(bglStrBuf[0])),
      "Tierra: One Cell Start. Waiting to connect.");
  FEPrintf(HLP_X, HLP_Y, 1, -1, (char *)(&(bglStrBuf[0])));

  if ( TBglAccept() == 0 ){
    if ((cc =TBglRecv(BglSockBuf[SockBufSize-1].fd)) <= 0){
      tsprintf((char *)(&(bglStrBuf[0])), "Connection Closed cc=%d\n", cc);
      FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
      FD_CLR((I32u)(BglSockBuf[SockBufSize-1].fd), &BglAfds);
      (void)tclosesocket(BglSockBuf[SockBufSize-1].fd);
      shutdown(BglSockBuf[SockBufSize-1].fd, 2);
      SockBufSize--;
    }
    else{
      if (BglConnectFlg == 1){
	TBglOneCellContFlg = 0;
	while(1){
	  if ((cc =TBglRecv(BglSock.fd)) <= 0){
	    tsprintf((char *)(&(bglStrBuf[0])),
                "Connection Closed cc=%d\n", cc);
	    FEError(-12307, EXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
	  }
	  cc = TBglRecv(BglSock.fd);  /* For receive ReqMessage and ReqOV */
	  if ( cc <= 0 ){ TBglOneCellContFlg = 1; }
	  if (TBglOneCellContFlg) break; /* Wait for continue command */
	}
      }
      else{
	tsprintf((char *)(&(bglStrBuf[0])), "Connection Error\n");
	FEError(-12307, EXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
      }
    }
  }
#ifdef SIGBLOCK
  TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static void TBglSetReqAppTbl()
{
  I32s i,j;
  FILE* fp;
  I8s buf[85];

  for (i=0; i<2; i++)
    for (j=0; j<NCHRS; j++)
      TBglReqAppTbl[i][j] = 1;	
	
  fp = tfopen((I8s *)"./etc/req_app", (I8s *)"r");
  if (fp == NULL){
    tsprintf((char *)(&(bglStrBuf[0])),
	     "Tierra TBglSetReqAppTbl file ./etc/req_app not opened\n");
    FEError(-1908, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
  }
  else{
    tfgets (buf, 84, fp);
    while (1){
      if (buf[0] != '#'){
	TBglGetReqApp(buf);
      }
      tfgets (buf, 84, fp);
      if (feof(fp))
	break;
    }
    tfclose(fp);
  }
#ifdef DEBUG2
  for (j=0; j<NCHRS; j++)
    DebugPrint3("TBglReqAppTbl[%d] = %hu, %hu\n",
		j, TBglReqAppTbl[0][j], TBglReqAppTbl[1][j]);
#endif /* DEBUG2 */
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s TBglGetReqApp(data)
     const I8s* data;
{
  I32s i;
  switch (*data){
  case 'C':
    if (!strncmp((const char *)data,
      (const char *)"Connect", 7)){
      sscanf((const char *)data,
          (const char *)"Connect = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCCONNECT]),
             (unsigned short *)(&TBglReqAppTbl[1][MCCONNECT]));
    }
    break;
  case 'F':
    if (!strncmp((const char *)data,
      (const char *)"FEMenu", 6)){
      sscanf((const char *)data,
          (const char *)"FEMenu = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCFEMENU]),
             (unsigned short *)(&TBglReqAppTbl[1][MCFEMENU]));
    }
    break;
  case 'O':
    if (!strncmp((const char *)data,
      (const char *)"OV", 2)){
      sscanf((const char *)data,
          (const char *)"OV = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCOV_S]),
             (unsigned short *)(&TBglReqAppTbl[1][MCOV_S]));
      TBglReqAppTbl[0][MCOV_Q] = TBglReqAppTbl[0][MCOV_S];
      TBglReqAppTbl[1][MCOV_Q] = TBglReqAppTbl[1][MCOV_S];
      TBglReqAppTbl[0][MCOV_S_I] = TBglReqAppTbl[0][MCOV_S];
      TBglReqAppTbl[1][MCOV_S_I] = TBglReqAppTbl[1][MCOV_S];
    }
    break;
  case 'g':
    if (!strncmp((const char *)data,
      (const char *)"genome_save", 11)){
      sscanf((const char *)data,
          (const char *)"genome_save = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCOV_SAVE_CELL]),
             (unsigned short *)(&TBglReqAppTbl[1][MCOV_SAVE_CELL]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"genome", 6)){
      sscanf((const char *)data,
          (const char *)"genome = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCOV_GA]),
             (unsigned short *)(&TBglReqAppTbl[1][MCOV_GA]));
      for (i=0; i<2; i++){
	TBglReqAppTbl[i][MCOV_GA_I] = TBglReqAppTbl[i][MCOV_GA];
	TBglReqAppTbl[i][MCOV_GS]   = TBglReqAppTbl[i][MCOV_GA];
	TBglReqAppTbl[i][MCOV_GS_I] = TBglReqAppTbl[i][MCOV_GA];
      }
    }
    break;
  case 'i':
    if (!strncmp((const char *)data,
      (const char *)"info_plan", 9)){
      sscanf((const char *)data,
          (const char *)"info_plan = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_PLAN]),
             (unsigned short *)(&TBglReqAppTbl[1][MCINFO_PLAN]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_size", 9)){
      sscanf((const char *)data,
          (const char *)"info_size = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_SIZE]),
             (unsigned short *)(&TBglReqAppTbl[1][MCINFO_SIZE]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_gen", 8)){
      sscanf((const char *)data,
          (const char *)"info_gen = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_GEN]),
             (unsigned short *)(&TBglReqAppTbl[1][MCINFO_GEN]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_mem", 8)){
      sscanf((const char *)data,
          (const char *)"info_mem = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_MEM]),
             (unsigned short *)(&TBglReqAppTbl[1][MCINFO_MEM]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_es", 7)){
      sscanf((const char *)data,
          (const char *)"info_es = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_EFFICS]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCINFO_EFFICS]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_eg", 7)){
      sscanf((const char *)data,
          (const char *)"info_eg = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_EFFICG]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCINFO_EFFICG]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"info_z", 6)){
      sscanf((const char *)data,
          (const char *)"info_z = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCINFO_Z]),
             (unsigned short *)(&TBglReqAppTbl[1][MCINFO_Z]));
    }
    break;
  case 'm':
    if (!strncmp((const char *)data,
      (const char *)"misc_histolog", 13)){
      sscanf((const char *)data,
          (const char *)"misc_histolog = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMISC_HISTO]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCMISC_HISTO]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"misc_inject", 11)){
      sscanf((const char *)data,
          (const char *)"misc_inject = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMISC_INJECT]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCMISC_INJECT]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"misc_tping_s", 12)){
      sscanf((const char *)data,
          (const char *)"misc_tping_s = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMISC_TPING_S_ON]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCMISC_TPING_S_ON]));
      TBglReqAppTbl[0][MCMISC_TPING_S_OFF]
	= TBglReqAppTbl[0][MCMISC_TPING_S_ON];
      TBglReqAppTbl[1][MCMISC_TPING_S_OFF]
	= TBglReqAppTbl[1][MCMISC_TPING_S_ON];
    }
    else if (!strncmp((const char *)data,
      (const char *)"misc_tping_c", 12)){
      sscanf((const char *)data,
          (const char *)"misc_tping_c = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMISC_TPING_C_ON]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCMISC_TPING_C_ON]));
      TBglReqAppTbl[0][MCMISC_TPING_C_OFF]
	= TBglReqAppTbl[0][MCMISC_TPING_C_ON];
      TBglReqAppTbl[1][MCMISC_TPING_C_OFF]
	= TBglReqAppTbl[1][MCMISC_TPING_C_ON];
    }
    else if (!strncmp((const char *)data,
      (const char *)"misc_micro", 10)){
      sscanf((const char *)data,
          (const char *)"misc_micro = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMISC_MICRO_TOGGLE]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCMISC_MICRO_TOGGLE]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"migration_tracking", 18)){
      sscanf((const char *)data,
          (const char *)"migration_tracking = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMIG_TRACK_ON]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCMIG_TRACK_ON]));
      TBglReqAppTbl[0][MCMIG_TRACK_OFF] = TBglReqAppTbl[0][MCMIG_TRACK_ON];
      TBglReqAppTbl[1][MCMIG_TRACK_OFF] = TBglReqAppTbl[1][MCMIG_TRACK_ON];
    }
    break;
  case 'v':
    if (!strncmp((const char *)data,
      (const char *)"val_alter", 9)){
      sscanf((const char *)data,
          (const char *)"val_alter = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCVAL_ALT]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCVAL_ALT]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"val_exam_id", 11)){
      sscanf((const char *)data,
          (const char *)"val_exam_id = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCVAL_EXAM]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCVAL_EXAM]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"val_exam_all", 12)){
      sscanf((const char *)data,
          (const char *)"val_exam_all = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCVAL_EXAM_ALL]), 
	     (unsigned short *)(&TBglReqAppTbl[1][MCVAL_EXAM_ALL]));
    }
    break;
  case 's':
    if (!strncmp((const char *)data,
      (const char *)"save_only", 9)){
      sscanf((const char *)data,
          (const char *)"save_only = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCSAVE]),
             (unsigned short *)(&TBglReqAppTbl[1][MCSAVE]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"save_quit", 9)){
      sscanf((const char *)data,
          (const char *)"save_quit = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCSAVEQUIT]),
             (unsigned short *)(&TBglReqAppTbl[1][MCSAVEQUIT]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"suspend_life", 12)){
      sscanf((const char *)data,
          (const char *)"suspend_life = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCSUSPENDLIFE]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCSUSPENDLIFE]));
    }
    break;
  case 'q':
    if (!strncmp((const char *)data,
      (const char *)"quit", 4)){
      sscanf((const char *)data,
          (const char *)"quit = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCQUIT]),
             (unsigned short *)(&TBglReqAppTbl[1][MCQUIT]));
    }
    break;
  case 'd':
    if (!strncmp((const char *)data,
      (const char *)"debug_brk_put", 13)){
      sscanf((const char *)data,
          (const char *)"debug_brk_put = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMICRO_BRK_PUT]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCMICRO_BRK_PUT]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"debug_brk_clear", 15)){
      sscanf((const char *)data,
          (const char *)"debug_brk_clear = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMICRO_BRK_CLEAR]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCMICRO_BRK_CLEAR]));
    }
    else if (!strncmp((const char *)data,
      (const char *)"debug_brk_repeat", 16)){
      sscanf((const char *)data,
          (const char *)"debug_brk_repeat = %hu,%hu", 
	     (unsigned short *)(&TBglReqAppTbl[0][MCMICRO_BRK_REPEAT]),
	     (unsigned short *)(&TBglReqAppTbl[1][MCMICRO_BRK_REPEAT]));
    }
    break;
  default:
    break;
  }
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s TBglChkReqApp(com)
     I32u com;
{
  I32s ret;	

  if (BglSock.user.priv == BGL_PRIV_S){
    if (TBglReqAppTbl[0][com])	ret = 1;
    else			ret = 0;
  }
  else{
    if (TBglReqAppTbl[1][com])	ret = 1;
    else			ret = 0;
  }
  DebugPrint4("TBglReqAppTbl[%hu] = %hu,%hu, ret = %d\n", 
	      com, TBglReqAppTbl[0][com],TBglReqAppTbl[1][com],ret);
  return ret;
}


#ifdef NET
/*******************************************************
 * Buffer for migration tracking start and stop
 */

/*
 * MigrBufReset
 *	reset the start and stop buffer
 */
void MigrBufReset()
{
  MigrStopBufSize	= 0;
  MigrStartBufSize	= 0;
}


/*
 * MigrStartPut(newTrack)
 *	Put newTrack into buffer to look for a new cell to track
 */
void MigrStartPut(new)
     BglTrackFlag new;
{
  int i;
  
  if (!strcmp((const char *)(&(new.param[0])),
    (const char *)"Mutated"))
    return;
  
  /* look for an existing flag into buffer */
  for (i = 0 ; (i < MigrStartBufSize)
       && ((new.node	 - MigrStartBuf[i].node)
	   || (new.portnb  - MigrStartBuf[i].portnb)
	   || (new.session - MigrStartBuf[i].session)
	   || strcmp((const char *)(&(new.param[0])),
           (const char *)(&(MigrStartBuf[i].param[0]))))
       ; i++);

  if (i < MigrStartBufSize)  return;	/* if flag exists exit function */

  if (MigrStartBufSize == MIGR_BUF_MAX){
    for (i = 0 ; i < MIGR_BUF_MAX - 1 ; i++)
      MigrStartBuf[i] = MigrStartBuf[i + 1]; /* if buffer is empty, 
						  skip last element */
    MigrStartBufSize = i;
  }

  MigrStartBuf[MigrStartBufSize++] = new;
}

/*
 * MigrStartGet(newTrack, gen)
 *	Check if gen must be tracked into buffer and return true if it is
 */
int MigrStartGet(out, gen)
     BglTrackFlag* out;
     Genotype* gen;
{
  I8s	label[4];
  I32s	size;
  int	i, found = 0;

  if (! MigrStartBufSize)    return 0;

  /* look for a matching flag into buffer */
  i = MigrStartBufSize;
  do{
    i--;
    if (! MigrStartBuf[i].param[0]) /* normal migration tracking */
      found = 1;
    else{
      sscanf((const char *)(&(MigrStartBuf[i].param[0])),
          (const char *)"%d%3s", &size, label);
      if (! MigrStartBuf[i].param[4]) /* size tracking */
	found = (size == gen->size);
      else			/* genotype tracking */
	found = ((size == gen->size) &&
            (!strcmp((const char *)(&(label[0])),
            (const char *)(&(gen->label[0])))));
    }
  }
  while ((i > 0) && !found);

  if (found){
      *out = MigrStartBuf[i];
      MigrStartBufSize--;
      for(;i<MigrStartBufSize;i++)
          MigrStartBuf[i]=MigrStartBuf[i+1];
  }

  return found;
}


/*
 * MigrStopPut(oldTrack)
 *	put oldTrack into track stop buffer
 */
void MigrStopPut(new)
     BglTrackFlag new;
{
  int i;
  
  /* look for an existing flag into buffer */
  for (i = 0 ; (i < MigrStopBufSize)
       && ((new.node	 - MigrStopBuf[i].node)
	   || (new.portnb  - MigrStopBuf[i].portnb)
	   || (new.session - MigrStopBuf[i].session)
	   || strcmp((const char *)(&(new.param[0])),
           (const char *)(&(MigrStopBuf[i].param[0]))))
       ; i++);

  if (i < MigrStopBufSize) return;/* if flag exists exit function */

  if (MigrStopBufSize == MIGR_BUF_MAX){
    for (i = 0 ; i < MIGR_BUF_MAX - 1 ; i++)
      MigrStopBuf[i] = MigrStopBuf[i + 1];   /* if buffer is empty, 
						  skip last element */
    MigrStopBufSize = i;
  }

  MigrStopBuf[MigrStopBufSize++] = new;
}

/*
 * MigrStopGet(oldTrack)
 *	remover oldTrack from migration stop buffer
 */
int MigrStopGet(out)
     BglTrackFlag out;
{
  int i;

  for (i = 0 ; (i < MigrStopBufSize)
       && ((out.node	 - MigrStopBuf[i].node)
	   || (out.portnb  - MigrStopBuf[i].portnb)
	   || (out.session - MigrStopBuf[i].session)
	   || strcmp((const char *)(&(out.param[0])),
           (const char *)(&(MigrStopBuf[i].param[0]))))
       ; i++);

  if (i >= MigrStopBufSize)    return 0;

  MigrStopBufSize--;
  for(;i<MigrStopBufSize;i++)
    MigrStopBuf[i] = MigrStopBuf[i+1];

  return 1;
}
#endif /* NET */
