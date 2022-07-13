/* beagleUTIL_MigrMgr.h -- -*- C -*-
 *
 * @(#)beagleUTIL_MigrMgr.h	1.13	03/08/99 18:44:00
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Aug  1 11:14:19 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Mar  8 18:43:15 1999
 * Update Count    : 34
 * Status          : 
 *             Migration utility
 */

#ifndef beagleUTIL_MigrMgr_h
#define beagleUTIL_MigrMgr_h

#include "clnt.h"

extern N32u UTIL_MigrMgr_LastNode;


/*********************************************************************************/
/* timeout control
 */

/* put the LastMigrDataIn to the current time */
extern void UTIL_MigrMgr_TimeoutInit	P_((void));

/* compare LastMigrDataIn + Timeout with current time */
extern int  UTIL_MigrMgr_TimeoutChek	P_((void));


/*********************************************************************************/
/* session control
 */

/* give a new number session */
extern I32s UTIL_MigrMgr_NewSession	P_((void));

/* get the session number of the last migration data in */
extern void UTIL_MigrMgr_Checksession	P_((I32s));

/* compare CurrentSession with LastReceivedSession */
extern int  UTIL_MigrMgr_ValidMigrSession	P_((void));

/*********************************************************************************/
/* sleeping and apocalypse control
 */

/* put the stopping node to 0 */
extern void UTIL_MigrMgr_TrackingStopReset	P_((void));

/* store the node of the last traking stop request */
extern void UTIL_MigrMgr_TrackingStopRequest	P_((void));

/* say if a stoping request have been done and forget it*/
extern int UTIL_MigrMgr_StopRequestCheck	P_((void));



/*********************************************************************************/
/* migration control
 */

/* put parameter and initialize the buffer */
void UTIL_MigrMgr_PutParameter	P_((I8s parameter[8]));

/* return the value of a node number for controling tracking */
extern N32u UTIL_MigrMgr_BuffCtrlRead	P_((I8s localparam[8]));

/* Initialize the value of the migration control port */
void UTIL_MigrMgr_CtrlPortInit	P_((char *));

/* return the value of the migration control port */
char *UTIL_MigrMgr_CtrlPortGet	P_((void));

/* function to write datas into a file */
int UTIL_MigrMgr_Write	P_((BglMigrationDataSt*));

/* close the last file */
void UTIL_MigrMgr_Close	P_((void));


/*********************************************************************************/
/* Migration port
 */

/* Initialize the value of the migration port */
void UTIL_MigrMgr_PortChange	P_((void));

/* return the value of the migration port */
char *UTIL_MigrMgr_PortGet	P_((void));

#endif	/* beagleUTIL_MigrMgr_h */
