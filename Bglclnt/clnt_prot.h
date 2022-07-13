/* clnt_prot.h -- -*- C -*-
 * 
 * @(#)clnt_prot.h	1.13	10/14/99 18:36:48
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Oct 14 18:35:18 1999
 * Update Count    : 47
 * Status          :
 */

/* Prototype definition for client beagle code                             */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef clnt_prot_h
#define clnt_prot_h

/* Functions in clnt_com.c */
extern I32s	do_abort		P_((void));

/* Functions in clnt_dat.c */
extern I32s	do_BGL_ABORT		P_((I8s*));
extern I32s	do_BGL_QUIT		P_((I8s*));
extern I32s	do_BGL_REPLY		P_((I8s*));
extern I32s	do_BGL_REPLYMES		P_((I8s*));
extern I32s	do_BGL_STATS		P_((I8s*));
extern I32s	do_BGL_PLAN		P_((I8s*));
extern I32s	do_BGL_HISTCLEAR	P_((I8s*));
extern I32s	do_BGL_SIZHIST		P_((I8s*));
extern I32s	do_BGL_GENHIST		P_((I8s*));
extern I32s	do_BGL_MEMHIST		P_((I8s*));
extern I32s	do_BGL_ESHIST		P_((I8s*));
extern I32s	do_BGL_EGHIST		P_((I8s*));
extern I32s	do_BGL_SIZQUERY_O	P_((I8s*));
extern I32s	do_BGL_MC_STATE		P_((I8s*));
extern I32s	do_BGL_MC_SPY		P_((I8s*));
extern I32s	do_BGL_TPING_S		P_((I8s*));
extern I32s	do_BGL_TPING_C		P_((I8s*));
extern I32s	do_BGL_VAR		P_((I8s*));
extern I32s	do_BGL_OVFRAME		P_((I8s*));
extern I32s	do_BGL_OV_G_INST	P_((I8s*));
extern I32s	do_BGL_OV		P_((I8s*));
extern I32s	do_BGL_OV_G		P_((I8s*));
extern I32s	do_BGL_MESSAGE		P_((I8s*));
extern I32s	do_BGL_MIGRATION	P_((I8s*));


/* Prototype clnt_entry.c */
extern I32s	BglGetDefaultNiceValue	P_((void));
extern I8s	BglParseArgs		P_((I8s*, BglSockSt*));
extern void	BglGetUname		P_((I8s*));
extern void	BglGetPasswd		P_((BglUserSt*));
extern I32s	BglTtyRaw		P_((I32s));
extern I32s	BglTtyReset		P_((I32s));

/* Prototype beagleRoutines.c */
extern void	BglExit			P_((I32s));

/* Prototype beagleMessageMgr.c */
extern I32s	BglMM_Send		P_((I32s, BglMesSt*));

#endif /* clnt_prot_h */
