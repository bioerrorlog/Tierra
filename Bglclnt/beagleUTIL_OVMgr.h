/* beagleUTIL_OVMgr.h -- -*- C -*-
 *
 * @(#)beagleUTIL_OVMgr.h	1.3	03/18/99 18:09:15
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Tue Dec 24 11:15:22 1996
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Thu Mar 18 18:09:02 1999
 * Update Count    : 13
 * Status          : 
 */

#ifndef beagleUTIL_OVMgr_h
#define beagleUTIL_OVMgr_h

#include "clnt.h"

struct OVDATA_T{
  struct OVDATA_T*	next;
  struct OVDATA_T*	prev;
  I32u start;
  I32u length;
};
typedef struct OVDATA_T	OvData_t;


extern int	UTIL_Ov_AddData	P_((I32u, I32u));
extern int	UTIL_Ov_DelThis	P_((OvData_t*));
extern OvData_t* UTIL_Ov_SearchByStart	P_((I32u));
extern OvData_t* UTIL_Ov_SearchByLength	P_((I32u));
extern OvData_t* UTIL_Ov_SearchByPoint	P_((I32u));
extern int	UTIL_Ov_ClearAll	P_((void));
extern OvData_t* UTIL_Ov_GetRoot	P_((void));


#endif /* beagleUTIL_OVMgr_h */
