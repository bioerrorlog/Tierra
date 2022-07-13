/* beagleUTIL_Convert.h -- -*- C -*-
 * 
 * @(#)beagleUTIL_Convert.h	1.4	05/10/99 15:56:21
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Nov 21 15:31:46 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 19:55:30 1998
 * Update Count    : 10
 * Status          : 
 */


#ifndef beagleUTIL_Convert_h
#define beagleUTIL_Convert_h

#include "clnt.h"

typedef struct in_addr	ip_t;
  
extern int	UTIL_Convert_Initialize	P_((void));
extern char*	UTIL_Convert_IpToName	P_((ip_t*));
extern ip_t*	UTIL_Convert_NameToIp	P_((char*));

#endif /* beagleUTIL_Convert_h */
