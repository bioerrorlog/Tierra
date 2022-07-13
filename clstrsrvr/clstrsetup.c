#include "tierra.h"
#include "globals.h"

I8s GetAVar(data, alterflag, MonReq, buflen)
I8s *data;
I32s alterflag, MonReq, buflen;
{   I32s rtncode, fldwdth;
    char fmtbuf[MAXPRINTFMTSIZ], *fldnam;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

    rtncode=0;
    switch (*data)
    {   
    case 'A':
        if (!strncmp(data, "ApocalypseFreq", 14))
        {   if (MonReq)
            {   strcpy(vqu.name, "ApocalypseFreq");
                tsprintf(vqu.value, "%d", ApocalypseFreq);
            }
            else if (alterflag)
                sscanf(data, "ApocalypseFreq = %d", &ApocalypseFreq);
            vqu.type = 'i';
            vqu.i = ApocalypseFreq;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "ApocFixFrac", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "ApocFixFrac");
                tsprintf(vqu.value, "%f", ApocFixFrac);
            }
            else if (alterflag)
                sscanf(data, "ApocFixFrac = %f", &ApocFixFrac);
            vqu.type = 'f';
            vqu.f = ApocFixFrac;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "ApocRanFrac", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "ApocRanFrac");
                tsprintf(vqu.value, "%f", ApocRanFrac);
            }
            else if (alterflag)
                sscanf(data, "ApocRanFrac = %f", &ApocRanFrac);
            vqu.type = 'f';
            vqu.f = ApocRanFrac;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "AutoAddIPMap", 12))
        {   if (MonReq)
            {   strcpy(vqu.name, "AutoAddIPMap");
                tsprintf(vqu.value, "%d", AutoAddIPMap);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "AutoAddIPMap = %d", &AutoAddIPMap);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.h = AutoAddIPMap;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "AutoRemIPMap", 12))
        {   if (MonReq)
            {   strcpy(vqu.name, "AutoRemIPMap");
                tsprintf(vqu.value, "%d", AutoRemIPMap);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "AutoRemIPMap = %d", &AutoRemIPMap);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.h = AutoRemIPMap;
            rtncode=1;
            break;
        }
        break;
    case 'B':
        if (!strncmp(data, "BasSubNetChgFrq", 15))
        {   if (MonReq)
            {   strcpy(vqu.name, "BasSubNetChgFrq");
                tsprintf(vqu.value, "%d", BasSubNetChgFrq);
            }
            else if (alterflag)
                sscanf(data, "BasSubNetChgFrq = %d", &BasSubNetChgFrq);
            vqu.type = 'i';
            vqu.i = BasSubNetChgFrq;
            rtncode=1;
            break;
        }
        break;
    case 'C':
        if (!strncmp(data, "ClsIPSUpdInt", 12))
        {   if (alterflag)
                sscanf(data, "ClsIPSUpdInt = %d", &ClsIPSUpdInt);
            vqu.type = 'i';
            vqu.i = ClsIPSUpdInt;
            rtncode=1;
            break;
        }
        break;
    case 'F':
        if (!strncmp(data, "FirstSrvrApocWait", 17))
        {   if (alterflag)
                sscanf(data, "FirstSrvrApocWait = %d", &FirstSrvrApocWait);
            vqu.type = 'i';
            vqu.i = FirstSrvrApocWait;
            rtncode=1;
            break;
        }
        break;
    case 'G':
        fldnam="GenebankPath";
        if (!strncmp(data, fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(vqu.name, fldnam);
                tsprintf(vqu.value, "%s", GenebankPath);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(GenebankPath)<buflen)?
                        sizeof(GenebankPath):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf(data, &(fmtbuf[0]), GenebankPath);
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 's';
            vqu.s = GenebankPath;
            rtncode=1;
            break;
        }
        break;
    case 'H':
#ifdef __MWERKS__
        if (!strncmp(data, "MacHost", 7))
        {   if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "MacHost = %s", &MacHost);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 's';
            vqu.s = MacHost;
            rtncode=1;
            break;
        }
#endif /* __MWERKS__ */
        break;
    case 'I':
        if (!strncmp(data, "IntrvlOutBndCap", 15))
        {   if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "IntrvlOutBndCap = %d", &IntrvlOutBndCap);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = IntrvlOutBndCap;
            rtncode=1;
            break;
        }
        break;
    case 'L':
        if (!strncmp(data, "LocalPort", 9))
        {   if (MonReq)
            {   strcpy(vqu.name, "LocalPort");
                tsprintf(vqu.value, "%hu", LocalPort);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "LocalPort = %hu", &LocalPort);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'h';
            vqu.h = LocalPort;
            rtncode=1;
            break;
        }
        break;
    case 'M':
        if (!strncmp(data, "MaxOutBandWidth", 12))
        {   if (alterflag)
                sscanf(data, "MaxOutBandWidth = %d", &MaxOutBandWidth);
            vqu.type = 'i';
            vqu.i = MaxOutBandWidth;
            rtncode=1;
            break;
        }
        break;
    case 'm':
        fldnam="map_fnC";
        if (!strncmp(data, fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(vqu.name, fldnam);
                tsprintf(vqu.value, "%s", map_fnC);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(map_fnC)<buflen)?
                        sizeof(map_fnC):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf(data, &(fmtbuf[0]), map_fnC);
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 's';
            vqu.s = map_fnC;
            rtncode=1;
            break;
        }
        fldnam="map_fnS";
        if (!strncmp(data, fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(vqu.name, fldnam);
                tsprintf(vqu.value, "%s", map_fnS);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(map_fnS)<buflen)?
                        sizeof(map_fnS):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf(data, &(fmtbuf[0]), map_fnS);
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 's';
            vqu.s = map_fnS;
            rtncode=1;
            break;
        }
        break;
    case 'n':
        if (!strncmp(data, "new_soup", 8))
        {   if (MonReq)
            {   strcpy(vqu.name, "new_soup");
                tsprintf(vqu.value, "%d", new_soup);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "new_soup = %d", &new_soup);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = new_soup;
            rtncode=1;
            break;
        }
        break;
    case 'O':
        if (!strncmp(data, "OnLineStat", 10))
        {   if (MonReq)
            {   strcpy(vqu.name, "OnLineStat");
                tsprintf(vqu.value, "%d", OnLineStat);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "OnLineStat = %d", &OnLineStat);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = OnLineStat;
            rtncode=1;
            break;
        }
        break;
    case 'P':
        if (!strncmp(data, "PendReqTime", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "PendReqTime");
                tsprintf(vqu.value, "%d", PendReqTime);
            }
            else if (alterflag)
                sscanf(data, "PendReqTime = %d", &PendReqTime);
            vqu.type = 'i';
            vqu.i = PendReqTime;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "PingDatLog", 10))
        {   if (MonReq)
            {   strcpy(vqu.name, "PingDatLog");
                tsprintf(vqu.value, "%d", PingDatLog);
            }
            else if (alterflag)
                sscanf(data, "PingDatLog = %d", &PingDatLog);
            vqu.type = 'i';
            vqu.i = PingDatLog;
            rtncode=1;
            break;
        }
#ifdef NET
#if NETTYPE == UDPASM
        if (!strncmp(data, "PktSndDelay", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "PktSndDelay");
                tsprintf(vqu.value, "%d", PktSndDelay);
            }
            else if (alterflag)
                sscanf(data, "PktSndDelay = %d", &PktSndDelay);
            vqu.type = 'i';
            vqu.i = PktSndDelay;
            rtncode=1;
            break;
        }
#endif /* NETTYPE == UDPASM */
#endif /* NET */
        break;
    case 'S':
        if (!strncmp(data, "SubNetCnt", 9))
        {   if (MonReq)
            {   strcpy(vqu.name, "SubNetCnt");
                tsprintf(vqu.value, "%d", SubNetCnt);
            }
            else if (alterflag)
            {   sscanf(data, "SubNetCnt = %d", &SubNetCnt);
                SubNetCnt=(SubNetCnt>0)?SubNetCnt:0;
            }
            vqu.type = 'i';
            vqu.i = SubNetCnt;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SubNetChgFrqRanFrac", 19))
        {   if (MonReq)
            {   strcpy(vqu.name, "SubNetChgFrqRanFrac");
                tsprintf(vqu.value, "%f", SubNetChgFrqRanFrac);
            }
            else if (alterflag)
                sscanf(data, "SubNetChgFrqRanFrac = %f", &SubNetChgFrqRanFrac);
            vqu.type = 'f';
            vqu.f = SubNetChgFrqRanFrac;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SubNetChgFrqFixFrac", 19))
        {   if (MonReq)
            {   strcpy(vqu.name, "SubNetChgFrqFixFrac");
                tsprintf(vqu.value, "%f", SubNetChgFrqFixFrac);
            }
            else if (alterflag)
                sscanf(data, "SubNetChgFrqFixFrac = %f", &SubNetChgFrqFixFrac);
            vqu.type = 'f';
            vqu.f = SubNetChgFrqFixFrac;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SubNetChgApocProb", 17))
        {   if (MonReq)
            {   strcpy(vqu.name, "SubNetChgApocProb");
                tsprintf(vqu.value, "%f", SubNetChgApocProb);
            }
            else if (alterflag)
                sscanf(data, "SubNetChgApocProb = %lf",
                    &SubNetChgApocProb);
            vqu.type = 'f';
            vqu.f = (float)SubNetChgApocProb;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "ServerSleep", 11))
        {   if (alterflag)
                sscanf(data, "ServerSleep = %d", &ServerSleep);
            vqu.type = 'i';
            vqu.i = ServerSleep;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SpdZeroTime", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "SpdZeroTime");
                tsprintf(vqu.value, "%d", SpdZeroTime);
            }
            else if (alterflag)
                sscanf(data, "SpdZeroTime = %d", &SpdZeroTime);
            vqu.type = 'i';
            vqu.i = SpdZeroTime;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SrvrApocSleep", 13))
        {   if (MonReq)
            {   strcpy(vqu.name, "SrvrApocSleep");
                tsprintf(vqu.value, "%d", SrvrApocSleep);
            }
            else if (alterflag)
                sscanf(data, "SrvrApocSleep = %d", &SrvrApocSleep);
            vqu.type = 'i';
            vqu.i = SrvrApocSleep;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "SrvrOutBndFrac", 14))
        {   if (alterflag)
                sscanf(data, "SrvrOutBndFrac = %f", &SrvrOutBndFrac);
            vqu.type = 'f';
            vqu.f = SrvrOutBndFrac;
            rtncode=1;
            break;
        }
        break;
    case 'T':
        if (!strncmp(data, "TierraLog", 9))
        {   if (MonReq)
            {   strcpy(vqu.name, "TierraLog");
                tsprintf(vqu.value, "%d", TierraLog);
            }
            else if (alterflag)
                sscanf(data, "TierraLog = %d", &TierraLog);
            vqu.type = 'i';
            vqu.i = TierraLog;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "TierraNice", 10))
        {   if (MonReq)
            {   strcpy(vqu.name, "TierraNice");
                tsprintf(vqu.value, "%d", TierraNice);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf(data, "TierraNice = %d", &TierraNice);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = TierraNice;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "TieMsgBkLog", 11))
        {   if (MonReq)
            {   strcpy(vqu.name, "TieMsgBkLog");
                tsprintf(vqu.value, "%d", TieMsgBkLog);
            }
            else if (alterflag)
                if (sscanf(data, "TieMsgBkLog = %d", &TieMsgBkLog))
                    globtcibuf.tci_queuesize=TieMsgBkLog;
            vqu.type = 'i';
            vqu.i = TieMsgBkLog;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "TieMsgMaxAge", 12))
        {   if (MonReq)
            {   strcpy(vqu.name, "TieMsgMaxAge");
                tsprintf(vqu.value, "%d", TieMsgMaxAge);
            }
            else if (alterflag)
                if (sscanf(data, "TieMsgMaxAge = %d", &TieMsgMaxAge))
                    globtcibuf.tci_maxmsgage=TieMsgMaxAge;
            vqu.type = 'i';
            vqu.i = TieMsgMaxAge;
            rtncode=1;
            break;
        }
        if (!strncmp(data, "TieSubNet", 9))
        {   if (MonReq)
            {   strcpy(vqu.name, "TieSubNet");
                tsprintf(vqu.value, "%d", TieSubNet);
            }
            else if (alterflag)
            {   if(alterflag==2)
                {   if(sscanf(data, "TieSubNet = %d", &TieSubNet))
                        TieSubNet=(SubNetCnt<2)?0:((TieSubNet>
                            (SubNetCnt-1))?(SubNetCnt-1):TieSubNet);
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = TieSubNet;
            rtncode=1;
            break;
        }
#if NETTYPE == UDPASM
        if (!strncmp(data, "TieMTU", 6))
        {   if (MonReq)
            {   strcpy(vqu.name, "TieMTU");
                tsprintf(vqu.value, "%d", TieMTU);
            }
            else if (alterflag)
                if(sscanf(data, "TieMTU = %d", &TieMTU))
                    globtcibuf.tci_pktmtu=TieMTU;

            vqu.type = 'i';
            vqu.i = TieMTU;
            rtncode=1;
            break;
        }
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP

        if (!strncmp(data, "TCPLocSelTmeSec", 15))
        {   if (MonReq)
            {   strcpy(vqu.name, "TCPLocSelTmeSec");
                tsprintf(vqu.value, "%d", TCPLocSelTmeSec);
            }
            else if (alterflag)
                if(sscanf(data, "TCPLocSelTmeSec = %d", &TCPLocSelTmeSec))
                    globtcibuf.tci_tcplocseltimeout.tv_sec=TCPLocSelTmeSec;
            vqu.type = 'i';
            vqu.i = TCPLocSelTmeSec;
            rtncode=1;
            break;
        }

        if (!strncmp(data, "TCPLocSelTmeUSec", 16))
        {   if (MonReq)
            {   strcpy(vqu.name, "TCPLocSelTmeUSec");
                tsprintf(vqu.value, "%d", TCPLocSelTmeUSec);
            }
            else if (alterflag)
                if(sscanf(data, "TCPLocSelTmeUSec = %d", &TCPLocSelTmeUSec))
                    globtcibuf.tci_tcplocseltimeout.tv_usec=TCPLocSelTmeUSec;
            vqu.type = 'i';
            vqu.i = TCPLocSelTmeUSec;
            rtncode=1;
            break;
        }

        if (!strncmp(data, "TCPRemSelTmeSec", 15))
        {   if (MonReq)
            {   strcpy(vqu.name, "TCPRemSelTmeSec");
                tsprintf(vqu.value, "%d", TCPRemSelTmeSec);
            }
            else if (alterflag)
                if(sscanf(data, "TCPRemSelTmeSec = %d", &TCPRemSelTmeSec))
                    globtcibuf.tci_tcplocseltimeout.tv_sec=TCPRemSelTmeSec;
            vqu.type = 'i';
            vqu.i = TCPRemSelTmeSec;
            rtncode=1;
            break;
        }

        if (!strncmp(data, "TCPRemSelTmeUSec", 16))
        {   if (MonReq)
            {   strcpy(vqu.name, "TCPRemSelTmeUSec");
                tsprintf(vqu.value, "%d", TCPRemSelTmeUSec);
            }
            else if (alterflag)
                if(sscanf(data, "TCPRemSelTmeUSec = %d", &TCPRemSelTmeUSec))
                    globtcibuf.tci_tcplocseltimeout.tv_usec=TCPRemSelTmeUSec;
            vqu.type = 'i';
            vqu.i = TCPRemSelTmeUSec;
            rtncode=1;
            break;
        }

#endif /* NETTYPE == TCP */
        if (!strncmp(data, "TieMinOutBndFrac", 16))
        {   if (alterflag)
                sscanf(data, "TieMinOutBndFrac = %f", &TieMinOutBndFrac);
            vqu.type = 'f';
            vqu.f = TieMinOutBndFrac;
            rtncode=1;
            break;
        }
        break;
    case 'X':
        if (!strncmp(data, "XDRBufMaxSize", 13))
        {   if (MonReq)
            {   strcpy(vqu.name, "XDRBufMaxSize");
                tsprintf(vqu.value, "%d", XDRBufMaxSize);
            }
            else if (alterflag)
                sscanf(data, "XDRBufMaxSize = %d", &XDRBufMaxSize);
            vqu.type = 'i';
            vqu.i = XDRBufMaxSize;
            rtncode=1;
            break;
        }
        break;
    default:
        rtncode=0;
        break;
    }

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return (I8s)rtncode;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void clstrsrvrsetup(argc, argv)
    I32s argc;
    I8s *argv[];
{   FILE *inf;
    I32s i, feofrtn;
#ifdef _WIN32
    int gethostnameSTAT, gethostnameERRORTIE;
    WORD wVersionRequested;
    WSADATA WSAData;
#endif /* _WIN32 */

    FILE *fp;
    struct hostent *LocalHost;
    struct in_addr LocalHostAddr;
#ifdef __MWERKS__
    char tmpbuf1[64], tmpbuf2[48];
    char addrstr[100];
    struct hostent *thisHost;
    struct in_addr addr;
#else /* __MWERKS__ */
    I8s tmpbuf1[39], tmpbuf2[39];
#endif /* __MWERKS__ */
    I8s LocalPortStr[7];
#ifdef AMIGA
    I8s *hostnamenv;
#endif /* AMIGA */
#ifdef SIGBLOCK       
    sigset_t SigSaveSet;  
#endif /* SIGBLOCK */
#ifdef SIGBLOCK
    TieSignal(SIGPIPE, SIG_IGN);
    TieSignal(SIGHUP, SIG_IGN);
    TieSignal(SIGALRM, SIG_IGN);
#endif /* SIGBLOCK */
#ifdef __TURBOC__
    timezone = (I32s) 5L *60L * 60L;
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
    SetSigSet();
#ifdef unix
    TieSignal(SIGINT, T_sig_int);
#endif /* unix */
#ifdef DECVAX
    TieSignal(SIGINT, T_sig_int);
#endif /* DECVAX */
#endif /* SIGBLOCK */

    if (argc > 1)
    {   tsprintf(soup_fn, "%s", argv[1]);
    }
    else
    {
#ifdef IBM3090
        tsprintf(soup_fn, "srvr_cfg.io.d");
#else /* IBM3090 */
        tsprintf(soup_fn, "srvr_cfg");
#endif /* IBM3090 */
    }
    inf = tfopen(soup_fn, "r");
    if (inf == NULL)
    {   tsprintf(Fbuf,
            "GetSoup() file %s not opened, exiting\n", soup_fn);
        FEError(-1908, EXIT, WRITE, Fbuf);
    }
    tfgets(Buff, 84, inf);
    while (1)
    {   if (*Buff != '#' && strlen(Buff) > 3 && !GetAVar(Buff, 2, 0, sizeof(Buff)))
        {   *(Buff + 40) = 0;
            tsprintf(Fbuf,
                "GetSoup() bad soup_in line: %s\n", Buff);
            FEError(-1909, NOEXIT, NOWRITE, Fbuf);
        }
        tfgets(Buff, 84, inf);

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        feofrtn=feof(inf);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if (feofrtn)
            break;
        if((((*Buff)>='0')&&((*Buff)<='9'))||
            (!strncmp("space", Buff, 5))||
            (!strncmp("center", Buff, 6)))
            break;
    }

    NumCells=0;

    tsrand(tietime(NULL));

    ToggleLog(TierraLog, &tfp_log, TIERRALOGNAME, &tielogpos);
    ToggleLog(PingDatLog, &ping_fp, TPINGLOGNAME, &tpinglogpos);
    tsprintf(Buff, "%s%s", GenebankPath, map_fnC);
    if ((fp = tfopen(&(Buff[0]), "r")) == NULL)
    {   tsprintf(Fbuf, "Node Machine Map file %s can't open\n", Buff);
        FEError(-1911, EXIT, WRITE, Fbuf);
    }
    GetIPMap(fp, 0);  /* fill in IPMap array */
    tfclose(fp);
    tsprintf(Buff, "%s%s", GenebankPath, map_fnS);
    if ((fp = tfopen(&(Buff[0]), "r")) == NULL)
    {   tsprintf(Fbuf, "Node Machine Map file %s can't open\n", Buff);
        FEError(-1911, EXIT, WRITE, Fbuf);
    }
    GetIPMap(fp, 1);  /* fill in IPMap array */
    tfclose(fp);
    LocalClusterAddr = IPMapS[0].address;
    ApocalypseNowFreq=((I32s)
        (ApocFixFrac*((double)ApocalypseFreq)))+
        (tlrand()%(((I32s)(ApocRanFrac*((double)
        ApocalypseFreq)))+1));

#ifdef _WIN32
    wVersionRequested=0x0101;
    if(WSAStartup(wVersionRequested, &WSAData))
    {   tsprintf(Fbuf, "Winsock initialization failure\n");
        FEError(-1917, EXIT, WRITE, Fbuf);
    }
#endif /* _WIN32 */


    /* get a LocalHostName */
    memset(tmpbuf1, 0, sizeof(tmpbuf1));
    memset(tmpbuf2, 0, sizeof(tmpbuf2));
#ifdef _WIN32
    gethostnameSTAT = tgethostname(tmpbuf1, sizeof(tmpbuf1));
    if (gethostnameSTAT)
    {   switch (gethostnameERRORTIE = WSAGetLastError())
        {   case WSAEFAULT:
            tfprintf(stderr,
            "gethostname: WSAEFAULT  The namelen parameter is too small\n");
            break;
        case WSANOTINITIALISED:
            tfprintf(stderr,
"gethostname: WSANOTINITIALISED A successful WSAStartup() must occur before using this API.\n");
            break;
        case WSAENETDOWN:
            tfprintf(stderr,
"gethostname: WSAENETDOWN The Windows Sockets implementation has detected that the network subsystem has failed.\n");
            break;
        case WSAEINPROGRESS:
            tfprintf(stderr,
"gethostname: WSAEINPROGRESS    A blocking Windows Sockets operation is in progress.\n");
            break;
        default:
            tfprintf(stderr, "gethostname: Mystery Error.\n");
        }
    }
#else /* _WIN32 */
#ifdef AMIGA
    if(hostnamenv=getenv("HOSTNAME"))
    {   strcpy(&(tmpbuf1[0]),hostnamenv);
        free(hostnamenv);
    }
#else /* AMIGA */
    tgethostname(tmpbuf1, sizeof(tmpbuf1));
#endif /* AMIGA */
#endif /* _WIN32 */
#ifndef AMIGA
#ifndef _WIN32
#ifndef DECVAX
    (void) getdomainname(tmpbuf2, sizeof(tmpbuf2));
#endif /* DECVAX */
#endif /* _WIN32 */
#endif /* AMIGA */
#ifdef __MWERKS__
    tsprintf(LocalHostName, "%s", tmpbuf1);
#else /* __MWERKS__ */
#ifdef _WIN32
    tsprintf(LocalHostName, "%s", tmpbuf1);
#else /* _WIN32 */
    tsprintf(LocalHostName, "%s.%s", tmpbuf1, tmpbuf2);
#endif /* _WIN32 */
#endif /* __MWERKS__ */

    tsprintf(Fbuf, "LocalHostName = %s\n", LocalHostName);
    FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
    if ((LocalHost = tgethostbyname(tmpbuf1)) == NULL)
    {   tsprintf(Fbuf, "Error tgethostbyname(tmpbuf1) = %s\n", tmpbuf1);
        FEError(-1914, EXIT, WRITE, Fbuf);
    }
    LocalHostAddr.s_addr = pr_inet((I8s**)
        (LocalHost->h_addr_list), (I32s)(LocalHost->h_length));
    LocalNetAddr.node = LocalHostAddr.s_addr;
    LocalNetAddr.portnb = htons(LocalPort);
#ifndef __MWERKS__
#ifndef _WIN32
#ifdef AMIGA
    Nfds = getdtablesize();
#else /* AMIGA */
    Nfds = open_max();
#endif /* AMIGA */
#endif /* _WIN32  */
#endif /* __MWERKS__ */
    TieCommOnOffLine(OnLineStat, 1);
    TieCommChgSubNet(0, 1, 1, 0);
    for(i=0;i<(I32s)NumNodesC;i++)
        IPMapC[i].PrivPingData.subnet=TieSubNet;
    for(i=0;i<(I32s)NumNodesS;i++)
        IPMapS[i].PrivPingData.subnet=TieSubNet;
    tsprintf(LocalPortStr, "%hu", LocalPort);
    tsprintf(Fbuf, "LocalPort=%hu\n", LocalPort);
    FEPrintf(MSG_X, PLN_Y, 0, 0, &(Fbuf[0]));
#ifndef __MWERKS__
#ifndef _WIN32
#endif /* _WIN32  */
#endif /* __MWERKS__ */

#ifndef __MWERKS__
#ifndef AMIGA
#ifndef _WIN32
    nice(TierraNice);
#endif /* _WIN32 */
#endif /* AMIGA */
#endif /* __MWERKS__ */

    tfclose(inf);

#ifdef __TURBOC__
    ctrlbrk(T_sig_int);
#endif /* __TURBOC__ */

#ifndef __GUI__
#if (INTERFACE == BASICC)||(INTERFACE == BASICA)
#ifdef TSysV
    nodelay(stdscr, FALSE);
#else /* TSysV */
#if INTERFACE == BASICA
    nodelay(stdscr, FALSE);
#endif /* INTERFACE == BASICA */
#endif /* TSysV */
#endif /* (INTERFACE == BASICC)||(INTERFACE == BASICA) */
#endif /* __GUI__ */

    new_soup=0;
}
