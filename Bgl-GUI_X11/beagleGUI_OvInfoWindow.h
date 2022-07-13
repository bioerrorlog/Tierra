/* beagleGUI_OvInfoWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_OvInfoWindow.h	1.2	05/22/98 18:25:22
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:45:51 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:25:06 1998
 * Update Count    : 12
 * Status          : 
 */

#ifndef beagleGUI_OvInfoWindow_h
#define beagleGUI_OvInfoWindow_h

#include "clnt.h"

extern void	CreateOvInfoWindow	P_((Widget));
extern void	RedrawOvInfoArea	P_((Widget, XExposeEvent*,
					    String*,Cardinal*));

#endif /* beagleGUI_OvInfoWindow_h */
