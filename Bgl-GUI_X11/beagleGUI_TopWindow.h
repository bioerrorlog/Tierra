/* beagleGUI_TopWindow.h -- -*- C -*-
 *
 * @(#)beagleGUI_TopWindow.h	1.12	07/27/98 14:51:29
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 13:32:26 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Jul 27 14:50:57 1998
 * Update Count    : 32
 * Status          : 
 */

#ifndef beagleGUI_TopWindow_h
#define beagleGUI_TopWindow_h

#include "clnt.h"

extern void	CreateTopWindow		P_((Widget));
extern void	TopDisplayNOVChange	P_((int));
extern void	TopDisplayConnectionChange	P_((void));
extern void	TopDisplayCondition		P_((char*));
extern void	TopDisplayTPingSStatsToOn	P_((void));
extern void	TopDisplayTPingCStatsToOn	P_((void));
extern void	TopDisplayTPingSStatsToOff	P_((void));
extern void	TopDisplayTPingCStatsToOff	P_((void));
extern void	TopDisplayMigrationStatsToOn	P_((void));
extern void	TopDisplayMigrationStatsToOff	P_((void));
extern void	TopDisplayMCStateChange		P_((void));
extern void	TopDisplayMicroDebuggerToKeypressMode	P_((void));
extern void	TopDisplayMicroDebuggerToDelayMode	P_((void));
extern void	TopDisplayMicroDebuggerToOffMode	P_((void));
extern void	TopDisplayConnectionPrint	P_((char *));
extern void	TopDisplaySuspendLifeLoop	P_((void));
extern void	TopDisplayResumeLifeLoop	P_((void));

#endif /* beagleGUI_TopWindow_h */

