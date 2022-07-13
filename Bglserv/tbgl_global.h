/* tbgl_global.h -- -*- C -*-
 * 
 * @(#)tbgl_global.h	1.21	10/18/99 09:57:00
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Feb 26 18:36:51 1997
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Oct 18 09:56:51 1999
 * Update Count    : 72
 * Status          : 
 */

/* Header of global variables for hooking the beagle code                  */
/* Tsukasa Kimezawa                                                        */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#ifndef tbgl_global_h
#define tbgl_global_h

#ifdef  BGL_GLOBAL_VALUE_DEFINE
#define GLOBAL
#define GLOBAL_VAL(v)  = (v)
#else
#define GLOBAL extern
#define GLOBAL_VAL(v)  /* */
#endif /* BGL_GLOBAL_VALUE_DEFINE */

#define BGL_READ           (I8s)'R'
#define BGL_UNREAD         (I8s)'U'
#define BGL_MASK_MES        0       /* FEMessage */
#define BGL_MASK_MICRO      1       /* MICRO Debugger */
#define BGL_MASK_INFO_S     2       /* INFO Size */
#define BGL_MASK_INFO_G     3       /* INFO Gene */
#define BGL_MASK_INFO_M     4       /* INFO Mem */
#define BGL_MASK_INFO_ES    5	    /* INFO Effic Size */
#define BGL_MASK_INFO_EG    6	    /* INFO Effic Gene */
#define BGL_MASK_OV         7       /* OV */

#define SOCK_BUF_MAX	    10	/* socket buffer size */
GLOBAL BglSockSt BglSock;	/* Global variable for connecting Beagle */  
GLOBAL BglSockSt BglSockBuf[SOCK_BUF_MAX];
GLOBAL int SockBufSize;		/* Global buffer for connecting Beagle */ 



GLOBAL SaveCodeSt	SaveBuf; /* buffer for saving one gene */
typedef struct 
{
  I32s	size;
  I32s  gi, origgi;
} BglLastCellViewedSt;
GLOBAL BglLastCellViewedSt BglLastCellViewed;
/* buffer which store the last creature that had been showed for 0verview */

GLOBAL I32s BglConnectFlg GLOBAL_VAL(0); /* Flag of if Beagle is connecting */
GLOBAL fd_set BglMask;		/* Mask if there are messages to be sent */

GLOBAL BglPlanSt		BglPlanBuf;
GLOBAL BglHistClearSt		BglHistClearBuf;
GLOBAL I32s Bgl_nnn GLOBAL_VAL(0);
GLOBAL I8s BglTblHistClear;	/* Table of read or unread BglHistClearBuf */
GLOBAL BglSizHistSt		*BglSizHistBuf;
GLOBAL I32s BglSizHistAllocSiz;		/* Allocate size of BglSizHistBuf */
GLOBAL I8s* BglTblSizHist;	/* Table of read or unread BglSizHistBuf */
GLOBAL I32s BglTblSizHistAllocSiz;	/* Allocate size of BglTblSizHist */
GLOBAL BglGenHistSt		*BglGenHistBuf;
GLOBAL I32s BglGenHistAllocSiz;		/* Allocate size of BglGenHistBuf */
GLOBAL I8s* BglTblGenHist;	/* Table of read or unread BglGenHistBuf */
GLOBAL I32s BglTblGenHistAllocSiz;	/* Allocate size of BglTblGenHist */
GLOBAL BglMemHistSt		*BglMemHistBuf;
GLOBAL I32s BglMemHistAllocSiz;		/* Allocate size of BglMemHistBuf */
GLOBAL I8s* BglTblMemHist;	/* Table of read or unread BglMemHistBuf */
GLOBAL I32s BglTblMemHistAllocSiz;	/* Allocate size of BglTblMemHist */
GLOBAL BglEfficSHistSt		*BglEfficSHistBuf;
GLOBAL I32s BglEfficSHistAllocSiz;	/* Allocate size of BglEfficSHistBuf */
GLOBAL I8s* BglTblEfficSHist;	/* Table of read or unread BglEfficSHistBuf */
GLOBAL I32s BglTblEfficSHistAllocSiz;	/* Allocate size of BglTblEfficSHist */
GLOBAL BglEfficGHistSt		*BglEfficGHistBuf;
GLOBAL I32s BglEfficGHistAllocSiz;	/* Allocate size of BglEfficGHistBuf */
GLOBAL I8s* BglTblEfficGHist;	/* Table of read or unread BglEfficGHistBuf */
GLOBAL I32s BglTblEfficGHistAllocSiz;	/* Allocate size of BglTblEfficGHist */
GLOBAL BglSizQueryOutputSt	*BglSizQueryOutputBuf;
GLOBAL BglVarSt			*BglVarBuf;
GLOBAL I32s TBglVarNumMax ; 	/* Maximum Number of BglVarBuf */
GLOBAL BglInjectGeneSt		BglInjectGeneBuf;
GLOBAL BglMCSpySt		BglMCSpyBuf;
GLOBAL BglOvFrameSt		BglOvFrameBuf;
GLOBAL BglOvGOutputSt		BglOvGOutputBuf;
GLOBAL BglOvGInstSt		*BglOvGInstBuf;
#ifdef NET
GLOBAL BglTPingSt	*BglTPingSBuf;    /* for Server (IPMapS) data */
GLOBAL I32s BglTPingSAllocSiz;	/* Allocate size of BglTPingSBuf */
GLOBAL I8s* BglTblTPingS;	/* Table of read or unread BglTPingSBuf */
GLOBAL I32s BglNumIPMapSDat GLOBAL_VAL(0);/* Number of IPMapS Data */
GLOBAL I32s TBgl_TPingS_Mode GLOBAL_VAL(0);/* Flag for TPingS send mode */
GLOBAL BglTPingSt	*BglTPingCBuf;    /* for Cluster (IPMapC) data */
GLOBAL I32s BglTPingCAllocSiz;	/* Allocate size of BglTPingCBuf */
GLOBAL I8s* BglTblTPingC;	/* Table of read or unread BglTPingCBuf */
GLOBAL I32s BglNumIPMapCDat GLOBAL_VAL(0);/* Number of IPMapC Data */
GLOBAL I32s TBgl_TPingC_Mode GLOBAL_VAL(0);/* Flag for TPingC send mode */
GLOBAL BglMigrationConnectSt	BglMigrationConnectBuf;
GLOBAL BglMigrationDataSt	BglMigrationDataBuf;
#endif /* NET */
GLOBAL BrkPointSt		BglBrkPointBuf;


GLOBAL I32s TBglNextFlg GLOBAL_VAL(0); 	/* Flag for break the MC debugger */
GLOBAL I32s TBglNextBrkFlg GLOBAL_VAL(0); 	/* Flag for break the MC debugger */
GLOBAL I32s TBglOneCellContFlg GLOBAL_VAL(1);/* Flag for continue from pause */
GLOBAL I32s TBglSuspendLifeFlg GLOBAL_VAL(0);/* Flag for Suspend LIFE LOOP */

GLOBAL I32s BglMigrCtrlSockFd;


#undef GLOBAL
#undef GLOBAL_VAL

#endif /* tbgl_global */
