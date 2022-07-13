/* beagleMessageMgrPt.h -- -*- C -*-
 * 
 * @(#)beagleMessageMgrPt.h	1.1	03/09/98 15:58:32
 *
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Fri Mar  6 12:18:02 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Mar  9 11:00:06 1998
 * Update Count    : 3
 * Status          : 
 */

#ifndef beagleMessageMgrPt_h
#define beagleMessageMgrPt_h

enum MM_beagle_global_message_state
{MMS_UNTRUSTWORTHY = 0
 , MMS_NORMAL		/* Normal state */
 , MMS_NEED		/* User want to data */
 , MMS_REQUESTED	/* Beagle requested to Tierra */
 , MMS_CHANGED		/* New data received and change */
};
typedef enum MM_beagle_global_message_state	MM_state_t;

#endif /* beagleMessageMgrPt_h */
