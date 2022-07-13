/* bglstd_print.c -- -*- C -*-
 * 
 * Printf functions for stdio mode beagle
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Nov 12 11:40:43 1998
 * Update Count    : 12
 * Status          : 
 */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #132XX, in use:   */
/* BglError labels: #132XX, in use:   */

#ifndef lint
static char* sccsProgramId="@(#)bglstd_print.c	1.7	11/12/98 11:40:58";
#endif /* lint */

#include "clnt.h"

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void 
BglStdPrintf(scr, scr_x, scr_y, scr_a, slee, str)
     I32s scr;
     I32s scr_x, scr_y;
     I32s scr_a;
     I32s slee;
     I8s *str;
{
  if(scr == BGL_ERR_SCR) {
    tfprintf(stderr,"%s\n",str);
    tfflush(stderr);
  }
  else if(scr == BGL_ROOT_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_MENU_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_MES_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_DIALOG_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_STAT_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_PLN_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INFOP_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INFOS_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INFOG_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INFOM_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INFOZ_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_MC_STATE_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_MC_SPY_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_HIST_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_VARA_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_VARE_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_OV_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_GENE_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_TPING_S_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_TPING_C_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_INJT_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_OV_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_GENE_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
  else if(scr == BGL_OPT_SCR) {
    tfprintf(stdout,"%s\n",str);
    tfflush(stdout);
  }
}
