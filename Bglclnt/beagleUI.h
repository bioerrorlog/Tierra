/* beagleUI.h -- -*- C -*-
 *
 * @(#)beagleUI.h	1.14	03/24/99 17:42:25
 * 
 *- YOU CAN USE FOLLOWING SERVICE FUNCTIONs FOR BEAGLE GUI.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Sep 17 17:00:37 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Mar 24 17:41:58 1999
 * Update Count    : 122
 * Status          : 
 */

#ifndef beagleUI_h
#define beagleUI_h

#include "clnt.h"

extern void	BglSetupUI	P_((BglSockSt*, int*, char**));
extern void	BglCreateBaseFrame	P_((BglSockSt*));
extern void	BglEventLoop	P_((BglSockSt*));

extern void	BglUI_Exit	P_((I32s));
extern void	BglUI_Printf	P_((I32s, I32s, I32s, I32s, I32s, I8s*));
extern char*	BglUI_StrInput	P_((I8s*));
extern char*	BglUI_SecretStrInput	P_((I8s*));
extern void	BglUI_MainMenu	P_((void));
extern void	BglUI_MicroDebuggerKeyWait	P_((void));
extern void	BglUI_Connection_Changed	P_((void));
extern void	BglUI_CommunicationCondition	P_((char*));
extern void	BglUI_SizeHist_Changed		P_((void));
extern void	BglUI_GeneHist_Changed	P_((void));
extern void	BglUI_MemHist_Changed	P_((void));
extern void	BglUI_ESHist_Changed	P_((void));
extern void	BglUI_EGHist_Changed	P_((void));
extern void	BglUI_ReplyMes_Changed	P_((void));
extern void	BglUI_Stats_Changed	P_((void));
extern void	BglUI_Plan_Changed	P_((void));
extern void	BglUI_Message_Changed	P_((void));
extern void	BglUI_TPingSStatsToOn	P_((void));
extern void	BglUI_TPingCStatsToOn	P_((void));
extern void	BglUI_TPingSStatsToOff	P_((void));
extern void	BglUI_TPingCStatsToOff	P_((void));
extern void	BglUI_TPingS_Changed	P_((void));
extern void	BglUI_TPingC_Changed	P_((void));
extern void	BglUI_HistClear_Changed	P_((void));
extern void	BglUI_OvFrame_Changed	P_((void));
extern void	BglUI_Ov_Changed	P_((void));
extern void	BglUI_OvGInst_Changed	P_((void));
extern void	BglUI_OvG_Changed	P_((void));
extern void	BglUI_SizeQuery_Changed	P_((void));
extern void	BglUI_MCState_Changed	P_((void));
extern void	BglUI_MCSpy_Changed	P_((void));
extern void	BglUI_MicroDebuggerToKeypressMode	P_((void));
extern void	BglUI_MicroDebuggerToDelayMode	P_((void));
extern void	BglUI_MicroDebuggerToOffMode	P_((void));
extern void	BglUI_Var_Changed	P_((void));
extern void	BglUI_MigrationStatsToOn	P_((void));
extern void	BglUI_MigrationStatsToOff	P_((void));
extern void	BglUI_Migration_Changed	P_((void));
extern void	BglUI_SuspendLifeLoop	P_((void));
extern void	BglUI_ResumeLifeLoop	P_((void));

#endif /* beagleUI_h */
