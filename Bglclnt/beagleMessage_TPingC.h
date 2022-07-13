/* beagleMessage_TPingC.h -- -*- C -*-
 * 
 * @(#)beagleMessage_TPingC.h	1.2	05/22/98 19:45:54
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 09:34:46 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:45:38 1998
 * Update Count    : 4
 * Status          : 
 */

#ifndef beagleMessage_TPingC_h
#define beagleMessage_TPingC_h

#include "clnt.h"
#include "beagleMessageMgrP.h"

/*
 * BglMM_HowManyTPingC() & BglMM_PickTPingC()
 *	first returns how many TPings are into buffer. Second return value
 * of the FIFO.
 */
extern I32s		BglMM_HowManyTPingC	P_((void));
extern BglTPingSt*	BglMM_PickTPingC	P_((void));

/*  _
 * (-)BglMM_TPingC_store()
 *  T	store one value into buffer. Don't use with graphical interface.
 */
extern void	BglMM_TPingC_store	P_((BglTPingSt*));

/*
 * BglMM_Misc_tCOn_need
 *	send request to put TPingC on. (menubreak included)
 */
extern I32s	BglMM_Misc_tCOn_need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_tCOn	P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_Misc_tCOff_need
 *	send request to put TPingC off. (menubreak included)
 */	
extern I32s	BglMM_Misc_tCOff_need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_tCOff	P_((void));
#endif /* B_MECHANISM */

#endif /*beagleMessage_TPingC_h*/
