/* @(#)configur.h       1.1 11/15/94 */
/* configur.h 30-3-98 configurable parameters at compile time */
/* Tierra Simulator V6.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/*
   #ifndef lint
   static char     sccsid[] = "@(#)configur.h 1.5     7/21/92";
   #endif
 */

#ifdef DJGPP
#undef unix
#endif /* DJGPP */

#include "license.h"

#ifndef CONFIGUR_H
#define CONFIGUR_H
#define VER 6.02

#define MICRO   /* define for micro step debugging */
#define IO      /* define for buffered input-output */
/* #define NET       *//* define for network version of tierra */
/* #define SHADOW */
/* #define READPROT */ /* define to implement read protection of soup */
#define WRITEPROT  /* define to implement write protection of soup */
/* #define EXECPROT */ /* define to implement execute protection of soup */
#define ERRORTIE     /* use to include error checking code */
/* #define MEM_PROF  *//* profile dynamic memory usage */

#ifdef NET
/* #define DYNIPADR */  /* define for dynamically assigned ip address */
#endif

#if (defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR))&&defined(unix)
#ifndef MEM_CHECKER
#define SIGBLOCK
#endif /* MEM_CHECKER */
#endif /* (defined(TIERRA)||defined(BGL_CLNT))&&defined(unix) */

#define STACK_SIZE 10
#ifdef SHADOW
#define ALOC_REG 12
#define NUMREG 6  /* NUMREG = ALOC_REG / 2 */
#else /* SHADOW */
#define ALOC_REG 6
#define NUMREG 6  /* NUMREG = ALOC_REG */
#endif /* SHADOW */

/* FRONTEND types: */

#define STDIO     0
#define BASIC     1

#ifndef FRONTEND
#define FRONTEND      STDIO
#endif /* FRONTEND */

/* OPSYS types: */

#define UNIX      0
#define BCDOS     1  /* Dos with Borland C */
#define WIN32TIE  2  /* Win95 / Win32 / MSVC 4.x */
#define DJGPPDOS  3  /* Dos with DJGPP */
#define MACTIE    4
#define AMIGADOS  5
#define DECVMS    6

#ifdef __MWERKS__
#ifdef FRONTEND
#undef FRONTEND
#endif
#define FRONTEND  BASIC
#define NET
#define OPSYS     MACTIE
#define TIESHELL "noshell"
#endif /* __MWERKS__ */

#ifdef __TURBOC__  /* applies to GUI Windows */
#define OPSYS     BCDOS
#define TIESHELL "command"
#endif /* __TURBOC__ */

#ifdef __WATCOMC__
#define OPSYS     WATDOS
#define TIESHELL "command"
#endif /* __WATCOMC__ */

#ifdef DJGPP
#define OPSYS     DJGPPDOS
#define TIESHELL "command"
#endif /* DJGPP */

#ifdef _WIN32
#define OPSYS WIN32TIE
#define TIESHELL "COMMAND"
#endif /* _WIN32 */

#ifdef unix
#define OPSYS     UNIX
#define TIESHELL "csh"  /* DAN #define TIESHELL "xterm" */
#endif /* unix */

#ifdef AMIGA
#define OPSYS     AMIGADOS
#define TIESHELL "newshell"
#endif /* AMIGA */

#ifdef DECVAX
#define OPSYS     DECVMS
#define TIESHELL "spawn"
#endif /* DECVAX */

#ifdef __GUI__  /* Windows 3.1 with WIN32s installed */
#undef FRONTEND
#define FRONTEND BASIC
#endif /* __GUI__ */

/* INTERFACE types: */

#define STDIOU  0  /* stdio frontend with unix */
#define STDIOB  1  /* stdio frontend with Borland C */
#define STDIOW  2  /* stdio frontend with Watcom C */
#define STDIOD  3  /* stdio frontend with DJGPP */
#define STDIOM  4  /* stdio frontend with MetroWerks for the Mac */
#define STDIOA  5  /* stdio frontend with Amiga */
#define STDIOV  6  /* stdio frontend with VAX */

#define BASICC  7  /* basic frontend with Curses */
#define BASICB  8  /* basic frontend with Borland C */
#define BASICW  9  /* basic frontend with Watcom C - Win32 */
#define BASICD 10  /* basic frontend with DJGPP */
#define BASICM 11  /* basic frontend with MetroWerks for the Mac */
#define BASICA 12  /* basic frontend with Amiga */
#define BASICV 13  /* basic frontend with VAX */

#ifndef INTERFACE
#define INTERFACE ((FRONTEND * 7) + OPSYS)
#endif /* INTERFACE */

#ifdef ARGTIE
#undef MEM_PROF
#endif /* ARGTIE */

/* the following definitions should be provided by the compilers,
   except for IBM3090 which must be manually defined */

/* __MWERKS__   = Metrowerks CodeWarrior for Macintosh and Power Mac */
/* __WATCOMC__  = Watcom C for any platform */
/* __TURBOC__   = Turbo C for DOS */
/* IBM3090      = IBM 3090 the compiler will not define this, do it manually */
/* unix         = for unix systems */
/* __GNUC__     = gcc unix compiler */
/* ANSI         = for ANSI environment */
/* __WIN32__    = 32-bit WIN32s installed in Windows 3.1 */

#endif /* CONFIGUR_H */
