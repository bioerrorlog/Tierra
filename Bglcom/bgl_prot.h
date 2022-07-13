/* bgl_prot.h -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : Tsukasa Kimezawa
 * Created On      :
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Wed Nov 11 13:52:27 1998
 * Update Count    : 67
 * Status          : 
 */
/* @(#)bgl_prot.h	1.14	05/10/99 15:57:38 */
/* Prototype definition for common beagle code                             */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef bgl_prot_h
#define bgl_prot_h

/* Prototype bglcode.c */
extern I32s fsmbuild		P_((void));
extern I32s ddbuild		P_((void));
extern I32s BglMakeMesData	P_((const I32s, const I8s*, const I32s, I8s*));
extern I32s BglMakeMes		P_((const I32s, const I8s*, const I32s, 
				    BglMesSt*));
extern void BglComDecode	P_((const I8s*, const I32s));
extern void BglDatDecode	P_((I8s*, const I32u));

/* Prototype bglnet.c */
extern I32s BglSend		P_((const I32s, const BglMesSt*));
extern I32s BglRecv		P_((const I32s));
extern I32s BglReplyAck		P_((const I32s, const I32s));
extern I32s BglReplyMes		P_((const I32s, const I8s*));
extern I32s BglReplyWait	P_((const I32s, const I32s, const fd_set*,
				    const I32s, const I32s, const double));

/* Prototype bglprint.c */
extern void BglPrintf		P_((I32s, I32s, I32s, I32s, I32s, I8s*));
extern void BglError		P_((I32s, I32s, I32s, I8s*));

/* Prototype bgltcp.c */
extern I32s bgl_connectTCP	P_((I8s*, I8s*));
extern I32s bgl_connectUDP	P_((I8s*, I8s*));
extern I32s bgl_passiveTCP	P_((I8s*, I32s));
extern I32s bgl_open_max	P_((void));
extern I32s bgl_select		P_((const I32s, fd_set*, const double));
extern I32s bglwrite		P_((const I32s, I8s*, const I32s));
extern I32s bglread		P_((const I32s, const I32s, I8s*, const I32s));

#endif /* bgl_prot_h */
