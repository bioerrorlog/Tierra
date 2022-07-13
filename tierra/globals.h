/* @(#)globals.h 1.27 1/30/95 */
/* globals.h 30-3-98 Artificial Life simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/*  Tom Ray & Virtual Life */

/*
 * sccsid: @(#)globals.h 1.38    12/8/91
 */

#ifndef LEXTERN_H
#define LEXTERN_H

#ifndef EXTERN
#define EXTERN extern
#endif

#include "license.h"

#ifdef BGL
#include "../Bglcom/bgl_dat_xdr.h"
#endif /* BGL */

#ifndef BGL_CLNT

#ifdef TIERRA
extern I32s alive;          /* run simulator alive * 1,000,000 instructions */
extern I32s AliveGen;       /* 0=alive in InstExe.m, 1=alive in generations */
EXTERN double Generations; /* elapsed generations (AvgPop/TimeBirth-Death) */
extern I32s TierraSleep; /* sleep (sec) if key or mouse det 0 - no sleep */
                         /* for Win32 - if > 0 sleep on signal */
extern I32s SpeedUpdate; /* freq, in seconds, of updating calc of speed */
EXTERN I32s TierraClock;    /* clock time at last plan() */
EXTERN I32s LastSpeedUpdate; /* time of last Speed update */
EXTERN Pcells ce;     /* cell that is currently active, formerly ThisSlice */
EXTERN I8s CellHalt;        /* the currently active cell has halted */
EXTERN I8s CpuHalt;         /* the currently active cpu has halted */
EXTERN BrkTrapCond BrkTrapCondBuf;
extern I32s WatchExe; /* mark executed instructions in genome in genebank */
extern I32s WatchMov; /* set mov bits in genome in genebank */
extern I32s WatchTem; /* set template bits in genome in genebank */
extern InstDef idt[];
EXTERN I8s TierraWait;

EXTERN I32s CpuLoadLimitSleepNow;  /* cpu load limit sleep flag */
EXTERN I32s CpuLoadLimitLstSlp; /* time of last cpu load limit sleep */
EXTERN I32s CpuLoadLimitRunTime;
EXTERN I32s CpuLoadLimitSleepTime;
extern double CpuLoadLimitProp; /* portion of CpuLoadLimitPeriod to run */
extern I32s CpuLoadLimitPeriod; /* millisec, 0 - no cpu load limit */
extern I32s M_tvWait_sec;   /* time out for select in OV mode (sec.) */
extern I32s M_OV_threshold; /* Threshold for time out in OV mode */

#ifdef NET
EXTERN I32s ClusterServerUp;
EXTERN I32s FEBirthLocal;   /* count of local births per million */
EXTERN I32s FEBirthInject;  /* count of injection births per million */
EXTERN I32s FEBirthEject;   /* count of ejected births per million */
EXTERN I32s FEFecundYesSum; /* count of deaths of reproductives */
EXTERN I32s FEFecunZeroSum; /* count of deaths of non-reproductives */
EXTERN I32s FEFecunEjecSum; /* count of ejected daughters per million */
EXTERN I32s BirthLocal;     /* count of local births per million */
EXTERN I32s BirthInject;    /* count of injection births per million */
EXTERN I32s BirthEject;     /* count of ejected births per million */
EXTERN I32s FecundYesSum;   /* count of deaths of reproductives */
EXTERN I32s FecunZeroSum;   /* count of deaths of non-reproductives */
EXTERN I32s FecunEjecSum;   /* count of ejected daughters per million */
extern I32s NodeSelSucRatPrec; /* stop after NodeSelSucRatPrec         */
                               /* node selection attempts              */
                               /* <0 = parameter not used              */
extern I32s GetIPPStrtIdxC;    /* getipp start for IPMapC              */
                               /* >=0 : start index in IPMap array     */
                               /* -1  : own node                       */
                               /* -2  : random start                   */
extern I32s GetIPPStrtIdxS;    /* getipp start for IPMapS              */
                               /* >=0 : start index in IPMap array     */
                               /* -1  : own node                       */
                               /* -2  : random start                   */
extern I32s BrkIPPStrtIdxC;    /* stop on specific IPMapC start index  */
                               /* >=0 : break on given start index     */
                               /* < 0 : not used                       */
extern I32s BrkIPPStrtIdxS;    /* stop on specific IPMapS start index  */
                               /* >=0 : break on given start index     */
                               /* < 0 : not used                       */
extern I32s BrkIPPStopIdxC;    /* stop on specific IPMapC stop index   */
                               /* >=0 : break on given stop index      */
                               /* < 0 : not used                       */
extern I32s BrkIPPStopIdxS;    /* stop on specific IPMapS stop index   */
                               /* >=0 : break on given stop index      */
                               /* < 0 : not used                       */
extern I32s NetRcvUpdFreq; /* freq in sec of net message check */
EXTERN I32s LstRcvTime; /* time of last net message check */
EXTERN BglTrackFlag  MigrStartBuf[MIGR_BUF_MAX];   /* need new cell to track */
EXTERN int MigrStartBufSize;
EXTERN BglTrackFlag  MigrStopBuf[MIGR_BUF_MAX];    /* need to stop tracking */
EXTERN int MigrStopBufSize;
EXTERN I32s  MigrationTrackCount; /* count of trackings on this node */
extern I32s SensMapAtt; /* count of sensory system attempts for sensory */
                           /* system mapping                            */
                           /* <  0 = map all regardless of count        */
                           /* >= 0 = map only those with this count     */
EXTERN I32s SrvrApocFlag;   /* 1 = server triggered apocalype */
EXTERN I32s SrvSubNetSet;   /* 1 = subnet set from server */
extern float ImmigLimFrac; /* fraction of incoming messages accepted */
                           /* - < 0 = no limit (normal operation) */
                           /* used to prevent flooding during sampling */
extern I32s PendReqMax; /* max # pending TPing requests before node removal */
EXTERN I32u MessageLen; /* Message length */
EXTERN I8s ping_fn[40];  /* file for writing TPing data */
EXTERN I32s PipmapC;     /* global pointer into IPMapC array */
EXTERN I32s PipmapS;     /* global pointer into IPMapS array */
EXTERN I32s clilen;
EXTERN struct sockaddr_in client;
EXTERN I32s CellSockUDP; /* UDP Socket for internet comm between Cells */
EXTERN I32s MonSockUDP;  /* UDP Socket for Network Monitor */
EXTERN I32s MonSockTCP;  /* TCP Socket for Network Monitor */
EXTERN I32s MsgRdSockTCP;   /* TCP Socket for incoming messages */
EXTERN MonSock MonSockTCP_ID[64];  /* Multiple TCP Socket for Network Monitor */
EXTERN I32s SuMonFlg;  /* Flag for if SU Monitor is exit(=1) or not(=0) */
EXTERN I32s MonTCPNid; /* Number of used MonSockTCP_ID */
EXTERN fd_set Afds, Rfds, Wfds, Efds;
EXTERN struct timeval tvWait;   /* time out for select */
EXTERN I8s RecvBuf[2048];
EXTERN I32s RecvLength;
EXTERN struct sockaddr_in remot;
EXTERN I32s remotlen;
EXTERN I32s SendLength;
EXTERN I8s *SendBuf;
EXTERN I8s *RemotHostName;
EXTERN I8s *RemotHostAddr;
EXTERN I32s NetWriteSoupFlg;
extern I32s BklogProcFreq;
EXTERN I32s LastBklogProc;

#ifdef __MWERKS__
extern I8s MacHost[80];
extern I8s MacSite[80];
#endif /* __MWERKS__ */

#endif /* NET */

#ifdef MICRO
EXTERN I32s MC_step;
EXTERN I32s soupviewstrt;
EXTERN I8s dbgdisptype;
EXTERN GlIndex rcnttardy; /* most recent thrd analysis data ready at brk */
#endif /* MICRO */

EXTERN I32s histostrtidx;
EXTERN I32s histoentcnt;
EXTERN I32s histodsplnct;

#ifdef __GUI__
#ifdef WEXTERN_H
#undef WEXTERN_H
EXTERN HANDLE hInst;
EXTERN HWND hwnd;
EXTERN UINT message;
EXTERN UINT wParam;
EXTERN LONG lParam;
EXTERN HDC hdcnew;
EXTERN PAINTSTRUCT ps;
EXTERN FARPROC lpfnGetStrDlgProc;
EXTERN short cxChar, cyChar, nVscrollPos, nHscrollPos, nPaintBeg, nPaintEnd;
EXTERN wScreen *FEw;
#endif /* WEXTERN_H */
#endif /* __GUI__ */

#ifdef MEM_PROF
EXTERN MemProfStat MemStat; /* structure for memory profile statistics */
#endif /* MEM_PROF */

extern I32s ThrdAnaMaxFec;     /* fecundity required to stop thread    */
                               /* analysis data collection for cell    */
                               /* <  0 = any value of fecundity > 1    */
                               /* =  0 = stop after fecundity =        */
                               /*        FecundityAvg                  */
                               /* >  0 = stop at this number of        */
                               /*        children                      */

extern I32s ThrdAnaStop;       /* condition required to stop thread    */
                               /* analysis data collection for cell    */
                               /* 0 = stop if fecundity conditions are */
                               /*     met                              */
                               /* 1 = stop on cell death of specified  */
                               /*     cause if fecundity conditions    */
                               /*     are met                          */
                               /* 2 = stop on cell migration if        */
                               /*     fecundity conditions are met     */
                               /* 3 = stop on remote divide if         */
                               /*     fecundity conditions are met     */
                               /* 4 = stop on local divide if          */
                               /*     fecundity conditions are met     */
                               /* 5 = stop on completed life cycle     */

extern I32s ThrdAnaTrmCode;    /* specific cause of death at which to */
                               /* indicate thread analysis data ready */
                               /* 0 = any cause of death              */
                               /* ex. see REAP_LAZY                   */

extern I32s ThrdAnaPartSave;   /* (cell death before criteria are met) */
                               /* == 0 no partial save                 */
                               /* != 0 partial save                    */

extern I32s GenPerDivMut;
EXTERN I32s TotDivMut;
extern I32s GenPerCroInsSamSiz;
EXTERN I32s TotCroInsSamSiz;
extern I32s GenPerInsIns;
EXTERN I32s TotInsIns;
extern I32s GenPerDelIns;
EXTERN I32s TotDelIns;
extern I32s GenPerCroIns;
EXTERN I32s TotCroIns;
extern I32s GenPerDelSeg;
EXTERN I32s TotDelSeg;
extern I32s GenPerInsSeg;
EXTERN I32s TotInsSeg;
extern I32s GenPerCroSeg;
EXTERN I32s TotCroSeg;
extern I32s GenPerFlaw;   /* generations per flaw */
EXTERN I32s RateFlaw;     /* frequency of flaws */
EXTERN I32s TotFlaw;      /* total number of flaws in this run */
EXTERN I32s CountFlaw;   /* counter for flaw random number */
extern I32s GenPerMovMut; /* generations per move mutation */
EXTERN I32s TotMovMut;    /* total number of move mutations in this run */
EXTERN I32s CountMovMut;
EXTERN I32s RateMovMut;   /* 1 / frequency of mutations per mov event */
extern I32s GenPerBkgMut; /* generations per background mutation */
EXTERN I32s TotMut;   /* total number of background mutations in this run */
EXTERN I32s CountMutRate;
EXTERN I32s RateMut;      /* number of instructions per mutations */
extern float MutBitProp; /* proportion of mutations that are bit flips */
extern I32s MateSizeEp;    /* used with CroInsSamSiz genetic operator */
EXTERN I32s EjectToSelf;   /* count of ejections to self node per million */
EXTERN I32s FEEjectToSelf; /* count of ejections to self node per million */
EXTERN I32s DeathNum;       /* count of deaths per million instruction */
EXTERN I32s FEDeathNum;     /* count of deaths per million instruction */
EXTERN I32s BirthNum;       /* count of births, includes injections */
EXTERN I32s FEBirthNum;     /* count of births, includes injections */
extern I32s LazyTol;        /* tolerance for non-reproductive cells */
EXTERN I32s AgeSum;         /* sum of cr->d.inst at death per million */
EXTERN I32s FecunditySum;   /* sum of cr->d.fecundity at death per million */
EXTERN double RepInstEff;   /* average (ce->d.repinst / size) per million */
EXTERN double RepInstEffSum;/* sum of (ce->d.repinst / size) per million */
EXTERN I32s RepInst;        /* average instructions per reproduction */
EXTERN I32s RepInstSum;     /* sum of ce->d.repinst per million */
EXTERN I32s RepNum;         /* number of replications per million */
EXTERN TmpList tmpl;        /* list of .tmp files */
EXTERN indx_t GIndx;        /* global index structure */
EXTERN FILE *oufr;
EXTERN FILE *sgfile;
EXTERN HpInst soup;
EXTERN I32s FESpeed;        /* seconds elapsed in last million */
EXTERN I32s BitBucket;      /* place to dump return values */
EXTERN I32s AverageSize;    /* average size of cells in soup */
EXTERN Pcells BottomDummy;  /* dummy cell at bottom of reaper queue */
EXTERN Pcells BottomReap; /* index of cell bottom reaper queue, last to die */
EXTERN I32s BrkupCou;       /* count of output files break.n */
EXTERN I32s BrkupCum;       /* cumulative count of bytes output to break.n */
extern I32s BrkupSiz;       /* size of output files break.n in K */
#if (INTERFACE == BASICB)||(INTERFACE == BASICD)
I32s DispText = 14, DispBack = 0, InfoText = 15, InfoBack = 6;
#endif /* (INTERFACE == BASICB)||(INTERFACE == BASICD) */
EXTERN I8s SmplNam[120]; /* pathname of sample extracted on */
EXTERN I32s pos;         /* file pointer for position in break.X files */
EXTERN I32s CelArSiz;    /* size of each cells array */
EXTERN I32s CellsSize;   /* number of allocated elements in cells array */
EXTERN I32s CountJmpSou; /* counter for source track jumping */
extern I32s CumGeneBnk;  /* Use cumulative gene files, or overwrite */
extern I32s debug;       /* output info to screen for debugging */
EXTERN I32s debug_switch;
extern I32s DiskOut;     /* 0 = no output to disk, 1 = output to disk */
EXTERN Event Disturb;    /* time of disturbance */
extern float DistFreq; /* freq. of disturbance, in multiples of recovery t */
extern Event DistNext;   /* time of next disturbance */
extern float DistProp;   /* proportion of cells killed in each disturbance */
extern I32s DivSameSiz;  /* 0 = off, 1 = daughter cell must be same size */
extern I32s DivSameGen;  /* 0 = off, 1 = daught cell must be same genotype */
extern I32s DropDead;   /* millions instruction since last divide till dead */
extern I32s MinSpeed;   /* minimum speed to continue run */
extern I32s EjectRate;   /* which cells are randomly ejected from soup */
EXTERN CellInd extr;     /* which cell to isolate */
EXTERN I8s ExtrG[20];    /* last geno extracted */
EXTERN I32s ExtractCount;
EXTERN I32s FirstOutDisk;
EXTERN I32s FreeBlocks;  /* number of free blocks of memory */
extern I32s DiskBank;    /* turn disk-genebanker on and off */
extern I32s FindTimeM; /* set trap at certain InstExe, for debugging */
extern I32s FindTimeI; /* set trap at certain InstExe, for debugging */
extern I32s GeneBnkerOvrd;
extern I32s DeconstructOn; /* instruction trace log switch 0=off, 1=on */
EXTERN I32s TGeneBnker;  /* 0 = don't keep track genotyes, 1 = keep track */
EXTERN I32s GeneBnkUsage;  /* count of gene bank users. */
EXTERN double AvgPop;      /* average population over the last million */
EXTERN I8s **GenInList;    /* pointers to soup_in genome names */
EXTERN I8s *GenInBuf;      /* buffer containing soup_in genome names */
EXTERN I8s GoDown;      /* flag to bring system down to defragment memory */
EXTERN HistType *Hist;    /* pointer to struct for histograms */
EXTERN I32s HistSize;     /* Num of elements alloced in Hist */
EXTERN I32s HistAlocSiz;  /* allocated size of Hist array, in bytes */
EXTERN float HistNStars;  /* ratio of counts to stars */
EXTERN I32s HistPrint; /* boolean to tell us weather to print hists, to log */
extern I8s IMapFile[80];  /* inst map filename, default.map is assumed */
EXTERN I8s IMode;         /* info display (plan,histo_size,histo_geno etc) */
EXTERN PInst is;           /* struct for passing info between parse & execute */
EXTERN I32s isolate;       /* isolate the genome of the cell extr */
extern float JmpSouTra;    /* source track switches per average size */
extern float JumpTrackProb;/* prob of switching track during a jump of IP */
EXTERN Event LastDiv;      /* instructions executed at last divide */
extern I32s ThreadAnalysis;
extern I32s SplitTissueAna;
extern I32s SouUpdTrk;
EXTERN I32s ThrdAnaTimeLastOn;  /* time thread analysis was last turned on */
extern I32s DeadMemInit;     /* 0 = no change */
                             /* 1 = zero      */
                             /* 2 = randomize */
EXTERN I32s Max_hits;      /* cardinality of most populous size class */
extern I32s MaxCpuPerCell; /* maximum number of CPUs allowed per cell */
extern I32s MaxFreeBlocks; /* number allocated elements in FreeMemry array */
extern I32s MaxIOBufSiz;   /* maximum size of IOS buffer */
extern float MaxMalMult;   /* multiple of cell size allowed for mal() */
extern I32s MaxPutBufSiz;  /* maximum size for put IO buffer */
extern I32s MaxGetBufSiz;  /* maximum size for get IO buffer */
extern I32s MaxSigBufSiz;  /* maximum size for signals buffer */
EXTERN I32s MaxPop;        /* maximum population of any genotype in soup/GB */
EXTERN I32s MaxMemry;   /* maximum memory usage of any genotype in soup/GB */
EXTERN Genotype MaxGenPop; /* genotype with maximum population in soup/GB */
EXTERN Genotype MaxGenMem; /* genotype with maximum memory usage in soup/GB */
extern I32s MemModeFree;   /* unix chmod for when mem is dealloced,  */
extern I32s MemModeMine;  /* unix chmod for when mem is owned by a creature */
extern I32s MemModeProt;  /* chmod when mem is owned by another creature */
extern I32s MalMode;    /* 0= first fit, 1= better fit, 2= random preference,
3= near mother's address, 4= near dx address, 5= near top of stack address */
extern I32s MalReapTol; /* 1 = reap oldest creature within MalTol */
extern I32s MalSamSiz;  /* force memory alloc to be same size as parent */
extern I32s MalTol;     /* multiple of avgsize to search for free block */
EXTERN I32s MalLimit;    /* search limit for memory allocation */
extern I32s MinCellSize; /* minimum cell size */
extern I32s MinGenMemSiz; /* minimum size for genetic memory of cells */
extern I32s MinTemplSize; /* minimum template size */
extern float MovPropThrDiv;/* min proportion of daught cell filled by mov */
EXTERN I32s NumCelAr;      /* number of cells arrays */
extern I32s NumCellsMin; /* minimum # cells allowed (after inoculation) */
EXTERN I32s NumGenDG;    /* present # of permanent genotypes saved to disk */
EXTERN I32s NumGenotypes; /* # of genotypes of adult cells extant in soup */
EXTERN I32s NumSizes;     /* # of sizes of adult cells extant in soup */
EXTERN I32s PhotonSize;   /* number of instructions in photon */
extern I32s PhotonWidth;  /* amount by which photons slide to determine fit */
EXTERN I32s RateJmpSou;
EXTERN I32s reaped;    /* 0 = reaper has not killed, 1 = reaper has killed */
extern float ReapRndProp; /* rnd prop of top of reaper Q to reap from */
extern I32s SaveFreq;     /* frequency of saving core_out, soup_out and list */
extern I32s SavRenewMem;  /* free & renew dynamic memory after save to disk */
extern I32s seed;        /* seed for random number generator */
extern I32s SizDepSlice; /* 0 = slice constant, 1 = slice size dependent */
EXTERN I32s siz_sl;      /* allocated size of *sl array */
extern I32s SliceSize;   /* number of instructions executed in each slice */
extern I32s SliceStyle;  /* select style of allocating slice size */
EXTERN I32s SoupBot;    /* index FreeMemry struct for bottom of soup memory */
EXTERN I32s SoupTop;    /* index of FreeMemry struct for top of soup memory */
EXTERN Event TimeStats; /* time of last calculation of stats */
EXTERN I32s TimeGenIndiv; /* cpu cycles per replication, on average */
EXTERN double TimePop;    /* sum of ttime * NumCells for each million */
EXTERN Pcells TopDummy;   /* dummy cell at top of reaper queue */
EXTERN Pcells TopReap;    /* cell at top reaper queue, next to die */
extern I8s PhotonWord[80];   /* alphabetic representation of photon */
EXTERN Instruction PhotonInst[80];/* instructional representation of photon */
EXTERN I32s FreeMemCurrent; /* current amount of free memory in soup */
extern float SearchLimit;   /* limit on search, as multiple of average size */
extern I32s AbsSearchLimit;   /* absolute limit on search */
EXTERN I32s Search_limit; /* limit how far address instructions will search */
extern float PutLimit;      /* distance for intercellular communication */
EXTERN I32s Put_limit;      /* limit on intercellular communications distance */
extern I32s StrictIP; /* 1 = IP must be valid, 0 = IP mapped by Hamming dist */
extern I32s MinSoupSize; /* maximum soup size */
extern I32s MaxSoupSize; /* minimum soup size */
EXTERN Pcells Fp cells;  /* cells array */
EXTERN SList **sl;       /* list of size classes for genebanker */
EXTERN Pmf FreeMemry;    /* free memory array */
extern double PhotonPow; /* power for photon match slice size */
extern double SlicePow;  /* power for size dependent slice */
extern I32s SavMinNum;   /* minimum number of individuals to save genotype */
extern float SavThrMem;  /* thresh memory prop. to save genotype */
extern float SavThrPop;  /* thresh population prop. to save genotype */
extern float SlicFixFrac; /* fixed fraction of slice size */
extern float SlicRanFrac; /* random fraction of slice size */
EXTERN LastOut lo;        /* last data output to disk */
EXTERN void (*slicer) P_((void));
EXTERN I32s EffDaught;
EXTERN I32s HistSortOrder;

#if OPSYS == MACTIE
EXTERN MenuHandle appleMenu;
#if INTERFACE == BASICM
EXTERN int LINES;
EXTERN int COLS;
EXTERN WindowPeek wp;
EXTERN FontInfo fontInfo;
EXTERN short rowHeight;
#endif /* INTERFACE == BASICM */
#endif /* OPSYS == MACTIE */
extern I32s InstExeUpdFreq;
EXTERN I32s InstExeUpdNow;
extern I16u MonPort;            /* Port Number of Network Monitor */
extern I16u MigrCtrlPort;       /* Port Number for Beagle short connection */
#ifdef BGL
extern I32s TrackNormLike;      /* more for size tracking and gene tracking */
EXTERN BglMessageSt* BglMessageBuf;     /* Buffer for BglMessageBuf */
#endif /* BGL */
#endif /* TIERRA */

#if defined(TIERRA)||defined(CLSTRSRVR)
EXTERN VarQuer vqu;         /* structure for variable query */
extern I8s GenebankPath[80]; /* path for genebank */
extern I32s new_soup;      /* 0 = processed soup core, 1 = new soup core */
extern I32s TierraLog;     /* boolean for FE logging */
extern I32s TierraNice; /* Nice value for cpu */
EXTERN I8s soup_fn[85];  /* place for soup_in filename */
EXTERN FILE *tfp_log;   /* file pointer for log */
EXTERN I32s tielogpos;   /* file pointer for position in output log files */
EXTERN I8u MSG_X;
EXTERN I8u MSG_Y;
EXTERN I8u ERR_X;
EXTERN I8u ERR_Y;
EXTERN I8u PLN_X;
EXTERN I8u PLN_Y;
EXTERN I8u HLP_X;
EXTERN I8u HLP_Y;
EXTERN I32s RandIx1, RandIx2, RandIx3;  /* for tsrand() */
EXTERN double TrandArray[98]; /* for trand() */
EXTERN time_t tiestrtme;   /* Tierra start time */
EXTERN I32s AgeAvg;         /* average cr->d.inst at death per million */
EXTERN I32s Speed;       /* difference in TierraClock time between plans() */
EXTERN double FecundityAvg; /* average cr->d.fecundity per million */
EXTERN I32s fe_lines;    /* approx hight in chars of screen */
EXTERN I32s fe_width;    /* approx width in chars of screen */
#if (FRONTEND!=BASIC) || defined(_WIN32)
EXTERN int LINES;
EXTERN int COLS;
#endif /* (FRONTEND!=BASIC) || defined(_WIN32) */
EXTERN I8s FGSbuf[90];      /* buff for FEGetStr() */
EXTERN I8s TC_Menu;     /* flag unix sigint, to do a menu */
EXTERN I32s SoupSize;    /* size of soup memory, measured in instructions */

#ifdef SETENDIAN
EXTERN I32s EndianType; /* 0-little, 1-big */
#endif /* SETENDIAN */

EXTERN I32s FE_DV_Mode;
EXTERN Event InstExe;           /* counter of instructions executed */

#ifdef NET
extern I32s ApocalypseFreq; /* frequency of apocalypse, in millions */
extern float ApocFixFrac;   /* fixed fraction of apocalypse frequency */
extern float ApocRanFrac;   /* random fraction of apocalypse frequency */
EXTERN I32s ApocalypseNowFreq; /* current freq of apocalypse, in millions */
EXTERN I32s ApocalypseNow;  /* Apocalypse flag */
EXTERN I32s ApocToggle;  /* Apocalypse toggle */
extern I32s AutoAddIPMap; /* automatically add new nodes to IPMap array ? */
extern I32s AutoRemIPMap; /* automatically remove inactive or offline nodes */
extern I32s BasSubNetChgFrq; /* frequency of subnet change */
extern float SubNetChgFrqRanFrac; /* random fraction of BasSubNetChgFrq */
extern float SubNetChgFrqFixFrac; /* fixed fraction of BasSubNetChgFrq */
extern I16u LocalPort;
extern I8s map_fnC[40];  /* mapfile for Internet, local cluster */
extern I8s map_fnS[40];  /* mapfile for Internet, cluster servers */
extern I32s OnLineStat; /* 0 = offline, 1 = online */
extern I32s PendReqTime; /*max pending TPing request time before node removal*/
extern I32s PingDatLog;     /* save log of ping data in ping.dat, boolean*/
extern I32s SubNetCnt; /* number of subnets */
extern double SubNetChgApocProb; /* subnet transition apocalypse prob */
extern I32s SpdZeroTime; /* max time before Speed zero */
extern I32s SrvrApocSleep; /* sleep time after server triggered apocalype */
extern I32s TieMsgBkLog, TieMsgMaxAge;
EXTERN TieCommInfo globtcibuf;
extern I32s TieSubNet; /* subnet number */
extern float TieMinOutBndFrac; /* fraction of MaxOutBandWidth allocated for */
                               /* minimum outgoing bandwidth of tierra nodes */
EXTERN FILE *ping_fp;
EXTERN I32s tpinglogpos; /* file pointer for position in tping log files */
EXTERN NetAddr LocalClusterAddr;
EXTERN MapNode *IPMapS; /* list of Tierra cluster servers */
EXTERN I8s LocalHostName[NAMBUFSIZ];
EXTERN NetAddr LocalNetAddr;
EXTERN I32u NumNodesC;  /* number of nodes in local cluster */
EXTERN I32u NumNodesS;  /* number of cluster servers */
EXTERN MapNode *IPMapC; /* list of Tierra nodes in the local cluster */
EXTERN I32s SubNetChgTime; /* time of next subnet change */
EXTERN ComCount  FEMonCom[6];
EXTERN ComCount  MonCom[6];
EXTERN I32s RemOutBndCap; /* remaining outgoing bandwidth capacity */
EXTERN I16u RemotPort;
EXTERN Event InstExeConnect; /* counter of inst executed while connected */
EXTERN I32s OpSys;           /* nature of the OS */
EXTERN int sigioerrflag;
EXTERN I32s Nfds;      /* Number of descriptors */
EXTERN TieCommInfo *globcominfobuf;
EXTERN I32s OutCapUpdTog;  /* remaining outgoing capacity toggle */
extern double MinSubNetApoRat; /* < MinSubNetApoRat, no subnet contamination */

#if NETTYPE == UDP || NETTYPE == UDPASM
extern I32s TieMTU;
extern I32s PktSndDelay; /* delay after outgoing packet in usec */
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */

#if NETTYPE == TCP
extern I32s TCPRemSelTmeSec;
extern I32s TCPRemSelTmeUSec;
extern I32s TCPLocSelTmeSec;
extern I32s TCPLocSelTmeUSec;
#endif /* NETTYPE == TCP */

#endif /* NET */

#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)
extern I32s hangup;/* 0 = exit on error, 1 = hangup on error, for debugging */
extern double MinComSizRat; /* >= MinComSizRat : same narrow tissue */
extern double LifeCycFrct; /* taev_instP<= LifeCycFrct :     */
                           /* list gene as executed in       */
                           /* selected portion of life cycle */
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)\
    ||defined(PROBE)||defined(SOUPUPDTRC)
EXTERN I32s ShadowUsed;     /* Shadow registers are being used */
EXTERN InstDef *id;
EXTERN IDRegs IDregs;       /* definitions of special registers */
EXTERN I32s InstBitNum;   /* number of bits in instructions */
EXTERN I32s Nop0;          /* instruct num corresponding to nop0 */
EXTERN I32s Nop1;          /* instruct num corresponding to nop1 */
EXTERN I32s NopS;          /* sum of nops */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(CLSTRSRVR)||defined(SOUPUPDTRC)
EXTERN prec_enc_size prcencsize;
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(CLSTRSRVR)||defined(SOUPUPDTRC) */

#ifdef CLSTRSRVR
extern I32s ClsIPSUpdInt; /* cluster IPMapS update interval */
extern I32s FirstSrvrApocWait; /* time before 1st server Apocalypse */
extern I32s IntrvlOutBndCap; /* bandwidth capacity reset interval */
extern I32s MaxOutBandWidth; /* maximum band width available */
extern I32s ServerSleep; /* cluster server sleep interval */
extern float SrvrOutBndFrac;   /* fraction of MaxOutBandWidth */
                              /* allocated to server */
EXTERN I32s srvactivity;
EXTERN I32s SrvrApocOK; /* server Apocalypse startup wait is over */
EXTERN double FpAgeAvg; /* average cr->d.inst at death per million */
EXTERN time_t LstOutBndCapRst; /* time of last RemOutBndCap reset */
#endif /* CLSTRSRVR */

#if defined(TIERRA)||defined(ARGTIE)||defined(PROBE)
extern I32s GeneBnker;   /* 0 = don't keep track genotyes, 1 = keep track */
#endif /* defined(TIERRA)||defined(ARGTIE)||defined(PROBE) */

#if defined(TIERRA)||defined(ARGTIE)||defined(PROBE)||defined(CLSTRSRVR)
extern I32s NumCells;    /* present number of cells in soup */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(PROBE)||defined(CLSTRSRVR) */

#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR)
#ifdef SIGBLOCK
EXTERN sigset_t SigSet;
#endif /* SIGBLOCK */
#endif /* defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||defined(SOUPUPDTRC)
EXTERN I32s InstNum;      /* number of instructions in set */
EXTERN I8s Buf85[85];    /* nice global text buff for all sorts of messages */
EXTERN I8s Buf85a[85];   /* nice global text buff for all sorts of messages */
EXTERN I8s Buf85b[85];   /* nice global text buff for all sorts of messages */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(CLSTRSRVR)||defined(SOUPUPDTRC)
extern I32s XDRBufMaxSize; /* max XDR encode/decode buffer size */
EXTERN I8s Fbuf[90]; /* global text buff for FEPrintf & FEError messages */
EXTERN I8s Buff[120];    /* nice global text buff for all sorts of messages */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(CLSTRSRVR)||defined(SOUPUPDTRC) */

#endif /* LEXTERN_H */
