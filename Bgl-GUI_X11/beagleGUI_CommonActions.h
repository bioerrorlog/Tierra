/* beagleGUI_CommonActions.h -- -*- C -*-
 *
 * @(#)beagleGUI_CommonActions.h	1.3	05/22/98 17:01:03
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Oct 17 17:15:06 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 17:00:48 1998
 * Update Count    : 6
 * Status          : 
 */

#ifndef	beagleGUI_CommonActions_h
#define	beagleGUI_CommonActions_h

#include "clnt.h"

/********************Actions for cascade menus****************************/
extern void	PositionCascPullDownMenu	P_((Widget, XEvent*, String*,
						    Cardinal*));
extern void	CascMenuPopup	P_((Widget, XEvent*, String*, Cardinal*));
extern void	CascMenuEnter	P_((Widget, XEvent*, String*,Cardinal*));
extern void	AllCascMenuPopdown	P_((Widget, XEvent*, String*,
					    Cardinal*));
/********************Actions for cascade menus****************************/

#endif /* beagleGUI_CommonActions_h */
