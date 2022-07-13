/*
 * beagleMessage_Misc.h -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * SCCS Status     : @(#)beagleMessage_Misc.h	1.4	10/18/99 09:50:28
 * Author          : Marc Chaland
 * Created On      : Wed Oct 29 16:01:55 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:50:16 1999
 * Update Count    : 14
 * Status          : Unknown, Use with caution!
 */

#ifndef beagleMessage_Misc_h
#define beagleMessage_Misc_h

#include "clnt.h"
#include "beagleMessageMgrP.h"

/*
 * BglMM_HowManySizeQuery() & BglMM_PickSizeQuery()
 *	first returns how many size queries are into buffer. Second return value
 * of the FIFO.
 */
extern I32s			BglMM_HowManySizeQuery	P_((void));
extern BglSizQueryOutputSt*	BglMM_PickSizeQuery	P_((void));
/*  _
 * (-)BglMM_SizQuery_store()
 *  T	store one value into buffer. Don't use with graphical interface.
 */
extern void	BglMM_SizQuery_store(BglSizQueryOutputSt* d);

/*
 * BglMM_SizeQuery_Need(size)
 *	Send request size query on size size. (with menubreak)
 */
extern void	BglMM_SizeQuery_Need	P_((I8s*));
#ifdef B_MECHANISM
extern I32s	BglMM_B_SizeQuery	P_((I8s*));
extern I32s	BglMM_B_SizeQuery_need	P_((void));
extern void	BglMM_B_ReqSizeQuery_Need	P_((char*));
#endif /* B_MECHANISM */


/*
 * BglMM_Misc_h_need
 *	Send request hist. logging. (with menubreak)
 */
extern I32s	BglMM_Misc_h_need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_h		P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_Inject_Need(name, num, address)
 *	Send request inject num gene(s) name at address. (with menubreak)
 */
extern void	BglMM_Inject_Need	P_((I8s*,I8s*,I8s*));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_Inject	P_((I8s*,I8s*,I8s*));
extern I32s	BglMM_B_Misc_i		P_((void));
extern I32s	BglMM_B_Misc_ie		P_((I8s*,I8s*,I8s*));
#endif /* B_MECHANISM */

/*
 * BglMM_Suspend_Need()
 *	Send request Suspend Tierra Life Loop.
 */
extern void	BglMM_Suspend_Need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_Suspend	P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_Resume_Need()
 *	Send request Resume Tierra Life Loop.
 */
extern void	BglMM_Resume_Need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_Resume	P_((void));
#endif /* B_MECHANISM */

#endif /* beagleMessage_Misc_h */
