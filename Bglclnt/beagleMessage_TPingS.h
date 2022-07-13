/* beagleMessage_TPingS.h -- -*- C -*-
 * 
 * @(#)beagleMessage_TPingS.h	1.2	05/22/98 19:48:39
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue May 19 09:33:56 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:48:28 1998
 * Update Count    : 4
 * Status          : 
 */

#ifndef beagleMessage_TPingS_h
#define beagleMessage_TPingS_h

#include "clnt.h"
#include "beagleMessageMgrP.h"

/*
 * BglMM_HowManyTPingS() & BglMM_PickTPingS()
 *	first returns how many TPings are into buffer. Second return value
 * of the FIFO.
 */
extern I32s		BglMM_HowManyTPingS	P_((void));
extern BglTPingSt*	BglMM_PickTPingS	P_((void));

/*  _
 * (-)BglMM_TPing_store()
 *  T	store one value into buffer. Don't use with graphical interface.
 */
extern void	BglMM_TPingS_store	P_((BglTPingSt*));

/*
 * BglMM_Misc_tSOn_need
 *	send request to put TPingS on. (menubreak included)
 */
extern I32s	BglMM_Misc_tSOn_need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_tSOn	P_((void));
#endif /* B_MECHANISM */

/*
 * BglMM_Misc_tSOff_need
 *	send request to put TPingS off. (menubreak included)
 */	
extern I32s	BglMM_Misc_tSOff_need	P_((void));
#ifdef B_MECHANISM
extern I32s	BglMM_B_Misc_tSOff	P_((void));
#endif /* B_MECHANISM */

#endif /*beagleMessage_TPingS_h*/
