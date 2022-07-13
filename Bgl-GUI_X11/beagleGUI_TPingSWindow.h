/* beagleGUI_TPingSWindow.h -- -*- C -*-
 * 
 * @(#)beagleGUI_TPingSWindow.h	1.2	05/22/98 18:54:17
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 11:43:53 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:53:58 1998
 * Update Count    : 2
 * Status          : 
 */

#ifndef beagleGUI_TPingSWindow_h
#define beagleGUI_TPingSWindow_h

#include "clnt.h"

extern void	CreateTPingSWindow	P_((Widget));
extern void	UpdateTPingSInfo	P_((void));
extern void	TPingSAreaClean		P_((void));
extern void	TPingSInfoTextDecode	P_((Widget, XExposeEvent*,
					    String*, Cardinal*));

#endif /* beagleGUI_TPingSWindow_h */
