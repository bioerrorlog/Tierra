/* beagleMain.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Mon Aug 19 17:40:05 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Oct 14 18:31:29 1999
 * Update Count    : 89
 * Status          : 
 */

#define EXTERN
#define BGL_GLOBAL_VALUE_DEFINE 1
#include "clnt.h"
#include "clnt_fsm.h"
#include "beagleUI.h"
#include "beagleRoutines.h"

/*
 *- SPECIAL VALIABLE NAME
 *- =====================
 */

/* None */

/*
 *- HERE ARE EXTERN FUNCTION DEFINEs. ( without defined in header file )
 *- ====================================================================
 */

/* None */

/*
 *- HERE ARE GLOBAL VARIABLE (EFFECTIVE IN THIS FILE ONLY) DEFINEs.
 *- ===============================================================
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

/* None */

/*
 *- HERE ARE STATIC FUNCTIONs PROTOTYPE DEFINEs.
 *- ============================================
 *- These functions call from in this file only.
 *- So another files functions cannot search these.
 *- I want to avoid from linking trouble.
 */

static int	ReNice	P_((I32s));


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int
main(argc, argv)
     int argc;
     char** argv;
{
  I32s beagleNiceValue;
  BglSockSt s;

#ifdef SIGBLOCK
  SetSigSet();
  TieSignal(SIGPIPE, SIG_IGN);
#endif /* SIGBLOCK */

  beagleNiceValue = BglGetDefaultNiceValue();
  ReNice(beagleNiceValue);

  if ( argc >= 2 )		/* connect to tierra */
    BglClntSetup(&s, argv[1]);
  else
    BglClntSetup(&s, BGL_DEFAULT_FILE);
  BglSetupUI(&s, &argc, argv);
  BglCreateBaseFrame(&s);
  BglEventLoop(&s);

  return 0;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int
ReNice(niceValue)
     I32s niceValue;
{
#ifndef __MWERKS__
#ifndef AMIGA
#ifdef _WIN32
  if(!SetPriorityClass(GetCurrentProcess(),
		       (niceValue)?IDLE_PRIORITY_CLASS:NORMAL_PRIORITY_CLASS)){
    BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1,
	      "SetPriorityClass failed");
    return 1;
  }
#else /* _WIN32 */
  nice(niceValue);
#endif /* _WIN32 */
#endif /* AMIGA */
  tsprintf(bglStrBuf, "Beagle Nice value = %d", niceValue);
  BglPrintf(BGL_MES_SCR, BGL_MES_X, BGL_MES_Y, 0, 0, bglStrBuf);
#endif /* __MWERKS__ */

  return 0;	/* No Error */
}
