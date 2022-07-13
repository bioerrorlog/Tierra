/* beagleUTIL_SockMgr.h -- -*- C -*-
 *
 * @(#)beagleUTIL_SockMgr.h	1.4	05/22/98 20:04:55
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Thu Apr  3 17:30:31 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Fri May 22 20:04:37 1998
 * Update Count    : 12
 * Status          : 
 */

#ifndef beagleUTIL_SockMgr_h
#define beagleUTIL_SockMgr_h

#include "clnt.h"

/**********************************************************************
 * Connection Buffer
 */

/*
 * UTIL_Sock_Push_Connection(sock) & UTIL_Sock_Pop_Connection
 *	Push sock into socket buffer. Pop top stack : delete top connection
 * from the stack.
 */
extern void		UTIL_Sock_Push_Connection	P_((BglSockSt *));
extern void		UTIL_Sock_Pop_Connection	P_((void));

/*
 * UTIL_Sock_Swap_Connection(i)
 *	put ith item on top of the stack and shift forwars the other data.
 */
extern void		UTIL_Sock_Swap_Connection	P_((int, I32s));

/*
 * UTIL_Sock_GetCurrent_Connection & UTIL_Sock_Get_Connection(i)
 *	return the top element of the stack or the ith element.
 */
extern BglSockSt*	UTIL_Sock_GetCurrent_Connection	P_((void));
extern BglSockSt*	UTIL_Sock_Get_Connection	P_((int));

/**********************************************************************
 * Pre-connection Buffer
 */

/*
 * UTIL_Sock_Push_PreConnection(sock) & UTIL_Sock_Pop_PreConnection
 *	push and pop data into pre-connected buffer (1 sized).
 */
extern void		UTIL_Sock_Push_PreConnection	P_((BglSockSt*));
extern BglSockSt*	UTIL_Sock_Pop_PreConnection	P_((void));

/**********************************************************************
 * Virtual Connection (6th layer)
 */

/*
 * UTIL_Sock_IsConnection_Changed
 *	return true if connection has changed.
 */
extern int	UTIL_Sock_IsConnection_Changed		P_((void));

/*
 * UTIL_Sock_Open_VirtualConnection & UTIL_Sock_Close_VirtualConnection
 *	Open and close virtual connection (6th level). This function
 * must be used once the TCP connection is accepted and tierra accepted
 * connection from the 5th level (see beagleMessage_Connect.c).
 */
extern int	UTIL_Sock_Close_VirtualConnection	P_((void));
extern int	UTIL_Sock_Open_VirtualConnection	P_((void));

#endif /* beagleUTIL_SockMgr_h */
