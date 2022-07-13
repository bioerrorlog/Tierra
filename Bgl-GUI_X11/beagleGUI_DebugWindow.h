/* beagleGUI_DebugWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_DebugWindow.h	1.6	11/30/98 14:41:48
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:48:41 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 30 14:41:29 1998
 * Update Count    : 18
 * Status          : 
 */

#ifndef beagleGUI_DebugWindow_h
#define beagleGUI_DebugWindow_h

#include "clnt.h"

extern void	CreateDebugWindow	P_((Widget));
extern void	UpdateDebugInfo		P_((void));
extern void	DebugWindowResetMapCode	P_((void));

#endif /* beagleGUI_DebugWindow_h */
