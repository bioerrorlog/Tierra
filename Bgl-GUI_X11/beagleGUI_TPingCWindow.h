/* beagleGUI_TPingCWindow.h -- -*- C -*-
 * 
 * @(#)beagleGUI_TPingCWindow.h	1.2	05/22/98 18:51:27
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 11:45:47 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:51:13 1998
 * Update Count    : 2
 * Status          : 
 */

#ifndef beagleGUI_TPingCWindow_h
#define beagleGUI_TPingCWindow_h

#include "clnt.h"

extern void	CreateTPingCWindow	P_((Widget));
extern void	UpdateTPingCInfo	P_((void));
extern void	TPingCAreaClean		P_((void));
extern void	TPingCInfoTextDecode	P_((Widget, XExposeEvent*,
					    String*, Cardinal*));

#endif /* beagleGUI_TPingCWindow_h */
