/* @(#)soup_in.h 1.15 1/19/95 */
/* soup_in.h 30-3-98 default parameters for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */

/*
 * sccsid: @(#)soup_in.h 1.5 7/21/92
 */

#ifndef SOUP_IN_H
#define SOUP_IN_H

#include "license.h"

#ifdef TIERRA
I32s BrkupSiz = 1024; /* size of output file in K: break.1, break.2 ... */
I32s CumGeneBnk = 0;  /* Use cumulative gene files, or overwrite */
I32s debug = 0;       /* 0 = off, 1 = on, printf statements for debugging */
I32s DiskBank = 1;    /* turn disk-genebanker on and off */
I32s DiskOut = 1;     /* output data to disk (1 = on, 0 = off) */
I32s FindTimeM = 0;   /* set trap at certain InstExe, for debugging */
I32s FindTimeI = 0;   /* set trap at certain InstExe, for debugging */
I32s GeneBnkerOvrd = 0;
I32s DeconstructOn = 0; /* instruction trace log switch */
I32s MaxSigBufSiz = 8; /* max signal buffer size */
I32s MaxFreeBlocks = 600; /* initial # of structure for memory allocation */
I32s SaveFreq = 100;  /* frequency of saving core_out, soup_out and list */
I32s SavRenewMem = 1; /* free and renew dynamic memory after saving to disk */
I32s SavMinNum = 2;   /* minimum number of individuals to save genotype */
float SavThrMem = (float).015; /* threshold memory occupancy to save genotype */
float SavThrPop = (float).015; /* threshold population proportion to save genotype */
I32s TierraLog = 0; /* 0 = no log file, 1 = write tierra.log file to disk */
I32s ThreadAnalysis = 0; /* 0 = thread analysis off, 1 = thread analysis on */
I32s SplitTissueAna = 0; /* 0 = do not collect split tissue data */
                         /* 1 = collect split tissue data */
I32s SouUpdTrk = 0; /* 0 = do not track soup memory updates */
                    /* 1 = track soup memory updates */
I32s ThrdAnaMaxFec = -1;
I32s ThrdAnaStop = 0;
I32s ThrdAnaTrmCode = 0;
I32s ThrdAnaPartSave = 0;
I32s DeadMemInit=0;   /* 0 = no change */
                      /* 1 = zero      */
                      /* 2 = randomize */
I32s TierraSleep = 0; /* sleep (sec) if key or mouse detected 0 - no sleep */
                      /* for Win32 - if > 0 sleep on signal */
I32s WatchExe = 0;      /* mark executed instructions in genome in genebank */
I32s WatchMov = 0;      /* set mov bits in genome in genebank */
I32s WatchTem = 0;      /* set template bits in genome in genebank */
I32s TierraNice = 19;        /* Nice value for cpu */
I16u MonPort = 17501; /*  Port number for network monitor */
I16u MigrCtrlPort = 17503; /*  Port number for Beagle short connexion */
I32s alive = 500;    /* how many millions of instruction will we run */
I32s AliveGen=1;     /* 0=alive in InstExe.m, 1=alive in generations */
float DistFreq = (float).3; /* freq of disturbance, factor of recovery time */
float DistProp = (float).2; /* prop of population affected by disturbance */
I32s DivSameSiz = 0; /* produce offspring of same size, stop size change */
I32s DivSameGen = 0; /* produce offspring of same genotype, stop evolution */
I32s DropDead = 5;   /* stop system if no reproduction in last x million */
I32s MinSpeed = -1; /* min speed to continue run (-1 = no effect) */
I32s EjectRate = 50; /* rate at which cells are randomly ejected from soup */
I32s GenPerBkgMut = 16; /* mut rate control by generations ("cosmic ray") */
I32s GenPerFlaw = 32;   /* flaw control by generations */
I32s GenPerMovMut = 8; /* mutation rate control by generations (copy mut) */
I32s GenPerDivMut = 64;
I32s GenPerCroInsSamSiz = 64;
I32s GenPerInsIns = 64;
I32s GenPerDelIns = 64;
I32s GenPerCroIns = 64;
I32s GenPerDelSeg = 64;
I32s GenPerInsSeg = 64;
I32s GenPerCroSeg = 64;
float MutBitProp = (float).2; /* proportion of mutations that are bit flips */
I8s IMapFile[80] = "opcode.map";  /* map of instructions set into space */
float JmpSouTra = (float)1.;     /* source track switches per average size */
float JumpTrackProb = (float).2; /* prob of switching track during jump of IP */
I32s LazyTol = 5; /* tolerance for non-reproductive cells */
I32s MalMode = 2;   /* 0 = first fit, 1 = better fit, 2 = random preference,
                       3 = near mother's address, 4 = near dx address,
                       5 = near top of stack address */
I32s MalReapTol = 1;/* 0 = reap by queue, 1 = reap oldest creat wthn MalTol */
I32s MalSamSiz = 0; /* force memory alloc to be same size as parent */
I32s MalTol = 5;    /* multiple of avgsize to search for free block */
I32s MateSizeEp = 1;   /* critters size +- val => allowed to mate with */
I32s MaxCpuPerCell = 16;   /* maximum number of CPUs allowed per cell */
I32s MaxIOBufSiz = 8;      /* maximum size for IOS buffer */
float MaxMalMult = (float)3.; /* multiple of cell size allowed for mal() */
I32s MaxPutBufSiz = 4;     /* maximum size for put IO buffer */
I32s MaxGetBufSiz = 4;     /* maximum size for get IO buffer */
I32s MinCellSize = 12;     /* minimum size for cells */
I32s MinGenMemSiz = 12;    /* minimum size for genetic memory of cells */
I32s MinTemplSize = 1;     /* minimum size for templates */
float MovPropThrDiv = (float).7;  /* min prop of dghtr cell filled by mov */
I32s MemModeFree = 0; /* unix chmod for when mem is dealloced,  */
I32s MemModeMine = 0; /* unix chmod for when mem is owned by creature,  */
I32s MemModeProt = 2; /* unix chmod, mem is owned by another creature */
I32s new_soup = 1;    /* 1 = this a new soup, 0 = restarting an old run */
I32s NumCellsMin = 1; /* minimum # cells allowed (after inoculation) */
double PhotonPow = 1.5; /* power for photon match slice size */
I32s PhotonWidth = 8;   /* amount by which photons slide to find best fit */
I8s PhotonWord[80] = "chlorophill";  /* word used to define photon */
float PutLimit = (float)20.;    /* distance for intercellular communication */
float ReapRndProp = (float)0.0; /* rnd prop of top of reaper Q to reap from */
float SearchLimit = (float)5.;  /* distance for template search */
I32s AbsSearchLimit = 0;  /* absolute template search range limit */
I32s seed = 0; /* seed for random number generator, 0 uses time to set seed */
I32s SizDepSlice = 1; /* set slice size by size of creature */
double SlicePow = 1;  /* set power for slice size, use when SizDepSlice = 1 */
double CpuLoadLimitProp = 1.0; /* portion of CpuLoadLimitPeriod to run */
I32s CpuLoadLimitPeriod = 0; /* seconds, default 0 - no cpu load limit */
I32s SliceSize = 25;  /* slice size when SizDepSlice = 0 */
I32s SliceStyle = 2;  /* choose style of determining slice size */
float SlicFixFrac = (float)0.; /* fixed fraction of slice size */
float SlicRanFrac = (float)2.; /* random fraction of slice size */
I32s SpeedUpdate = 60; /* freq, in seconds, of updating calculation of speed */
I32s StrictIP = 1; /* 1 = IP must be valid, 0 = IP mapped by Hamming dist */
I32s MinSoupSize=60000L; /* min size of soup in instructions */
I32s MaxSoupSize=60000L; /* max size of soup in instructions */
Event DistNext = {0,0};   /* time of next disturbance */
I32s M_tvWait_sec = 25;    /* time out for select in OV mode (sec.) */
I32s M_OV_threshold = 100; /* Threshold for time out in OV mode */

#ifdef __MWERKS__
I8s GenebankPath[80] = ":gb:";  /* path for genebanker output */
#else /* __MWERKS__ */
I8s GenebankPath[80] = "gb/";   /* path for genebanker output */
#endif /* __MWERKS__ */

#ifdef NET
I32s BklogProcFreq=3; /* message backlog processing frequency */
I32s SensMapAtt = -1;
I32s PingDatLog = 0;   /*save log of ping data in ping.dat */
I16u LocalPort = 18001; /*  Port number for local node */
I32s TrackNormLike = 0; /*  mode for gene and size tracking */
I32s ApocalypseFreq = 0; /* frequency of apocalypse, in millions */
I32s SrvrApocSleep=60;  /* sleep time after server triggered apocalype */
I32s TieMsgBkLog=200; /* maximum message backlog */
I32s TieMsgMaxAge=60; /* maximum message age */
I32s NodeSelSucRatPrec=-1; /* node selection success rate precision */
/*
 * =  -2 - random start
 * =  -1 - local node
 * >=  0 - map node index
 */
I32s GetIPPStrtIdxC=-1; /* cluster map getipp start */
I32s GetIPPStrtIdxS=-1; /* server map getipp start */
I32s BrkIPPStrtIdxC=-1; /* break on this cluster map getipp start */
I32s BrkIPPStrtIdxS=-1; /* break on this server map getipp start */
I32s BrkIPPStopIdxC=-1; /* break on this cluster map getipp start */
I32s BrkIPPStopIdxS=-1; /* break on this server map getipp start */

I32s OnLineStat=1; /* online ? 0=no, 1=yes */
I32s SubNetCnt=1; /* number of subnets */
I32s TieSubNet=0; /* subnet number */
I32s BasSubNetChgFrq=0; /* frequency of subnet change */
float SubNetChgFrqRanFrac=(float)0.0; /* random fraction of BasSubNetChgFrq */
float SubNetChgFrqFixFrac=(float)0.0; /* fixed fraction of BasSubNetChgFrq */
double SubNetChgApocProb=0.9; /* subnet transition apocalypse prob */
float ApocFixFrac = (float)0.0; /* fixed fraction of apocalypse frequency */
float ApocRanFrac = (float)2.0; /* random fraction of apocalypse frequency */
float ImmigLimFrac=(-1.0); /* fraction of incoming messages accepted */
                           /* - < 0 = no limit (normal operation) */
                           /* used to prevent flooding during sampling */
I32s PendReqMax = 5; /* max # pending TPing requests before Speed zero */
I32s PendReqTime = 86400; /* pending TPing request time before node removal */
I32s SpdZeroTime = 60; /* max time before Speed zero */
I32s NetRcvUpdFreq = 3; /* freq in sec of net message check */
I32s AutoAddIPMap = 0; /* automatically add new nodes to IPMap array ? */
                       /* 0=no, 1=yes */
I32s AutoRemIPMap = 0; /* automatically remove inactive or offline nodes */
                       /* from IPMap array ?  0=no, 1=yes */
double MinSubNetApoRat = 0.25; /* < MinSubNetApoRat, no subnet contamination */

#ifndef DECVAX
I8s map_fnC[40] = "MapFileC"; /* mapfile for Internet, local cluster */
I8s map_fnS[40] = "MapFileS"; /* mapfile for Internet, cluster servers */
#else /* DECVAX */
I8s map_fnC[40] = "MapFileC."; /* mapfile for Internet, local cluster */
I8s map_fnS[40] = "MapFileS."; /* mapfile for Internet, cluster servers */
#endif /* DECVAX */

#ifdef __MWERKS__
I8s MacHost[80] = "nohost";  /* word used to define host on macintosh */
I8s MacSite[80] = "nosite";  /* word used to define site on macintosh */
#endif /* __MWERKS__ */

#if NETTYPE == UDP || NETTYPE == UDPASM
I32s TieMTU=1500;      /* default Tierra MTU */
I32s PktSndDelay=0;    /* default delay after outgoing packet in usec */
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */

#if NETTYPE == TCP
I32s TCPLocSelTmeSec=0;         /* local TCP select timeout in  sec  */
I32s TCPLocSelTmeUSec=100000;   /*                          and usec */
I32s TCPRemSelTmeSec=5;         /* remote TCP select timeout in  sec */
I32s TCPRemSelTmeUSec=0;        /*                          and usec */
#endif /* NETTYPE == TCP */
#endif /* NET */

#if FRONTEND == BASIC
I32s InstExeUpdFreq=5000; /* message backlog processing frequency */
#endif /* FRONTEND == BASIC */

InstDef idt[] =
{   {0, 1, "nop0", nop, pnop, "", {0}},  /* no decode args */
    {0, 1, "nop1", nop, pnop, "", {0}},  /* no decode args */
    {0, 1, "add", add, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "add2", add, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "adrb", adr, decadr, "", {0}},  /* "rr  " */
    {0, 1, "adrf", adr, decadr, "", {0}},  /* "rr  " */
    {0, 1, "adro", adr, decadr, "", {0}},  /* "rr  " */
    {0, 1, "and", and, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "and2", and, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "call", tcall, ptcall, "", {0}},  /* no decode args */
    {0, 1, "clrf", clrf, pnop, "", {0}},  /* no decode args */
    {0, 1, "clrfi", clrfi, pnop, "", {0}},  /* no decode args */
    {0, 1, "clrrf", clrrf, pnop, "", {0}},  /* no decode args */
    {0, 1, "csync", csync, pnop, "", {0}},  /* no decode args */
    {0, 1, "dec", add, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "dec2", add, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "dec4", add, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "decC", add, dec1d1s, "cc", {0}},  /* "cc" */
    {0, 1, "div", idiv, dec1d2s, "", {0}},  /* "r" */
    {0, 1, "div2", idiv, dec1d2s, "", {0}},  /* "r" */
#ifdef USE_PORT
    {0, 1, "divide", divide, dec3s, "", {0}},  /* "rr" */
#else /* USE_PORT */
    {0, 1, "divide", divide, dec2s, "", {0}},  /* "rr" */
#endif /* USE_PORT */
    {0, 1, "enter", enter, pnop, "", {0}},  /* no decode args */
    {0, 1, "exch", exch, dec2s, "", {0}},  /* "rr" */
    {0, 1, "getregs", getregs, dec1s, "t", {0}},  /* "#" */
    {0, 1, "halt", halt, pnop, "", {0}},  /* no decode args */
    {0, 1, "ifE", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifS", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifZ", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifequal", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifgrtr", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifless", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifsig", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "ifz", skip, dec2s, "", {0}},  /* "rr" */
    {0, 1, "inc", add, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "incA", add, dec1d1s, "aa", {0}},  /* "aa" */
    {0, 1, "incB", add, dec1d1s, "bb", {0}},  /* "bb" */
    {0, 1, "incC", add, dec1d1s, "cc", {0}},  /* "cc" */
    {0, 1, "jmpb", adr, decjmp, "", {0}},  /* "r" */
    {0, 1, "jmpf", adr, decjmp, "", {0}},  /* "r" */
    {0, 1, "jmpo", adr, decjmp, "", {0}},  /* "r" */
    {0, 1, "join", join, pnop, "", {0}},  /* no decode args */
    {0, 1, "mal", malchm, dec1d3s, "", {0}},  /* "rr r" */
    {0, 1, "movBA", movdd, dec1d1s, "ba", {0}},  /* "ba" */
    {0, 1, "movDC", movdd, dec1d1s, "dc", {0}},  /* "dc" */
    {0, 1, "movdd", movdd, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "movdi", movdi, pmovdi, "", {0}},  /* "rr" */
    {0, 1, "movdi2", movdi, pmovdi, "", {0}},  /* "rr" */
    {0, 1, "movdi4", movdi, pmovdi, "", {0}},  /* "rr" */
    {0, 1, "movid", movid, pmovid, "", {0}},  /* "rr" */
    {0, 1, "movid2", movid, pmovid, "", {0}},  /* "rr" */
    {0, 1, "movid4", movid, pmovid, "", {0}},  /* "rr" */
    {0, 1, "movii", movii, pmovii, "", {0}},  /* "rr" */
    {0, 1, "movii2", movii, pmovii, "", {0}},  /* "rr" */
    {0, 1, "movii4", movii, pmovii, "", {0}},  /* "rr" */
    {0, 1, "mul", mul, dec1d2s, "", {0}},  /* "r" */
    {0, 1, "mul2", mul, dec1d2s, "", {0}},  /* "r" */
    {0, 1, "not", not, dec1d, "", {0}},  /* "r" */
    {0, 1, "notl", notl, dec1d, "", {0}},  /* "rr" */
    {0, 1, "not0", not0, dec1d, "", {0}},  /* "rr" */
    {0, 1, "offAACD", offset, dec1d3s, "aacd", {0}},  /* "aacd" */
    {0, 1, "offBBCD", offset, dec1d3s, "bbcd", {0}},  /* "bbcd" */
    {0, 1, "offset", offset, dec1d3s, "", {0}},  /* "rrrr" */
    {0, 1, "ior", ior, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "ior2", ior, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "pop", pop, dec1d, "", {0}},  /* "r" */
    {0, 1, "popA", pop, dec1d, "a", {0}},  /* "a" */
    {0, 1, "popB", pop, dec1d, "b", {0}},  /* "b" */
    {0, 1, "popC", pop, dec1d, "c", {0}},  /* "c" */
    {0, 1, "popD", pop, dec1d, "d", {0}},  /* "d" */
    {0, 1, "popE", pop, dec1d, "e", {0}},  /* "e" */
    {0, 1, "popF", pop, dec1d, "f", {0}},  /* "f" */
    {0, 1, "push", push, dec1s, "", {0}},  /* "r" */
    {0, 1, "pushA", push, dec1s, "a", {0}},  /* "a" */
    {0, 1, "pushB", push, dec1s, "b", {0}},  /* "b" */
    {0, 1, "pushC", push, dec1s, "c", {0}},  /* "c" */
    {0, 1, "pushD", push, dec1s, "d", {0}},  /* "d" */
    {0, 1, "pushE", push, dec1s, "e", {0}},  /* "e" */
    {0, 1, "pushF", push, dec1s, "f", {0}},  /* "f" */
    {0, 1, "rand", movdd, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "ret", pop, dec1d, "", {0}},  /* no decode args */
    {0, 1, "rolld", rolld, pnop, "", {0}},  /* no decode args */
    {0, 1, "rollu", rollu, pnop, "", {0}},  /* no decode args */
    {0, 1, "shl", shl, dec1d, "", {0}},  /* "r" */
    {0, 1, "shr", shr, dec1d, "", {0}},  /* "r" */
    {0, 1, "slicexit", slicexit, pnop, "", {0}},  /* no decode args */
    {0, 1, "split", split, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "stup", stup, pnop, "", {0}},  /* no decode args */
    {0, 1, "stdn", stdn, pnop, "", {0}},  /* no decode args */
    {0, 1, "sub", add, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "sub2", add, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "subAAC", add, dec1d2s, "aac", {0}},  /* "bac" */
    {0, 1, "subBAC", add, dec1d2s, "bac", {0}},  /* "bac" */
    {0, 1, "subCAB", add, dec1d2s, "cab", {0}},  /* "cab" */
    {0, 1, "subCBA", add, dec1d2s, "cba", {0}},  /* "cba" */
    {0, 1, "subCCD", add, dec1d2s, "ccd", {0}},  /* "ccd" */
#ifdef NET
#ifdef USE_PORT
    {0, 1, "surf", migrate, dec2s, "", {0}},  /* "r" */
    {0, 1, "surff", migrate, dec2s, "", {0}},  /* "r" */
#else /* USE_PORT */
    {0, 1, "surf", migrate, dec1s, "", {0}},  /* "r" */
    {0, 1, "surff", migrate, dec1s, "", {0}},  /* "r" */
#endif /* USE_PORT */
#endif /* NET */
    {0, 1, "togbf", togbf, pnop, "", {0}},  /* no decode args */
    {0, 1, "togdf", togdf, pnop, "", {0}},  /* no decode args */
    {0, 1, "toger", toger, pnop, "", {0}},  /* no decode args */
    {0, 1, "togsr", togsr, pnop, "", {0}},  /* no decode args */
    {0, 1, "togdr", togdr, pnop, "", {0}},  /* no decode args */
    {0, 1, "ttime", ttime, dec1d, "", {0}},  /* no decode args */
    {0, 1, "xor", xor, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "xor2", xor, dec1d2s, "", {0}},  /* "rrr" */
    {0, 1, "zero", movdd, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "zeroD", movdd, dec1d1s, "dd", {0}},  /* "dd" */
#ifdef IO
    {0, 1, "get", get, dec1d1s, "", {0}},  /* "r" */
    {0, 1, "put", put, dec1d1s, "", {0}},  /* "rr" */
    {0, 1, "puticc", puticc, pputicc, "", {0}},  /* "rr" */
#endif                          /* IO */
#if PLOIDY > 1
    {0, 1, "trso", trso, pnop, "", {0}},  /* no decode args */
    {0, 1, "trde", trde, pnop, "", {0}},  /* no decode args */
    {0, 1, "trex", trex, pnop, "", {0}},  /* no decode args */
#else                           /* PLOIDY > 1 */
    {0, 1, "trso", NULL, NULL, "", {0}},
    {0, 1, "trde", NULL, NULL, "", {0}},
    {0, 1, "trex", NULL, NULL, "", {0}},
#endif                          /* PLOIDY > 1 */
#ifdef SHADOW
    {0, 1, "A", regorder, dec1s, "a", {0}},  /* "a" */
    {0, 1, "B", regorder, dec1s, "b", {0}},  /* "b" */
    {0, 1, "C", regorder, dec1s, "c", {0}},  /* "c" */
    {0, 1, "D", regorder, dec1s, "d", {0}},  /* "d" */
#else                           /* SHADOW */
    {0, 1, "A", NULL, NULL, "", {0}},
    {0, 1, "B", NULL, NULL, "", {0}},
    {0, 1, "C", NULL, NULL, "", {0}},
    {0, 1, "D", NULL, NULL, "", {0}},
#endif                          /* SHADOW */
#ifdef NET
#ifdef USE_PORT
    {0, 1, "tpings", tpingsnd, dec2s, "", {0}},  /* "r" */
#else /* USE_PORT */
    {0, 1, "tpings", tpingsnd, dec1s, "", {0}},  /* "r" */
#endif /* USE_PORT */
    {0, 1, "tpingr", tpingrec, dec1d, "", {0}},  /* "r" */
    {0, 1, "getip", getip, dec1d, "", {0}},  /* "r" */
    {0, 1, "getipp", getipp, dec1d, "", {0}},  /* "r" */
    {0, 1, "getippf", getipp, dec1d, "", {0}},  /* "r" */
#endif                          /* NET */
    {0, 1, "END", NULL, NULL, "", {0}}  /* this line must be last! */
};
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)
I32s hangup = 0;   /* 0 = exit on error, 1 = hangup on error for debugging */
double MinComSizRat = 0.41; /* >= MinComSizRat : same narrow tissue */
double LifeCycFrct = -1.0;  /* taev_instP<= LifeCycFrct :     */
                            /* list gene as executed in       */
                            /* selected portion of life cycle */
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(PROBE)||defined(ARGTIE)
I32s GeneBnker = 0;   /* turn genebanker on and off */
I32s NumCells = 1;    /* # of creatures and gaps used to inoculate new soup */
#endif /* defined(TIERRA)||defined(PROBE)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)
I32s XDRBufMaxSize = 200000; /* max XDR encode/decode buffer size */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#endif /* SOUP_IN_H */
