/* bglstd_prot.h -- -*- C -*-
 * 
 * Prototype definition for stdio mode beagle
 * 
 * @(#)bglstd_prot.h	1.8	05/14/99 23:32:58
 *
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:11:30 1998
 * Update Count    : 44
 * Status          : 
 */

/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

/* bglstdprot.h */
#ifndef SLS_bglstd_prot_h
#define SLS_bglstd_prot_h

#include "clnt.h"

/* Prototype bgl_print.c */
extern void BglStdPrintf	P_((I32s, I32s, I32s, I32s, I32s, I8s*));

/* Prototype bglstd_proc.c */
extern void BglStdEventLoop	P_((BglSockSt*));
extern void BglStdSetup		P_((BglSockSt*));

#ifdef __TURBOC__
#if FRONTEND == STDIO
extern int Bgl_sig_int P_((void));
#endif /* FRONTEND == STDIO */
#endif /* __TURBOC__ */
#ifdef __MWERKS__
extern void Bgl_sig_int P_((I32s sig));
#endif /* __MWERKS__ */
#ifdef unix
extern void Bgl_sig_int P_((int sig));
#endif /* unix */
#ifdef DECVAX
extern void Bgl_sig_int P_((int));
#endif /* DECVAX */

/* Prototype bglstd_menu.c */
extern void BglStdMainMenu	P_((void));
extern void BglStdMisc_mk	P_((void));

#endif /* SLS_bglstd_prot_h */

