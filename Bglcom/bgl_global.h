/* bgl_global.h -- -*- C -*-
 * 
 * @(#)bgl_global.h	1.19	11/22/99 10:32:59
 *
 * Global value for common beagle code
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 10:32:42 1999
 * Update Count    : 20
 * Status          : 
 */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef bgl_global_h
#define bgl_global_h

#ifdef  BGL_GLOBAL_VALUE_DEFINE
#define GLOBAL
#define GLOBAL_VAL(v)  = (v)
#else
#define GLOBAL extern
#define GLOBAL_VAL(v)  /* */
#endif /* BGL_GLOBAL_VALUE_DEFINE */

GLOBAL I32s SendCount GLOBAL_VAL(0);
GLOBAL I32s RecvCount GLOBAL_VAL(0);

#ifndef BGL_GLOBAL_VALUE_DEFINE
extern struct fsm_trans ttstab[];
extern struct data_decode ddstab[];
#endif /* BGL_GLOBAL_VALUE_DEFINE */

GLOBAL I32u ttfsm[NCHRS][NCHRS];
GLOBAL I32s ttstate;
GLOBAL I32u dfsm[NCHRS];
GLOBAL I8s bglStrBuf[BGL_STRBUF_SIZ];
GLOBAL I8s bglStrBuf2[180];

/* Buffer for BglDataSt */
GLOBAL BglQuitSt		BglQuitBuf;
GLOBAL BglMCStateSt		BglMCStateBuf;
GLOBAL BglReplySt		BglReplyBuf;
GLOBAL BglReplyMesSt		BglReplyMesBuf;

#undef GLOBAL
#undef GLOBAL_VAL

#endif /* bgl_global_h */

