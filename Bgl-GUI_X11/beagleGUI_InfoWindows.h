/* beagleGUI_InfoWindows.h -- -*- C -*-
 *
 * @(#)beagleGUI_InfoWindows.h	1.5	05/22/98 18:10:07
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:38:52 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 18:09:44 1998
 * Update Count    : 10
 * Status          : 
 */

#ifndef beagleGUI_InfoWindow_h
#define beagleGUI_InfoWindow_h

#include "clnt.h"

extern void	InfoWindowPutTop		P_((Widget));
extern void	CreateConfirmationDialog	P_((Widget));
extern void	CreateInformationDialog		P_((Widget));
extern void	CreateWarningDialog		P_((Widget));
extern void	CreateMessageDialog		P_((Widget));
extern int	ConfirmationMessage		P_((char*));
extern int	InformationMessage		P_((char*));
extern int	WarningMessage			P_((char*));
extern int	MessageMessage			P_((char*));

#endif /* beagleGUI_InfoWindow_h */
