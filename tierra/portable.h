/* @(#)portable.h       1.2 12/15/94 */
/* portable.h 30-3-98 definitions for portability */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/*
   #ifndef lint
   static char     sccsid[] = "@(#)portable.h 1.30     11/16/91";
   #endif
 */

#include "license.h"

#ifndef PORTABLE_H
#define PORTABLE_H

#if defined(__TRS6000__)
#ifndef unix
#define unix
#endif /* unix */
#endif

#ifdef __MWERKS__
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <console.h>
#define ANSI

#define Hp     *
#define Fp     *
#define Np     *
typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short I16s;             /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef int I32s;              /* 32 bit integer */
typedef unsigned int I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#define dec2s mwdec2s
#define EINVAL 22
#define windowWidth             560
#define windowHeight    400
#define displayOffset   0
#endif /* __MWERKS__ */

#ifdef __TURBOC__
#if (!defined __CONSOLE__) && (defined __WIN32__)
#define __GUI__
#endif /* no apparent constant for Windows in Borland */
#include <stdlib.h>
#include <limits.h>
#include <alloc.h>
#include <dir.h>
#include <dos.h>
#include <mem.h>
#define ANSI

#ifdef __FLAT__

#define Hp     *
#define Fp     *
#define Np     *
typedef signed char I8s;        /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef signed short I16s;      /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef signed long I32s;       /* 32 bit integer */
typedef unsigned long I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#else /* __FLAT__ */

#if (defined __GUI__) || (defined __CONSOLE__)

#define Hp     *
#define Fp     *
#define Np     *

#else /* __GUI__ */

#define Hp     huge  *
#define Fp     far   *
#define Np     near  *

#endif /* __GUI__ */

typedef signed char I8s;        /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef signed int I16s;        /* 16 bit integer */
typedef unsigned int I16u;      /* 16 bit integer */
typedef signed long I32s;       /* 32 bit integer */
typedef unsigned long I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* __FLAT__ */

#endif /* __TURBOC__ */

#ifdef MSDOS    /* always defined by Microsoft C compiler & NDPC compiler */
             /* Microsoft C sets _osmode == OS2_MODE or DOS_MODE */

#ifndef DJGPP

#define ANSI
#define Hp     huge  *
#define Fp     far   *
#define Np     near  *
typedef signed char I8s;        /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef signed int I16s;        /* 16 bit integer */
typedef unsigned int I16u;      /* 16 bit integer */
typedef signed long I32s;       /* 32 bit integer */
typedef unsigned long I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */


#endif /* DJGPP */
#endif /* MSDOS */

#ifdef _WIN32
#define ANSI
#define __i386__
#define Hp     *
#define Fp     *
#define Np     *
typedef signed char I8s;        /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef signed short I16s;      /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef signed int I32s;       /* 32 bit integer */
typedef unsigned int I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */
#endif /* _WIN32 */

#ifdef unix

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */

#ifdef __GNUC__
#define ANSI
#else
#ifdef __sgi
#define ANSI
#endif /* __sgi*/
#endif /* __GNUC__ */

#define Hp     *
#define Fp     *
#define Np     *
typedef signed char I8s;        /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef signed short I16s;      /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef signed int I32s;        /* 32 bit integer */
typedef unsigned int I32u;      /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* unix */

#ifdef __WATCOMC__

#define Hp     *
#define Fp     *
#define Np     *
typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short int I16s;         /* 16 bit integer */
typedef unsigned short int I16u;  /* 16 bit integer */
typedef long I32s;              /* 32 bit integer */
typedef unsigned long I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* __WATCOMC__ */

#ifdef DJGPP

#include <limits.h>
#include <pc.h>
#define ANSI
#define Hp     *
#define Fp     *
#define Np     *
typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short I16s;             /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef int I32s;              /* 32 bit integer */
typedef unsigned int I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* DJGPP */

#ifdef IBM3090

#define Hp     *
#define Fp     *
#define Np     *
typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short I16s;             /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef long I32s;              /* 32 bit integer */
typedef unsigned long I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* IBM3090 */

#ifdef AMIGA

#include <stdlib.h>
#include <limits.h>

#define ANSI
#define Hp     *
#define Fp     *
#define Np     *

typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short I16s;             /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef int I32s;              /* 32 bit integer */
typedef unsigned int I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* AMIGA */

#ifdef DECVAX

#include <stdlib.h>
#include <limits.h>

#define ANSI
#define Hp     *
#define Fp     *
#define Np     *

typedef char I8s;               /*  8 bit integer */
typedef unsigned char I8u;      /*  8 bit integer */
typedef short I16s;             /* 16 bit integer */
typedef unsigned short I16u;    /* 16 bit integer */
typedef int I32s;              /* 32 bit integer */
typedef unsigned int I32u;     /* 32 bit integer */
typedef unsigned int N32u;      /* 32 bit integer - net format */
typedef unsigned short N16u;    /* 16 bit integer - net format */

#endif /* DECVAX */

#define  I8U_MAX  ((I8u) (~0))
#define  I8S_MAX  ((I8s) ( I8U_MAX >> 1))
#define I16U_MAX ((I16u) (~0))
#define I16S_MAX ((I16s) (I16U_MAX >> 1))
#define I32U_MAX ((I32u) (~0))
#define I32S_MAX ((I32s) (I32U_MAX >> 1))
#define INTU_MAX  ((unsigned int)(~0))
#define INTS_MAX  ((int)(INTU_MAX>>1))

#ifdef ANSI     /* ANSI prototyping */
#define P_(A) A
#else /* non-ANSI prototyping */
#define P_(A) ()
#define const
#endif

typedef N32u             xdrwrd;
typedef I32s Ints;
typedef I32u Intu;

#endif /* #ifndef PORTABLE_H */
