/*
 * beagleMessage_File.h -- 
 * 
 * This work has been done at ATR HIP
 * 
 * SCCS Status     : @(#)beagleMessage_File.h	1.2	10/18/99 09:24:52
 * Author          : Marc Chaland
 * Created On      : Wed Nov 19 16:18:24 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:24:35 1999
 * Update Count    : 4
 * Status          : Under construction
 */

#ifndef beagleMessage_File_h
#define beagleMessage_File_h

#include "clnt.h"
#include "beagleMessageMgrP.h"

/*
 * BglMM_File_Save
 *	Send request for saving soup file.
 */
extern void	BglMM_File_Save		P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_File_Save	P_((void));
extern I32s	BglMM_B_File_s		P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_File_Quit
 *	Send request for tierra to quit.
 */
extern void	BglMM_File_Quit		P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_File_Quit	P_((void));
extern I32s	BglMM_B_File_Q		P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_File_SaveAndQuit
 *	Send request for saving soup file and quitting tierra.
 */
extern void	BglMM_File_SaveAndQuit	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_File_SaveAndQuit	P_((void));
extern I32s	BglMM_B_File_q		P_((void));

extern I32s	BglMM_B_File_Close	P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_File_Exit
 *	exit from beagle.
 */
extern void	BglMM_File_Exit		P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_File_Exit	P_((void));
extern void	BglMM_B_File_E		P_((void));
#endif /* B_MECHANISM */

#endif /* beagleMessage_File_h */
