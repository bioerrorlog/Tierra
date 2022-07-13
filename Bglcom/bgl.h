/* bgl.h -- -*- C -*-
 * 
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Jul 23 19:07:40 1998
 * Update Count    : 26
 * Status          : 
 */
/* bgl.h  header for common beagle code                                    */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */


#ifndef bgl_h
#define bgl_h

#include "tierra.h"
#include "globals.h"

#ifdef TBSD
#include <sgtty.h>
#endif

#ifdef TSysV
#include <termio.h>
#endif

#include "bgl_define.h"
#include "bgl_data_code.h"
#include "bgl_dat_xdr.h"
#include "bgl_global.h"
#include "bgl_prot.h"

#include "bglprint.h"

#endif /* bgl_h */
