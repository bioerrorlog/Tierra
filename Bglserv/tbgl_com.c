/* tbgl_com.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:38:32 1999
 * Update Count    : 315
 * Status          : 
 */

/* Command of functions for hooking the beagle code                        */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf labels: #121XX, in use:   12101 - 12118 */
/* FEError labels: #121XX, in use:   12101 - 12104 */


#ifndef lint
static char* sccsProgramId="@(#)tbgl_com.c	1.78	11/22/99 16:38:49";
#endif /* lint */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#include "bgl.h"
#include "tbgl.h"

static I32s	InfoSizeOrderRecv	P_((void));
static I32s	InfoGeneOrderRecv	P_((void));
static I32s	SaveCell_ThrdAnaDatRdy	P_((void));
static I32s	SaveCell_SpecificCell	P_((void));


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s no_op()
{	
  DebugPrint("/*** no_op ***/\n");
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_abort()
{
  DebugPrint( "/**** do_abort *****/\n" );
  return 1;
}



I32s do_PreConnect()
{
  DebugPrint("/*** do_PreConnect ***/\n");
  BglRecv(BglMigrCtrlSockFd);
  if (BglConnectFlg)
    return BglReplyAck(BglMigrCtrlSockFd, BGL_PRE_CON_REF);
  
  TBglChangeCommunicationPort(BglReplyMesBuf.mes);

  return BglReplyAck(BglMigrCtrlSockFd, BGL_PRE_CON_OK);
}


I32s do_Connect()
{
  I32s cc;

  DebugPrint("/*** do_Connect ***/\n");
  cc = TBglRecv(BglSockBuf[SockBufSize-1].fd);
  if (cc <= 0){
    tsprintf((char *)(&(bglStrBuf[0])),
        "TBglRecv error in do_Connect:tbgl_com.c");
    FEError(-12101, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
    return cc;
  }

  if (strncmp((const char *)(&(BglSockBuf[
      SockBufSize-1].bglVersion[0])), BGL_VERSION,
	      strlen(BGL_VERSION)) != 0){
    tsprintf((char *)(&(bglStrBuf[0])),
	     "Mismatch Version of Observational tool. Version = %s\n",
	     BGL_VERSION);
    FEError(-12102, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
    cc = BglReplyAck(BglSockBuf[SockBufSize-1].fd, BGL_MIS_VERSION);
    return cc;
  }

  if ( BglConnectFlg >= 1 ){
    tsprintf((char *)(&(bglStrBuf[0])),
        "Bgl connection already established.\n");
    FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
    cc = BglReplyAck(BglSockBuf[SockBufSize-1].fd, BGL_OVR);
    BglConnectFlg++;	/* set this variable for closing second socket */
    return cc;
  }

  BglSock = BglSockBuf[SockBufSize-1];
  SockBufSize--;
  BglConnectFlg = 1;

  if ( BglSock.user.priv == BGL_PRIV_S ){
    if(BglSuAuthentCheck()){
      if (TBglChkReqApp(MCCONNECT)){
	tsprintf((char *)(&(bglStrBuf[0])),
            "Su Beagle is accepted for %s\n",
            BglSock.user.name);
	FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
	cc = BglReplyAck(BglSock.fd, BGL_SU_ACCEPTED);
	return cc;
      }
      else{
	tsprintf((char *)(&(bglStrBuf[0])),
            "SU Beagle is rejected for %s\n",
            BglSock.user.name);
	FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
	cc = BglReplyAck(BglSock.fd, BGL_APP_REF);
	return cc;
      }
    }
    else{
      tsprintf((char *)(&(bglStrBuf[0])),
          "Incorrect user name or passwd. user = %s\n",
	       BglSock.user.name);
      FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
      cc = BglReplyAck(BglSock.fd, BGL_WRONG_PASSWD);
      return cc;
    }
  }

  if ( BglSock.user.priv == BGL_PRIV_N ){
    if (TBglChkReqApp(MCCONNECT)){
      tsprintf((char *)(&(bglStrBuf[0])),
          "Non SU Beagle is accepted for %s\n",
	       BglSock.user.name);
      FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
      cc = BglReplyAck(BglSock.fd, BGL_NONSU_ACCEPTED);
      return cc;
    }	
    else{
      tsprintf((char *)(&(bglStrBuf[0])),
          "Non SU Beagle is rejected for %s\n",
	       BglSock.user.name);
      FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
      cc = BglReplyAck(BglSock.fd, BGL_APP_REF);
      return cc;
    }
  }
  tsprintf((char *)(&(bglStrBuf[0])),
      "Unknown treatment for %s(Flg=%d, priv = %c)\n",
	   BglSock.user.name, BglConnectFlg,
	   BglSock.user.priv);
  FEPrintf(BGL_MES_X, BGL_MES_Y, 0, -1, (char *)(&(bglStrBuf[0])));
  cc = BglReplyAck(BglSock.fd, BGL_REF);
  return cc;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_FEMenu()
{
  I32s cc = 0;

  DebugPrint("/*** do_FEMenu ***/\n");
  if (TBglChkReqApp(MCFEMENU)){
    cc = BglReplyAck (BglSock.fd, BGL_FEMENU_OK);
    TBglRecvFunc( BGL_BLOCK );
    return 1;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_FEMENU_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_FEMenu_com()
{
  DebugPrint("/*** do_FEMenu_com ***/\n");
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_one_cell_cont()
{
  DebugPrint("/*** do_one_cell_cont ***/\n");
  TBglOneCellContFlg = 1L;
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_save()
{
  I32s cc;

  DebugPrint("/*** do_save ***/\n");
  if (TBglChkReqApp(MCSAVE)){
    WriteSoup(0);
    cc = BglReplyAck(BglSock.fd, BGL_SAVE_OK);
    tsprintf((char *)(&(bglStrBuf[0])),
        "%s", "Accept the request save.");
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_SAVE_REF);
    tsprintf((char *)(&(bglStrBuf[0])),
        "%s", "Refuse the request save.");	
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
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
I32s do_save_quit()
{
  I32s cc;

  DebugPrint("/*** do_save_quit ***/\n");
  if (TBglChkReqApp(MCSAVEQUIT)){	
    tsprintf((char *)(&(bglStrBuf[0])), "%s", 
	     "Accepted the request quit & save. Tierra will be going down...");
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
    WriteSoup(0);
#ifdef _WIN32
    Sleep(3000);
#else /* _WIN32 */
    sleep(3);
#endif /* _WIN32 */
    cc = BglReplyAck(BglSock.fd, BGL_SAVEQUIT_OK);
    TBglExit(1);
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_SAVEQUIT_REF);
    tsprintf((char *)(&(bglStrBuf[0])),
        "%s", "Refused the request quit & save.");	
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
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
I32s do_quit()
{
  I32s cc;

  DebugPrint("/*** do_quit ***/\n");
  if (TBglChkReqApp(MCQUIT)){
    tsprintf((char *)(&(bglStrBuf[0])), "%s", 
	     "Accepted the request Quit. Tierra will be going down....");
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
#ifdef _WIN32
    Sleep(3000);
#else /* _WIN32 */
    sleep(3);
#endif /* _WIN32 */
    cc = BglReplyAck(BglSock.fd, BGL_QUIT_OK);
    TBglExit(1);
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_QUIT_REF);
    tsprintf((char *)(&(bglStrBuf[0])), "%s", "Refuse the request Quit.");	
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
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
I32s do_SuspendLife()
{
  I32s cc;

  DebugPrint("/*** do_SuspendLife ***/\n");
  if (TBglChkReqApp(MCSUSPENDLIFE)){
    tsprintf((char *)(&(bglStrBuf[0])),
        "%s", "Tierra Simulation is Suspended");
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
    cc = BglReplyAck(BglSock.fd, BGL_SUSPENDLIFE_OK);
    TBglSuspendLifeFlg = 1;	/* Set on the Suspend Life Flag */
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_SUSPENDLIFE_REF);
    tsprintf((char *)(&(bglStrBuf[0])),
        "%s", "Refuse the request Suspend.");	
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
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
I32s do_ResumeLife()
{
  I32s cc;

  DebugPrint("/*** do_ResumeLife ***/\n");
  if (TBglChkReqApp(MCSUSPENDLIFE)){
    tsprintf((char *)(&(bglStrBuf[0])), "%s", "Tierra Simulation is Resumed");
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
    cc = BglReplyAck(BglSock.fd, BGL_RESUMELIFE_OK);
    TBglSuspendLifeFlg = 0;	/* Set off the Suspend Life Flag */
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_RESUMELIFE_REF);
    tsprintf((char *)(&(bglStrBuf[0])), "%s", "Refuse the request Resume.");	
    FEPrintf(MSG_X, MSG_Y, 0, -1, (char *)(&(bglStrBuf[0])));
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
I32s do_send_message()
{
  I32s cnt=0, cc;
  BglMesSt *tmp, sendmes;			
  I32s n_send,		total_len=0;
  I32s n_Stats,		l_Stats;
  I32s n_Plan,		l_Plan;
  I32s n_Message,	l_Message;
  I32s n_MCState,	l_MCState;
  I32s n_MCSpy,		l_MCSpy;
  I32s n_InfoHistClr,	l_InfoHistClr;
  I32s n_InfoSiz,	l_InfoSiz;
  I32s n_InfoGen,	l_InfoGen;
  I32s n_InfoMem,	l_InfoMem;
  I32s n_InfoES,	l_InfoES;
  I32s n_InfoEG,	l_InfoEG;
#ifdef NET
  I32s n_TPingS,	l_TPingS;
  I32s n_TPingC,	l_TPingC;
#endif /* NET */
	
  DebugPrint("/*** do_send_message ***/\n");
  if (MC_step > -1L)	DebugDisplay(ce);

  n_Stats =	TBglCntStats();
  n_Plan =	TBglCntPlan();
  n_MCState =	TBglCntMCState();
  n_MCSpy =	TBglCntMCSpy();
#ifdef NET
  n_TPingS =	TBglCntTPing(1);
  n_TPingC =	TBglCntTPing(0);
#endif /* NET */
  n_Message =	TBglCntMessage();
  n_InfoHistClr = TBglCntInfoHistClr();
  n_InfoSiz =	TBglCntInfoSiz();
  n_InfoGen =	TBglCntInfoGen();
  n_InfoMem =	TBglCntInfoMem();
  n_InfoES =	TBglCntInfoEfficS();
  n_InfoEG =	TBglCntInfoEfficG();

  n_send = n_Stats + n_Plan + n_MCState + n_MCSpy
#ifdef NET
    + n_TPingS + n_TPingC
#endif /* NET */
      + n_Message + n_InfoHistClr + n_InfoSiz + n_InfoGen + n_InfoMem
	+ n_InfoES + n_InfoEG;

  DebugPrint1("n_send = %d\n", n_send);
  if( n_send > 0 ){
    tmp = (BglMesSt*)tcalloc(n_send, sizeof(BglMesSt), 12101);

    if( n_Stats > 0 ){
      l_Stats = TBglMakeBufStats(tmp, cnt, n_Stats);
      if( l_Stats > 0 ){
	cnt += n_Stats;
	total_len += l_Stats;
      }
    }

    if( n_Plan > 0 ) {
      l_Plan = TBglMakeBufPlan(tmp, cnt, n_Plan);
      if( l_Plan > 0 ){
	cnt += n_Plan;
	total_len += l_Plan;
      }
    }

    if( n_MCState > 0 ){
      l_MCState = TBglMakeBufMCState(tmp, cnt, n_MCState);
      if( l_MCState > 0 ){
	cnt += n_MCState;
	total_len += l_MCState;
      }
    }

    if( n_MCSpy > 0 ){
      l_MCSpy = TBglMakeBufMCSpy(tmp, cnt, n_MCSpy);
      if( l_MCSpy > 0 ){
	cnt += n_MCSpy;
	total_len += l_MCSpy;
      }
    }

#ifdef NET
    if( n_TPingS > 0 ){
      l_TPingS = TBglMakeBufTPing(tmp, cnt, n_TPingS, 1);
      if( l_TPingS > 0 ){
	cnt += n_TPingS;
	total_len += l_TPingS;
      }
    }
    if( n_TPingC > 0 ){
      l_TPingC = TBglMakeBufTPing(tmp, cnt, n_TPingC, 0);
      if( l_TPingC > 0 ){
	cnt += n_TPingC;
	total_len += l_TPingC;
      }
    }
#endif /* NET */

    if( n_Message > 0 ){
      l_Message = TBglMakeBufMessage(tmp, cnt, n_Message);
      if( l_Message > 0 ){
	cnt += n_Message;
	total_len += l_Message;
      }
    }

    if( n_InfoHistClr > 0 ){
      l_InfoHistClr = TBglMakeBufInfoHistClr(tmp, cnt, n_InfoHistClr);
      if( l_InfoHistClr > 0 ){
	cnt += n_InfoHistClr;
	total_len += l_InfoHistClr;
	BglHistClearBuf.flg = BGL_READ;
      }
    }

    if( n_InfoSiz > 0 ){
      l_InfoSiz = TBglMakeBufInfoSiz(tmp, cnt, n_InfoSiz);
      if( l_InfoSiz > 0 ){
	cnt += n_InfoSiz;
	total_len += l_InfoSiz;
      }
    }

    if( n_InfoGen > 0 ){
      l_InfoGen = TBglMakeBufInfoGen(tmp, cnt, n_InfoGen);
      if( l_InfoGen > 0 ){
	cnt += n_InfoGen;
	total_len += l_InfoGen;
      }
    }

    if( n_InfoMem > 0 ){
      l_InfoMem = TBglMakeBufInfoMem(tmp, cnt, n_InfoMem);
      if( l_InfoMem > 0 ){
	cnt += n_InfoMem;
	total_len += l_InfoMem;
      }
    }

    if ( n_InfoES > 0 ){
      l_InfoES = TBglMakeBufInfoEfficS(tmp, cnt, n_InfoES);
      if ( l_InfoES > 0 ){
	cnt += n_InfoES;
	total_len += l_InfoES;
      }
    }

    if ( n_InfoEG > 0 ){
      l_InfoEG = TBglMakeBufInfoEfficG(tmp, cnt, n_InfoEG);
      if ( l_InfoEG > 0 ){
	cnt += n_InfoEG;
	total_len += l_InfoEG;
      }
    }

    if(cnt > 0 && total_len > 0 && total_len < BGL_MAX_RECV_LEN){
      cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, cnt);
      tfree(tmp, n_send * sizeof(BglMesSt), 12345);
    }
    else{
      BglReplyAck(BglSock.fd, BGL_DO_MESSAGE_ERR);
      cc = 0;
    }

  }
  else{
    BglReplyAck(BglSock.fd, BGL_DO_MESSAGE_ERR);
    cc = 0;
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
I32s do_info_plan()
{
  I32s cc;

  DebugPrint("/*** do_info_plan ***/\n");
  if (TBglChkReqApp(MCINFO_PLAN)){

    if (IMode != PLN_STATS)      CloseGeneBanker();
    if (Hist){
      thfree((void*)Hist, HistAlocSiz, 2101);
      Hist = (HistType*) NULL;
      HistAlocSiz = 0;
    }
    IMode = PLN_STATS;

    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);

    FEVidInit(0);
    FEPlan(0);

    tsprintf((char *)(&(bglStrBuf[0])),
	     "Now in Plan Display mode, updated every million time steps");
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(bglStrBuf[0])));

    cc = BglReplyAck(BglSock.fd, BGL_INFO_PLAN_OK);
  }
  else{
    tsprintf((char *)(&(bglStrBuf[0])),
        "Refused the request do_info_plan");
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(bglStrBuf[0])));
    cc = BglReplyAck(BglSock.fd, BGL_INFO_PLAN_REF);
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
static I32s	InfoSizeOrderRecv()
{
  I32s	order;
  I32s	daughter;
  I32s	cc;

  if ( (cc = TBglRecv(BglSock.fd) ) <= 0 ) return cc;
  if (sscanf((const char *)(&(BglReplyMesBuf.
    mes[0])), (const char *)"%d %d", &order, &daughter ) == 2 ){
    if ( order == 1 ){		HistSortOrder = FREQ_SIZE; }
    else if ( order == 2 ){	HistSortOrder = SIZE_FREQ; }
    else{ /* NOP */ }
    if ( daughter == 1 ){	EffDaught = 0; }
    else if ( daughter == 2 ){	EffDaught = 1; }
    else{ /* NOP */ }
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
static I32s	InfoGeneOrderRecv()
{
  I32s	order;
  I32s	daughter;
  I32s	cc;

  if ( (cc = TBglRecv(BglSock.fd) ) <= 0 ) return cc;
  if ( sscanf( (const char *)(&(BglReplyMesBuf.mes[0])),
    (const char *)"%d %d", &order, &daughter ) == 2 ){
    if ( order == 1 ){		HistSortOrder = FREQ_GEN; }
    else if ( order == 2 ){	HistSortOrder = GEN_FREQ; }
    else{ /* NOP */ }
    if ( daughter == 1 ){	EffDaught = 0; }
    else if ( daughter == 2 ){	EffDaught = 1; }
    else{ /* NOP */ }
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
I32s do_info_size()
{
  I32s cc, nnn, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_size ***/\n");
  if (TBglChkReqApp(MCINFO_SIZE)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_SIZ_OK)) <= 0 ) return cc;
    if ( (cc = InfoSizeOrderRecv()) <= 0 ) return cc;

    if (IMode == PLN_STATS)      OpenGeneBanker();
    IMode = SIZ_HIST;

    FD_SET(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);

    query_species(2);

    nnn = TBglCntInfoSiz();
    tmp = (BglMesSt*)tcalloc(nnn + 1, sizeof(BglMesSt), 12103);	
    len1 = TBglMakeBufInfoHistClr(tmp, 0, 1);
    len2 = TBglMakeBufInfoSiz(tmp, 1, nnn);	
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len1 + len2, nnn + 1);
    tfree(tmp, (nnn+1) * sizeof(BglMesSt), 12103);
    BglHistClearBuf.flg = BGL_READ;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_SIZ_REF);
    cc = 0;
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
I32s do_info_gen()
{
  I32s cc, nnn, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_gen ***/\n");
  if (TBglChkReqApp(MCINFO_GEN)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_GEN_OK)) <= 0 ) return cc;
    if ( (cc = InfoGeneOrderRecv()) <= 0 ) return cc;

    if (IMode == PLN_STATS)      OpenGeneBanker();
    IMode = GEN_HIST;

    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_SET(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);

    query_species(2);

    nnn = TBglCntInfoGen();
    tmp = (BglMesSt*)tcalloc(nnn + 1, sizeof(BglMesSt), 12104);	
    len1 = TBglMakeBufInfoHistClr(tmp, 0, 1);
    len2 = TBglMakeBufInfoGen(tmp, 1, nnn);	
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len1 + len2, nnn + 1);
    tfree(tmp, (nnn+1) * sizeof(BglMesSt), 12345);
    BglHistClearBuf.flg = BGL_READ;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_GEN_REF);
    cc = 0;
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
I32s do_info_mem()
{
  I32s cc, nnn, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_mem ***/\n");
  if (TBglChkReqApp(MCINFO_MEM)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_MEM_OK)) <= 0 ) return cc;
    if ( (cc = InfoSizeOrderRecv()) <= 0 ) return cc;

    if (IMode == PLN_STATS)      OpenGeneBanker();
    IMode = SIZM_HIST;

    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_SET(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);

    query_species(2);

    nnn = TBglCntInfoMem();
    tmp = (BglMesSt*)tcalloc(nnn + 1, sizeof(BglMesSt), 11111);	
    len1 = TBglMakeBufInfoHistClr(tmp, 0, 1);
    len2 = TBglMakeBufInfoMem(tmp, 1, nnn);	
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len1 + len2, nnn + 1);
    tfree(tmp, (nnn+1) * sizeof(BglMesSt), 12345);
    BglHistClearBuf.flg = BGL_READ;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_MEM_REF);
    cc = 0;
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
I32s do_info_es()
{
  I32s cc, nnn, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_es ***/\n");
  if (TBglChkReqApp(MCINFO_EFFICS)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_EFFICS_OK)) <= 0 ) return cc;
    if ( (cc = InfoSizeOrderRecv()) <= 0 ) return cc;

    if (IMode == PLN_STATS)      OpenGeneBanker();
    IMode = SIZ_EFF;

    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_SET(BGL_MASK_INFO_ES, &BglMask);
    FD_CLR(BGL_MASK_INFO_EG, &BglMask);

    query_species(2);

    nnn = TBglCntInfoEfficS();
    tmp = (BglMesSt*)tcalloc(nnn + 1, sizeof(BglMesSt), 11111);	
    len1 = TBglMakeBufInfoHistClr(tmp, 0, 1);
    len2 = TBglMakeBufInfoEfficS(tmp, 1, nnn);	
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len1 + len2, nnn + 1);
    tfree(tmp, (nnn+1) * sizeof(BglMesSt), 12345);
    BglHistClearBuf.flg = BGL_READ;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_EFFICS_REF);
    cc = 0;
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
I32s do_info_eg()
{
  I32s cc, nnn, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_eg ***/\n");
  if (TBglChkReqApp(MCINFO_EFFICG)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_EFFICG_OK)) <= 0 ) return cc;
    if ( (cc = InfoGeneOrderRecv()) <= 0 ) return cc;

    if (IMode == PLN_STATS)      OpenGeneBanker();
    IMode = GEN_EFF;

    FD_CLR(BGL_MASK_INFO_S,  &BglMask);
    FD_CLR(BGL_MASK_INFO_G,  &BglMask);
    FD_CLR(BGL_MASK_INFO_M,  &BglMask);
    FD_CLR(BGL_MASK_INFO_ES, &BglMask);
    FD_SET(BGL_MASK_INFO_EG, &BglMask);

    query_species(2);

    nnn = TBglCntInfoEfficG();
    tmp = (BglMesSt*)tcalloc(nnn + 1, sizeof(BglMesSt), 11111);	
    len1 = TBglMakeBufInfoHistClr(tmp, 0, 1);
    len2 = TBglMakeBufInfoEfficG(tmp, 1, nnn);	
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len1 + len2, nnn + 1);
    tfree(tmp, (nnn+1) * sizeof(BglMesSt), 12345);
    BglHistClearBuf.flg = BGL_READ;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_EFFICG_REF);
    cc = 0;
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
I32s do_info_z()
{
  I32s cc, tc, len = 0;
  I32u siz;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_info_z ***/\n");
  if (TBglChkReqApp(MCINFO_Z)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_INFO_Z_OK)) <= 0 ) return cc;

    OpenGeneBanker();
    while(1){
      BglReplyBuf.ack = BGL_INFO_Z_CONT;
      cc =TBglRecv(BglSock.fd);
      if ( ( cc <= 0 )||( BglReplyBuf.ack == BGL_INFO_Z_BREAK ) ) break;
      else
          siz=atoi((const char *)(&(BglReplyMesBuf.mes[0])));
      tc = TBgl_query_size(siz);
      if (tc > 0){  
	tmp = (BglMesSt*)tcalloc(tc , sizeof(BglMesSt), 12104);
	len = TBglMakeBufInfoZ(tmp, 0, tc);	
	cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, tc);
	tfree(tmp, tc * sizeof(BglMesSt), 12104);
	if ( cc <= 0 ) break;
      }
      else{
	tsprintf((char *)(&(bglStrBuf[0])),
            "Genome size %d is invalid.\n", siz);
	cc = BglReplyMes(BglSock.fd, bglStrBuf);
	if ( cc <= 0 ) break;
      }
      cc =TBglRecv(BglSock.fd);
      if ( ( cc <= 0 )||( BglReplyBuf.ack != BGL_INFO_Z_CONT ) ) break;
    }	/* End of while(1) */
    CloseGeneBanker();
  }
  else{
    BglReplyAck(BglSock.fd, BGL_INFO_Z_REF);
    return 0;
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
I32s do_val_alter()
{
  I32s cc;
  I8s buf[90];
#ifdef NET
  I32s TBasSubNetChgFrq, TSubNetCnt, TTieSubNet;
  double TSubNetChgFrqRanFrac, TSubNetChgFrqFixFrac;
#endif /* NET */
  I32s TThreadAnalysis;

  DebugPrint("/*** do_val_alter ***/\n");
  if (TBglChkReqApp(MCVAL_ALT) == 0){
    DebugPrint("Refused do_val_alter\n");
    cc = BglReplyAck(BglSock.fd, BGL_VAR_ALTER_REF);
  }
  else{
    DebugPrint("Accepted do_val_alter\n");
    if ( ( cc = BglReplyAck(BglSock.fd, BGL_VAR_ALTER_OK) ) <= 0 ) return cc;
    if ( ( cc = TBglRecv(BglSock.fd) ) <= 0 ) return cc;
    strcpy((char *)(&(buf[0])), (const char *)
        (&(BglReplyMesBuf.mes[0])));

#ifdef NET
    TTieSubNet=TieSubNet;
    TSubNetCnt=SubNetCnt;
    TBasSubNetChgFrq=BasSubNetChgFrq;
    TSubNetChgFrqRanFrac=SubNetChgFrqRanFrac;
    TSubNetChgFrqFixFrac=SubNetChgFrqFixFrac;
#endif /* NET */
    TGeneBnker = GeneBnker;
    TThreadAnalysis=ThreadAnalysis;

    if (!GetAVar(buf, 1, 0, sizeof(buf))){
      tsprintf((char *)(&(bglStrBuf[0])),
	       "Tierra do_val_alter() Not a valid soup_in variable: %s",buf);
      FEError(-12103, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
      /* FEError include message send to Beagle routine .... */
    }
    else{
      ToggleLog(TierraLog, &tfp_log, (I8s *)TIERRALOGNAME, &tielogpos);
#ifdef NET
      ToggleLog(PingDatLog, &ping_fp, (I8s *)TPINGLOGNAME, &tpinglogpos);
#endif /* NET */
      tsprintf((char *)(&(bglStrBuf[0])),
          "beagle: Var change @ %d %5.5d - %.30s",
	       InstExe.m, InstExe.i, buf);
#if INTERFACE == BASICM
      FEClrmsg(MSG_Y + 8, "");
#endif /* INTERFACE == BASICM */
      FEPrintf(MSG_X, MSG_Y + 8, 0, 0, (char *)(&(bglStrBuf[0])));
      if ( ( cc = BglReplyMes(BglSock.fd, bglStrBuf) ) <= 0 ) return cc;
    }

    if ( ( cc = TBglRecv(BglSock.fd) ) <= 0 ) return cc;

    if(TGeneBnker!=GeneBnker)
    {   if((TGeneBnker)&&(!GeneBnker))
        {   GeneBnkerOvrd=0;
            CloseGeneBanker();
        }
        else
            if((!TGeneBnker)&&(GeneBnker))
            {   GeneBnkerOvrd=1;
                OpenGeneBanker();
            }
    }
    if(TThreadAnalysis!=ThreadAnalysis)
    {   if((TThreadAnalysis)&&(!ThreadAnalysis))
        {   CloseGeneBanker();
            ThrdAnaDatDump();
        }
        else
        {   if((!TThreadAnalysis)&&(ThreadAnalysis))
            {   OpenGeneBanker();
                ThrdAnaTimeLastOn=(I32s)tietime(NULL);
            }
        }
    }

#ifdef NET
    if((TSubNetCnt!=SubNetCnt)||
       (TBasSubNetChgFrq!=BasSubNetChgFrq)||
       (TSubNetChgFrqRanFrac!=SubNetChgFrqRanFrac)||
       (TSubNetChgFrqFixFrac!=SubNetChgFrqFixFrac)||
       (TTieSubNet!=TieSubNet))
      TieCommChgSubNet(TTieSubNet,0,0,1);
#endif /* NET */
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
I32s do_val_exam()
{
  I32s cc;
  I8s buf[90];
  BglMesSt tmp, sendmes;
  BglVarSt tmpData;
  XDR msgHndl;
  I8s* xdrMsgBuf;
  I8s* xdrMsgBuf2;
#ifdef SIGBLOCK
  sigset_t SigSaveSet;
#endif /* SIGBLOCK */

  DebugPrint("/*** do_val_exam ***/\n");
  if (TBglChkReqApp(MCVAL_EXAM)){
    if ( ( cc = BglReplyAck(BglSock.fd, BGL_VAR_EXAM_OK) <= 0 ) ) return cc;
    if ( ( cc = TBglRecv(BglSock.fd) ) <= 0 ) return cc;
    strcpy((char *)(&(buf[0])), (const char *)
        (&(BglReplyMesBuf.mes[0])));
    if (GetAVar(buf, 0, 1, sizeof(buf))){

      tmpData.index = 0;
      tmpData.type = vqu.type;
      strcpy((char *)(&(tmpData.name[0])),
          (const char *)(&(vqu.name[0])));
      strcpy((char *)(&(tmpData.value[0])),
          (const char *)(&(vqu.value[0])));

      xdrMsgBuf = (I8s*)thcalloc(XDRBufMaxSize, sizeof(I8u), 12120);
      txdrmem_create(&msgHndl, xdrMsgBuf, XDRBufMaxSize, XDR_ENCODE);

#ifdef SIGBLOCK
      SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
      if (!xdr_BglVarSt(&msgHndl, &tmpData)){
#ifdef SIGBLOCK
	TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
	txdr_destroy(&msgHndl);
	tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
	tsprintf((char *)(&(bglStrBuf[0])),
		 "XDR encode error on the function do_val_exam()");
	TBgl_make_femessages(bglStrBuf,strlen(
            (const char *)(&(bglStrBuf[0]))));
	return 0; /* Communication (Encode) Error */
      }
#ifdef SIGBLOCK
      TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

      xdrMsgBuf2 = (I8s*)thcalloc(XDRBufMaxSize+4, sizeof(I8u), 12121);
      tmp.len = txdr_getpos(&msgHndl);
      tmp.len = BglMakeMesData(BGL_VAR, xdrMsgBuf, tmp.len, xdrMsgBuf2);
      tmp.buf = (I8s*)tcalloc(1, tmp.len, 12310);
      memcpy(tmp.buf, xdrMsgBuf2, tmp.len);
      txdr_destroy(&msgHndl);
      tfree(xdrMsgBuf, XDRBufMaxSize*sizeof(char), 12120);
      tfree(xdrMsgBuf2, (XDRBufMaxSize+4)*sizeof(char), 12121);

      cc = TBglSendDat(BglSock.fd, &sendmes, &tmp, tmp.len, 1);
    }
  }
  else{
    BglReplyAck(BglSock.fd, BGL_VAR_EXAM_REF);
    cc = 0;
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
I32s do_val_exam_all()
{
  I32s cc, n_send, total_len;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_val_exam_all ***/\n");
  if ( TBglChkReqApp(MCVAL_EXAM_ALL) && (n_send = TBglGetAVar()) > 0){
    if ( ( cc = BglReplyAck(BglSock.fd, BGL_VAR_EXAM_OK) ) <= 0 ) return cc;
    tmp = (BglMesSt*)tcalloc(n_send, sizeof(BglMesSt), 12105);	
    total_len = TBglMakeBufVar(tmp, 0, n_send);
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, n_send);
    tfree(tmp, n_send * sizeof(BglMesSt), 12105);
  }
  else
    cc = BglReplyAck(BglSock.fd, BGL_VAR_EXAM_REF);
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_histolog()
{
  I32s cc;

  DebugPrint("/*** do_misc_histolog ***/\n");
  if (TBglChkReqApp(MCMISC_HISTO)){
    cc = BglReplyAck(BglSock.fd, BGL_MISC_HIST_OK);
    HistPrint = (TierraLog && !HistPrint) ? 1 : 0;
    tsprintf((char *)(&(bglStrBuf[0])),
	     "%s \n", (TierraLog) ? (HistPrint) ? "Logging Histograms \n" :
	     "NOT Logging Histograms \n" : "Log NOT on ! \n");
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_MISC_HIST_REF);
    tsprintf((char *)(&(bglStrBuf[0])),
	     "Your request Histlog was refused. Owner set disapproval.\n");
  }
  FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(bglStrBuf[0])));
  if ( cc <= 0 ) return cc;

  cc = BglReplyMes(BglSock.fd, bglStrBuf);

  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_inject()
{
  I32s actNumInject;
  I32s cc;

  DebugPrint("/*** do_misc_inject ***/\n");
  if (TBglChkReqApp(MCMISC_INJECT) == 0){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_MISC_INJECT_REF)) <= 0 ) return cc;
  }
  else{
    if ( (cc = BglReplyAck(BglSock.fd, BGL_MISC_INJECT_OK)) <= 0 ) return cc;
    if ( (cc = TBglRecv(BglSock.fd)) <= 0 ) return cc;
    if (strncmp((const char *)(&(BglInjectGeneBuf.gene[0])),
        (const char *)"ESC", 3) != 0 ){
      actNumInject = InjectFromBank(BglInjectGeneBuf.gene,
				    BglInjectGeneBuf.addr, 0,
				    BglInjectGeneBuf.num);
      if( actNumInject != BglInjectGeneBuf.num ){
	tsprintf((char *)(&(bglStrBuf[0])),
            "only %d actually injected", actNumInject);
      }
      else{
	tsprintf((char *)(&(bglStrBuf[0])),
            "injection of %d %s at %d from Beagle completed",
            BglInjectGeneBuf.num, BglInjectGeneBuf.gene,
            BglInjectGeneBuf.addr);
      }
    }
    else{
      tsprintf((char *)(&(bglStrBuf[0])), "User abort of Injection!");
    }
    DebugPrint1("%s", (char *)(&(bglStrBuf[0])));
    cc = BglReplyMes(BglSock.fd, bglStrBuf);
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
I32s do_misc_tping_s_on()
{
#ifdef NET
  I32s cc, len, siz, i;
  BglMesSt *tmp, sendmes;
#endif /* NET */

  DebugPrint("/*** do_misc_tping_s_on ***/\n");
#ifdef NET
  if ( TBglChkReqApp(MCMISC_TPING_S_ON) == 0 ){
    return BglReplyAck(BglSock.fd, BGL_TPING_S_REF);
  }
  else{
    if ( ( cc = BglReplyAck(BglSock.fd, BGL_TPING_S_OK) ) <= 0 ){
      return cc;
    }

    TBgl_TPingS_Mode = 1;
    siz = sizeof(BglTPingSt);
    if ( BglTPingSBuf == NULL ){
      BglTPingSBuf = (BglTPingSt*)tcalloc(NumNodesS, siz, 12106);
      BglTblTPingS = (I8s*)tcalloc(NumNodesS, sizeof(I8s), 12106);
    } 
    else{
      BglTPingSBuf = (BglTPingSt*)trecalloc((void*)BglTPingSBuf, 
					    NumNodesS*siz,
					    BglTPingSAllocSiz*siz,
					    12106);
      BglTblTPingS = (I8s*)trecalloc((void*)BglTblTPingS,
				     NumNodesS*sizeof(I8s),
				     BglTPingSAllocSiz*sizeof(I8s),
				     12106);
    }
    BglTPingSAllocSiz = BglNumIPMapSDat = NumNodesS;
    if ( BglNumIPMapSDat > 0 ){
      for ( i = 0; i < BglNumIPMapSDat; i++ ){
	TBglCpyIPMap(i, i, 'b', 1);
	BglTblTPingS[i] = BGL_UNREAD;
      } 
      tmp = (BglMesSt*)tcalloc(BglNumIPMapSDat, sizeof(BglMesSt), 12108);
      len = TBglMakeBufTPing(tmp, 0, BglNumIPMapSDat, 1);
      cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, BglNumIPMapSDat);
      tfree(tmp, BglNumIPMapSDat * sizeof(BglMesSt), 12108);
      return cc;
    }
    else{
      return 0;
    }
  }
#else /* NET */
  return BglReplyAck(BglSock.fd, BGL_TPING_S_REF_NONET);
#endif /* NET */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_tping_s_off()
{
  I32s cc;

  DebugPrint("/*** do_misc_tping_s_off ***/\n");
#ifdef NET
  if (TBglChkReqApp(MCMISC_TPING_S_OFF)){
    TBgl_TPingS_Mode = 0;
    cc = BglReplyAck(BglSock.fd, BGL_TPING_S_OK);
    if(BglTPingSBuf != NULL){
      tfree(BglTPingSBuf, BglTPingSAllocSiz*sizeof(BglTPingSt), 12106);
      tfree(BglTblTPingS, BglTPingSAllocSiz*sizeof(I8s), 12106);
      BglTPingSBuf = NULL;
      BglTPingSAllocSiz = BglNumIPMapSDat = 0;
    }
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_TPING_S_REF);
  }
  return cc;
#else /* NET */
  cc = BglReplyAck(BglSock.fd, BGL_TPING_S_REF_NONET);
  return cc;
#endif /* NET */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_tping_c_on()
{
  I32s cc;
#ifdef NET
  I32s len, siz, i;
  BglMesSt *tmp, sendmes;
#endif /* NET */

  DebugPrint("/*** do_misc_tping_c_on ***/\n");
#ifdef NET
  if ( TBglChkReqApp(MCMISC_TPING_C_ON) == 0 ){
    return BglReplyAck(BglSock.fd, BGL_TPING_C_REF);
  }
  else{
    if ( ( cc = BglReplyAck(BglSock.fd, BGL_TPING_C_OK) ) <= 0 ){
      return cc;
    }

    TBgl_TPingC_Mode = 1;
    siz = sizeof(BglTPingSt);
    if ( BglTPingCBuf == NULL ){
      BglTPingCBuf = (BglTPingSt*)tcalloc(NumNodesC, siz, 12107);
      BglTblTPingC = (I8s*)tcalloc(NumNodesC, sizeof(I8s), 12107);
    } 
    else{
      BglTPingCBuf = (BglTPingSt*)trecalloc((void*)BglTPingCBuf, 
					    NumNodesC*siz,
					    BglTPingCAllocSiz*siz,
					    12107);
      BglTblTPingC = (I8s*)trecalloc((void*)BglTblTPingC, 
				     NumNodesC*sizeof(I8s),
				     BglTPingCAllocSiz*sizeof(I8s),
				     12107);
    }
    BglTPingCAllocSiz = BglNumIPMapCDat = NumNodesC;
    if ( BglNumIPMapCDat > 0 ){
      for ( i = 0; i < BglNumIPMapCDat; i++ ){
	TBglCpyIPMap(i, i, 'b', 0);
	BglTblTPingC[i] = BGL_UNREAD;
      } 
      tmp = (BglMesSt*)tcalloc(BglNumIPMapCDat, sizeof(BglMesSt), 12109);
      len = TBglMakeBufTPing(tmp, 0, BglNumIPMapCDat, 0);
      cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, BglNumIPMapCDat);
      tfree(tmp, BglNumIPMapCDat * sizeof(BglMesSt), 12109);
      return cc;
    }
    else{
      return 0;
    }
  }
#else /* NET */
  cc = BglReplyAck(BglSock.fd, BGL_TPING_C_REF_NONET);
  return cc;
#endif /* NET */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_tping_c_off()
{
  I32s cc;

  DebugPrint("/*** do_misc_tping_c_off ***/\n");
#ifdef NET
  if (TBglChkReqApp(MCMISC_TPING_C_OFF)){
    TBgl_TPingC_Mode = 0;
    cc = BglReplyAck(BglSock.fd, BGL_TPING_C_OK);
    if(BglTPingCBuf != NULL){
      tfree(BglTPingCBuf, BglTPingCAllocSiz*sizeof(BglTPingSt), 12107);
      tfree(BglTblTPingC, BglTPingCAllocSiz*sizeof(I8s), 12107);
      BglTPingCBuf = NULL;
      BglTPingCAllocSiz = BglNumIPMapCDat = 0;
    }
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_TPING_C_REF);
  }
  return cc;
#else /* NET */
  cc = BglReplyAck(BglSock.fd, BGL_TPING_C_REF_NONET);
  return cc;
#endif /* NET */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_micro()
{
  I32s cc, l_MCSpy=0, l_MCState=0, cnt=0, total_len = 0;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_misc_micro ***/\n");
  if (TBglChkReqApp(MCMISC_MICRO_TOGGLE)==0){
    DebugPrint("Refused do_misc_micro\n");
    cc = BglReplyAck(BglSock.fd, BGL_MC_TOGGLE_REF);
    return cc;
  }
  
  if ( !NumCells){		/* if there are no cells */
    DebugPrint("No cells in soup\n");
    cc = BglReplyAck(BglSock.fd, BGL_MC_TOGGLE_REF);
    return cc;
  }

  if (MC_step == 1L){
    MC_step = -1L;
  }
  else if (MC_step == 0L){
    MC_step = 1L;
  }	
  else{
    MC_step = 0L;
  }

  if (MC_step > -1L)	DebugDisplay(ce);

  tmp = (BglMesSt*)tcalloc(2, sizeof(BglMesSt), 12107);	

  l_MCState = TBglMakeBufMCState(tmp, cnt, 1);
  if( l_MCState > 0 ){
    cnt++ ;
    total_len += l_MCState;
  }

  if( MC_step > -1L ){
    l_MCSpy = TBglMakeBufMCSpy(tmp, cnt, 1);
    if( l_MCSpy > 0 ){
      cnt++ ;
      total_len += l_MCSpy;
    }
  }

  if(cnt > 0 && total_len > 0 ){
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, cnt);
    tfree(tmp, 2 * sizeof(BglMesSt), 12107);
    if ( cc <= 0 ) return cc;
  }
  if ( (cc = BglReplyAck(BglSock.fd,BGL_MC_TOGGLE_OK)) <= 0 ) return cc;
  cc = TBglRecv(BglSock.fd);
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_micro_step_n()
{
  DebugPrint("/*** do_micro_step_n ***/\n");
  TBglNextFlg = 1L;
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_micro_step_q()
{
  DebugPrint("/*** do_micro_step_q ***/");
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_micro_step_slice()
{	
  I32s cc, l_MCState=0, cnt=0, total_len = 0;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_micro_step_slice ***/\n");
  if (BglMCStateBuf.Slice == 'T')   BglMCStateBuf.Slice = 't';
  else				    BglMCStateBuf.Slice = 'T';

  DebugPrint1("Changed BglMCStateBuf.Slice = %c\n", BglMCStateBuf.Slice);

  tmp = (BglMesSt*)tcalloc(1, sizeof(BglMesSt), 12108);	

  l_MCState = TBglMakeBufMCState(tmp, cnt, 1);
  if( l_MCState > 0 ){
    cnt ++ ;
    total_len += l_MCState;
  }

  cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, cnt);
  tfree(tmp, cnt * sizeof(BglMesSt), 12109);
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_micro_step_cpu()
{	
  I32s cc, l_MCState=0, cnt=0, total_len = 0;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_micro_step_cpu ***/\n");
  if (BglMCStateBuf.Cpu == 'u')		BglMCStateBuf.Cpu = 'U';
  else					BglMCStateBuf.Cpu = 'u';
  DebugPrint1("Changed BglMCStateBuf.Cpu = %c\n", BglMCStateBuf.Cpu);

  tmp = (BglMesSt*)tcalloc(1, sizeof(BglMesSt), 12110);

  l_MCState = TBglMakeBufMCState(tmp, cnt, 1);
  if( l_MCState > 0 ){
    cnt ++ ;
    total_len += l_MCState;
  }
  cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, cnt);
  tfree(tmp, cnt * sizeof(BglMesSt), 12110);
  return cc;
}

/*
 * do_micro_brk_put
 *	put a breakpoint. return flag returned by BglReplyAck.
 */
I32s do_micro_brk_put()
{
  int	ackflag;
  I32s	cc;
  
  /* receive data which follow command */
  DebugPrint("/*** do_micro_brk_put ***/\n");
  cc = TBglRecv(BglSock.fd);
  if (cc <= 0){
    tsprintf((char *)(&(bglStrBuf[0])),
        "TBglRecv error in do_micro_brk_put:tbgl_com.c");
    FEError(-12250, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
    return cc;
  }
  if (TBglChkReqApp(MCMICRO_BRK_PUT)){
    switch (BglBrkPointBuf.type){
    case BGL_BRK_CELL_OFF:
      ackflag = TBgl_Brk_GivenGen_Offset();
      break;
    case BGL_BRK_CURCELL_OFF:
      ackflag = TBgl_Brk_CurCell_Offset();
      break;
    case BGL_BRK_ABSINST:
      ackflag = TBgl_Brk_AbsTimeInst();
      break;
    case BGL_BRK_RELINST:
      ackflag = TBgl_Brk_IntrvlTimeInst();
      break;
    case BGL_BRK_THRDSIZE:
      ackflag = TBgl_Brk_ThrdAnaDatSizeRdy();
      break;
    case BGL_BRK_THRDCELL:
      ackflag = TBgl_Brk_ThrdAnaDatGeneRdy();
      break;
    default:
      ackflag = 0;
    }

    if (ackflag)
      cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_OK);
    else
      cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_REF);
  }
  else
    cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_REF);

  TBglNextBrkFlg = 1;
  MC_step	 = -1L;
  /* ask for next breakpoint */

  return cc;
}

/*
 * do_micro_brk_clear
 *	clear a breakpoint. return flag returned by BglReplyAck.
 */
I32s do_micro_brk_clear()
{
  int	ackflag;
  I32s	cc;
  
  DebugPrint("/*** do_micro_brk_clear ***/\n");
  if (TBglChkReqApp(MCMICRO_BRK_CLEAR)){
    ackflag = 1;
    GoBrkClr();
  }
  else
    ackflag = 0;

  if (ackflag)
    cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_OK);
  else
    cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_REF);

  return cc;
}

/*
 * do_micro_brk_repeat
 *	repeat a breakpoint. return flag returned by BglReplyAck.
 */
I32s do_micro_brk_repeat()
{
  I32s	cc;
  int	ackflag;
  
  DebugPrint("/*** do_micro_brk_repeat ***/\n");
  if (TBglChkReqApp(MCMICRO_BRK_REPEAT)){
    ackflag = BrkTrapCondBuf.savbrkdat.BrkSet;
    if (ackflag)
      RclBrkDat();
  }
  else
    ackflag = 0;
  ackflag = ackflag && BrkTrapCondBuf.savbrkdat.BrkSet;

  if (ackflag)
    cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_OK);
  else
    cc = BglReplyAck(BglSock.fd, BGL_MC_BRK_REF);

  TBglNextBrkFlg = 1;
  MC_step	 = -1L;
  /* ask for next breakpoint */

  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_micro_step_newcpu()
{	
  I32s cc, l_MCState=0, cnt=0, total_len = 0;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_micro_step_newcpu ***/\n");
  BglMCStateBuf.Cpu = 'N';
  DebugPrint1("Changed BglMCStateBuf.Cpu = %c\n", BglMCStateBuf.Cpu);

  tmp = (BglMesSt*)tcalloc(1, sizeof(BglMesSt), 12110);

  l_MCState = TBglMakeBufMCState(tmp, cnt, 1);
  if( l_MCState > 0 ){
    cnt++ ;
    total_len += l_MCState;
  }
  cc = TBglSendDat(BglSock.fd, &sendmes, tmp, total_len, cnt);
  tfree(tmp, sizeof(BglMesSt), 12110);
  if (TBglNewestNotCurrentCpu())
    TBglNextFlg = 1L;

  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_misc_inject_ret()
{
  DebugPrint("/*** do_misc_inject_ret ***/\n");
  return 1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_start_Inst()
{
  I32s cc=0, len, len1, len2, n_Frame, n_Inst, n ;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_OV_start_Inst ***/\n");
  if (TBglChkReqApp(MCOV_S_I)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_S_OK)) <= 0 ) return cc;
    if (!FD_ISSET(BGL_MASK_OV, &BglMask))  FD_SET(BGL_MASK_OV, &BglMask);

    n_Frame = TBglOvFrame();
    n_Inst = TBglOvInst();

    n = n_Frame + n_Inst;

    tmp = (BglMesSt*)tcalloc(n , sizeof(BglMesSt), 12113);
    len1 = TBglMakeBufOvFrame(tmp, 0, n_Frame);
    len2 = TBglMakeBufOvInst(tmp, n_Frame, n_Inst);
    len = len1 + len2;

    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, n);
    tfree(tmp, n * sizeof(BglMesSt), 12113);

    cc = cc + TBglOvInit();
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_S_REF);
    cc = 0;
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
I32s do_OV_start()
{
  I32s cc;

  DebugPrint("/*** do_OV_start ***/\n");
  if (TBglChkReqApp(MCOV_S)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_S_OK)) <= 0 ) return cc;
    if (!FD_ISSET(BGL_MASK_OV, &BglMask))  FD_SET(BGL_MASK_OV, &BglMask);

    cc = TBglOvInit();
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_S_REF);
    cc = 0;
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
I32s do_OV_quit()
{
  DebugPrint("/*** do_OV_quit ***/\n");
  if (TBglChkReqApp(MCOV_Q)){
    BglReplyAck(BglSock.fd, BGL_OV_Q_OK);
    if (FD_ISSET(BGL_MASK_OV, &BglMask))  FD_CLR(BGL_MASK_OV, &BglMask);
    return 1;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_Q_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_gene_addr_Inst()
{
  I32s cc, addr;
  I32s n_Frame, n_Inst, n, len, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_OV_gene_addr_Inst ***/\n");
  if (TBglChkReqApp(MCOV_GA_I)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_GA_OK)) <= 0 ) return cc;

    n_Frame = TBglOvFrame();
    n_Inst = TBglOvInst();
    n = n_Frame + n_Inst;
    tmp = (BglMesSt*)tcalloc(n , sizeof(BglMesSt), 12117);
    len1 = TBglMakeBufOvFrame(tmp, 0, n_Frame);
    len2 = TBglMakeBufOvInst(tmp, n_Frame, n_Inst);
    len = len1 + len2;
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, n);
    tfree(tmp, n * sizeof(BglMesSt), 12117);

    while (1){
      if ( (cc = TBglRecv(BglSock.fd)) <= 0 ) break;
      addr=atoi((const char *)(&(BglReplyMesBuf.mes[0])));
      cc = TBglGetGeneFromAddr(BglSock.fd, addr);
      if (cc >0){
	BglReplyBuf.ack = BGL_ACK_DEFAULT;
	cc = TBglRecv(BglSock.fd);
	if (cc <= 0)					break;
	if (BglReplyBuf.ack == BGL_OV_G_END)		break;
      }
      else
	break;
    }
    return cc;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_GA_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_gene_addr()
{
  I32s cc, addr;

  DebugPrint("/*** do_OV_gene_addr ***/\n");
  if (TBglChkReqApp(MCOV_GA)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_GA_OK)) <= 0 ) return cc;

    while (1){
      if ( (cc = TBglRecv(BglSock.fd)) <= 0 ) break;
      addr=atoi((const char *)(&(BglReplyMesBuf.mes[0])));
      cc = TBglGetGeneFromAddr(BglSock.fd, addr);
      if (cc >0){
	BglReplyBuf.ack = BGL_ACK_DEFAULT;
	cc = TBglRecv(BglSock.fd);
	if (cc <= 0)	                	break;
	if (BglReplyBuf.ack == BGL_OV_G_END)   	break;
      }
      else
	break;
    }
    return cc;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_GA_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_gene_size_Inst()
{
  I32s cc, siz;
  I8s buf1[5], lbl[4];
  I32s n_Frame, n_Inst, n, len, len1, len2;
  BglMesSt *tmp, sendmes;

  DebugPrint("/*** do_OV_gene_size_Inst ***/\n");
  if (TBglChkReqApp(MCOV_GS_I)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_GS_OK)) <= 0 ) return cc;

    n_Frame = TBglOvFrame();
    n_Inst = TBglOvInst();
    n = n_Frame + n_Inst;

    tmp = (BglMesSt*)tcalloc(n , sizeof(BglMesSt), 12118);
    len1 = TBglMakeBufOvFrame(tmp, 0, n_Frame);
    len2 = TBglMakeBufOvInst(tmp, n_Frame, n_Inst);
    len = len1 + len2;
    cc = TBglSendDat(BglSock.fd, &sendmes, tmp, len, n);
    tfree(tmp, n * sizeof(BglMesSt), 12118);
    if ( cc <= 0 ) return cc;

    OpenGeneBanker();		/* session with GeneBanker */
    
    cc = TBglRecv(BglSock.fd);

    while(cc > 0){
      memcpy(buf1, &BglReplyMesBuf.mes[0], 4);
      buf1[4] = '\0';
      siz = atoi((const char *)(&(buf1[0])));
      memcpy(lbl, &BglReplyMesBuf.mes[4], 3);
      lbl[3] = '\0';
      cc = TBglGetGeneFromSiz(BglSock.fd, siz, lbl);
      if ( cc <= 0 ) break;
      BglReplyBuf.ack = BGL_ACK_DEFAULT;
      if ( (cc = TBglRecv(BglSock.fd)) <= 0 ) break;
      if (BglReplyBuf.ack == BGL_OV_G_END)
	break;
      else
	cc = TBglRecv(BglSock.fd);
    }

    CloseGeneBanker();		/* close session */

    return cc;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_GS_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_gene_size()
{
  I32s cc, siz;
  I8s buf1[5], lbl[4];

  DebugPrint("/*** do_OV_gene_size ***/\n");
  if (TBglChkReqApp(MCOV_GS)){
    if ( (cc = BglReplyAck(BglSock.fd, BGL_OV_GS_OK)) <= 0 ) return cc;

    OpenGeneBanker();
    
    cc = TBglRecv(BglSock.fd);

    while (cc > 0){
      memcpy(buf1, &BglReplyMesBuf.mes[0], 4);
      buf1[4] = '\0';
      siz = atoi((const char *)(&(buf1[0])));
      memcpy(lbl, &BglReplyMesBuf.mes[4], 3);
      lbl[3] = '\0';
      cc = TBglGetGeneFromSiz(BglSock.fd, siz, lbl);
      if (cc <= 0) break;
      BglReplyBuf.ack = BGL_ACK_DEFAULT;	
      if ( (cc = TBglRecv(BglSock.fd)) <= 0 ) break;
      if (BglReplyBuf.ack == BGL_OV_G_END)
	break;
      else
	cc = TBglRecv(BglSock.fd);
    }

    CloseGeneBanker();

    return cc;
  }
  else{
    BglReplyAck(BglSock.fd, BGL_OV_GS_REF);
    return 0;
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_OV_save_cell()
{
  I32s	cc;
  
  /* receive data which follow command */
  DebugPrint("/*** do_OV_save_cell ***/\n");
  cc = TBglRecv(BglSock.fd);
  if (cc <= 0){
    tsprintf((char *)(&(bglStrBuf[0])),
        "TBglRecv error in do_OV_save_cell:tbgl_com.c");
    FEError(-12110, NOEXIT, NOWRITE, (char *)(&(bglStrBuf[0])));
    return cc;
  }

  if (SaveBuf.mode){/* most recent thread analysis data trapped */
    return SaveCell_ThrdAnaDatRdy();
  }
  else{
    return SaveCell_SpecificCell();
  }
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s SaveCell_ThrdAnaDatRdy()
{
  I32s	cc;
  GList* tgl;
  /* Check if file already exists ( But all time over write OK ) */
  cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_OK);

  if (cc <= 0)    return cc;

  if ( (cc = TBglRecv(BglSock.fd)) <= 0 )    return cc;

  if (BglReplyBuf.ack == BGL_OV_SAVE_REFUSE)  return 0;

  /* Write file now it's ok for Beagle */
  if ( TBglChkReqApp(MCOV_SAVE_CELL) ){

    if((rcnttardy.si)>=0){
      if((tgl=sl[rcnttardy.si]->g[rcnttardy.gi]=
	  gq_read(rcnttardy.si, rcnttardy.gi))){

	WritAscFile(sl[rcnttardy.si]->
		    g[rcnttardy.gi], &(SmplNam[0]), 1,
#ifdef NET
		    sl[rcnttardy.si]->sucsiznslrat,
		    sl[rcnttardy.si]->svsucsiznsl,
		    sl[rcnttardy.si]->svsiznsl
#else /* NET */
		    0.0, 0, 0
#endif /* NET */
		    , SaveBuf.detailrpt, 0, 0, 0, NULL, 0,
                    sl[rcnttardy.si]->genelkup,
                    &(sl[rcnttardy.si]->slst_gendef),
                    SplitTissueAna);

	tsprintf((char *)(&(bglStrBuf[0])),
		 "most recent thread analysis data trapped is saved in the file name %s by %s format",
		 &(SmplNam[0]),
		 SaveBuf.detailrpt? "long" : "short");
	if ( ( cc = BglReplyMes(BglSock.fd, bglStrBuf) ) <= 0 ) return cc;
	cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_OK);
	return cc;
      }
    }
    else{
      tsprintf((char *)(&(bglStrBuf[0])), "no thread analysis data trapped");
      if ( ( cc = BglReplyMes(BglSock.fd, bglStrBuf) ) <= 0 ) return cc;
    }

    cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
    return cc;
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
    return 0;
  }
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static I32s SaveCell_SpecificCell()
{
  I32s  gidx;
  I32s	size;
  I8s	filename[81], genfilename[81];
  I32s	cc, err;
  FILE *file, *afp;
  head_t head;
  indx_t *indx, *tindx;
  I32s        n;

  /* generate the complete filename */
  tsprintf((char *)(&(filename[0])),
    "%s%s", GenebankPath, SaveBuf.filename);
  
  /* Check if file already exists */
  if ((file = tfopen((I8s *)(&(filename[0])), (I8s *)"rb")))
  { cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
    tfclose(file);
  }
  else
    cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_OK);

  if (cc <= 0)    return cc;

  if ( (cc = TBglRecv(BglSock.fd)) <= 0 )    return cc;

  if (BglReplyBuf.ack == BGL_OV_SAVE_REFUSE)  return 0;

  /* Write file now it's ok for Beagle */
  if ( TBglChkReqApp(MCOV_SAVE_CELL) && BglLastCellViewed.size &&
       ( (SaveBuf.tarpt && ThreadAnalysis) || (! SaveBuf.tarpt))){
    /* Check if file is writable */
    if ( !(file = tfopen((I8s *)(&(filename[0])), (I8s *)"w"))){
      cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
      return 0;
    }
    tfclose(file);

    /* Write file, after having find the requested cell in GeneBanker*/
    if (GeneBnker){
      size = BglLastCellViewed.size;
      gidx = (SaveBuf.curorig == 'o') ?
	(BglLastCellViewed.origgi) : (BglLastCellViewed.gi);

      err = 0;
      if((I32u)((unsigned long)(sl[size]->g[gidx]))==(I32u)1){
	tsprintf((char *)(&(genfilename[0])),
            "%s%.4d.gen", GenebankPath, size);
	      
	if((afp=tfopen(genfilename, (I8s *)"rb"))){
	  head = read_head(afp);
	  indx = read_indx(afp, &head);
	  if (indx){
	    n=find_gen(indx, Int2Lbl(gidx), head.n);
	    if(n<head.n){
	      tindx = indx + n;
	      sl[size]->g[gidx]= get_gen(afp, &head, tindx, n);
	      thfree((void *)indx, head.n_alloc*sizeof(indx_t), 600);
	      indx = NULL;
	      tfclose(afp);
	    }
	    else
	      err=1;
	  }
	  else
	    err=1;
	}
	else
	  err=1;
      }

      if ( (!((size < 1) || (size > (siz_sl - 1)) || (!(sl[size]))))
	  && (gidx < sl[size]->a_num) && (sl[size]->g[gidx])
	  && !err){
	WritAscFile(sl[size]->g[gidx], filename, SaveBuf.tarpt,
#ifdef NET
		    sl[size]->sucsiznslrat,
		    sl[size]->svsucsiznsl,
		    sl[size]->svsiznsl
#else  /* NET */
		    0.0, 0, 0
#endif /* NET */
		    , SaveBuf.detailrpt, 0, 0, 0, NULL, 0,
                    sl[size]->genelkup, &(sl[size]->slst_gendef),
                    SplitTissueAna);

	tsprintf((char *)(&(bglStrBuf[0])),
		 "%sthe %s genome is saved into %s by %s format",
		 SaveBuf.tarpt? "the thread analysis of " : "",
		 SaveBuf.curorig == 'o'? "origin of" : "current",
		 filename,
		 SaveBuf.detailrpt? "long" : "short");
	if ( ( cc = BglReplyMes(BglSock.fd, bglStrBuf) ) <= 0 ) return cc;
	cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_OK);
	return cc;
      }
    }

    cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
    return cc;
  }
  else{
    cc = BglReplyAck(BglSock.fd, BGL_OV_SAVE_REFUSE);
    return 0;
  }
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_migTrack_on()
{
  I32s cc;
#ifdef NET
  BglTrackFlag Migr;
#endif /* NET */

  DebugPrint("/*** do_migTrack_on ***/\n");
#ifdef NET
  if (TBglChkReqApp(MCMIG_TRACK_ON)){
    if ((cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_OK)) <= 0) return cc;
    if ((cc = TBglRecv(BglMigrCtrlSockFd)) <= 0 ) return cc;

    Migr.node     = BglMigrationConnectBuf.node;
    Migr.portnb   = BglMigrationConnectBuf.portnb;
    Migr.session  = BglMigrationConnectBuf.session;
    strcpy((char *)(&(Migr.param[0])),
        (const char *)(&(BglMigrationConnectBuf.param[0])));
    MigrStartPut(Migr);
  }
  else{
    cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_REF);
  }
#else /* NET */
  cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_REF_NONET);
#endif /* NET */
  return cc;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_migTrack_off()
{
  I32s cc;
#ifdef NET
  BglTrackFlag Migr;
#endif /* NET */

  DebugPrint("/*** do_migTrack_off ***/\n");
#ifdef NET
  if (TBglChkReqApp(MCMIG_TRACK_OFF)){
    if ((cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_OK)) <= 0) return cc;
    if ((cc = TBglRecv(BglMigrCtrlSockFd)) <= 0) return cc;

    Migr.node	 = BglMigrationConnectBuf.node;
    Migr.portnb  = BglMigrationConnectBuf.portnb;
    Migr.session = BglMigrationConnectBuf.session;
    strcpy((char *)(&(Migr.param[0])),
        (const char *)(&(BglMigrationConnectBuf.param[0])));
    MigrStopPut(Migr);
  }
  else{
    cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_REF);
  }
#else /* NET */
  cc = BglReplyAck(BglMigrCtrlSockFd, BGL_MIG_REQ_REF_NONET);
#endif /* NET */
  return cc;
}
