#ifndef CLSTRSETUP_H
#define CLSTRSETUP_H

/* observational parameters: */

#ifndef DECVAX
I8s map_fnC[40] = "MapFileC"; /* mapfile for Internet, local cluster */
I8s map_fnS[40] = "MapFileS"; /* mapfile for Internet, cluster servers */
#else /* DECVAX */
I8s map_fnC[40] = "MapFileC."; /* mapfile for Internet, local cluster */
I8s map_fnS[40] = "MapFileS."; /* mapfile for Internet, cluster servers */
#endif /* DECVAX */

I16u LocalPort = 18001; /*  Port number for local node */

#ifdef __MWERKS__
I8s MacHost[80] = "nohost";  /* word used to define host on macintosh */
I8s MacSite[80] = "nosite";  /* word used to define site on macintosh */
#endif /* __MWERKS__ */

/* environmental variables: */

I32s TieMsgBkLog=200; /* maximum message backlog */
I32s TieMsgMaxAge=60; /* maximum message age */

#if NETTYPE == UDP || NETTYPE == UDPASM
I32s TieMTU=1500;      /* default MTU */
I32s PktSndDelay=0;
#endif /* NETTYPE == UDP || NETTYPE == UDPASM */

#if NETTYPE == TCP
I32s TCPLocSelTmeSec=0;         /* local TCP select timeout in  sec  */
I32s TCPLocSelTmeUSec=100000;   /*                          and usec */
I32s TCPRemSelTmeSec=5;         /* remote TCP select timeout in  sec */
I32s TCPRemSelTmeUSec=0;        /*                          and usec */
#endif /* NETTYPE == TCP */

I32s OnLineStat=1; /* online ? 0=no, 1=yes */
I32s SubNetCnt=1; /* number of subnets */
I32s TieSubNet=0; /* subnet number */
I32s BasSubNetChgFrq=0; /* frequency of subnet change */
float SubNetChgFrqRanFrac=(float)0.0; /* random fraction of BasSubNetChgFrq */
float SubNetChgFrqFixFrac=(float)0.0; /* fixed fraction of BasSubNetChgFrq */
double SubNetChgApocProb=0.9; /* subnet transition apocalypse prob */
float SrvrOutBndFrac=(float)0.1; /* fraction of MaxOutBandWidth */
                                 /* allocated to server */
float TieMinOutBndFrac=(float)0.1; /* fraction of MaxOutBandWidth allocated */
                                   /* for minimum outgoing bandwidth of */
                                   /* tierra nodes */
I32s PendReqTime = 86400; /* pending TPing request time before node removal */
I32s SpdZeroTime = 60; /* max time before Speed zero */
I32s MaxOutBandWidth=-1; /* maximum band width available to server */
I32s IntrvlOutBndCap=60; /* bandwidth capacity reset interval */
I32s ServerSleep=1; /* cluster server sleep interval */
I32s XDRBufMaxSize = 200000; /* max XDR encode/decode buffer size */

#ifdef __MWERKS__
I8s GenebankPath[80] = ":gb-Net:";  /* path for genebanker output */
#else /* __MWERKS__ */
I8s GenebankPath[80] = "gb-Net/";   /* path for genebanker output */
#endif /* __MWERKS__ */

I32s ClsIPSUpdInt=60; /* cluster IPMapS update interval */

I32s ApocalypseFreq = 0; /* frequency of apocalypse, in millions */
float ApocFixFrac = (float)0.0; /* fixed fraction of apocalypse frequency */
float ApocRanFrac = (float)2.0; /* random fraction of apocalypse frequency */
I32s SrvrApocSleep=60; /* sleep time after server triggered apocalype */
I32s FirstSrvrApocWait=60; /* time before 1st server Apocalypse */

I32s TierraLog = 0; /* 0 = no log file, 1 = write tierra.log file to disk */
I32s PingDatLog = 0;   /*save log of ping data in ping.dat */
I32s new_soup=1; /* 0 = restart, 1 = fresh start */
I32s TierraNice = 19;        /* Nice value for cpu */
I32s AutoAddIPMap = 1; /* automatically add new nodes to IPMap array ? */
                       /* 0=no, 1=yes */
I32s AutoRemIPMap = 1; /* automatically remove inactive or offline nodes */
                       /* from IPMap array ?  0=no, 1=yes */
I32s NumCells = 1;    /* # of creatures and gaps used to inoculate new soup */

#endif /* CLSTRSETUP_H */
