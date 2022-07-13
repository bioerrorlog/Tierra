/* beagleRoutines.h -- -*- C -*-
 *
 * @(#)beagleRoutines.h	1.6	05/22/98 19:52:52
 * 
 * This is a header file for beagleRoutines.c and that users files.
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Sep 17 12:16:09 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:52:08 1998
 * Update Count    : 17
 * Status          : 
 */

#ifndef beagleRoutines_h
#define beagleRoutines_h

#include "clnt.h"

extern void	BglClntSetup	P_((BglSockSt*, char *));
extern void	BglOneAction	P_((BglSockSt*));
extern void	BglExit		P_((I32s));

#endif /* beagleRoutines_h */
