/* bgl_dat_xdr.x -- -*- C -*-
 * 
 * This work has been done at ATR HIP
 * 
 * Author          : YOSHIKAWA Tooru
 * Created On      : Wed Jul  1 11:30:02 1998
 * Last Modified By: YOSHIKAWA Tooru
 * Last Modified On: Mon Nov 22 16:34:43 1999
 * Update Count    : 48
 * Status          : 
 */
/* Copyright (c) : ATR Human Information Processing Research Laboratories  */

#undef unix
#ifdef RPC_HDR
%#ifndef bgl_dat_xdr_h
%#define bgl_dat_xdr_h
%#ifdef DECVAX
%#include "multinet_root:[multinet.include.sys]types.h"
%#else /* DECVAX */
%#include <sys/types.h>
%#endif /* DECVAX */
%#ifdef _WIN32
%#include <winsock.h>
%#endif /* _WIN32 */
%#include "portable.h"
%#if defined(unix)||defined(amiga)
%#include <rpc/xdr.h>
%#else /* defined(unix)||defined(amiga) */
%#include "xdr.h"
%#endif /* defined(unix)||defined(amiga) */
#endif /* RPC_HDR */

#ifdef RPC_XDR
%#include "port_xdr.h"
#endif /* RPC_XDR */

%#include "tierra.h"
%#include "bgl_define.h"


struct BglConnectSt { /* data structure for Connection */
  I8s	host[64];
  I8s	dotted[64];
  I8s	ver[BGL_MAX_VERSION_LEN];  	/* Version # of this monitor */
  I8s	mode;               		/* Whether Su or Not su Monitor */
  I8s	name[BGL_MAX_USERNAME_LEN];	/* user name */
  I8s	passwd[BGL_MAX_PASSWD_LEN];	/* pass word */
};


struct BglQuitSt {
  I32s	state;
};


struct BglMessageSt {        /* data structure for FEMessage */
  I8s	message[BGL_MAX_MESSAGE_LEN];
};


struct BglMCStateSt {
  I8s	MC_step;
  I8s	Slice;
  I8s	Cpu;
};


struct BglStatsSt {          /* data structure for FEStats */
  I32s	GeneBnker;
  I32s	InstExe_m;
  I32s	InstExe_i;
  I32s	NumCells;
  I32s	NumGenotypes;
  I32s	NumSizes;
  I8s	ExtrG_gen[BGL_EXTR_G_LEN];
};


struct BglPlanSt {		/* data structure for FEPlan */
  I32s	GeneBnker;		/* Element #0 */
  I32s	InstExe_m;		/* Element #1 */
  double	Generations;	/* Element #2 */
  I32s	tp;			/* Element #3 */
  /* I8s	date[32]; */
  I32s	NumCells;		/* Element #4 */
  I32s	NumGenotypes;		/* Element #5 */
  I32s	NumSizes;		/* Element #6 */
  I32s	Speed;			/* Element #7 */
  I32s	RateMut;		/* Element #8 */
  I32s	RateMovMut;		/* Element #9 */
  I32s	RateFlaw;		/* Element #10 */
  I32s	AverageSize;		/* Element #11 */
  double	AvgPop;		/* Element #12 */
  double	FecundityAvg;	/* Element #13 */
  double	RepInstEff;	/* Element #14 */
  I32s	FEBirthNum;		/* Element #15 */
  I32s	FEBirthLocal;		/* Element #16 */
  I32s	FEBirthInject;		/* Element #17 */
  I32s	FEBirthEject;		/* Element #18 */
  I32s	FEDeathNum;		/* Element #19 */
  I32s	FEFecundYesSum;		/* Element #20 */
  I32s	FEFecunZeroSum;		/* Element #21 */
  I32s	FEFecunEjecSum;		/* Element #22 */
  I32s	MaxPop;			/* Element #23 */
  I32s	MaxGenPop_size;		/* Element #24 */
  I8s	MaxGenPop_label[4];	/* Element #25 */
  I32s	MaxMemry;		/* Element #26 */
  I32s	MaxGenMem_size;		/* Element #27 */
  I8s	MaxGenMem_label[4];	/* Element #28 */
  I32s  NumGenDG;               /* Element #29 */
  I8s	NetFlg;			/* Element #30 */
  I32u  NumNodes;               /* Element #31 */
  I32s	FESImmigraNum;		/* Element #32 */
  I32s	FERImmigraNum;		/* Element #33 */
  I32s	FEEjectToSelf;		/* Element #34 */
  I32s	FESPingReqNum;		/* Element #35 */
  I32s	FESPingRepNum;		/* Element #36 */
  I32s	FESSendAddNum;		/* Element #37 */
  I32s	FESByeNum;		/* Element #38 */
  I32s	FERPingReqNum;		/* Element #39 */
  I32s	FERPingRepNum;		/* Element #40 */
  I32s	FERSendAddNum;		/* Element #41 */
  I32s	FERByeNum;		/* Element #42 */
  I8s	MemProFlg;		/* Element #43 */
  I32s	DynMemUse;		/* Element #44 */
  I32s	TotNumAloc;		/* Element #45 */
}; 


struct BglHistClearSt {	/* Flag for Histogram clear information */
  I8s	flg;
};


struct BglSizHistSt {	/* data structure for size_histogram */
  I32s	index;
  I32u  size;
  I32s	num;
  I32s	count;
};


struct BglGenHistSt {	/* data structure for gen_histogram */
  I32s	index;
  I32u  size;
  I8s	lbl[4];
  I32s	count;
};


struct BglMemHistSt{	/* data structure for mem_histogram */
  I32s	index;
  I32u  size;
  I32s	num;
  I32s	count;
};


struct BglEfficSHistSt{	/* data structure for effic(size)_histogram */
  I32s	index;
  I32u  size;
  I32s	num;
  double	dblcount;
};


struct BglEfficGHistSt{	/* data structure for effic(gene)_histogram */
  I32s	index;
  I32u  size;
  I8s	lbl[4];
  double	dblcount;
};


struct BglSizQueryOutputSt{	/* data structure for size_query output */
  I32s	count;
  I8s	lbl[4];
  I32s	moves;
  I32s	flags;
  I32s	bits;
  I32s	csS;
  I8s	WatchBits[BGL_MAX_WATCH_BITS];
};


struct BglVarSt{
  I32s	index;
  I8s	type;
  I8s	name[BGL_MAX_VAR_LEN];
  I8s	value[BGL_MAX_VAR_LEN];
};


struct BglInjectGeneSt{
  I8s	gene[8];
  I32s	num;
  I32s	addr;
};


struct BglEventSt {  /* structure for time measured in executed instructions */
  I32s	m;     /* count of millions of instructions */
  I32s	i;     /* count of instructions */
};


struct BrkPointSt
{
  I8s		type;
  I8s		gene[BGL_EXTR_G_LEN];
  I8s		parasite[BGL_EXTR_G_LEN];
  I32s		offset;
  BglEventSt	time;
};


struct BglTPingSt {
  I8s	mode;	/* b = birth, d = death, a = alter, n = not change */
  I32s	id;
  N32u	node;
  N16u	portnb;
  I32s	FecundityAvg;
  I32s	Speed;
  I32s	NumCells;
  I32s	AgeAvg;
  I32s	SoupSize;
  I32u	TransitTime;
  I32u	Time;
  I32s	InstExec;
  I32s	InstExecConnect;
  I32s  OS;
};


struct BglOvFrameSt {
  I32s	InstNum;
  I32s	SoupSize;
  I32s	NumCells;
  I32s	NumSizes;
  I32s	NumGenotypes;
};


struct  BglOvGInstSt {
  I32u  op;
  I8s	mn[BGL_OV_G_INST_LEN];
};


struct BglOvMesSt {
  I8s	event;
  I32u	start;
  I32u	length;
};


struct BglReplySt {
  I32s	ack;
};


struct BglReplyMesSt {
  I8s	mes[BGL_MAX_MESSAGE_LEN];
};


struct BglMigrationConnectSt {
  N32u	node;		/* Beagle's machine node */
  N16u	portnb;		/* Migration data port number */
  I32s  session;	/* Migration number of session */
  I8s	param[8];	/* parameter for migration tracking */
};


struct BglMigrationDataSt {
  N32u	node;		/* Tierra side node */
  I8s	localparameter[8];	/* name of the current cell */
  I32s	storelparam;	/* if true, Beagle store this name into buffer */
  I32s  session;	/* session number */
  I8s	information;	/* info on the event : B (Birth) D (Death) 
			 * R (Remote Birth) M (migration) I (Immigration)
			 * A (Apocalypse) S (Sleeping Node)
			 * n (normal Termination) */
  I32s	ranID;		/* ID of the cell on the network */
  I32s	tierraTime;	/* time of tierra when request was sent */
  I32s	beagleTime;	/* time of beagle when the request was received */
  I32s	fecundity;	/* fecundity of the cell on the machine */
};


struct SaveCodeSt
{
  I8s	filename[BGL_MAX_LETTERS]; /* file name */
  I8s	mode;		/* Disassemble mode. 
			 * 1 for most recent thrd analysis data trapped,
			 * other for another mode */
  I8s	curorig;	/* 'o' for origin, 'c' for current genotype */
  I8s	tarpt;		/* true if we want to save thread analysis */
  I8s	detailrpt;	/* true if we want to save more detail data */
};


struct BglOvGOutputSt {
  I8s	comment[BGL_OV_G_COMMENT_LEN];
  I8s	type;
  I32u	start;
  I8s	label[4];
  I8s	difforig;	/* 'c' if creature have the same origin */
  I32u	genome_siz;
  xdrwrd	genome<>;
};


struct BglMCSpySt{
  I32s	Ploidy; 				/* Element #0 */
  I32s	MinCellSize;				/* Element #1 */
  I32s	Stack_size;				/* Element #2 */
  I32s	Numreg;					/* Element #3 */
  I32s	cp_md_p;				/* Element #4 */
  I32s	cp_md_s;				/* Element #5 */
  I32s	cp_q_this_a;				/* Element #6 */
  I32s	cp_q_this_i;				/* Element #7 */
  I32s	cp_mm_s;				/* Element #8 */
  I8s	lbl[4];					/* Element #9 */
  I32s	soup;					/* Element #10 */
  I8s	id[BGL_MICRO_SPY_ID_LEN_P_1];		/* Element #11 */
  I32s	cp_mm_p;				/* Element #12 */
  I32s	cp_c_ib;				/* Element #13 */
  I32s	cp_c_c_st[BGL_MICRO_SPY_ST_N];		/* Element #14 - 23 */
  I32s	cp_c_c_re[BGL_MICRO_SPY_RE_N];		/* Element #24 - 35 */
  I32s	cp_c_c_sp;				/* Element #36 */
  I32s	cp_c_c_ip;				/* Element #37 */
  I32s	cp_c_c_threadid;			/* Element #38 */
  I8s	off_char;				/* Element #39 */
  I32s	off_int;				/* Element #40 */
  I32s	cp_c_c_fl_E;				/* Element #41 */
  I8s	d_data[BGL_MICRO_SPY_DATA_LEN];		/* Element #42 */
  I32s	cp_c_c_ex;				/* Element #43 */
  I32s	cp_c_c_so;				/* Element #44 */
  I32s	cp_c_c_de;				/* Element #45 */
  I32s	cp_c_ac;				/* Element #46 */
  I32s	cp_c_n;					/* Element #47 */
  I8s	regcol0[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #48 */
  I8s	regcol1[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #49 */
  I8s	regcol2[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #50 */
  I8s	regcol3[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #51 */
  I8s	regcol4[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #52 */
  I8s	regcol5[BGL_MICRO_SPY_NUMSPECREG_P_1];	/* Element #53 */
  I8s	hostbuf1[30];				/* Element #54 */
  I8s	hostbuf2[30];				/* Element #55 */
  I8s	hostbuf3[30];				/* Element #56 */
  I8s	hostbuf4[30];				/* Element #57 */
  I8s	hostbuf5[30];				/* Element #58 */
  I8s	hostbuf6[30];				/* Element #59 */
};


#ifdef RPC_HDR
%#endif	/* bgl_dat_xdr_h */
#endif
