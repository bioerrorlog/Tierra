/* clnt_fsm.h -- -*- C -*-
 *
 * @(#)clnt_fsm.h	1.10	07/24/98 11:15:11
 * 
 * Header of FSM definition for client beagle code
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri Jul 24 11:14:57 1998
 * Update Count    : 17
 * Status          : 
 */

/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef clnt_fsm_h
#define clnt_fsm_h

struct fsm_trans ttstab[] = {
  /* Now State,	Input Signal,		Next State,	Action  */
  /* ---------  -------------		-----------	------  */
  {FSINVALID,	MCANY,			FSINVALID,	do_abort}
};
#define NTRANS (sizeof(ttstab)/sizeof(ttstab[0]))

struct  data_decode ddstab[] = {
  /* BGL type,		do_BGL  */
  /* -----------	------  */
  {BGL_QUIT,		do_BGL_QUIT},
  {BGL_REPLY,		do_BGL_REPLY},
  {BGL_REPLYMES,	do_BGL_REPLYMES},
  {BGL_STATS,		do_BGL_STATS},
  {BGL_PLAN,		do_BGL_PLAN},
  {BGL_HISTCLEAR,	do_BGL_HISTCLEAR},
  {BGL_SIZHIST,		do_BGL_SIZHIST},
  {BGL_GENHIST,		do_BGL_GENHIST},
  {BGL_MEMHIST,		do_BGL_MEMHIST},
  {BGL_ESHIST,		do_BGL_ESHIST},
  {BGL_EGHIST,		do_BGL_EGHIST},
  {BGL_SIZQUERY_O,	do_BGL_SIZQUERY_O},
  {BGL_MC_STATE,	do_BGL_MC_STATE},
  {BGL_MC_SPY,		do_BGL_MC_SPY},
  {BGL_TPING_S,		do_BGL_TPING_S},
  {BGL_TPING_C,		do_BGL_TPING_C},
  {BGL_VAR,		do_BGL_VAR},
  {BGL_OVFRAME,		do_BGL_OVFRAME},
  {BGL_OV_G_INST,	do_BGL_OV_G_INST},
  {BGL_OV,		do_BGL_OV},
  {BGL_OV_G,		do_BGL_OV_G},
  {BGL_MESSAGE,		do_BGL_MESSAGE},
  {BGL_MIGRATION,	do_BGL_MIGRATION},
  {BGL_INVALID,		do_BGL_ABORT}
};
#define NDATAS (sizeof(ddstab)/sizeof(ddstab[0]))

#endif /* clnt_fsm_h */
