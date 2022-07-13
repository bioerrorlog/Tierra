/* clnt_com.c -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      : 
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Dec 22 18:19:19 1997
 * Update Count    : 8
 * Status          : 
 */

/* Command functions of client beagle code                                 */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */
/* MemProf  labels: #111XX, in use:   */
/* BglError labels: #111XX, in use:   */

#ifndef lint
static char* sccsProgramId="@(#)clnt_com.c	1.4	12/22/97 18:19:34";
#endif /* lint */

#include "clnt.h"

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
I32s do_abort()
{
  DebugPrint( "do_abort\n" );
  return (1);
}
