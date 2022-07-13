/* bglstd_show.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:12:51 1999
 * Update Count    : 46
 * Status          :
 */
/* Display functions for stdio mode beagle                                 */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #133XX, in use:   */
/* BglError labels: #133XX, in use:   */

#include "clnt.h"
#include "beagleMessageMgr.h"
#include "beagleMessage_Ov.h"
#include "bglstd_show.h"
#include <time.h>

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowReplyMes()
{
  BglReplyMesSt *mes;

  while( BglMM_HowManyReplyMes() ){
    mes = BglMM_PickReplyMes();
    tsprintf(bglStrBuf, "%s", mes->mes);
    BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowMessage()
{
  BglMessageSt* data;
  while( BglMM_HowManyMessage() ){
    data = BglMM_PickMessage();
    tsprintf(bglStrBuf, "%s", data->message);
    BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowStats()
{
  BglStatsSt* data;

  while( BglMM_HowManyStats() ){
    data = BglMM_PickStats();
    tsprintf(bglStrBuf,
	     "---------------------------------------------------------");
    BglStdPrintf(BGL_STAT_SCR, BGL_STAT_X, BGL_STAT_Y, 0, 0, bglStrBuf);
    
    tsprintf(bglStrBuf,
	     " InstExe.m    =     %d  InstExec.i = %d NumCells = %d",
	     (int)data->InstExe_m, (int)data->InstExe_i, (int)data->NumCells);
    BglStdPrintf(BGL_STAT_SCR, BGL_STAT_X, BGL_STAT_Y + 1, 0, 0, bglStrBuf);

    if(data->GeneBnker > 0) {
      tsprintf(bglStrBuf,
	       " NumGenotypes =    %d NumSizes   = %d ",
	       (int)data->NumGenotypes, (int)data->NumSizes);	
      BglStdPrintf(BGL_STAT_SCR, BGL_STAT_X, BGL_STAT_Y + 2, 0, 0, bglStrBuf);

      tsprintf(bglStrBuf, " Extracted = %s  ", data->ExtrG_gen);
      BglStdPrintf(BGL_STAT_SCR, BGL_STAT_X, BGL_STAT_Y + 3, 0, 0, bglStrBuf);
    }
    tsprintf(bglStrBuf,
	     "---------------------------------------------------------");
    BglStdPrintf(BGL_STAT_SCR, BGL_STAT_X, 
		 BGL_STAT_Y + ((data->GeneBnker) ? 4 : 2), 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowPlan()
{
  BglPlanSt* data;

  while( BglMM_HowManyPlan() ){
    data = BglMM_PickPlan();
    tsprintf(bglStrBuf, "InstExeC  %8d  Generation%8.0f  %9d     %s",
	     data->InstExe_m, data->Generations, data->tp,
	     ctime((time_t*)&(data->tp)));
    BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y, 0, 0, bglStrBuf);

    if(data->GeneBnker){
      tsprintf(bglStrBuf,
	       "NumCells  %8d  NumGen    %8d  NumSizes  %8d  Speed     %8d",
	       data->NumCells, data->NumGenotypes, data->NumSizes, data->Speed);
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+1, 0, 0, bglStrBuf);
    }
    else{ /* (GeneBnker) */
      tsprintf(bglStrBuf, "NumCells  %8d  Speed     %8d",
	       data->NumCells, data->Speed);
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+1, 0, 0, bglStrBuf);
    }
    tsprintf(bglStrBuf,
	     "RateMut   %8d  RatMovMut %8d  RateFlaw  %8d",
	     data->RateMut, data->RateMovMut, data->RateFlaw);	
    BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+2, 0, 0, bglStrBuf);

    tsprintf(bglStrBuf,
	     "AvgSize   %8d  AvgPop    %8.0f  FecundAvg %8.1f  RepInstEf %8.1f",
	     data->AverageSize, data->AvgPop,
	     data->FecundityAvg, data->RepInstEff);
    BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+3, 0, 0, bglStrBuf);

    tsprintf(bglStrBuf,
	     "Births    %8d  BirthLoca %8d  BirthInje %8d  BirthEjec %8d",
	     data->FEBirthNum, data->FEBirthLocal,
	     data->FEBirthInject, data->FEBirthEject);
    BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+4, 0, 0, bglStrBuf);

    tsprintf(bglStrBuf,
	     "Deaths    %8d  DeathFecY %8d  DeathFecZ %8d  DeathEjec %8d",
	     data->FEDeathNum, data->FEFecundYesSum,
	     data->FEFecunZeroSum, data->FEFecunEjecSum);	
    BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+5, 0, 0, bglStrBuf);

    if(data->GeneBnker){
      tsprintf(bglStrBuf,
	       "MaxGenPop %8d  (%.4d%3.3s) MaxGenMem %8d (%.4d%3.3s)  NumGenDG  %8d",
	       data->MaxPop, data->MaxGenPop_size, data->MaxGenPop_label, 
	       data->MaxMemry, data->MaxGenMem_size, data->MaxGenMem_label,
	       data->NumGenDG); 
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y+6, 0, 0, bglStrBuf);
    }

    if(data->NetFlg == BGL_FLG_TRUE){
      tsprintf(bglStrBuf,
	       "NumNodes  %8hu  SendImmig %8d  RecvImmig %8d  EjeToSelf %8d",
	       data->NumNodes, data->FESImmigraNum,
	       data->FERImmigraNum, data->FEEjectToSelf);
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, 
		BGL_PLN_Y + ((data->GeneBnker) ? 7 : 6), 0, 0, bglStrBuf);
      tsprintf(bglStrBuf,
	       "SePingReq %8d  SePingRep %8d  SeSendAdd %8d  SendBye   %8d",
	       data->FESPingReqNum, data->FESPingRepNum, 
	       data->FESSendAddNum, data->FESByeNum);
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, 
		BGL_PLN_Y + ((data->GeneBnker) ? 8 : 7), 0, 0, bglStrBuf);
      tsprintf(bglStrBuf,
	       "RePingReq %8d  RePingRep %8d  ReSendAdd %8d  RecvBye   %8d",
	       data->FERPingReqNum, data->FERPingRepNum, 
	       data->FERSendAddNum, data->FERByeNum);
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, 
		BGL_PLN_Y + ((data->GeneBnker) ? 9 : 8), 0, 0, bglStrBuf);
    }

    if(data->MemProFlg == BGL_FLG_TRUE){
      tsprintf(bglStrBuf,
	       "    DynMemUse = %8d  TotNumAloc   = %8d",
	       data->DynMemUse, data->TotNumAloc);	
      BglStdPrintf(BGL_PLN_SCR, BGL_PLN_X, BGL_PLN_Y + 8, 0, 0, bglStrBuf);
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
void BglStdShowMCState()
{
  BglMCStateSt* data;

  while( BglMM_HowManyMCState() ){
    data = BglMM_PickMCState();
    tsprintf(bglStrBuf, 
	     "---------------------------------------------------");
    BglStdPrintf(BGL_MC_STATE_SCR, BGL_MC_STATE_X, BGL_MC_STATE_Y, 0, 0, bglStrBuf);
    tsprintf(bglStrBuf, 
	     " MC_step = %c Slice = %c Cpu = %c n_OV = %d ",
	     data->MC_step, data->Slice, data->Cpu, BglMM_HowManyOv());
    BglStdPrintf(BGL_MC_STATE_SCR, BGL_MC_STATE_X, BGL_MC_STATE_Y+1, 0, 0, bglStrBuf);
    tsprintf(bglStrBuf, 
	     "---------------------------------------------------");
    BglStdPrintf(BGL_MC_STATE_SCR, BGL_MC_STATE_X, BGL_MC_STATE_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowMCSpy()
{
  BglMCSpySt* data;

  while( BglMM_HowManyMCSpy() ){
    data = BglMM_PickMCSpy();

    /* Display Line 0: */
    if(data->Stack_size > 0){
      tsprintf(bglStrBuf,
	       "  Cell %2d:%3d %.4d%3.3s @ %7d Slice=%4d Stack [%11d]  ",
	       (int)data->cp_q_this_a, (int)data->cp_q_this_i, data->cp_mm_s,
	       data->lbl, data->cp_mm_p, (int)data->cp_c_ib,
	       data->cp_c_c_st[0]);
      if (data->cp_c_c_sp == 0) bglStrBuf[65] = '<';
    }
    else{
      tsprintf(bglStrBuf,
	       "  Cell %2d:%3d %.4d%3s @ %7d Slice=%4d                      ",
	       (int)data->cp_q_this_a, (int)data->cp_q_this_i, data->cp_mm_s,
	       data->lbl, data->cp_mm_p, (int)data->cp_c_ib);
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y, 0, 0, bglStrBuf);

    /* Display Line 1: */
    if(data->Stack_size > 1){
      tsprintf(bglStrBuf,
	       "    IP [%11d] (%c%-7d) = 0x%02x %9.9s   [%11d]  ",
	       data->cp_c_c_ip, data->off_char, data->off_int,
	       (unsigned int)data->soup, data->id, data->cp_c_c_st[1]);
      if (data->cp_c_c_sp == 1) bglStrBuf[65] = '<';
    }
    else{
      tsprintf(bglStrBuf,
	       "    IP [%11d] (%c%-7d) = 0x%02x %9.9s                  ",
	       data->cp_c_c_ip, data->off_char, data->off_int,
	       (unsigned int)data->soup, data->id);
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 1,0,0,bglStrBuf);

    /* Display Line 2: */
    if(data->Stack_size > 2){
      if(data->Numreg > 0){
	tsprintf(bglStrBuf,
		 "%3s AX [%11d] %-29s [%11d]  ",
		 data->regcol0, data->cp_c_c_re[0],
		 data->hostbuf1, data->cp_c_c_st[2]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol0, data->hostbuf1, data->cp_c_c_st[2]);
      }
      if (data->cp_c_c_sp == 2) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 0){
	tsprintf(bglStrBuf,
		 "%3s AX [%11d] %-29s                ",
		 data->regcol0, data->cp_c_c_re[0], data->hostbuf1);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol0, data->hostbuf1);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 2,0,0,bglStrBuf);

    /* Display Line 3: */
    if(data->Stack_size > 3){
      if(data->Numreg > 1){
	tsprintf(bglStrBuf,
		 "%3s BX [%11d] %-29s [%11d]  ",
		 data->regcol1, data->cp_c_c_re[1],
		 data->hostbuf2, data->cp_c_c_st[3]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol1, data->hostbuf2, data->cp_c_c_st[3]);
      }
      if (data->cp_c_c_sp == 3) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 1){
	tsprintf(bglStrBuf,
		 "%3s BX [%11d] %-29s                ",
		 data->regcol1, data->cp_c_c_re[1], data->hostbuf2);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol1, data->hostbuf2);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 3,0,0,bglStrBuf);

    /* Display Line 4: */
    if(data->Stack_size > 4){
      if(data->Numreg > 2){
	tsprintf(bglStrBuf,
		 "%3s CX [%11d] %-29s [%11d]  ",
		 data->regcol2, data->cp_c_c_re[2],
		 data->hostbuf3, data->cp_c_c_st[4]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol2, data->hostbuf3, data->cp_c_c_st[4]);
      }
      if (data->cp_c_c_sp == 4) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 2){
	tsprintf(bglStrBuf,
		 "%3s CX [%11d] %-29s                ",
		 data->regcol2, data->cp_c_c_re[2], data->hostbuf3);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol2, data->hostbuf3);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 4,0,0,bglStrBuf);

    /* Display Line 5: */
    if(data->Stack_size > 5){
      if(data->Numreg > 3){
	tsprintf(bglStrBuf,
		 "%3s DX [%11d] %-29s [%11d]  ",
		 data->regcol3, data->cp_c_c_re[3],
		 data->hostbuf4, data->cp_c_c_st[5]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol3, data->hostbuf4, data->cp_c_c_st[5]);
      }
      if (data->cp_c_c_sp == 5) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 3){
	tsprintf(bglStrBuf,
		 "%3s DX [%11d] %-29s                ",
		 data->regcol3, data->cp_c_c_re[3], data->hostbuf4);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol3, data->hostbuf4);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 5,0,0,bglStrBuf);

    /* Display Line 6: */
    if(data->Stack_size > 6){
      if(data->Numreg > 4){
	tsprintf(bglStrBuf,
		 "%3s EX [%11d] %-29s [%11d]  ",
		 data->regcol4, data->cp_c_c_re[4],
		 data->hostbuf5, data->cp_c_c_st[6]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol4, data->hostbuf5, data->cp_c_c_st[6]);
      }
      if (data->cp_c_c_sp == 6) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 4){
	tsprintf(bglStrBuf,
		 "%3s EX [%11d] %-29s                ",
		 data->regcol4, data->cp_c_c_re[4], data->hostbuf5);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol4, data->hostbuf5);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 6,0,0,bglStrBuf);

    /* Display Line 7: */
    if(data->Stack_size > 7){
      if(data->Numreg > 5){
	tsprintf(bglStrBuf,
		 "%3s FX [%11d] %-29s [%11d]  ",
		 data->regcol5, data->cp_c_c_re[5],
		 data->hostbuf6, data->cp_c_c_st[7]);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s [%11d]  ",
		 data->regcol5, data->hostbuf6, data->cp_c_c_st[7]);
      }
      if (data->cp_c_c_sp == 7) bglStrBuf[65] = '<';
    }
    else{
      if(data->Numreg > 5){
	tsprintf(bglStrBuf,
		 "%3s FX [%11d] %-29s                ",
		 data->regcol5, data->cp_c_c_re[5], data->hostbuf6);
      }
      else{
	tsprintf(bglStrBuf,
		 "%3s                  %-29s                ",
		 data->regcol5, data->hostbuf6);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 7,0,0,bglStrBuf);

    /* Display Line 8: */
    if(data->Ploidy > 1){
      if(data->Stack_size > 8){
	tsprintf(bglStrBuf,
		 "    Flag: %2d   Stk: %2d      Tracks: Ex So De       [%11d]  ",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp, data->cp_c_c_st[8]);
	if (data->cp_c_c_sp == 8) bglStrBuf[65] = '<';
      }
      else{
	tsprintf(bglStrBuf,
		 "    Flag: %2d   Stk: %2d      Tracks: Ex So De                      ",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp);
      }
    }
    else{
      if(data->Stack_size > 8){
	tsprintf(bglStrBuf,
		 "    Flag: %2d   Stk: %2d                             [%11d]  ",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp, data->cp_c_c_st[8]);
	if (data->cp_c_c_sp == 8) bglStrBuf[65] = '<';
      }
      else{
	tsprintf(bglStrBuf,
		 "    Flag: %2d   Stk: %2d                                      ",
		 (int)data->cp_c_c_fl_E, data->cp_c_c_sp);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 8,0,0,bglStrBuf);

    /* Display Line 9: */
    if(data->Ploidy > 1){
      if(data->Stack_size > 9){
	tsprintf(bglStrBuf,
		 "    %-30.30s  %2d %2d %2d       [%11d]  ",
		 data->d_data,
		 data->cp_c_c_ex, data->cp_c_c_so, data->cp_c_c_de,
		 data->cp_c_c_st[9]);
	if (data->cp_c_c_sp == 9) bglStrBuf[65] = '<';
      }
      else{
	tsprintf(bglStrBuf,
		 "    %-30.30s  %2d %2d %2d                      ",
		 data->d_data,
		 data->cp_c_c_ex, data->cp_c_c_so, data->cp_c_c_de);
      }
    }
    else {
      if(data->Stack_size > 9){
	tsprintf(bglStrBuf,
		 "    %-30.30s                 [%11d]  ",
		 data->d_data, data->cp_c_c_st[9]);
	if (data->cp_c_c_sp == 9) bglStrBuf[65] = '<';
      }
      else{
	tsprintf(bglStrBuf,
		 "    %-30.30s                                ",
		 data->d_data);
      }
    }
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y + 9,0,0,bglStrBuf);

    /* Display Line 10: */
    tsprintf(bglStrBuf,
	     "           %3d of %3d Cpus  thread [%6d]                        ",
	     data->cp_c_ac, data->cp_c_n, data->cp_c_c_threadid);
    if (data->cp_c_c_sp == 10) bglStrBuf[65] = '<';
    BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X, BGL_MC_SPY_Y+10,0,0,bglStrBuf);
  }
}



void BglStdShowTPingSStatsToOn()
{
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Tping (Server) On");
}

void BglStdShowTPingCStatsToOn()
{
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Tping (Cluster) On");
}

void BglStdShowTPingSStatsToOff()
{
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Tping (Server) Off");
}

void BglStdShowTPingCStatsToOff()
{
  BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, "Tping (Cluster) Off");
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowTPingS()
{
  BglTPingSt* t;

  while( BglMM_HowManyTPingS() ){
    t = BglMM_PickTPingS();

   tsprintf(bglStrBuf,
   "(Server)  %c %u %hu %6d %6d %4d %6d %6d %6u %6u %4d %4d %1d",
	     t->mode, t->node, t->portnb,
	     t->FecundityAvg, t->Speed, (int)t->NumCells, t->AgeAvg,
	     t->SoupSize, t->TransitTime, t->Time, (I32s)t->InstExec,
	     t->InstExecConnect, t->OS);
    BglStdPrintf(BGL_TPING_S_SCR,
		 BGL_TPING_S_X, BGL_TPING_S_Y+t->id,0,0,bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowTPingC()
{
  BglTPingSt* t;

  while( BglMM_HowManyTPingC() ){
    t = BglMM_PickTPingC();

    tsprintf(bglStrBuf,
    "(Cluster) %c %u %hu %6d %6d %4d %6d %6d %6u %6u %4d %4d %1d",
	     t->mode, t->node, t->portnb,
	     t->FecundityAvg, t->Speed, (int)t->NumCells, t->AgeAvg,
	     t->SoupSize, t->TransitTime, t->Time, (I32s)t->InstExec,
	     t->InstExecConnect, t->OS);
    BglStdPrintf(BGL_TPING_C_SCR,
		 BGL_TPING_C_X, BGL_TPING_C_Y+t->id,0,0,bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowVar()
{
  BglVarSt* v;

  while( BglMM_HowManyVar() ){
    v = BglMM_PickVar();
    tsprintf(bglStrBuf, "%s %s", v->name, v->value);
    BglStdPrintf(BGL_VARE_SCR, BGL_VARE_X, BGL_VARE_Y+v->index,0,0,bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowHistClear()
{
  /* NOP */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowSizHist()
{
  BglSizHistSt* data;

  while( BglMM_HowManySizeHist() ){
    data = BglMM_PickSizeHist();
    tsprintf(bglStrBuf, "%d: %.4hu %d %d",
	     data->index, data->size, data->num, data->count);
    BglStdPrintf(BGL_INFOS_SCR, BGL_INFOS_X, BGL_INFOS_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowGenHist()
{
  BglGenHistSt *data;
  while( BglMM_HowManyGeneHist() ){
    data = BglMM_PickGeneHist();
    tsprintf(bglStrBuf, "%d: %.4hu%3.3s %d",
	     data->index, data->size, data->lbl, data->count);
    BglStdPrintf(BGL_INFOG_SCR, BGL_INFOG_X, BGL_INFOG_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowMemHist()
{
  BglMemHistSt *data;
  while( BglMM_HowManyMemHist() ){
    data = BglMM_PickMemHist();
    tsprintf(bglStrBuf, "%d: %.4hu %d %d",
	     data->index, data->size, data->num, data->count);
    BglStdPrintf(BGL_INFOM_SCR, BGL_INFOM_X, BGL_INFOM_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowESHist()
{
  BglEfficSHistSt* data;
  while( BglMM_HowManyEfficSHist() ){
    data = BglMM_PickEfficSHist();
    tsprintf(bglStrBuf, "%d: %.4hu %d %f",
	     data->index, data->size, data->num, data->dblcount);
    BglStdPrintf(BGL_INFOM_SCR, BGL_INFOM_X, BGL_INFOM_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowEGHist()
{
  BglEfficGHistSt* data;
  while( BglMM_HowManyEfficGHist() ){
    data = BglMM_PickEfficGHist();
    tsprintf(bglStrBuf, "%d: %.4hu%3.3s %f",
	     data->index, data->size, data->lbl, data->dblcount);
    BglStdPrintf(BGL_INFOG_SCR, BGL_INFOG_X, BGL_INFOG_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowSizQuery()
{
  BglSizQueryOutputSt* data;

  while( BglMM_HowManySizeQuery() ){
    data = BglMM_PickSizeQuery();
    tsprintf(bglStrBuf, "%d %3.3s %d %d %d %d %s",
	     data->count, data->lbl, data->moves, data->flags, 
	     data->bits, data->csS, data->WatchBits); 
    BglStdPrintf(BGL_INFOZ_SCR, BGL_INFOZ_X, BGL_INFOZ_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowOv()
{
  BglOvMesSt* data;

  while( BglMM_HowManyOv() ){
    data = BglMM_PickOv();
    tsprintf(bglStrBuf,
	     "%c %d %d ", data->event, data->start, data->length);
    BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowOvFrame()
{
  BglOvFrameSt* data;

  while( BglMM_HowManyOvFrame() ){
    data = BglMM_PickOvFrame();
    tsprintf(bglStrBuf, "%d, %d, %d, %d, %d",
	     data->InstNum,   data->SoupSize,   data->NumCells,
	     data->NumSizes,  data->NumGenotypes);
    BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowOvGInst()
{
  BglOvGInstSt* data;

  while( BglMM_HowManyOvGInst() ){
    data = BglMM_PickOvGInst();
    tsprintf(bglStrBuf, "%x, %s", data->op, data->mn);
    BglStdPrintf(BGL_OV_SCR, BGL_OV_X, BGL_OV_Y, 0, 0, bglStrBuf);
  }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void BglStdShowOvG()
{
  BglOvGOutputSt* data;
  I32s i, addr;

  while( BglMM_HowManyOvG() ){
    data = BglMM_PickOvG();
    tsprintf(bglStrBuf, "Genome = %4.4d%3s (%s)",
	     (int)data->genome_siz, data->label, data->comment);
    BglStdPrintf(BGL_GENE_SCR, BGL_GENE_X, BGL_GENE_Y, 0, 0, bglStrBuf);
    tsprintf(bglStrBuf, "Start Address = %d, End Address = %d",	
	     (I32s)data->start,  data->start + data->genome_siz);
    BglStdPrintf(BGL_GENE_SCR, BGL_GENE_X, BGL_GENE_Y + 1, 0, 0, bglStrBuf);
    tsprintf(bglStrBuf, "No. Address : Instruction");
    BglStdPrintf(BGL_GENE_SCR, BGL_GENE_X, BGL_GENE_Y + 2, 0, 0, bglStrBuf);
    for (i=0; i < data->genome_siz; i++){
      addr = data->start + i;
      tsprintf(bglStrBuf, "%4d %6d : %2x %9s", 
	       (int)i, (int)addr, (int)(*((I8s*)data->genome.genome_val+i)),
	       BglMM_InstrDisassemble(*((I8s*)data->genome.genome_val+i)));
      BglStdPrintf(BGL_GENE_SCR, BGL_GENE_X, BGL_GENE_Y+2+i,0,0,bglStrBuf);
    }
  }
}

void	BglStdShowMicroDebuggerToKeypressMode()
{
  BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X,BGL_MICRO_Y,0,0,
	       "MICRO Debugger Keypress-mode");
}

void	BglStdShowMicroDebuggerToDelayMode()
{
  BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X,BGL_MICRO_Y,0,0,
	       "MICRO Debugger Delay-mode");
}

void	BglStdShowMicroDebuggerToOffMode()
{
  BglStdPrintf(BGL_MC_SPY_SCR, BGL_MC_SPY_X,BGL_MICRO_Y,0,0,
	       "MICRO Debugger Off");
}

void	BglStdShowMigrationTracking()
{
  BglMigrationDataSt* d;
  char *info;
  char	tierraTime[26], beagleTime[26];

  while( BglMM_HowManyMigration() ){
    d = BglMM_PickMigration();
    switch(d->information){
    case 'B':
      info = "Birth";
      break;
    case 'D':
      info = "Death";
      break;
    case 'R':
      info = "Remote Birth";
      break;
    case 'M':
      info = "Migration";
      break;
    case 'I':
      info = "Immigration";
      break;
    case 'A':
      info = "Apocalypse";
      break;
    case 'S':
      info = "Sleeping Node";
      break;
    case 'n':
      info = "normal Termination";
      break;
    default:
      info = "unknown event";
      break;
    }
    strncpy(tierraTime, ctime((time_t*)&(d->tierraTime)), 26);
    strncpy(beagleTime, ctime((time_t*)&(d->beagleTime)), 26);
    tierraTime[24] = beagleTime[24] = '\0';
    tsprintf(bglStrBuf, "%-20s %-30s %5d %15d %5d %24s %24s",
	     info, inet_ntoa(*((struct in_addr *)(&(d->node)))),
	     d->session, d->ranID, d->fecundity,
	     tierraTime, beagleTime);
    BglStdPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
  }
}
