/* beagleGUI_MessageWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_MessageWindow.h	1.2	05/22/98 18:16:33
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:43:31 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:16:20 1998
 * Update Count    : 13
 * Status          : 
 */

#ifndef beagleGUI_MessageWindow_h
#define beagleGUI_MessageWindow_h

#include "clnt.h"

extern void	CreateMessageWindow	P_((Widget));
extern void	UpdateMessageData	P_((void));
extern void	UpdateRepMessageData	P_((void));

#endif /* beagleGUI_MessageWindow_h */
