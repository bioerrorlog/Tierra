/* bglprint.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Mar  8 18:49:20 1999
 * Update Count    : 19
 * Status          : Unknown, Use with caution!
 */
/* Printf routines of common beagle code                                   */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #104XX, in use:   */
/* BglError labels: #104XX, in use:   */


#include "bgl.h"

#ifdef BGL_SERV
#include "tbgl_prot.h"
#else
#include "beagleUI.h"
#endif /* BGL_SERV */

#ifdef BGL_CLNT
#include "clnt_prot.h"
#include "beagleUI.h"
#endif /* BGL_CLNT */


/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void
BglPrintf(scr, scr_x, scr_y, scr_a, slee, str)
     I32s scr, scr_x, scr_y, scr_a, slee;
     I8s *str;
{
#ifdef BGL_SERV
  FEPrintf(scr_x,scr_y,scr_a,slee,(char *)str);
#else /* BGL_SERV */
#ifdef SIGBLOCK
  sigset_t BglSigSaveSet;

  BglSigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
  BglUI_Printf(scr,scr_x,scr_y,scr_a,slee,str);
#ifdef SIGBLOCK
  TSigRelease(BglSigSaveSet);
#endif /* SIGBLOCK */
#endif /* BGL_SERV */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void
BglError (eno,  ex, wr, mes)
     I32s eno, ex, wr;
     I8s *mes;
{
#ifdef BGL_SERV
  FEError(eno, ex, wr, (char *)mes);
#else /* BGL_SERV */
  I8s *buf;

  buf = (I8s*)tcalloc(1, 24+strlen(mes)+1, 10401);
  tsprintf(buf, "Error(%d):%s", (int)eno, mes);
  BglPrintf(BGL_ERR_SCR, BGL_ERRMES_X, BGL_ERRMES_Y, 1, -1, buf);
  tfree(buf, 24+ strlen(mes)+1, 10401);
#endif /* BGL_SERV */

  if(ex == BGL_EXIT){
#ifdef BGL_CLNT
    BglExit(eno);	
#endif /* BGL_CLNT */
#ifdef BGL_SERV
    TBglExit(eno);	
#endif /* BGL_SERV */
  }
}
