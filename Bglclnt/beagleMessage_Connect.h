/*
 * beagleMessage_Connect.h -- 
 * 
 * This work has been done at ATR HIP
 * 
 * SCCS Status     : @(#)beagleMessage_Connect.h	1.7	10/18/99 09:23:50
 * Author          : Marc Chaland
 * Created On      : Wed Nov 19 12:24:45 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:22:42 1999
 * Update Count    : 11
 * Status          : Under construction
 */

#ifndef beagleMessage_Connect_h
#define beagleMessage_Connect_h

#include "clnt.h"

/**********************************************************************
 * Connection protocol functions (5th level)
 */

/*
 * BglMM_Connect_Open(connection, blk) & BglMM_Connect_ReOpen(i)
 *	open a new connection or the ith connection into the connection 
 * buffer. if blk is true, blocking mechanism is used.
 */
extern I32s	BglMM_Connect_Open	P_((char*, int));
extern void	BglMM_Connect_ReOpen	P_((int));

/*
 * BglMM_Connect_Close
 *	Close current connection and reconnect to a former connection
 */
extern void	BglMM_Connect_Close	P_((void));

extern void	BglMM_CloseBglSockFD	P_((I32s));

#endif /* beagleMessage_Connect_h */
