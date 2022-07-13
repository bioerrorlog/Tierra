/* bglstd_show.h -- -*- C -*-
 * 
 * @(#)bglstd_show.h	1.8	05/22/98 19:15:16
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Sep 18 10:38:57 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:14:56 1998
 * Update Count    : 47
 * Status          : 
 */


#ifndef bglstd_show_h
#define bglstd_show_h

#include "clnt.h"

void	BglStdShowReplyMes	P_((void));
void	BglStdShowMessage	P_((void));
void	BglStdShowStats	P_((void));
void	BglStdShowPlan	P_((void));
void	BglStdShowMCState	P_((void));
void	BglStdShowMCSpy	P_((void));

void	BglStdShowTPingSStatsToOn	P_((void));
void	BglStdShowTPingCStatsToOn	P_((void));
void	BglStdShowTPingSStatsToOff	P_((void));
void	BglStdShowTPingCStatsToOff	P_((void));
void	BglStdShowTPingS	P_((void));
void	BglStdShowTPingC	P_((void));
void	BglStdShowVar	P_((void));
void	BglStdShowHistClear	P_((void));
void	BglStdShowSizHist	P_((void));
void	BglStdShowGenHist	P_((void));
void	BglStdShowMemHist	P_((void));
void	BglStdShowESHist	P_((void));
void	BglStdShowEGHist	P_((void));
void	BglStdShowSizQuery	P_((void));
void	BglStdShowOv	P_((void));
void	BglStdShowOvFrame	P_((void));
void	BglStdShowOvGInst	P_((void));
void	BglStdShowOvG	P_((void));
void	BglStdShowMicroDebuggerToKeypressMode	P_((void));
void	BglStdShowMicroDebuggerToDelayMode	P_((void));
void	BglStdShowMicroDebuggerToOffMode	P_((void));
void	BglStdShowMigrationTracking	P_((void));

#endif /* bglstd_show_h */
