/* clnt_global.h -- -*- C -*-
 * 
 * @(#)clnt_global.h	1.16	11/22/99 10:27:41
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 10:27:22 1999
 * Update Count    : 32
 * Status          : 
 */

/* Global value for client beagle code                                     */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef clnt_global_h
#define clnt_global_h

#ifdef  BGL_GLOBAL_VALUE_DEFINE
#define GLOBAL
#define GLOBAL_VAL(v)  = (v)
#else
#define GLOBAL extern
#define GLOBAL_VAL(v)  /* */
#endif /* BGL_GLOBAL_VALUE_DEFINE */

#define BGL_DEFAULT_FILE		"BglDefaultFile"
#define BGL_DEFAULT_PORT		"17501"
#define	BGL_DEFAULT_MIGRCTRL_PORT	"17502"
#define BGL_DEFAULT_UNAME		"BglDefaultUname"
#define BGL_DEFAULT_PASSWD		"BglDefaultPasswd"

GLOBAL I32s		Bgl_step GLOBAL_VAL(-1);

GLOBAL double Bgl_wait0 GLOBAL_VAL( 10.0);/* Wait time for Bgl_select */
GLOBAL double Bgl_wait1 GLOBAL_VAL(  3.0);/* Wait time for polling interval */
GLOBAL double Bgl_wait2 GLOBAL_VAL(300.0);/* Timeout for Migration Tracking */
GLOBAL I32s   XDRBufMaxSize GLOBAL_VAL(200000); /* max XDR encode/decode buffer size */
GLOBAL I32s   BglOvGInstGCnt GLOBAL_VAL(0);

#endif /* clnt_global_h */
