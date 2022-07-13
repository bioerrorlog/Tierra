/* beagleMessage_Migr.h -- -*- C -*-
 * 
 * @(#)beagleMessage_Migr.h	1.6	12/10/97 19:10:20
 *
 * This work has been done at ATR HIP
 * 
 * Author          : Marc Chaland
 * Created On      : Fri Aug 15 17:14:39 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Dec 10 19:09:45 1997
 * Update Count    : 24
 * Status          : this module concerns migration tracking
 *                   perfectly work
 */

#ifndef beagleMessage_Migr_h
#define beagleMessage_Migr_h

#include "clnt.h"

/**************************************************
 * buffer for migration information
 */
extern I32s	BglMM_HowManyMigration	P_((void));
extern BglMigrationDataSt*	BglMM_PickMigration	P_((void));
extern void	BglMM_Migration_store	P_((BglMigrationDataSt*));


/**************************************************
 * migration control
 */
/*
 * BglMM_ChangeMigrCtrlPort
 *      Change migration port number to new short communication port number.
 *      But If Migration Tracking Mode is ON then nothing.
 *      (And Migration Data Port Number was keepd as at start time value)
 */
extern I32s	BglMM_ChangeMigrCtrlPort P_((void));


/*
 * BglMM_MigrationDataPortOpen
 *	open port for listening migration information and read IPMap for
 * migration control. return 0.
 */
extern I32s	BglMM_MigrationDataPortOpen P_((void));

/*
 * BglMM_MigrationDataCheckAndDo
 *	analyse recieved data and send acknoledgement to tierra
 */
extern void	BglMM_MigrationDataCheckAndDo P_((void));


/*
 * BglMM_MigrationOn_need
 *	initialise process manager for migration on request
 * Returns:	1 if good initialisation 0 else
 */
extern I32s	BglMM_MigrationOn_need	P_((void));
/*
 * BglMM_MigrationOff_need
 *	initialise process manager for migration off request
 * Returns:	1 if good initialisation 0 else
 */
extern I32s	BglMM_MigrationOff_need	P_((void));



#endif	/* beagleMessage_Migr_h */
