/* clnt_dat.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Jan 25 13:34:03 2000
 * Update Count    : 182
 * Status          : 
 */

/* Decode the data functions of client beagle code                         */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #112XX, in use:   11201 - 11202 */
/* BglError labels: #112XX, in use:   */


#ifndef lint
static char* sccsProgramId="@(#)clnt_dat.c	1.43	01/25/00 13:34:23";
#endif /* lint */

#include "clnt.h"
#include "beagleMessageMgr.h"
#include "beagleUTIL_SockMgr.h"

/*
 *- SPECIAL VALIABLE NAME
 *- ====================
 *-
 */

/* None */

/*
 *- HERE ARE GLOBAL VARIABLE/TYPE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ====================================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */
static BglOvFrameSt	BglOvFrameBuf;


/*
 *- HERE ARE EXTERN FUNCTION DEFINEs.
 *- ================================
 *- Most define are already in  above include files.
 */
extern void	BglMM_Reply_store	P_((BglReplySt*));
extern void	BglMM_ReplyMes_store	P_((BglReplyMesSt*));
extern void	BglMM_Stats_store	P_((BglStatsSt*));
extern void	BglMM_MCState_store	P_((BglMCStateSt*));
extern void	BglMM_Message_store	P_((BglMessageSt*));


/*
 *- HERE ARE THIS files SERVICE FUNCTIONs FOR CALL FROM ANOTHER FILES.
 *- ==================================================================
 *- (see "clnt_fsm.h")
 */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_ABORT(buf)
     I8s* buf;
{
  DebugPrint( "/*** do_BGL_ABORT ***/\n" );
  return 1;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_QUIT(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_QUIT ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglQuitSt(&msgHndl, &BglQuitBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_QUIT()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "Tierra was terminated by user");
    BglMM_Connect_Close();
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_REPLY(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_REPLY ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglReplySt(&msgHndl, &BglReplyBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_REPLY()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Reply_store(&BglReplyBuf);
  }
  txdr_destroy(&msgHndl);
  DebugPrint1("BglReplyBuf.ack=%d\n", BglReplyBuf.ack);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_REPLYMES(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_REPLYMES ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglReplyMesSt(&msgHndl, &BglReplyMesBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_REPLYMES()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_ReplyMes_store(&BglReplyMesBuf);
  }
  txdr_destroy(&msgHndl);
  DebugPrint1("do_BGL_REPLYMES : buf = %s", BglReplyMesBuf.mes);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_STATS(buf)
     I8s* buf;
{
  BglStatsSt tmpStats;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_STATS ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglStatsSt(&msgHndl, &tmpStats) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_STATS()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Stats_store(&tmpStats);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_PLAN(buf)
     I8s* buf;
{
  BglPlanSt tmpPlan;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_PLAN ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglPlanSt(&msgHndl, &tmpPlan) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_PLAN()");
  }
  else{
    n = txdr_getpos(&msgHndl);

    tmpPlan.MaxGenPop_label[3] = '\0';
    tmpPlan.MaxGenMem_label[3] = '\0';

    BglMM_Plan_store(&tmpPlan);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_HISTCLEAR(buf)
     I8s* buf;
{
  BglHistClearSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_HISTCLEAR ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglHistClearSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_HISTCLEAR()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_HistClear_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_SIZHIST(buf)
     I8s* buf;
{
  BglSizHistSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_SIZHIST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglSizHistSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_SIZHIST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Info_s_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_GENHIST(buf)
     I8s* buf;
{
  BglGenHistSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_GENHIST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglGenHistSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_GENHIST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    tmpData.lbl[3] = '\0';
    BglMM_Info_g_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MEMHIST(buf)
     I8s* buf;
{
  BglMemHistSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_MEMHIST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMemHistSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_MEMHIST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Info_m_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_ESHIST(buf)
     I8s* buf;
{
  BglEfficSHistSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_ESHIST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglEfficSHistSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_ESHIST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Info_es_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_EGHIST(buf)
     I8s* buf;
{
  BglEfficGHistSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_EGHIST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglEfficGHistSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_EGHIST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    tmpData.lbl[3] = '\0';
    BglMM_Info_eg_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_SIZQUERY_O(buf)
     I8s* buf;
{
  BglSizQueryOutputSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_SIZQUERY_O ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglSizQueryOutputSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_SIZQUERY_O()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    tmpData.lbl[3] = '\0';
    BglMM_SizQuery_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MC_STATE(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_MC_STATE ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMCStateSt(&msgHndl, &BglMCStateBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_MC_STATE()");
  }
  else{
    n = txdr_getpos(&msgHndl);

    if (BglMCStateBuf.MC_step == 'k')
      Bgl_step = 1L;	/* MICRO Debugger Keypress-mode */
    else if (BglMCStateBuf.MC_step == 'd')
      Bgl_step = 0L;	/* MICRO Debugger Delay-mode */
    else
      Bgl_step = -1L;	/* MICRO Debugger Off */

    BglMM_MCState_store(&BglMCStateBuf);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MC_SPY(buf)
     I8s* buf;
{
  BglMCSpySt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_MC_SPY ***/\n" );
  memset(tmpData.d_data, 0, BGL_MICRO_SPY_DATA_LEN);
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMCSpySt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_MC_SPY()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    tmpData.lbl[3] = '\0';
    tmpData.id[BGL_MICRO_SPY_ID_LEN] = '\0';
    tmpData.regcol0[BGL_MICRO_SPY_NUMSPECREG] = '\0';
    tmpData.regcol1[BGL_MICRO_SPY_NUMSPECREG] = '\0';
    tmpData.regcol2[BGL_MICRO_SPY_NUMSPECREG] = '\0';
    tmpData.regcol3[BGL_MICRO_SPY_NUMSPECREG] = '\0';
    tmpData.regcol4[BGL_MICRO_SPY_NUMSPECREG] = '\0';
    tmpData.regcol5[BGL_MICRO_SPY_NUMSPECREG] = '\0';

    BglMM_MCSpy_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_TPING_S(buf)
     I8s* buf;
{
  BglTPingSt tmpTping;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_TPING_S ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglTPingSt(&msgHndl, &tmpTping) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_TPING_S()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_TPingS_store(&tmpTping);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_TPING_C(buf)
     I8s* buf;
{
  BglTPingSt tmpTping;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_TPING_C ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglTPingSt(&msgHndl, &tmpTping) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_TPING_C()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_TPingC_store(&tmpTping);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_VAR(buf)
     I8s* buf;
{
  BglVarSt tmpVar;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_VAR ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglVarSt(&msgHndl, &tmpVar) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_VAR()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Var_store(&tmpVar);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_OVFRAME(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_OVFRAME ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglOvFrameSt(&msgHndl, &BglOvFrameBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_OVFRAME()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_OvFrame_store(&BglOvFrameBuf);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_OV_G_INST(buf)
     I8s* buf;
{
  BglOvGInstSt temp;
  XDR msgHndl;
  I32s n;
  
  DebugPrint( "/*** do_BGL_OV_G_INST ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglOvGInstSt(&msgHndl, &temp) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_OV_G_INST()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_OvGInst_store(&temp);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_OV(buf)
     I8s* buf;
{
  BglOvMesSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_OV ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglOvMesSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_OV()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Ov_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_OV_G(buf)
     I8s* buf;
{
  BglOvGOutputSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_OV_G ***/\n" );
  tmpData.genome.genome_val = (xdrwrd*)tcalloc(1, XDRBufMaxSize, 11201);
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglOvGOutputSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_OV_G()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_OvG_store(&tmpData);
  }
  txdr_destroy(&msgHndl);
  tfree(tmpData.genome.genome_val, XDRBufMaxSize, 11201);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MESSAGE(buf)
     I8s* buf;
{
  XDR msgHndl;
  I32s n;
  BglMessageSt	dataBuf;

  DebugPrint( "/*** do_BGL_MESSAGE ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMessageSt(&msgHndl, &dataBuf) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_MESSAGE()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Message_store(&dataBuf);
  }
  txdr_destroy(&msgHndl);

  return n;
}


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_BGL_MIGRATION(buf)
     I8s* buf;
{
  BglMigrationDataSt tmpData;
  XDR msgHndl;
  I32s n;

  DebugPrint( "/*** do_BGL_MIGRATION ***/\n" );
  txdrmem_create(&msgHndl, buf, XDRBufMaxSize, XDR_DECODE);
  if( !xdr_BglMigrationDataSt(&msgHndl, &tmpData) ){
    n = 0;	/* Decode ERROR */
    BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0,
	      "XDR decode error in the function do_BGL_MIGRATION()");
  }
  else{
    n = txdr_getpos(&msgHndl);
    BglMM_Migration_store(&tmpData);
  }
  txdr_destroy(&msgHndl);

  return n;
}
