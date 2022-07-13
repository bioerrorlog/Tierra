/* bglstd_global.h -- -*- C -*-
 * 
 * Global value definition for stdio mode beagle
 *
 * @(#)bglstd_global.h	1.4	03/10/98 18:05:12
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Tue Mar 10 18:04:47 1998
 * Update Count    : 11
 * Status          : 
 */

/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef bglstd_global_h
#define bglstd_global_h

#ifdef  BGLSTD_GLOBAL_VALUE_DEFINE
#define STD_GLOBAL
#define STD_GLOBAL_VAL(v)  = (v)
#else
#define STD_GLOBAL extern
#define STD_GLOBAL_VAL(v)  /* */
#endif /* BGLSTD_GLOBAL_VALUE_DEFINE */

STD_GLOBAL I32s	BglC_Menu STD_GLOBAL_VAL(0);/*flag unix sigint, to do a menu*/

#endif /* bglstd_global_h */
