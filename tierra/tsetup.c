/* @(#)tsetup.c    1.4 11/25/94 */
/* tsetup.c 30-3-98 setup routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #19XX, in use: 1900-1907 */
/* FEError labels: #19XX, in use: 1908-1910, 1912-1917, 1920-1923, 1925-1931 */

#ifndef CLSTRSRVR

#include "license.h"
#include "tierra.h"
#include "globals.h"
#ifdef TIEAUDIO
#include "tieaudsrv.h"
#endif /* TIEAUDIO */

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

#ifdef LUNA
#define SEEK_SET   0
#define SEEK_END   2
#endif

I8s corefile[120];

/*
 * GetAVar - read or set Tierra parameter values
 *
 * data - pointer to parameter name
 * alterflag = 0 - examine parameter
 *             1 - alter parameter where permitted while running
 *             2 - run restart
 *
 * Returns:
 */
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
    vqu.type = '0';
    switch (*data)
    {   case 'a':
        if (!strncmp((const char *)data, (const char *)"alive", 5))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])), (const char *)"alive");
                tsprintf((char *)(&((vqu.value)[0])), "%d", alive);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"alive = %d", &alive);
            vqu.type = 'i';
            vqu.i = alive;
            rtncode=1;
            break;
        }
        break;
    case 'A':
        if(!strncmp((const char *)data,
           (const char *)"AbsSearchLimit", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AbsSearchLimit");
                tsprintf((char *)(&((vqu.value)[0])), "%d", AbsSearchLimit);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"AbsSearchLimit = %d", &AbsSearchLimit);
            vqu.type = 'i';
            vqu.i = AbsSearchLimit;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"AliveGen", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])), (const char *)"AliveGen");
                tsprintf((char *)(&((vqu.value)[0])), "%d", AliveGen);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"AliveGen = %d", &AliveGen);
            vqu.type = 'i';
            vqu.i = AliveGen;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data,
            (const char *)"ApocalypseFreq", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ApocalypseFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ApocalypseFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ApocalypseFreq = %d", &ApocalypseFreq);
            vqu.type = 'i';
            vqu.i = ApocalypseFreq;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"ApocFixFrac", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ApocFixFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", ApocFixFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ApocFixFrac = %f", &ApocFixFrac);
            vqu.type = 'f';
            vqu.f = ApocFixFrac;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"ApocRanFrac", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ApocRanFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", ApocRanFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ApocRanFrac = %f", &ApocRanFrac);
            vqu.type = 'f';
            vqu.f = ApocRanFrac;
            rtncode=1;
            break;
        }
#endif /* NET */

#ifdef TIEAUDIO
        if (!strncmp((const char *)data,
            (const char *)"AudConnProcFreq", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AudConnProcFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", AudConnProcFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"AudConnProcFreq = %d", &AudConnProcFreq);
            vqu.type = 'i';
            vqu.i = AudConnProcFreq;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"AudioPopThresh", 14))
        {   if (MonReq)                                       
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AudioPopThresh");                 
                tsprintf((char *)(&((vqu.value)[0])), "%d", AudioPopThresh);
            }                                                 
            else if (alterflag)                               
            {   sscanf((const char *)data,
                    (const char *)"AudioPopThresh = %d", &AudioPopThresh);
            }                                                 
            vqu.type = 'i';                                   
            vqu.i = AudioPopThresh;                                 
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"AudioPort", 9))
        {   if (MonReq)                                       
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AudioPort");                 
                tsprintf((char *)(&((vqu.value)[0])), "%d", AudioPort);
            }                                                 
            else if (alterflag)                               
            {   sscanf((const char *)data,
                    (const char *)"AudioPort = %d", &AudioPort);
            }                                                 
            vqu.type = 'i';                                   
            vqu.i = AudioPort;                                 
            rtncode=1;
            break;
        }
#endif /* TIEAUDIO */

#ifdef NET
        if (!strncmp((const char *)data,
            (const char *)"AutoAddIPMap", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AutoAddIPMap");
                tsprintf((char *)(&((vqu.value)[0])), "%d", AutoAddIPMap);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"AutoAddIPMap = %d", &AutoAddIPMap);
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
        if (!strncmp((const char *)data,
            (const char *)"AutoRemIPMap", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"AutoRemIPMap");
                tsprintf((char *)(&((vqu.value)[0])), "%d", AutoRemIPMap);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"AutoRemIPMap = %d", &AutoRemIPMap);
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
#endif /* NET */
        break;
    case 'd':
        if (!strncmp((const char *)data,
            (const char *)"debug", 5))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"debug");
                tsprintf((char *)(&((vqu.value)[0])), "%d", debug);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"debug = %d", &debug);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = debug;
            rtncode=1;
            break;
        }
        break;
    case 'h':
        if (!strncmp((const char *)data, (const char *)"hangup", 6))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"hangup");
                tsprintf((char *)(&((vqu.value)[0])), "%d", hangup);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"hangup = %d", &hangup);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = hangup;
            rtncode=1;
            break;
        }
        break;
    case 'n':
        if (!strncmp((const char *)data, (const char *)"new_soup", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"new_soup");
                tsprintf((char *)(&((vqu.value)[0])), "%d", new_soup);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"new_soup = %d", &new_soup);
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
    case 's':
        if (!strncmp((const char *)data, (const char *)"seed", 4))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"seed");
                tsprintf((char *)(&((vqu.value)[0])), "%d", seed);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"seed = %d", &seed);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = seed;
            rtncode=1;
            break;
        }
        break;
    case 'B':
#ifdef NET
        if (!strncmp((const char *)data,
            (const char *)"BklogProcFreq", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BklogProcFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BklogProcFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BklogProcFreq = %d", &BklogProcFreq);
            vqu.type = 'i';
            vqu.i = BklogProcFreq;
            rtncode=1;
            break;
        }
#endif /* NET */
        if (!strncmp((const char *)data, (const char *)"BrkupSiz", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BrkupSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BrkupSiz);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"BrkupSiz = %d", &BrkupSiz);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = BrkupSiz;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data,
            (const char *)"GetIPPStrtIdxC", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GetIPPStrtIdxC");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GetIPPStrtIdxC);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GetIPPStrtIdxC = %d", &GetIPPStrtIdxC);
            vqu.type = 'i';
            vqu.i = GetIPPStrtIdxC;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"GetIPPStrtIdxS", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GetIPPStrtIdxS");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GetIPPStrtIdxS);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GetIPPStrtIdxS = %d", &GetIPPStrtIdxS);
            vqu.type = 'i';
            vqu.i = GetIPPStrtIdxS;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"BrkIPPStopIdxC", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BrkIPPStopIdxC");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BrkIPPStopIdxC);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BrkIPPStopIdxC = %d", &BrkIPPStopIdxC);
            vqu.type = 'i';
            vqu.i = BrkIPPStopIdxC;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"BrkIPPStopIdxS", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BrkIPPStopIdxS");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BrkIPPStopIdxS);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BrkIPPStopIdxS = %d", &BrkIPPStopIdxS);
            vqu.type = 'i';
            vqu.i = BrkIPPStopIdxS;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"BrkIPPStrtIdxC", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BrkIPPStrtIdxC");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BrkIPPStrtIdxC);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BrkIPPStrtIdxC = %d", &BrkIPPStrtIdxC);
            vqu.type = 'i';
            vqu.i = BrkIPPStrtIdxC;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"BrkIPPStrtIdxS", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BrkIPPStrtIdxS");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BrkIPPStrtIdxS);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BrkIPPStrtIdxS = %d", &BrkIPPStrtIdxS);
            vqu.type = 'i';
            vqu.i = BrkIPPStrtIdxS;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"BasSubNetChgFrq", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"BasSubNetChgFrq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", BasSubNetChgFrq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"BasSubNetChgFrq = %d", &BasSubNetChgFrq);
            vqu.type = 'i';
            vqu.i = BasSubNetChgFrq;
            rtncode=1;
            break;
        }
#endif /* NET */
        break;
    case 'C':
        if (!strncmp((const char *)data,
            (const char *)"CpuLoadLimitPeriod", 18))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"CpuLoadLimitPeriod");
                tsprintf((char *)(&((vqu.value)[0])),
                    "%d", CpuLoadLimitPeriod);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"CpuLoadLimitPeriod = %d",
                        &CpuLoadLimitPeriod);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = CpuLoadLimitPeriod;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"CpuLoadLimitProp", 16))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"CpuLoadLimitProp");
                tsprintf((char *)(&((vqu.value)[0])),
                    "%f", CpuLoadLimitProp);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"CpuLoadLimitProp = %lf",
                    &CpuLoadLimitProp);
            vqu.type = 'd';
            vqu.d = CpuLoadLimitProp;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"CumGeneBnk", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"CumGeneBnk");
                tsprintf((char *)(&((vqu.value)[0])), "%d", CumGeneBnk);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"CumGeneBnk = %d", &CumGeneBnk);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = CumGeneBnk;
            rtncode=1;
            break;
        }
        break;

    case 'D':
        if (!strncmp((const char *)data,
            (const char *)"DeconstructOn", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DeconstructOn");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DeconstructOn);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DeconstructOn = %d",
                    &DeconstructOn);
            vqu.type = 'i';
            vqu.i = DeconstructOn;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DiskBank", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DiskBank");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DiskBank);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DiskBank = %d", &DiskBank);
            vqu.type = 'i';
            vqu.i = DiskBank;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DiskOut", 7))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DiskOut");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DiskOut);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DiskOut = %d", &DiskOut);
            vqu.type = 'i';
            vqu.i = DiskOut;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DistFreq", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DistFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%f", DistFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DistFreq = %f", &DistFreq);
            vqu.type = 'f';
            vqu.f = DistFreq;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DistProp", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DistProp");
                tsprintf((char *)(&((vqu.value)[0])), "%f", DistProp);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DistProp = %f", &DistProp);
            vqu.type = 'f';
            vqu.f = DistProp;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DivSameSiz", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DivSameSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DivSameSiz);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DivSameSiz = %d", &DivSameSiz);
            vqu.type = 'i';
            vqu.i = DivSameSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DivSameGen", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DivSameGen");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DivSameGen);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DivSameGen = %d", &DivSameGen);
            vqu.type = 'i';
            vqu.i = DivSameGen;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DropDead", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DropDead");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DropDead);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"DropDead = %d", &DropDead);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = DropDead;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DistNext.m", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DistNext.m");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DistNext.m);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DistNext.m = %d", &(DistNext.m));
            vqu.type = 'i';
            vqu.i = DistNext.m;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DistNext.i", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DistNext.i");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DistNext.i);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DistNext.i = %d", &(DistNext.i));
            vqu.type = 'i';
            vqu.i = DistNext.i;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"DeadMemInit", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"DeadMemInit");
                tsprintf((char *)(&((vqu.value)[0])), "%d", DeadMemInit);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"DeadMemInit = %d",
                    &DeadMemInit);
            vqu.type = 'i';
            vqu.i = DeadMemInit;
            rtncode=1;
            break;
        }
        break;
    case 'E':
        if (!strncmp((const char *)data, (const char *)"EjectRate", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"EjectRate");
                tsprintf((char *)(&((vqu.value)[0])), "%d", EjectRate);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"EjectRate = %d", &EjectRate);
            vqu.type = 'i';
            vqu.i = EjectRate;
            rtncode=1;
            break;
        }
        break;
    case 'F':
        if (!strncmp((const char *)data, (const char *)"FindTimeM", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"FindTimeM");
                tsprintf((char *)(&((vqu.value)[0])), "%d", FindTimeM);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"FindTimeM = %d", &FindTimeM);
            vqu.type = 'i';
            vqu.i = FindTimeM;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"FindTimeI", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"FindTimeI");
                tsprintf((char *)(&((vqu.value)[0])), "%d", FindTimeI);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"FindTimeI = %d", &FindTimeI);
            vqu.type = 'i';
            vqu.i = FindTimeI;
            rtncode=1;
            break;
        }
        break;
    case 'G':
        if (!strncmp((const char *)data, (const char *)"GeneBnker", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GeneBnker");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GeneBnker);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GeneBnker = %d", &GeneBnker);
            vqu.type = 'i';
            vqu.i = GeneBnker;
            rtncode=1;
            break;
        }
        fldnam="GenebankPath";
        if (!strncmp((const char *)data, (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", GenebankPath);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(GenebankPath)<buflen)?
                        sizeof(GenebankPath):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf((const char *)data,
                        (const char *)(&(fmtbuf[0])), GenebankPath);
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
        if (!strncmp((const char *)data, (const char *)"GenPerBkgMut", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerBkgMut");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerBkgMut);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerBkgMut = %d", &GenPerBkgMut);
            vqu.type = 'i';
            vqu.i = GenPerBkgMut;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerFlaw", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerFlaw");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerFlaw);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerFlaw = %d", &GenPerFlaw);
            vqu.type = 'i';
            vqu.i = GenPerFlaw;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerMovMut", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerMovMut");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerMovMut);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerMovMut = %d", &GenPerMovMut);
            vqu.type = 'i';
            vqu.i = GenPerMovMut;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerDivMut", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerDivMut");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerDivMut);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerDivMut = %d", &GenPerDivMut);
            vqu.type = 'i';
            vqu.i = GenPerDivMut;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data,
            (const char *)"GenPerCroInsSamSiz", 18))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerCroInsSamSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerCroInsSamSiz);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerCroInsSamSiz = %d",
                    &GenPerCroInsSamSiz);
            vqu.type = 'i';
            vqu.i = GenPerCroInsSamSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerInsIns", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerInsIns");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerInsIns);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerInsIns = %d", &GenPerInsIns);
            vqu.type = 'i';
            vqu.i = GenPerInsIns;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerDelIns", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerDelIns");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerDelIns);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerDelIns = %d", &GenPerDelIns);
            vqu.type = 'i';
            vqu.i = GenPerDelIns;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerCroIns", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerCroIns");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerCroIns);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerCroIns = %d", &GenPerCroIns);
            vqu.type = 'i';
            vqu.i = GenPerCroIns;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerDelSeg", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerDelSeg");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerDelSeg);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerDelSeg = %d", &GenPerDelSeg);
            vqu.type = 'i';
            vqu.i = GenPerDelSeg;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerInsSeg", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerInsSeg");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerInsSeg);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerInsSeg = %d", &GenPerInsSeg);
            vqu.type = 'i';
            vqu.i = GenPerInsSeg;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GenPerCroSeg", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GenPerCroSeg");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GenPerCroSeg);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GenPerCroSeg = %d", &GenPerCroSeg);
            vqu.type = 'i';
            vqu.i = GenPerCroSeg;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"GetIPPStrtIdxC", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GetIPPStrtIdxC");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GetIPPStrtIdxC);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GetIPPStrtIdxC = %d", &GetIPPStrtIdxC);
            vqu.type = 'i';
            vqu.i = GetIPPStrtIdxC;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"GetIPPStrtIdxS", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"GetIPPStrtIdxS");
                tsprintf((char *)(&((vqu.value)[0])), "%d", GetIPPStrtIdxS);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"GetIPPStrtIdxS = %d", &GetIPPStrtIdxS);
            vqu.type = 'i';
            vqu.i = GetIPPStrtIdxS;
            rtncode=1;
            break;
        }
#endif /* NET */
        break;
    case 'H':
#ifdef __MWERKS__
#ifdef NET
        fldnam="MacHost";
        if (!strncmp((const char *)data,
            (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", MacHost);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(MacHost)<buflen)?
                        sizeof(MacHost):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf((const char *)data,
                        (const char *)(&(fmtbuf[0])), MacHost);
                }
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
#endif /* NET */
#endif /* __MWERKS__ */
        break;
    case 'L':
        if (!strncmp((const char *)data, (const char *)"LazyTol", 7))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"LazyTol");
                tsprintf((char *)(&((vqu.value)[0])), "%d", LazyTol);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"LazyTol = %d", &LazyTol);
            vqu.type = 'i';
            vqu.i = LazyTol;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"LifeCycFrct", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"LifeCycFrct");
                tsprintf((char *)(&((vqu.value)[0])), "%f", LifeCycFrct);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"LifeCycFrct = %lf", &LifeCycFrct);
            vqu.type = 'd';
            vqu.d = LifeCycFrct;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"LocalPort", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"LocalPort");
                tsprintf((char *)(&((vqu.value)[0])), "%hu", LocalPort);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"LocalPort = %hu", &LocalPort);
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
#endif /* NET */
        break;
    case 'I':
        fldnam="IMapFile";
        if (!strncmp((const char *)data,
            (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", IMapFile);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(IMapFile)<buflen)?
                        sizeof(IMapFile):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf((const char *)data,
                        (const char *)(&(fmtbuf[0])), IMapFile);
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 's';
            vqu.s = IMapFile;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"ImmigLimFrac", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ImmigLimFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", ImmigLimFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ImmigLimFrac = %f", &ImmigLimFrac);
            vqu.type = 'f';
            vqu.f = ImmigLimFrac;
            rtncode=1;
            break;
        }
#endif /* NET */
#if FRONTEND == BASIC
        if (!strncmp((const char *)data, (const char *)"InstExeUpdFreq", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"InstExeUpdFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", InstExeUpdFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"InstExeUpdFreq = %d", &InstExeUpdFreq);
            vqu.type = 'i';
            vqu.i = InstExeUpdFreq;
            rtncode=1;
            break;
        }
#endif /* FRONTEND == BASIC */
        break;
    case 'J':
        if (!strncmp((const char *)data, (const char *)"JmpSouTra", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"JmpSouTra");
                tsprintf((char *)(&((vqu.value)[0])), "%f", JmpSouTra);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"JmpSouTra = %f", &JmpSouTra);
            vqu.type = 'f';
            vqu.f = JmpSouTra;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"JumpTrackProb", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"JumpTrackProb");
                tsprintf((char *)(&((vqu.value)[0])), "%f", JumpTrackProb);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"JumpTrackProb = %f", &JumpTrackProb);
            vqu.type = 'f';
            vqu.f = JumpTrackProb;
            rtncode=1;
            break;
        }
        break;
    case 'M':
        if (!strncmp((const char *)data, (const char *)"MalMode", 7))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MalMode");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MalMode);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MalMode = %d", &MalMode);
            vqu.type = 'i';
            vqu.i = MalMode;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MalReapTol", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MalReapTol");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MalReapTol);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MalReapTol = %d", &MalReapTol);
            vqu.type = 'i';
            vqu.i = MalReapTol;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MalSamSiz", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MalSamSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MalSamSiz);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MalSamSiz = %d", &MalSamSiz);
            vqu.type = 'i';
            vqu.i = MalSamSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MalTol", 6))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MalTol");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MalTol);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MalTol = %d", &MalTol);
            vqu.type = 'i';
            vqu.i = MalTol;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MateSizeEp", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MateSizeEp");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MateSizeEp);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MateSizeEp = %d", &MateSizeEp);
            vqu.type = 'i';
            vqu.i = MateSizeEp;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxCpuPerCell", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxCpuPerCell");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxCpuPerCell);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MaxCpuPerCell = %d", &MaxCpuPerCell);
            vqu.type = 'i';
            vqu.i = MaxCpuPerCell;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxIOBufSiz", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxIOBufSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxIOBufSiz);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   sscanf((const char *)data,
                        (const char *)"MaxIOBufSiz = %d", &MaxIOBufSiz);
                    if (MaxIOBufSiz < 2)
                        MaxIOBufSiz = 2;
                }
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MaxIOBufSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxGetBufSiz", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxGetBufSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxGetBufSiz);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MaxGetBufSiz = %d", &MaxGetBufSiz);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MaxGetBufSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxPutBufSiz", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxPutBufSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxPutBufSiz);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MaxPutBufSiz = %d", &MaxPutBufSiz);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MaxPutBufSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxSigBufSiz", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxSigBufSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxSigBufSiz);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MaxSigBufSiz = %d", &MaxSigBufSiz);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MaxSigBufSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxFreeBlocks", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxFreeBlocks");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxFreeBlocks);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MaxFreeBlocks = %d", &MaxFreeBlocks);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MaxFreeBlocks;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MemModeFree", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MemModeFree");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MemModeFree);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MemModeFree = %d", &MemModeFree);
            vqu.type = 'i';
            vqu.i = MemModeFree;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MemModeMine", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MemModeMine");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MemModeMine);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MemModeMine = %d", &MemModeMine);
            vqu.type = 'i';
            vqu.i = MemModeMine;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MemModeProt", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MemModeProt");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MemModeProt);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MemModeProt = %d", &MemModeProt);
            vqu.type = 'i';
            vqu.i = MemModeProt;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MaxMalMult", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxMalMult");
                tsprintf((char *)(&((vqu.value)[0])), "%f", MaxMalMult);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MaxMalMult = %f", &MaxMalMult);
            vqu.type = 'f';
            vqu.f = MaxMalMult;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MinCellSize", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinCellSize");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MinCellSize);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MinCellSize = %d", &MinCellSize);
            vqu.type = 'i';
            vqu.i = MinCellSize;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MinComSizRat", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinComSizRat");
                tsprintf((char *)(&((vqu.value)[0])), "%f", MinComSizRat);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MinComSizRat = %lf", &MinComSizRat);
            vqu.type = 'd';
            vqu.d = MinComSizRat;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MinGenMemSiz", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinGenMemSiz");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MinGenMemSiz);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MinGenMemSiz = %d", &MinGenMemSiz);
            vqu.type = 'i';
            vqu.i = MinGenMemSiz;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MinSpeed", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinSpeed");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MinSpeed);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MinSpeed = %d", &MinSpeed);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = MinSpeed;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((const char *)data,
            (const char *)"MinSubNetApoRat", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinSubNetApoRat");
                tsprintf((char *)(&((vqu.value)[0])),
                    "%f", MinSubNetApoRat);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MinSubNetApoRat = %lf",
                    &MinSubNetApoRat);
            vqu.type = 'd';
            vqu.d = MinSubNetApoRat;
            rtncode=1;
            break;
        }
#endif /* NET */
        if (!strncmp((const char *)data, (const char *)"MinTemplSize ", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinTemplSize");
                tsprintf((char *)(&((vqu.value)[0])), "%d", MinTemplSize);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MinTemplSize = %d", &MinTemplSize);
            vqu.type = 'i';
            vqu.i = MinTemplSize;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MovPropThrDiv", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MovPropThrDiv");
                tsprintf((char *)(&((vqu.value)[0])), "%f", MovPropThrDiv);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MovPropThrDiv = %f", &MovPropThrDiv);
            vqu.type = 'f';
            vqu.f = MovPropThrDiv;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MutBitProp", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MutBitProp");
                tsprintf((char *)(&((vqu.value)[0])), "%f", MutBitProp);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"MutBitProp = %f", &MutBitProp);
            vqu.type = 'f';
            vqu.f = MutBitProp;
            rtncode=1;
            break;
        }

        if (!strncmp((const char *)data, (const char *)"MinSoupSize", 11))
        {   if (MonReq)                                       
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MinSoupSize");                 
                tsprintf((char *)(&((vqu.value)[0])), "%d", MinSoupSize);          
            }                                                 
            else if (alterflag)                               
            {   if (alterflag == 2)                           
                    sscanf((const char *)data,
                        (const char *)"MinSoupSize = %d", &MinSoupSize);
                else                                          
                {   rtncode=0;
                    break;
                }
            }                                                 
            vqu.type = 'i';                                   
            vqu.i = MinSoupSize;                                 
            rtncode=1;
            break;
        }

        if (!strncmp((const char *)data, (const char *)"MaxSoupSize", 11))
        {   if (MonReq)                                       
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MaxSoupSize");                 
                tsprintf((char *)(&((vqu.value)[0])), "%d", MaxSoupSize);
            }                                                 
            else if (alterflag)                               
            {   if (alterflag == 2)                           
                    sscanf((const char *)data,
                        (const char *)"MaxSoupSize = %d", &MaxSoupSize);
                else                                          
                {   rtncode=0;
                    break;
                }
            }                                                 
            vqu.type = 'i';                                   
            vqu.i = MaxSoupSize;                                 
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MonPort", 7))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MonPort");
                tsprintf((char *)(&((vqu.value)[0])), "%hu", MonPort);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MonPort = %hu", &MonPort);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'h';
            vqu.h = MonPort;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"MigrCtrlPort", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"MigrCtrlPort");
                tsprintf((char *)(&((vqu.value)[0])), "%hu", MigrCtrlPort);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"MigrCtrlPort = %hu", &MigrCtrlPort);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'h';
            vqu.h = MigrCtrlPort;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"M_tvWait_sec", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"M_tvWait_sec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", M_tvWait_sec);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"M_tvWait_sec = %d", &M_tvWait_sec);
            vqu.type = 'i';
            vqu.i = M_tvWait_sec;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"M_OV_threshold", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"M_OV_threshold");
                tsprintf((char *)(&((vqu.value)[0])), "%d", M_OV_threshold);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"M_OV_threshold = %d", &M_OV_threshold);
            vqu.type = 'i';
            vqu.i = M_OV_threshold;
            rtncode=1;
            break;
        }
        break;
    case 'm':
#ifdef NET
        fldnam="map_fnC";
        if (!strncmp((const char *)data, (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", map_fnC);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(map_fnC)<buflen)?
                        sizeof(map_fnC):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf((const char *)data,
                        (const char *)(&(fmtbuf[0])), map_fnC);
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
        if (!strncmp((const char *)data,
            (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", map_fnS);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                {   fldwdth=(sizeof(map_fnS)<buflen)?
                        sizeof(map_fnS):buflen;
                    tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                        fldnam, "%", fldwdth, "s");
                    sscanf((const char *)data,
                        (const char *)(&(fmtbuf[0])), map_fnS);
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
#endif /* NET */
        break;
    case 'N':
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"NetRcvUpdFreq", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"NetRcvUpdFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", NetRcvUpdFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"NetRcvUpdFreq = %d", &NetRcvUpdFreq);
            vqu.type = 'i';
            vqu.i = NetRcvUpdFreq;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"NodeSelSucRatPrec", 17))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"NodeSelSucRatPrec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", NodeSelSucRatPrec);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"NodeSelSucRatPrec = %d",
                    &NodeSelSucRatPrec);
            vqu.type = 'i';
            vqu.i = NodeSelSucRatPrec;
            rtncode=1;
            break;
        }
#endif /* NET */
        if (!strncmp((const char *)data, (const char *)"NumCellsMin", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"NumCellsMin");
                tsprintf((char *)(&((vqu.value)[0])), "%d", NumCellsMin);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"NumCellsMin = %d", &NumCellsMin);
            vqu.type = 'i';
            vqu.i = NumCellsMin;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"NumCells", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"NumCells");
                tsprintf((char *)(&((vqu.value)[0])), "%d", NumCells);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"NumCells = %d", &NumCells);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = NumCells;
            rtncode=1;
            break;
        }
        break;
    case 'O':
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"OnLineStat", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"OnLineStat");
                tsprintf((char *)(&((vqu.value)[0])), "%d", OnLineStat);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"OnLineStat = %d", &OnLineStat);
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
#endif /* NET */
        break;
    case 'P':
#ifdef NET
        if (!strncmp((const char *)data, (const char *)"PendReqMax", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PendReqMax");
                tsprintf((char *)(&((vqu.value)[0])), "%d", PendReqMax);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PendReqMax = %d", &PendReqMax);
            vqu.type = 'i';
            vqu.i = PendReqMax;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"PendReqTime", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PendReqTime");
                tsprintf((char *)(&((vqu.value)[0])), "%d", PendReqTime);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PendReqTime = %d", &PendReqTime);
            vqu.type = 'i';
            vqu.i = PendReqTime;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"PingDatLog", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PingDatLog");
                tsprintf((char *)(&((vqu.value)[0])), "%d", PingDatLog);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PingDatLog = %d", &PingDatLog);
            vqu.type = 'i';
            vqu.i = PingDatLog;
            rtncode=1;
            break;
        }
#endif /* NET */
        if (!strncmp((const char *)data, (const char *)"PhotonPow", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PhotonPow");
                tsprintf((char *)(&((vqu.value)[0])), "%f", PhotonPow);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PhotonPow = %lf", &PhotonPow);
            vqu.type = 'd';
            vqu.d = PhotonPow;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"PhotonWidth", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PhotonWidth");
                tsprintf((char *)(&((vqu.value)[0])), "%d", PhotonWidth);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PhotonWidth = %d", &PhotonWidth);
            vqu.type = 'i';
            vqu.i = PhotonWidth;
            rtncode=1;
            break;
        }
        fldnam="PhotonWord";
        if (!strncmp((const char *)data, (const char *)fldnam, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", PhotonWord);
            }
            else if (alterflag)
            {   fldwdth=(sizeof(PhotonWord)<buflen)?
                    sizeof(PhotonWord):buflen;
                tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                    fldnam, "%", fldwdth, "s");
                sscanf((const char *)data,
                    (const char *)(&(fmtbuf[0])), PhotonWord);
            }
            vqu.type = 's';
            vqu.s = PhotonWord;
            rtncode=1;
            break;
        }
#ifdef NET
#if NETTYPE == UDPASM
        if (!strncmp((const char *)data, (const char *)"PktSndDelay", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PktSndDelay((char *)&(");
                tsprintf((char *)(&((vqu.value)[0])), "%d", PktSndDelay);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PktSndDelay = %d", &PktSndDelay);
            vqu.type = 'i';
            vqu.i = PktSndDelay;
            rtncode=1;
            break;
        }
#endif /* NETTYPE == UDPASM */
#endif /* NET */
        if (!strncmp((const char *)data, (const char *)"PutLimit", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"PutLimit");
                tsprintf((char *)(&((vqu.value)[0])), "%f", PutLimit);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"PutLimit = %f", &PutLimit);
            vqu.type = 'f';
            vqu.f = PutLimit;
            rtncode=1;
            break;
        }
        break;
    case 'R':
        if (!strncmp((const char *)data, (const char *)"ReapRndProp", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ReapRndProp");
                tsprintf((char *)(&((vqu.value)[0])), "%f", ReapRndProp);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ReapRndProp = %f", &ReapRndProp);
            vqu.type = 'f';
            vqu.f = ReapRndProp;
            rtncode=1;
            break;
        }
        break;
    case 'S':
        if (!strncmp((const char *)data, (const char *)"SaveFreq", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SaveFreq");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SaveFreq);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SaveFreq = %d", &SaveFreq);
            vqu.type = 'i';
            vqu.i = SaveFreq;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SavRenewMem", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SavRenewMem");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SavRenewMem);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SavRenewMem = %d", &SavRenewMem);
            vqu.type = 'i';
            vqu.i = SavRenewMem;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SavMinNum", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SavMinNum");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SavMinNum);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SavMinNum = %d", &SavMinNum);
            vqu.type = 'i';
            vqu.i = SavMinNum;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SavThrMem", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SavThrMem");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SavThrMem);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SavThrMem = %f", &SavThrMem);
            vqu.type = 'f';
            vqu.f = SavThrMem;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SavThrPop", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SavThrPop");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SavThrPop);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SavThrPop = %f", &SavThrPop);
            vqu.type = 'f';
            vqu.f = SavThrPop;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SearchLimit", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SearchLimit");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SearchLimit);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SearchLimit = %f", &SearchLimit);
            vqu.type = 'f';
            vqu.f = SearchLimit;
            rtncode=1;
            break;
        }
        if (!strncmp((const char *)data, (const char *)"SizDepSlice", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SizDepSlice");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SizDepSlice);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SizDepSlice = %d", &SizDepSlice);
            vqu.type = 'i';
            vqu.i = SizDepSlice;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SlicePow", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SlicePow");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SlicePow);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SlicePow = %lf", &SlicePow);
            vqu.type = 'd';
            vqu.d = SlicePow;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SliceSize", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SliceSize");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SliceSize);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SliceSize = %d", &SliceSize);
            vqu.type = 'i';
            vqu.i = SliceSize;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SliceStyle", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SliceStyle");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SliceStyle);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SliceStyle = %d", &SliceStyle);
            vqu.type = 'i';
            vqu.i = SliceStyle;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SlicFixFrac", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SlicFixFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SlicFixFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SlicFixFrac = %f", &SlicFixFrac);
            vqu.type = 'f';
            vqu.f = SlicFixFrac;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SlicRanFrac", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SlicRanFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SlicRanFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SlicRanFrac = %f", &SlicRanFrac);
            vqu.type = 'f';
            vqu.f = SlicRanFrac;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SoupSize", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SoupSize");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SoupSize);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"SoupSize = %d", &SoupSize);
                else
                {   rtncode=0;
                    break;
                }
            }
            vqu.type = 'i';
            vqu.i = SoupSize;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SpeedUpdate", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SpeedUpdate");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SpeedUpdate);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SpeedUpdate = %d", &SpeedUpdate);
            vqu.type = 'i';
            vqu.i = SpeedUpdate;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SplitTissueAna", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SplitTissueAna");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SplitTissueAna);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SplitTissueAna = %d", &SplitTissueAna);
            vqu.type = 'i';
            vqu.i = SplitTissueAna;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SouUpdTrk", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SouUpdTrk");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SouUpdTrk);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SouUpdTrk = %d", &SouUpdTrk);
            vqu.type = 'i';
            vqu.i = SouUpdTrk;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((char *)data, (char *)"SrvrApocSleep", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SrvrApocSleep");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SrvrApocSleep);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SrvrApocSleep = %d", &SrvrApocSleep);
            vqu.type = 'i';
            vqu.i = SrvrApocSleep;
            rtncode=1;
            break;
        }
#endif /* NET */
        if (!strncmp((char *)data, (char *)"StrictIP", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"StrictIP");
                tsprintf((char *)(&((vqu.value)[0])), "%d", StrictIP);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"StrictIP = %d", &StrictIP);
            vqu.type = 'i';
            vqu.i = StrictIP;
            rtncode=1;
            break;
        }
#ifdef NET
        if (!strncmp((char *)data, (char *)"SensMapAtt", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SensMapAtt");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SensMapAtt);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SensMapAtt = %d",
                    &SensMapAtt);
            vqu.type = 'i';
            vqu.i = SensMapAtt;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SpdZeroTime", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SpdZeroTime");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SpdZeroTime);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SpdZeroTime = %d", &SpdZeroTime);
            vqu.type = 'i';
            vqu.i = SpdZeroTime;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SubNetCnt", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SubNetCnt");
                tsprintf((char *)(&((vqu.value)[0])), "%d", SubNetCnt);
            }
            else if (alterflag)
            {   sscanf((const char *)data,
                    (const char *)"SubNetCnt = %d", &SubNetCnt);
                SubNetCnt=(SubNetCnt>0)?SubNetCnt:0;
            }
            vqu.type = 'i';
            vqu.i = SubNetCnt;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SubNetChgFrqRanFrac", 19))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SubNetChgFrqRanFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SubNetChgFrqRanFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SubNetChgFrqRanFrac = %f",
                    &SubNetChgFrqRanFrac);
            vqu.type = 'f';
            vqu.f = SubNetChgFrqRanFrac;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SubNetChgFrqFixFrac", 19))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SubNetChgFrqFixFrac");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SubNetChgFrqFixFrac);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SubNetChgFrqFixFrac = %f",
                    &SubNetChgFrqFixFrac);
            vqu.type = 'f';
            vqu.f = SubNetChgFrqFixFrac;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"SubNetChgApocProb", 17))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"SubNetChgApocProb");
                tsprintf((char *)(&((vqu.value)[0])), "%f", SubNetChgApocProb);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"SubNetChgApocProb = %lf",
                    &SubNetChgApocProb);
            vqu.type = 'f';
            vqu.f = (float)SubNetChgApocProb;
            rtncode=1;
            break;
        }
#endif /* NET */
#ifdef __MWERKS__
#ifdef NET
        fldnam="MacSite";
        if (!strncmp((char *)data, fldna(char *)m, strlen(fldnam)))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)fldnam);
                tsprintf((char *)(&((vqu.value)[0])), "%s", MacSite);
            }
            else if (alterflag)
            {   fldwdth=(sizeof(MacSite)<buflen)?
                    sizeof(MacSite):buflen;
                tsprintf(&(fmtbuf[0]),"%s = %s%d%s",
                    fldnam, "%", fldwdth, "s");
                sscanf((const char *)data,
                    (const char *)(&(fmtbuf[0])), MacSite);
            }
            vqu.type = 's';
            vqu.s = MacSite;
            rtncode=1;
            break;
        }
#endif /* NET */
#endif /* __MWERKS__ */
        break;
    case 'T':
        if (!strncmp((char *)data, (char *)"ThreadAnalysis", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ThreadAnalysis");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ThreadAnalysis);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"ThreadAnalysis = %d",
                    &ThreadAnalysis);
            vqu.type = 'i';
            vqu.i = ThreadAnalysis;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"ThrdAnaMaxFec", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ThrdAnaMaxFec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ThrdAnaMaxFec);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                   (const char *)"ThrdAnaMaxFec = %d",
                   &ThrdAnaMaxFec))
                   if((alterflag!=2)&&ThreadAnalysis)
                       ThrdAnaDatDump();
            vqu.type = 'i';
            vqu.i = ThrdAnaMaxFec;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"ThrdAnaPartSave", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ThrdAnaPartSave");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ThrdAnaPartSave);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"ThrdAnaPartSave = %d",
                    &ThrdAnaPartSave))
                   if((alterflag!=2)&&ThreadAnalysis)
                       ThrdAnaDatDump();
            vqu.type = 'i';
            vqu.i = ThrdAnaPartSave;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"ThrdAnaStop", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ThrdAnaStop");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ThrdAnaStop);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                   (const char *)"ThrdAnaStop = %d",
                   &ThrdAnaStop))
                   if((alterflag!=2)&&ThreadAnalysis)
                       ThrdAnaDatDump();
            vqu.type = 'i';
            vqu.i = ThrdAnaStop;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"ThrdAnaTrmCode", 14))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"ThrdAnaTrmCode");
                tsprintf((char *)(&((vqu.value)[0])), "%d", ThrdAnaTrmCode);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                   (const char *)"ThrdAnaTrmCode = %d",
                   &ThrdAnaTrmCode))
                   if((alterflag!=2)&&ThreadAnalysis)
                       ThrdAnaDatDump();
            vqu.type = 'i';
            vqu.i = ThrdAnaTrmCode;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"TierraLog", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TierraLog");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TierraLog);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"TierraLog = %d", &TierraLog);
            vqu.type = 'i';
            vqu.i = TierraLog;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"TierraSleep", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TierraSleep");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TierraSleep);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"TierraSleep = %d", &TierraSleep);
            vqu.type = 'i';
            vqu.i = TierraSleep;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"TierraNice", 10))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TierraNice");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TierraNice);
            }
            else if (alterflag)
            {   if (alterflag == 2)
                    sscanf((const char *)data,
                        (const char *)"TierraNice = %d", &TierraNice);
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
#ifdef NET
        if (!strncmp((char *)data, (char *)"TieMsgBkLog", 11))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TieMsgBkLog");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TieMsgBkLog);
            }
            else if (alterflag)
                if (sscanf((const char *)data,
                    (const char *)"TieMsgBkLog = %d",
                    &TieMsgBkLog))
                    globtcibuf.tci_queuesize=TieMsgBkLog;
            vqu.type = 'i';
            vqu.i = TieMsgBkLog;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"TieMsgMaxAge", 12))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TieMsgMaxAge");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TieMsgMaxAge);
            }
            else if (alterflag)
                if (sscanf((const char *)data,
                    (const char *)"TieMsgMaxAge = %d",
                    &TieMsgMaxAge))
                    globtcibuf.tci_maxmsgage=TieMsgMaxAge;
            vqu.type = 'i';
            vqu.i = TieMsgMaxAge;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"TieSubNet", 9))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TieSubNet");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TieSubNet);
            }
            else if (alterflag)
            {   if((alterflag==2)||(!(LocalClusterAddr.node)))
                {   if(sscanf((const char *)data,
                        (const char *)"TieSubNet = %d", &TieSubNet))
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
        if (!strncmp((char *)data, (char *)"TieMTU", 6))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TieMTU");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TieMTU);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"TieMTU = %d", &TieMTU))
                    globtcibuf.tci_pktmtu=TieMTU;

            vqu.type = 'i';
            vqu.i = TieMTU;
            rtncode=1;
            break;
        }
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP

        if (!strncmp((char *)data, (char *)"TCPLocSelTmeSec", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TCPLocSelTmeSec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TCPLocSelTmeSec);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"TCPLocSelTmeSec = %d",
                    &TCPLocSelTmeSec))
                    globtcibuf.tci_tcplocseltimeout.tv_sec=TCPLocSelTmeSec;
            vqu.type = 'i';
            vqu.i = TCPLocSelTmeSec;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"TCPLocSelTmeUSec", 16))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TCPLocSelTmeUSec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TCPLocSelTmeUSec);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"TCPLocSelTmeUSec = %d",
                    &TCPLocSelTmeUSec))
                    globtcibuf.tci_tcplocseltimeout.tv_usec=TCPLocSelTmeUSec;
            vqu.type = 'i';
            vqu.i = TCPLocSelTmeUSec;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"TCPRemSelTmeSec", 15))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TCPRemSelTmeSec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TCPRemSelTmeSec);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"TCPRemSelTmeSec = %d",
                    &TCPRemSelTmeSec))
                    globtcibuf.tci_tcplocseltimeout.tv_sec=TCPRemSelTmeSec;
            vqu.type = 'i';
            vqu.i = TCPRemSelTmeSec;
            rtncode=1;
            break;
        }

        if (!strncmp((char *)data, (char *)"TCPRemSelTmeUSec", 16))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TCPRemSelTmeUSec");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TCPRemSelTmeUSec);
            }
            else if (alterflag)
                if(sscanf((const char *)data,
                    (const char *)"TCPRemSelTmeUSec = %d",
                    &TCPRemSelTmeUSec))
                    globtcibuf.tci_tcplocseltimeout.tv_usec=TCPRemSelTmeUSec;
            vqu.type = 'i';
            vqu.i = TCPRemSelTmeUSec;
            rtncode=1;
            break;
        }

#endif /* NETTYPE == TCP */

#ifdef BGL
        if (!strncmp((char *)data, (char *)"TrackNormLike", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"TrackNormLike");
                tsprintf((char *)(&((vqu.value)[0])), "%d", TrackNormLike);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"TrackNormLike = %d", &TrackNormLike);
            vqu.type = 'i';
            vqu.i = TrackNormLike;
            rtncode=1;
            break;
        }
#endif /* BGL */

#endif /* NET */
        break;
    case 'W':
        if (!strncmp((char *)data, (char *)"WatchExe", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"WatchExe");
                tsprintf((char *)(&((vqu.value)[0])), "%d", WatchExe);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"WatchExe = %d", &WatchExe);
            vqu.type = 'i';
            vqu.i = WatchExe;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"WatchMov", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"WatchMov");
                tsprintf((char *)(&((vqu.value)[0])), "%d", WatchMov);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"WatchMov = %d", &WatchMov);
            vqu.type = 'i';
            vqu.i = WatchMov;
            rtncode=1;
            break;
        }
        if (!strncmp((char *)data, (char *)"WatchTem", 8))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"WatchTem");
                tsprintf((char *)(&((vqu.value)[0])), "%d", WatchTem);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"WatchTem = %d", &WatchTem);
            vqu.type = 'i';
            vqu.i = WatchTem;
            rtncode=1;
            break;
        }
        break;
    case 'X':
        if (!strncmp((char *)data, (char *)"XDRBufMaxSize", 13))
        {   if (MonReq)
            {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"XDRBufMaxSize");
                tsprintf((char *)(&((vqu.value)[0])), "%d", XDRBufMaxSize);
            }
            else if (alterflag)
                sscanf((const char *)data,
                    (const char *)"XDRBufMaxSize = %d", &XDRBufMaxSize);
            vqu.type = 'i';
            vqu.i = XDRBufMaxSize;
            rtncode=1;
            break;
        }
        break;
    default:
        if (MonReq)
        {   strcpy(((char *)&(vqu.name[0])),
                    (const char *)"----");
            tsprintf((char *)(&((vqu.value)[0])), "0");
        }
        vqu.type = '0';
        rtncode=0;
        break;
    }

#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    return (I8s)rtncode;
}

/*
 * initialize variables for Tierra
 */
void tieinitvar()
{   pos=tielogpos=reaped=ExtractCount=0;
    initrcnttardy();
    TimeStats=InstExe;
    CountMutRate=CountFlaw=CountMovMut=CountJmpSou=0;
    RateMovMut=RateMut=RateFlaw=RateJmpSou=AgeSum=0;
    NumGenDG=BirthNum=DeathNum=RepNum=FecunditySum=0;
    FreeBlocks=FirstOutDisk=FESpeed=1L;
    TimePop=Generations=RepInstEffSum=0.0;
    FecundityAvg=1.0;
    FEDeathNum=DeathNum=FEBirthNum=BirthNum=0L;
    FEEjectToSelf=EjectToSelf=0L;
    RepInstSum=histostrtidx=histoentcnt=histodsplnct=0;
    ShadowUsed=AverageSize=0;
    TierraClock=LastSpeedUpdate=tiestrtme;
    AvgPop=0.0;  /* average population over the last million instructions */
    GenInList=NULL;  /* pointers to soup_in genome names */
    Hist=NULL;  /* pointer to struct for histograms */
    HistSize=0;  /* Num of elements alloced in Hist */
    HistAlocSiz=0;  /* allocated size of Hist array, in bytes */
    HistNStars=(float)0.0;  /* ratio of counts to stars */
    GenInBuf=0;  /* buffer containing soup_in genome names */
    GoDown=0; /* flag to bring system down to defragment memory */
    HistPrint=0;  /* boolean to tell us weather to print hists, to log */
    Nop0=0;   /* instruct num corresponding to nop0 */
    Nop1=1;   /* instruct num corresponding to nop1 */
    NopS=1;   /* Nop0 + Nop1 sum of Nops for template search */
    siz_sl=0; /* allocated size of *sl array */
    sl=NULL;  /* list of unique size classes, number of gts */
    GeneBnkUsage=0;  /* count of gene bank users. */
    EffDaught=0;
    HistSortOrder=0;
    BrkupCou=1;

#ifdef __TURBOC__
    _stklen = 32767;
#endif /* __TURBOC__ */

#ifdef MICRO
    MC_step = -1L;
    soupviewstrt = 0;
    dbgdisptype = 0;
#endif /* MICRO */

#ifdef NET
    RemOutBndCap=-1;
    SrvSubNetSet=SrvrApocFlag=ClusterServerUp=0;
    MigrationTrackCount=0;
    LastBklogProc=tiestrtme;
    LstRcvTime=tiestrtme;
    FEBirthLocal=BirthLocal=0L;
    FEBirthInject=BirthInject=0L;
    FEBirthEject=BirthEject=0L;
    FEFecundYesSum=FecundYesSum=0L;
    FEFecunEjecSum=FecunEjecSum=0L;
    tmpl.n=0; /* list of .tmp files */
    tmpl.l=NULL;  /* list of .tmp files */
    PipmapC=0;    /* global pointer into IPMapC array */
    PipmapS=0;    /* global pointer into IPMapS array */
    tvWait.tv_sec=0;  /* time out for select */
    tvWait.tv_usec=0;
    NetWriteSoupFlg=0;
#ifdef BGL
    MigrBufReset();
#endif /* BGL */
#endif /* NET */

#ifdef MEM_PROF
    InitMemProf();
#endif /* MEM_PROF */

#ifdef TIEAUDIO
    LastAudConnProc=tiestrtme;
#endif /* TIEAUDIO */

    enc_size_precomp();
}

/*
 * GetSoup - run startup
 * argc - from program invocation argc
 * argv - from program invocation argv
 */
void GetSoup(argc, argv)
    int argc;
    char *argv[];
{   FILE *inf;
    I32s AlocGenInList=0, AlocGenInBuf=0, feofrtn, lrandval, svNumCells;
    I32u i;
#ifdef _WIN32
#ifdef NET
    int gethostnameSTAT, gethostnameERRORTIE;
#endif /* NET */
#if defined(NET)||defined(BGL)||defined(TIEAUDIO)
    WORD wVersionRequested;
    WSADATA WSAData;
#endif /* defined(NET)||defined(BGL) */
#endif /* _WIN32 */

#ifdef NET
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
#ifndef BGL
    I32s QLEN = 5;
    I8s MonPortStr[7];
#endif /* BGL */
#endif /* __MWERKS__ */
    I8s LocalPortStr[7];
#endif /* NET */
#ifdef SIGBLOCK       
    sigset_t SigSaveSet;  
#endif /* SIGBLOCK */
    char fmtbuf[MAXPRINTFMTSIZ];

#ifdef SIGBLOCK
    TieSignal(SIGPIPE, SIG_IGN);
    TieSignal(SIGHUP, SIG_IGN);
    TieSignal(SIGALRM, SIG_IGN);
#endif /* SIGBLOCK */

#ifdef __TURBOC__
    timezone = (I32s) 5L *60L * 60L;
#endif /* __TURBOC__ */

#ifdef SIGBLOCK
#ifdef unix
    TieSignal(SIGINT, T_sig_int);
    TieSignal(SIGUSR1, SIG_IGN);
    TieSignal(SIGUSR2, SIG_IGN);
    TieSignal(SIGHUP, SIG_IGN);
#endif /* unix */
#ifdef DECVAX
    TieSignal(SIGINT, T_sig_int);
#endif /* DECVAX */
#endif /* SIGBLOCK */

    if (argc > 1)
    {   tsprintf((char *)(&(soup_fn[0])), "%s", argv[1]);
    }
    else
    {
#ifdef IBM3090
        tsprintf((char *)(&(soup_fn[0])), "soup_in.io.d");
#else /* IBM3090 */
        tsprintf((char *)(&(soup_fn[0])), "soup_in");
#endif /* IBM3090 */
    }
    inf = tfopen((I8s *)(&(soup_fn[0])), (I8s *)"r");
    if (inf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra GetSoup() file %s not opened, exiting\n", soup_fn);
        FEError(-1908, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    tfgets(Buff, 84, inf);
    while (1)
    {   if (*Buff != '#' && strlen((const char *)(&(Buff[0]))) > 3 &&
            !GetAVar(Buff, 2, 0, sizeof(Buff)))
        {   *(Buff + 40) = 0;
            tsprintf((char *)(&(Fbuf[0])),
                "Tierra GetSoup() bad soup_in line: %s\n", Buff);
            FEError(-1909, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
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
            (!strncmp((const char *)"space",
                (const char *)(&(Buff[0])), 5))||
            (!strncmp((const char *)"center",
                (const char *)(&(Buff[0])), 6)))
            break;
    }

#ifdef _WIN32
    Win32Startup();
#endif /* _WIN32 */

    Speed=(MinSpeed<ARBLRGSPEED)?ARBLRGSPEED:MinSpeed;
    tsprintf((char *)(&(Fbuf[0])), "%s%s", GenebankPath, "instrec");
    sgfile=tfopen(&(Fbuf[0]), (I8s *)"w");
    if (sgfile == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "instrec file %s not opened, exiting\n", soup_fn);
        FEError(-1916, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }

    tfprintf(sgfile,
        "instruc offset  thrdid  Cpu#  #Cpus  Inst    InstP    RanID\n");
    tfprintf(sgfile, "------- ------- ------- -----");
    tfprintf(sgfile, " ----- ------- ------- ---------- ----- -----\n");

    svNumCells=NumCells;
    NumCells=0;
    if(GeneBnker||ThreadAnalysis)
    {   if(new_soup)
           GeneBnkerOvrd=1;
        OpenGeneBanker();
    }
    else
    {   if(new_soup)
           GeneBnkerOvrd=0;
        CloseGeneBanker();
    }
    NumCells=svNumCells;
    if(ThreadAnalysis)
        ThrdAnaTimeLastOn=tiestrtme;
    if(TierraSleep)
    {   CpuLoadLimitRunTime=(I32s)(CpuLoadLimitProp*
            ((double)CpuLoadLimitPeriod));
        CpuLoadLimitSleepTime=(I32s)((1.0-CpuLoadLimitProp)*
            ((double)CpuLoadLimitPeriod));
    }

#if FRONTEND == BASIC
    InstExeUpdNow=InstExeUpdFreq;
#endif /* FRONTEND == BASIC */

    if (strncmp((const char *)(&(IMapFile[0])),
        (const char *)"-.map", 5))  /* not the default inst map */
    {   tsprintf((char *)(&(Buf85b[0])), "%s%s", GenebankPath, IMapFile);
        GetAMap((I8s *)(&(Buf85b[0])));
    }
    if (!seed)
    {   seed = (I32s) tietime(NULL);
        tsrand(seed);
        seed = tlrand();
        if (!new_soup)
        {   tsrand(seed);
            lrandval=tlrand();
        }
    }
    if (new_soup)
    {   tsrand(seed);
        lrandval=tlrand();
        tsprintf((char *)(&(Fbuf[0])), "seed = %d\n", seed);
        FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
        if (NumCells)
        {   GenInList = (I8s **) tcalloc((I32u)
                NumCells, sizeof(I8s *), 1904);
            AlocGenInList = NumCells * sizeof(I8s *);
            GenInBuf = (I8s *) tcalloc((I32u)
                NumCells * 30, sizeof(I8s), 1905);
            AlocGenInBuf = NumCells * 30 * sizeof(I8s);
            for (i = 0; i < (I32u)NumCells; i++)
            {   GenInList[i] = GenInBuf + (i * 30);

#ifdef SIGBLOCK
                SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
                tsprintf(&(fmtbuf[0]),"%s%d%s", "%",
                    (30<sizeof(Buff))?30: sizeof(Buff), "[^\n]");
                sscanf((const char *)(&(Buff[0])),
                    (const char *)(&(fmtbuf[0])), GenInList[i]);
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

                tfgets(Buff, 84, inf);
            }
        }
    }

    /* allocate arrays */
    tsprintf((char *)(&(Fbuf[0])),
        "sizeof(Instruction)   = %d\n", sizeof(Instruction));
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "sizeof(TCell)   = %d\n", sizeof(TCell));
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])), "sizeof(MemFr) = %d\n", sizeof(MemFr));
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
#if (defined __TURBOC__) && (!defined __GUI__)
    tsprintf((char *)(&(Fbuf[0])), "coreleft = %lu\n",  coreleft());
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /* (defined __TURBOC__) && (!defined __GUI__) */
    if (new_soup)
    {   if (!SoupSize)
            SoupSize=MinSoupSize+((I32s)                  
                (((MaxSoupSize-MinSoupSize)+1)*tdrand()));
        soup = (HpInst) thcalloc((I32u) InstrXdrWrdSize(SoupSize),     
            sizeof(xdrwrd), 1906);
        tsprintf((char *)(&(Fbuf[0])), "%d bytes allocated for soup\n",
            SoupSize * sizeof(Instruction));
        FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
        CelArSiz = CellsSize = (I32s) 16384 / (I32s) sizeof(TCell);
        NumCelAr = 1;
        if (NumCells > CellsSize - 2)
        {   NumCelAr = (NumCells + 2) / CelArSiz;
            if ((NumCells + 2) % CelArSiz)
                NumCelAr++;
            CellsSize = NumCelAr * CelArSiz;
        }
        cells = (Pcells Fp) tcalloc(NumCelAr, sizeof(Pcells), 1900);
        for (i = 0; i < (I32u)NumCelAr; i++)
        {   cells[i] = (Pcells) tcalloc(CelArSiz, sizeof(TCell), 1901);
            InitThrdAnaDat(&(cells[i]->cell_thrdanadat));
        }
        tsprintf((char *)(&(Fbuf[0])), "%d bytes allocated for cells\n",
            (CellsSize * sizeof(TCell)) + (NumCelAr * sizeof(TCell)));
        FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
        TopDummy = &cells[0][0];
        BottomDummy = &cells[0][1];
        FreeMemry = (MemFr Fp) tcalloc(MaxFreeBlocks, sizeof(MemFr), 1907);
        tsprintf((char *)(&(Fbuf[0])), "%d bytes allocated for MemFr\n",
            MaxFreeBlocks * sizeof(MemFr));
        FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));

#if (defined __TURBOC__) && (!defined __GUI__)
        tsprintf((char *)(&(Fbuf[0])),
            "coreleft = %u  tsetup (soup, cells, FreeMemry)\n",
            coreleft());
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /* (defined __TURBOC__) && (!defined __GUI__) */
        if (!soup || !cells || !FreeMemry)
            FEError(-1910, EXIT, WRITE,
                "Tierra GetSoup() setup malloc error, exiting\n");
        else
        {   tsprintf((char *)(&(Fbuf[0])),
                "tsetup: arrays allocated without error\n");
            FEPrintf(MSG_X, PLN_Y, 0, 0, &(Fbuf[0]));
        }
    }
    TotFlaw = TotMovMut = TotMut = TotDivMut = TotCroInsSamSiz =
        TotInsIns = TotDelIns = TotCroIns = TotDelSeg = TotInsSeg =
        TotCroSeg = isolate = 0;
    extr.a = extr.i = 0;
    Disturb.m = Disturb.i = 0L;
    if (DivSameGen)
        DivSameSiz = 1;

#if defined(NET)||defined(BGL)||defined(TIEAUDIO)
#ifdef _WIN32
    wVersionRequested=0x0101;
    if(WSAStartup(wVersionRequested, &WSAData))
    {   tsprintf((char *)(&(Fbuf[0])), "Winsock initialization failure\n");
        FEError(-1917, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* _WIN32 */
#endif /* defined(NET)||defined(BGL) */

#ifdef TIEAUDIO
    OpnTieAud();
#endif /* TIEAUDIO */
    if (new_soup)
        GetNewSoup();
    else
        GetOldSoup(inf);
    ToggleLog(TierraLog, &tfp_log, (I8s *)TIERRALOGNAME, &tielogpos);
#ifdef NET
    ToggleLog(PingDatLog, &ping_fp, (I8s *)TPINGLOGNAME, &tpinglogpos);

    tsprintf((char *)(&(Buff[0])), "%s%s", GenebankPath, map_fnC);
    if ((fp = tfopen(&(Buff[0]), (I8s *)"r")) == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Node Machine Map file %s can't open\n", Buff);
        FEError(-1911, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    GetIPMap(fp, 0);  /* fill in IPMap array */
    tfclose(fp);
    LdIPMapTPingDat(0);

    tsprintf((char *)(&(Buff[0])), "%s%s", GenebankPath, map_fnS);
    if((fp=tfopen(&(Buff[0]), (I8s *)"r")))
    {   GetIPMap(fp, 1);  /* fill in IPMap array */
        tfclose(fp);
        LdIPMapTPingDat(1);
    }

    ApocalypseNowFreq=((I32s)(ApocFixFrac*((double)ApocalypseFreq)))+
        (tlrand()%(((I32s)(ApocRanFrac*((double)ApocalypseFreq)))+1));
    if(NumNodesS)
    {   LocalClusterAddr = IPMapS[0].address;
        ClusterServerUp=1;
    }
#endif /* NET */

#ifdef NET
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
    tgethostname(tmpbuf1, sizeof(tmpbuf1));
#endif /* _WIN32 */
#ifndef AMIGA
#ifndef _WIN32
#ifndef DECVAX
    getdomainname((char *)(&(tmpbuf2[0])), sizeof(tmpbuf2));
#endif /* DECVAX */
#endif /* _WIN32 */
#endif /* AMIGA */
#ifdef __MWERKS__
    tsprintf(LocalHostName, "%s", tmpbuf1);
#else /* __MWERKS__ */
#ifdef _WIN32
    tsprintf(LocalHostName, "%s", tmpbuf1);
#else /* _WIN32 */
    memset((char *)(&(LocalHostName[0])), 0, sizeof(LocalHostName));
    tsprintf((char *)(&(LocalHostName[0])), "%s.%s", tmpbuf1, tmpbuf2);
#endif /* _WIN32 */
#endif /* __MWERKS__ */

    tsprintf((char *)(&(Fbuf[0])), "LocalHostName = %s\n", LocalHostName);
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
    if(PingDatLog)
        tfprintf(ping_fp, "%s\n", LocalHostName);
    if ((LocalHost = tgethostbyname(tmpbuf1)) == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Error tgethostbyname(tmpbuf1) = %s\n", tmpbuf1);
        FEError(-1914, EXIT, WRITE, (char *)(&(Fbuf[0])));
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
    TieCommChgSubNet(0, 1, new_soup, 0);
    tsprintf((char *)(&(LocalPortStr[0])), "%hu", LocalPort);
    tsprintf((char *)(&(Fbuf[0])), "LocalPort=%hu\n", LocalPort);
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
#ifndef BGL
#ifndef __MWERKS__
    tsprintf(MonPortStr, "%hu", MonPort);
    MonSockUDP = (I32s)passiveUDP(MonPortStr);
    tsprintf((char *)(&(Fbuf[0])), "MonSockUDP = %d\n", MonSockUDP);
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    MonSockTCP = (I32s)passiveTCP(MonPortStr, QLEN);
    tsprintf((char *)(&(Fbuf[0])), "MonSockTCP = %d\n", MonSockTCP);
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /* __MWERKS__ */
#endif /* BGL */
    FD_ZERO(&Afds);
    FD_ZERO(&Wfds);
    FD_ZERO(&Efds);
#ifndef __MWERKS__
    FD_SET(MonSockUDP, &Afds);
    FD_SET(MonSockTCP, &Afds);
#ifndef _WIN32  /* the Nfds parameter of the select() function
                   is igonored in the Winsock implementation, and
                   is only included in the arg list for compatiblity.
                   This is a good thing since Win doesn't have the
                   <sysent.h> library containing getdtablesize() */
#endif /* _WIN32  */
    {   I32s ifd;
        for (ifd = 0; ifd < MAX_MONS; ifd++)
        {   MonSockTCP_ID[ifd].fd = -1;
            MonSockTCP_ID[ifd].flg = 'F';
        }
    }
    MonTCPNid = 0;
#endif /* __MWERKS__ */

#endif /* NET */

#ifdef SIGBLOCK
    TieSignal(SIGIO, TieSigIO);
#ifdef TIEAUDIO
    SetAsyncSkt(audlstnskt);
#endif /* TIEAUDIO */
#ifdef NET
    SetAsyncSkt(globcominfobuf->tci_comrdhndl);
#endif /* NET */
#endif /* SIGBLOCK */

#ifndef __MWERKS__
#ifndef AMIGA
#ifdef _WIN32
    if(!SetPriorityClass(GetCurrentProcess(),
        (TierraNice)?IDLE_PRIORITY_CLASS:NORMAL_PRIORITY_CLASS))
    {   tsprintf((char *)(&(Fbuf[0])), "SetPriorityClass failed");
        FEError(-1923, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#else /* _WIN32 */
    nice(TierraNice);
#endif /* _WIN32 */
#endif /* AMIGA */
    tsprintf((char *)(&(Fbuf[0])), "TierraNice = %d\n", TierraNice);
    FEPrintf(MSG_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /* __MWERKS__ */

    tsprintf((char *)(&(Fbuf[0])),"tsetup: soup gotten\n");
    FEPrintf(MSG_X, PLN_Y, 0, 0, &(Fbuf[0]));
    if (SliceStyle == 1)
    {   PhotonSize = (I32s) strlen((const char *)(&(PhotonWord[0])));
        PhotonTranslate(PhotonInst, PhotonWord);
        slicer = SlicerPhoton;
    }
    else if (SliceStyle == 0)
        slicer = SlicerQueue;
    else      /* default SliceStyle == 2 */
        slicer = RanSlicerQueue;
    if (new_soup)
    {   if (GenInList)
        {   tfree((void *) GenInList, AlocGenInList, 1904);
            GenInList = NULL;
        }
        if (GenInBuf)
        {   tfree((void *) GenInBuf, AlocGenInBuf, 1905);
            GenInBuf = NULL;
        }
    }
    tfclose(inf);

#ifdef BGL
    TBglTierraSet();
#endif /* BGL */

#ifndef __GUI__

/* added to GetSoup from old tierra.c */
#ifdef __TURBOC__
#if FRONTEND == STDIO
    ctrlbrk(T_sig_int);
#endif /* FRONTEND == STDIO */
#if FRONTEND == BASIC
    CtrBufStr(&(Fbuf[0]),COLS,"Press Any Key for menu ...");
    FEPrintf(HLP_X, HLP_Y, 1, 0, &(Fbuf[0]));
#endif /* FRONTEND == BASIC */
#endif /* __TURBOC__ */

#else /* __GUI__ */
}               /* end GetSoup for __GUI__ */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void EndGetSoup()
{
#endif /* __GUI__ */

    if (GeneBnker)
        VerifyGB(1,1);
#ifdef MEM_PROF
    if (TierraLog)
        MemStatReport();
#endif /* MEM_PROF */

#ifndef __GUI__
    FEClrmsg(MSG_Y,
        ((OPSYS == BCDOS) || (OPSYS == DJGPPDOS) ||
            ((OPSYS == AMIGADOS) && (FRONTEND == BASIC))
            || (OPSYS == MACTIE) || (OPSYS == WIN32TIE) ?(I8s *)
"                       Press Any Key for Menu ...                             "
            :(I8s *)
"                    Press Interrupt Key for Menu ...                          "
        ));
#else /* __GUI__ */
    FEClrmsg(MSG_Y,
"                     Use Menu for Mode Selection ...                          "
        );
#endif /* __GUI__ */

#if ((INTERFACE == BASICC)&&(defined(TSysV)))||(INTERFACE == BASICA)
    tnodelay(FALSE);
#endif /* ((INTERFACE == BASICC)&&(defined(TSysV)))||(INTERFACE == BASICA) */

    new_soup = 0;
}

/*
 * GetNewSoup - startup for new run
 */
void GetNewSoup()
{   I32s i, j, ar = 0, ci = 2, cs, sad = 0, tNumCells = NumCells;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    NumCells=0;
    FreeMemCurrent = SoupSize;
    /* initialize FreeMemry array */
    MemInit();
    /* initialize cells array */
    for (i = 0; i < NumCelAr; i++)
        for (j = 0; j < CelArSiz; j++)
            InitCell(i, j, &cells[i][j]);
    TopDummy->ld = BottomDummy->ld = 1;
    ce = BottomReap = TopReap = &cells[0][2];

    /* read in the cell genotypes */
    for (i = 0; i < tNumCells; i++)
    {   if (!strncmp((const char *)"space",
            (const char *)(GenInList[i]), 5))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((const char *)(GenInList[i]),
                (const char *)"%*s%d", &cs);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            tsprintf((char *)(&(Fbuf[0])),
                "skipping %d instructions\n", cs);
            FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
        }
        else if (!strncmp((const char *)"center",
            (const char *)(GenInList[i]), 6))      
        {   cs=SoupSize/2;                                 
            tsprintf((char *)(&(Fbuf[0])), "skipping %d instructions\n", cs);
            FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));       
        }
        else
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((const char *)(GenInList[i]), "%d", &cs);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            tsprintf((char *)(&(Fbuf[0])),
                "GetNewSoup: loading %s into cell %d,%d\n",
                GenInList[i], ar, ci);
            FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
            if (!InjectFromBank(GenInList[i], sad, 0, 1))
            {   tsprintf((char *)(&(Fbuf[0])), "injection failure\n");
                FEError(-1912, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
            ci++;
        }
        if (!(ci % CelArSiz))
        {   ci = 0;
            ar++;
        }
        sad += cs;
    }

    if (GeneBnker)
    {   CalcGBStats(sl, siz_sl);
    }
    else
        CalcSoupStats();
    AdjCtrlVars();
    MaxGenPop.size = 0;
    MaxGenPop.label[0] = '\0';
    MaxGenMem.size = 0;
    MaxGenMem.label[0] = '\0';
}

/*
 * GetOldSoup - resume run
 *
 * inf - pointer to FILE structure of opened soup_in of run to be resumed
 */
void GetOldSoup(inf)
    FILE *inf;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */

    tfgets(Buff, 84, inf);
#ifdef NET
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"ApocalypseNow = %d",
        &ApocalypseNow);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"ApocalypseNowFreq = %d",
        &ApocalypseNowFreq);
#endif /* NET */
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"AverageSize = %d",
        &AverageSize);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"AvgPop = %lf",
        &AvgPop);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"BrkupCou = %d",
        &BrkupCou);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"BrkupCum = %d",
        &BrkupCum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CelArSiz = %d",
        &CelArSiz);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CellsSize = %d",
        &CellsSize);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TierraClock = %d",
        &TierraClock);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CountFlaw = %d",
        &CountFlaw);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CountJmpSou = %d",
        &CountJmpSou);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CountMovMut = %d",
        &CountMovMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CountMutRate = %d",
        &CountMutRate);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"debug_switch = %d",
        &debug_switch);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Disturb.m = %d", &Disturb.m);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Disturb.i = %d", &Disturb.i);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"ExtractCount = %d",
        &ExtractCount);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"FirstOutDisk = %d",
        &FirstOutDisk);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"FreeBlocks = %d",
        &FreeBlocks);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"FreeMemCurrent = %d",
        &FreeMemCurrent);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Generations = %lf",
        &Generations);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"InstExe.i = %d", &InstExe.i);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"InstExe.m = %d", &InstExe.m);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"isolate = %d",
        &isolate);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"LastDiv.i = %d", &LastDiv.i);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"LastDiv.m = %d", &LastDiv.m);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MalLimit = %d",
        &MalLimit);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"NumCelAr = %d",
        &NumCelAr);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"NumGenotypes = %d",
        &NumGenotypes);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"NumSizes = %d",
        &NumSizes);
#ifdef NET
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"PipmapC = %d",
        &PipmapC);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"PipmapS = %d",
        &PipmapS);
#endif /* NET */
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Put_limit = %d",
        &Put_limit);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RandIx1 = %d",
        &RandIx1);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RandIx2 = %d",
        &RandIx2);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RandIx3 = %d",
        &RandIx3);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RateFlaw = %d",
        &RateFlaw);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RateJmpSou = %d",
        &RateJmpSou);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RateMovMut = %d",
        &RateMovMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RateMut = %d",
        &RateMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"reaped = %d",
        &reaped);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Search_limit = %d",
        &Search_limit);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"SoupBot = %d",
        &SoupBot);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"SoupTop = %d",
        &SoupTop);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"Speed = %d",
        &Speed);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"BirthNum = %d",
        &BirthNum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"DeathNum = %d",
        &DeathNum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RepNum = %d",
        &RepNum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"FecunditySum = %d",
        &FecunditySum);

    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"EjectToSelf = %d",
        &EjectToSelf);

    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RepInstSum = %d",
        &RepInstSum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RepInstEffSum = %lf",
        &RepInstEffSum);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TimePop = %lf",
        &TimePop);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TimeStats.i = %d", &TimeStats.i);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TimeStats.m = %d", &TimeStats.m);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotFlaw = %d",
        &TotFlaw);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotMovMut = %d",
        &TotMovMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotMut = %d",
        &TotMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotDivMut = %d",
        &TotDivMut);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotCroInsSamSiz = %d",
        &TotCroInsSamSiz);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotInsIns = %d",
        &TotInsIns);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotDelIns = %d",
        &TotDelIns);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotCroIns = %d",
        &TotCroIns);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotDelSeg = %d",
        &TotDelSeg);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotInsSeg = %d",
        &TotInsSeg);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"TotCroSeg = %d",
        &TotCroSeg);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"filepos = %d",
        &pos);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"tielogpos = %d",
        &tielogpos);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"NumGenDG = %d",
        &NumGenDG);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxPop = %d",
        &MaxPop);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxGenPop.size = %d", &MaxGenPop.size);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxGenPop.label = %s", &(MaxGenPop.label[0]));
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxMemry = %d",
        &MaxMemry);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxGenMem.size = %d", &MaxGenMem.size);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"MaxGenMem.label = %s", &(MaxGenMem.label[0]));
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"RepInst = %d",
        &RepInst);
#ifdef NET
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"SubNetChgTime = %d",
        &SubNetChgTime);
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"tpinglogpos = %d",
        &tpinglogpos);
#endif /* NET */
    tfgets(Buff, 84, inf);
    sscanf((const char *)(&(Buff[0])),
        (const char *)"CpuLoadLimitSleepNow = %d",
        &CpuLoadLimitSleepNow);

#ifdef NETMON
    strcpy(Buff, "outfile_core");
#endif /* NETMON */
#ifdef IBM3090
    strcpy(Buff, "core_out.io.d");
#else
    strcpy((char *)(&(Buff[0])), (const char *)"core_out");
#endif
#ifdef NETMON
    strcpy(Buff, "core_out_net");
#endif /* NETMON */

    ReadDynMem();
    ReadGeneBank();

#ifdef NETMON
    DiskOut = 0;
#endif /* NETMON */

    if (DiskOut)
    {
#ifdef IBM3090
        if (BrkupSiz)
            tsprintf(Buff, "break.%d.d", BrkupCou);
        else
            tsprintf(Buff, "tierra.run");
#else
        if (BrkupSiz)
            tsprintf((char *)(&(Buff[0])),
                "%sbreak.%d", GenebankPath, BrkupCou);
        else
            tsprintf((char *)(&(Buff[0])), "%stierra.run", GenebankPath);
#endif
        if ((oufr = tfopen(Buff, (I8s *)"r+"))== NULL)
        {   oufr = tfopen(Buff, (I8s *)"w");
            if (oufr == NULL)
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra GetOldSoup() 1 file %s not opened, exiting", Buff);
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                FEError(-1920, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
            else
            {   tfclose(oufr);
                oufr = tfopen(&(Buff[0]), (I8s *)"r+");
                if (oufr == NULL)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra GetOldSoup() 1 file %s not opened, exiting",
                        Buff);
#ifdef SIGBLOCK
                    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                    FEError(-1915, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                else
                {   pos = 0;
                }
            }
        }
        tseek(oufr, pos, SEEK_SET);
    }
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
}

/*
 * WriteSoup - save run data
 *
 * close_disk 0 = do not close open files
 *            >0 = close open files (used for save before exit)
 */
void WriteSoup(close_disk)
    I32s close_disk;
{   FILE *ouf;
    time_t tp;
    I8s soupoutf[80];
    if (DiskOut)
    {   pos = tftell(oufr);
        if (close_disk)
            tfclose(oufr);
    }
    if(TierraLog)
        tielogpos=tftell(tfp_log);
#ifdef NET
    if(PingDatLog)
        tpinglogpos=tftell(ping_fp);
#endif /* NET */
    if (DiskBank)
        SavGeneBank();
    new_soup = 0;

#ifdef IBM3090
    tsprintf((char *)(&(soupoutf[0])), "soup_out.io.d");
#else
    tsprintf((char *)(&(soupoutf[0])), "%ssoup_out", GenebankPath);
#endif /* IBM3090 */

    tp = (I32s) tietime(NULL);
    ouf = tfopen(soupoutf, (I8s *)"w");
    if (ouf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra WriteSoup() 0 file %s not opened, exiting", soupoutf);
        FEError(-1921, EXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    tfprintf(ouf, "# tierra core:  %s\n", ctime(&tp));
    tfprintf(ouf, "# observational parameters:\n\n");

    tfprintf(ouf, "BrkupSiz = %d\n", BrkupSiz);
    tfprintf(ouf, "CumGeneBnk = %d\n", CumGeneBnk);
    tfprintf(ouf, "debug = %d\n", debug);
    tfprintf(ouf, "DiskBank = %d\n", DiskBank);
    tfprintf(ouf, "DiskOut = %d\n", DiskOut);
    tfprintf(ouf, "FindTimeM = %d\n", FindTimeM);
    tfprintf(ouf, "FindTimeI = %d\n", FindTimeI);
    tfprintf(ouf, "GeneBnker = %d\n", GeneBnker);
    tfprintf(ouf, "DeconstructOn = %d\n", DeconstructOn);
    tfprintf(ouf, "GenebankPath = %s\n", GenebankPath);
#ifdef __MWERKS__
#ifdef NET
    tfprintf(ouf, "MacHost = %s\n", MacHost);
#endif /* NET */
#endif /* __MWERKS__ */
    tfprintf(ouf, "hangup = %d\n", hangup);
    tfprintf(ouf, "MaxFreeBlocks = %d\n", MaxFreeBlocks);
    tfprintf(ouf, "SaveFreq = %d\n", SaveFreq);
    tfprintf(ouf, "SavRenewMem = %d\n", SavRenewMem);
    tfprintf(ouf, "SavMinNum = %d\n", SavMinNum);
    tfprintf(ouf, "SavThrMem = %g\n", SavThrMem);
    tfprintf(ouf, "SavThrPop = %g\n", SavThrPop);
    tfprintf(ouf, "TierraLog = %d\n", TierraLog);
    tfprintf(ouf, "WatchExe = %d\n", WatchExe);
    tfprintf(ouf, "WatchMov = %d\n", WatchMov);
    tfprintf(ouf, "WatchTem = %d\n", WatchTem);
    tfprintf(ouf, "TierraNice = %d\n", TierraNice);
    tfprintf(ouf, "MonPort = %hu\n", MonPort);
    tfprintf(ouf, "MigrCtrlPort = %hu\n", MigrCtrlPort);

#ifdef NET
    tfprintf(ouf, "\n# NET\n");
    tfprintf(ouf, "map_fnC = %s\n", map_fnC);
    tfprintf(ouf, "map_fnS = %s\n", map_fnS);
    tfprintf(ouf, "PingDatLog = %d\n", PingDatLog);
    tfprintf(ouf, "LocalPort = %hu\n", LocalPort);
    tfprintf(ouf, "AutoAddIPMap = %d\n", AutoAddIPMap);
    tfprintf(ouf, "AutoRemIPMap = %d\n", AutoRemIPMap);
#ifdef BGL
    tfprintf(ouf, "TrackNormLike = %d\n", TrackNormLike);
#endif /* BGL */
#endif /* NET */

    tfprintf(ouf, "\n# environmental variables:\n\n");

    tfprintf(ouf, "alive = %d\n", alive);
    tfprintf(ouf, "AliveGen = %d\n", AliveGen);
#ifdef NET
    tfprintf(ouf, "ApocalypseFreq = %d\n", ApocalypseFreq);
    tfprintf(ouf, "ApocFixFrac = %f\n", ApocFixFrac);
    tfprintf(ouf, "ApocRanFrac = %f\n", ApocRanFrac);
    tfprintf(ouf, "ImmigLimFrac = %f\n", ImmigLimFrac);
    tfprintf(ouf, "SrvrApocSleep = %d\n", SrvrApocSleep);
    tfprintf(ouf, "TieMsgBkLog = %d\n", TieMsgBkLog);
    tfprintf(ouf, "TieMsgMaxAge = %d\n", TieMsgMaxAge);
    tfprintf(ouf, "NodeSelSucRatPrec = %d\n", NodeSelSucRatPrec);
    tfprintf(ouf, "GetIPPStrtIdxC = %d\n", GetIPPStrtIdxC);
    tfprintf(ouf, "GetIPPStrtIdxS = %d\n", GetIPPStrtIdxS);
    tfprintf(ouf, "BrkIPPStrtIdxC = %d\n", BrkIPPStrtIdxC);
    tfprintf(ouf, "BrkIPPStrtIdxS = %d\n", BrkIPPStrtIdxS);
    tfprintf(ouf, "BrkIPPStopIdxC = %d\n", BrkIPPStopIdxC);
    tfprintf(ouf, "BrkIPPStopIdxS = %d\n", BrkIPPStopIdxS);

#if NETTYPE == UDPASM
    tfprintf(ouf, "TieMTU = %d\n", TieMTU);
    tfprintf(ouf, "PktSndDelay = %d\n", PktSndDelay);
#endif /* NETTYPE == UDPASM */

#if NETTYPE == TCP
    tfprintf(ouf, "TCPLocSelTmeSec = %d\n", TCPLocSelTmeSec);
    tfprintf(ouf, "TCPLocSelTmeUSec = %d\n", TCPLocSelTmeUSec);
    tfprintf(ouf, "TCPRemSelTmeSec = %d\n", TCPRemSelTmeSec);
    tfprintf(ouf, "TCPRemSelTmeUSec = %d\n", TCPRemSelTmeUSec);
#endif /* NETTYPE == TCP */

#endif /* NET */
    tfprintf(ouf, "DistFreq = %f\n", DistFreq);
    tfprintf(ouf, "DistProp = %f\n", DistProp);
    tfprintf(ouf, "DivSameGen = %d\n", DivSameGen);
    tfprintf(ouf, "DivSameSiz = %d\n", DivSameSiz);
    tfprintf(ouf, "DropDead = %d\n", DropDead);
    tfprintf(ouf, "MinSpeed = %d\n", MinSpeed);
    tfprintf(ouf, "EjectRate = %d\n", EjectRate);
    tfprintf(ouf, "GenPerBkgMut = %d\n", GenPerBkgMut);
    tfprintf(ouf, "GenPerFlaw = %d\n", GenPerFlaw);
    tfprintf(ouf, "GenPerMovMut = %d\n", GenPerMovMut);
    tfprintf(ouf, "GenPerDivMut = %d\n", GenPerDivMut);
    tfprintf(ouf, "GenPerCroInsSamSiz = %d\n", GenPerCroInsSamSiz);
    tfprintf(ouf, "GenPerInsIns = %d\n", GenPerInsIns);
    tfprintf(ouf, "GenPerDelIns = %d\n", GenPerDelIns);
    tfprintf(ouf, "GenPerCroIns = %d\n", GenPerCroIns);
    tfprintf(ouf, "GenPerDelSeg = %d\n", GenPerDelSeg);
    tfprintf(ouf, "GenPerInsSeg = %d\n", GenPerInsSeg);
    tfprintf(ouf, "GenPerCroSeg = %d\n", GenPerCroSeg);
    tfprintf(ouf, "MutBitProp = %f\n", MutBitProp);
    tfprintf(ouf, "IMapFile = %s\n", IMapFile);
    tfprintf(ouf, "JmpSouTra = %f\n", JmpSouTra);
    tfprintf(ouf, "JumpTrackProb = %f\n", JumpTrackProb);
    tfprintf(ouf, "LazyTol = %d\n", LazyTol);
    tfprintf(ouf, "MalMode = %d\n", MalMode);
    tfprintf(ouf, "MalReapTol = %d\n", MalReapTol);
    tfprintf(ouf, "MalSamSiz = %d\n", MalSamSiz);
    tfprintf(ouf, "MalTol = %d\n", MalTol);
    tfprintf(ouf, "MateSizeEp = %d\n", MateSizeEp);
    tfprintf(ouf, "MaxCpuPerCell = %d\n", MaxCpuPerCell);
    tfprintf(ouf, "MaxIOBufSiz = %d\n", MaxIOBufSiz);
    tfprintf(ouf, "MaxGetBufSiz = %d\n", MaxGetBufSiz);
    tfprintf(ouf, "MaxPutBufSiz = %d\n", MaxPutBufSiz);
    tfprintf(ouf, "MaxSigBufSiz = %d\n", MaxSigBufSiz);
    tfprintf(ouf, "MaxMalMult = %g\n", MaxMalMult);
    tfprintf(ouf, "MemModeFree = %d\n", MemModeFree);
    tfprintf(ouf, "MemModeMine = %d\n", MemModeMine);
    tfprintf(ouf, "MemModeProt = %d\n", MemModeProt);
    tfprintf(ouf, "MinCellSize = %d\n", MinCellSize);
    tfprintf(ouf, "MinComSizRat = %g\n", MinComSizRat);
    tfprintf(ouf, "LifeCycFrct = %g\n", LifeCycFrct);
    tfprintf(ouf, "MinGenMemSiz = %d\n", MinGenMemSiz);
    tfprintf(ouf, "MinTemplSize = %d\n", MinTemplSize);
    tfprintf(ouf, "MovPropThrDiv = %g\n", MovPropThrDiv);
    tfprintf(ouf, "new_soup = %d\n", new_soup);
    tfprintf(ouf, "NumCells = %d\n", NumCells);
    tfprintf(ouf, "NumCellsMin = %d\n", NumCellsMin);

#ifdef NET
    tfprintf(ouf, "PendReqMax = %d\n", PendReqMax);
    tfprintf(ouf, "PendReqTime = %d\n", PendReqTime);
    tfprintf(ouf, "SpdZeroTime = %d\n", SpdZeroTime);
    tfprintf(ouf, "MinSubNetApoRat = %g\n", MinSubNetApoRat);
#endif /*  NET */

    tfprintf(ouf, "PhotonPow = %g\n", PhotonPow);
    tfprintf(ouf, "PhotonWidth = %d\n", PhotonWidth);
    tfprintf(ouf, "PhotonWord = %s\n", PhotonWord);
    tfprintf(ouf, "PutLimit = %g\n", PutLimit);
    tfprintf(ouf, "ReapRndProp = %g\n", ReapRndProp);
    tfprintf(ouf, "SearchLimit = %g\n", SearchLimit);
    tfprintf(ouf, "AbsSearchLimit = %d\n", AbsSearchLimit);
    tfprintf(ouf, "seed = %d\n", seed);
    tfprintf(ouf, "SizDepSlice = %d\n", SizDepSlice);
    tfprintf(ouf, "SlicePow = %g\n", SlicePow);
    tfprintf(ouf, "SliceSize = %d\n", SliceSize);
    tfprintf(ouf, "SliceStyle = %d\n", SliceStyle);
    tfprintf(ouf, "SlicFixFrac = %g\n", SlicFixFrac);
    tfprintf(ouf, "SlicRanFrac = %g\n", SlicRanFrac);
    tfprintf(ouf, "SpeedUpdate = %d\n", SpeedUpdate);
    tfprintf(ouf, "StrictIP = %d\n", StrictIP);
    tfprintf(ouf, "TierraSleep = %d\n", TierraSleep);
    tfprintf(ouf, "MinSoupSize = %d\n", MinSoupSize);
    tfprintf(ouf, "MaxSoupSize = %d\n", MaxSoupSize);
    tfprintf(ouf, "ThrdAnaMaxFec = %d\n", ThrdAnaMaxFec);
    tfprintf(ouf, "ThrdAnaStop = %d\n", ThrdAnaStop);
    tfprintf(ouf, "ThrdAnaTrmCode = %d\n", ThrdAnaTrmCode);
    tfprintf(ouf, "ThrdAnaPartSave = %d\n", ThrdAnaPartSave);
    tfprintf(ouf, "ThreadAnalysis = %d\n", ThreadAnalysis);
    tfprintf(ouf, "SplitTissueAna = %d\n", SplitTissueAna);
    tfprintf(ouf, "SouUpdTrk = %d\n", SouUpdTrk);
    tfprintf(ouf, "DeadMemInit = %d\n", DeadMemInit);
    tfprintf(ouf, "SoupSize = %d\n", SoupSize);
#ifdef TIEAUDIO
    tfprintf(ouf, "AudioPopThresh = %d\n", AudioPopThresh);
    tfprintf(ouf, "AudioPort = %d\n", AudioPort);
#endif /* TIEAUDIO */
 
#ifdef NET
    tfprintf(ouf, "SensMapAtt = %d\n", SensMapAtt);
    tfprintf(ouf, "OnLineStat = %d\n", OnLineStat);
    tfprintf(ouf, "XDRBufMaxSize = %d\n", XDRBufMaxSize);
    tfprintf(ouf, "SubNetCnt = %d\n", SubNetCnt);
    tfprintf(ouf, "BasSubNetChgFrq = %d\n", BasSubNetChgFrq);
    tfprintf(ouf, "SubNetChgFrqRanFrac = %f\n", SubNetChgFrqRanFrac);
    tfprintf(ouf, "SubNetChgFrqFixFrac = %f\n", SubNetChgFrqFixFrac);
    tfprintf(ouf, "TieSubNet = %d\n", TieSubNet);
    tfprintf(ouf, "NetRcvUpdFreq = %d\n", NetRcvUpdFreq);
    tfprintf(ouf, "BklogProcFreq = %d\n", BklogProcFreq);
#endif /* NET */

#if FRONTEND == BASIC
    tfprintf(ouf, "InstExeUpdFreq = %d\n", InstExeUpdFreq);
#endif /* FRONTEND == BASIC */

#ifdef TIEAUDIO
    tfprintf(ouf, "AudConnProcFreq = %d\n", AudConnProcFreq);
#endif /* TIEAUDIO */

    tfprintf(ouf, "DistNext.m = %d\n", DistNext.m);
    tfprintf(ouf, "DistNext.i = %d\n", DistNext.i);

    tfprintf(ouf, "\n0 to stop GetAVar\n\n");
    /* end soup_in variables */

#ifdef NET
    tfprintf(ouf, "ApocalypseNow = %d\n", ApocalypseNow);
    tfprintf(ouf, "ApocalypseNowFreq = %d\n", ApocalypseNowFreq);
#endif /* NET */
    tfprintf(ouf, "AverageSize = %d\n", AverageSize);
    tfprintf(ouf, "AvgPop = %f\n", AvgPop);
    tfprintf(ouf, "BrkupCou = %d\n", BrkupCou);
    tfprintf(ouf, "BrkupCum = %d\n", BrkupCum);
    tfprintf(ouf, "CelArSiz = %d\n", CelArSiz);
    tfprintf(ouf, "CellsSize = %d\n", CellsSize);
    tfprintf(ouf, "TierraClock = %d\n", TierraClock);
    tfprintf(ouf, "CountFlaw = %d\n", CountFlaw);
    tfprintf(ouf, "CountJmpSou = %d\n", CountJmpSou);
    tfprintf(ouf, "CountMovMut = %d\n", CountMovMut);
    tfprintf(ouf, "CountMutRate = %d\n", CountMutRate);
    tfprintf(ouf, "debug_switch = %d\n", debug_switch);
    tfprintf(ouf, "Disturb.m = %d\n", Disturb.m);
    tfprintf(ouf, "Disturb.i = %d\n", Disturb.i);
    tfprintf(ouf, "ExtractCount = %d\n", ExtractCount);
    tfprintf(ouf, "FirstOutDisk = %d\n", FirstOutDisk);
    tfprintf(ouf, "FreeBlocks = %d\n", FreeBlocks);
    tfprintf(ouf, "FreeMemCurrent = %d\n", FreeMemCurrent);
    tfprintf(ouf, "Generations = %f\n", Generations);
    tfprintf(ouf, "InstExe.i = %d\n", InstExe.i);
    tfprintf(ouf, "InstExe.m = %d\n", InstExe.m);
    tfprintf(ouf, "isolate = %d\n", isolate);
    tfprintf(ouf, "LastDiv.i = %d\n", LastDiv.i);
    tfprintf(ouf, "LastDiv.m = %d\n", LastDiv.m);
    tfprintf(ouf, "MalLimit = %d\n", MalLimit);
    tfprintf(ouf, "NumCelAr = %d\n", NumCelAr);
    tfprintf(ouf, "NumGenotypes = %d\n", NumGenotypes);
    tfprintf(ouf, "NumSizes = %d\n", NumSizes);
#ifdef NET
    tfprintf(ouf, "PipmapC = %d\n", PipmapC);
    tfprintf(ouf, "PipmapS = %d\n", PipmapS);
#endif /* NET */
    tfprintf(ouf, "Put_limit = %d\n", Put_limit);
    tfprintf(ouf, "RandIx1 = %d\n", RandIx1);
    tfprintf(ouf, "RandIx2 = %d\n", RandIx2);
    tfprintf(ouf, "RandIx3 = %d\n", RandIx3);
    tfprintf(ouf, "RateFlaw = %d\n", RateFlaw);
    tfprintf(ouf, "RateJmpSou = %d\n", RateJmpSou);
    tfprintf(ouf, "RateMovMut = %d\n", RateMovMut);
    tfprintf(ouf, "RateMut = %d\n", RateMut);
    tfprintf(ouf, "reaped = %d\n", reaped);
    tfprintf(ouf, "Search_limit = %d\n", Search_limit);
    tfprintf(ouf, "SoupBot = %d\n", SoupBot);
    tfprintf(ouf, "SoupTop = %d\n", SoupTop);
    tfprintf(ouf, "Speed = %d\n", Speed);
    tfprintf(ouf, "BirthNum = %d\n", BirthNum);
    tfprintf(ouf, "DeathNum = %d\n", DeathNum);
    tfprintf(ouf, "RepNum = %d\n", RepNum);
    tfprintf(ouf, "FecunditySum = %d\n", FecunditySum);

    tfprintf(ouf, "EjectToSelf = %d\n", EjectToSelf);

    tfprintf(ouf, "RepInstSum = %d\n", RepInstSum);
    tfprintf(ouf, "RepInstEffSum = %f\n", RepInstEffSum);
    tfprintf(ouf, "TimePop = %f\n", TimePop);
    tfprintf(ouf, "TimeStats.i = %d\n", TimeStats.i);
    tfprintf(ouf, "TimeStats.m = %d\n", TimeStats.m);
    tfprintf(ouf, "TotFlaw = %d\n", TotFlaw);
    tfprintf(ouf, "TotMovMut = %d\n", TotMovMut);
    tfprintf(ouf, "TotMut = %d\n", TotMut);
    tfprintf(ouf, "TotDivMut = %d\n", TotDivMut);
    tfprintf(ouf, "TotCroInsSamSiz = %d\n", TotCroInsSamSiz);
    tfprintf(ouf, "TotInsIns = %d\n", TotInsIns);
    tfprintf(ouf, "TotDelIns = %d\n", TotDelIns);
    tfprintf(ouf, "TotCroIns = %d\n", TotCroIns);
    tfprintf(ouf, "TotDelSeg = %d\n", TotDelSeg);
    tfprintf(ouf, "TotInsSeg = %d\n", TotInsSeg);
    tfprintf(ouf, "TotCroSeg = %d\n", TotCroSeg);
    tfprintf(ouf, "filepos = %d\n", pos);
    tfprintf(ouf, "tielogpos = %d\n", tielogpos);
    tfprintf(ouf, "NumGenDG = %d\n", NumGenDG);
    tfprintf(ouf, "MaxPop = %d\n", MaxPop);
    tfprintf(ouf, "MaxGenPop.size = %d\n", MaxGenPop.size);
    tfprintf(ouf, "MaxGenPop.label = %-3s\n", MaxGenPop.label);
    tfprintf(ouf, "MaxMemry = %d\n", MaxMemry);
    tfprintf(ouf, "MaxGenMem.size = %d\n", MaxGenMem.size);
    tfprintf(ouf, "MaxGenMem.label = %-3s\n", MaxGenMem.label);
    tfprintf(ouf, "RepInst = %d\n", RepInst);
#ifdef NET
    tfprintf(ouf, "SubNetChgTime = %d\n", SubNetChgTime);
    tfprintf(ouf, "tpinglogpos = %d\n", tpinglogpos);
#endif /* NET */
    tfprintf(ouf, "CpuLoadLimitSleepNow = %d\n", CpuLoadLimitSleepNow);
    tfprintf(ouf, "CpuLoadLimitProp = %g\n", CpuLoadLimitProp);
    tfprintf(ouf, "CpuLoadLimitPeriod = %d\n", CpuLoadLimitPeriod);
    tfprintf(ouf, "CpuLoadLimitPeriod = %d\n", CpuLoadLimitPeriod);

    tfclose(ouf);

#ifdef NET
    SaveIPMap(0);
    SaveIPMap(1);
#endif /* NET */
    SavDynMem();

#ifdef MEM_PROF
    if (TierraLog)
        MemStatReport();
#endif /* MEM_PROF */
}

/*
 * SavDynMem - build core_out file - called from WriteSoup
 */
void SavDynMem()
{   I32s i, j;
#ifdef NET
    I32s k;
#endif /* NET */
    FILE *ouf;
    CellInd cind[3];
    Pcells cw;                  /* cell to be written */

#ifdef IBM3090
    strcpy((char *)(&(corefile[0])), "core_out.io.d");
#else /* IBM3090 */
    tsprintf((char *)(&(corefile[0])), "%score_out", GenebankPath);
#endif /* IBM3090 */

    ouf = tfopen((I8s *)(&(corefile[0])), (I8s *)"wb");
    if (ouf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra SavDynMem() file %s not opened, exiting", corefile);
        FEError(-1925, EXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    tfwrite((I8s Hp) soup, sizeof(Instruction), SoupSize, ouf);
    if (DiskOut)
        tfwrite((I8s *)&lo, sizeof(LastOut), 1, ouf);
    tfwrite((I8s *)&is, sizeof(PInst), 1, ouf);
    tfwrite((I8s *)&(TrandArray[0]), sizeof(double), 98, ouf);
    cind[0].a = BottomReap->q.htis.a;
    cind[0].i = BottomReap->q.htis.i;
    cind[1].a = TopReap->q.htis.a;
    cind[1].i = TopReap->q.htis.i;
    cind[2].a = ce->q.htis.a;
    cind[2].i = ce->q.htis.i;
    tfwrite((I8s *)&(cind[0]), sizeof(CellInd), 3, ouf);
    for (i = 0; i < NumCelAr; i++)
    {   tfwrite((I8s Fp) cells[i], sizeof(TCell), CelArSiz, ouf);
        for (j = 0; j < CelArSiz; j++)
        {   cw = &(cells[i][j]);

#ifdef EXEINSTSIZTIMSLC
            SaveCellGenBits(ouf, cw);
#endif /* EXEINSTSIZTIMSLC */

            if (cw->c.sy.sy)
                tfwrite((I8s Fp) cw->c.sy.sy, sizeof(Sync), cw->c.sy.n, ouf);
            if (cw->c.ar)
                tfwrite((I8s Fp) cw->c.ar, sizeof(Cpu), cw->c.n, ouf);
            if (cw->c.sig.sig)
                tfwrite((I8s Fp) cw->c.sig.sig, sizeof(I8s), cw->c.sig.siz,
                    ouf);
#ifdef NET
            if (cw->c.io.io)
            {   tfwrite((I8s Fp) cw->c.io.io, sizeof(IOS), cw->c.io.siz, ouf);
                for (k = 0; k < cw->c.io.siz; k++)
                    if (cw->c.io.io[k].d.d)
                        tfwrite((I8s Fp) cw->c.io.io[k].d.d, sizeof(I8s),
                            cw->c.io.io[k].d.len, ouf);
            }
#endif /* NET */
#ifdef IO
            SavePGbf(&(cw->c.pg.pgb_put), ouf);
            SavePGbf(&(cw->c.pg.pgb_get), ouf);
#endif /* IO */
            WriteThrdAnaDat(&(cw->cell_thrdanadat),ouf);
        }
    }
    tfwrite((I8s Hp) FreeMemry, sizeof(MemFr), MaxFreeBlocks, ouf);
    tfwrite((I8s Fp) & tmpl, sizeof(TmpList), 1, ouf);
    if (tmpl.n)
        tfwrite((I8s Fp) tmpl.l, sizeof(I32s), tmpl.n, ouf);
#ifdef NET
    tfwrite((I8s Fp)   MonCom, sizeof(ComCount), 6, ouf);
    tfwrite((I8s Fp) FEMonCom, sizeof(ComCount), 6, ouf);
#endif /* NET */
    tfclose(ouf);
}

/*
 * ReadDynMem - read core_out file for run resume - called by GetOldSoup
 */
void ReadDynMem()
{   I32s i, j, l;
#ifdef NET
    I32s k;
#endif /* NET */
    CellInd cind[3];
    FILE *inf;
    Pcells cr;                  /* cell to be read */
    CalLvlIntByIDArr calvlintrvlkupid;

#ifdef IBM3090
    strcpy((char *)(&(corefile[0])), "core_out.io.d");
#else /* IBM3090 */
    tsprintf((char *)(&(corefile[0])), "%score_out", GenebankPath);
#endif /* IBM3090 */

    inf = tfopen((I8s *)(&(corefile[0])), (I8s *)"rb");
    if (inf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra ReadDynMem() file %s not opened, exiting", corefile);
        FEError(-1926, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    soup = (HpInst) thcalloc((I32u) SoupSize, sizeof(Instruction), 1906);
    tfread((I8s Hp) soup, sizeof(Instruction), SoupSize, inf);
    if (DiskOut)
        tfread((I8s *)&lo, sizeof(LastOut), 1, inf);
    tfread((I8s *)&is, sizeof(PInst), 1, inf);
    tfread((I8s *)&(TrandArray[0]), sizeof(double), 98, inf);
    tfread((I8s *)&(cind[0]), sizeof(CellInd), 3, inf);
    cells = (Pcells *) tcalloc(NumCelAr, sizeof(Pcells), 1900);
    for (i = 0; i < NumCelAr; i++)
    {   cells[i] = (Pcells) tcalloc(CelArSiz, sizeof(TCell), 1901);
        tfread((I8s Fp) cells[i], sizeof(TCell), CelArSiz, inf);
        for (j = 0; j < CelArSiz; j++)
        {   cr = &(cells[i][j]);

#ifdef EXEINSTSIZTIMSLC
            ReadCellGenBits(inf, cr);
#endif /* EXEINSTSIZTIMSLC */

            if (cr->c.sy.sy)
            {   cr->c.sy.sy = (Sync *) tcalloc(cr->c.sy.n, sizeof(Sync), 707);
                tfread((I8s Fp) cr->c.sy.sy, sizeof(Sync), cr->c.sy.n, inf);
            }
            if (cr->c.ar)
            {   cr->c.ar = (Cpu *) tcalloc(cr->c.n, sizeof(Cpu), 1902);
                tfread((I8s Fp) cr->c.ar, sizeof(Cpu), cr->c.n, inf);
            }
            if (cr->c.sig.sig)
            {   cr->c.sig.sig = (I8s *) tcalloc(cr->c.sig.siz, sizeof(I8s),
                    706);
                tfread((I8s Fp) cr->c.sig.sig, sizeof(I8s), cr->c.sig.siz, inf);
            }
#ifdef NET
            if (cr->c.io.io)
            {   cr->c.io.io = (IOS *) tcalloc(cr->c.io.siz, sizeof(IOS), 700);
                tfread((I8s Fp) cr->c.io.io, sizeof(IOS), cr->c.io.siz, inf);
                for (k = 0; k < cr->c.io.siz; k++)
                    if (cr->c.io.io[k].d.d)
                    {   cr->c.io.io[k].d.d = (I8s Fp)    
                            tcalloc(cr->c.io.io[k].d.len,
                            sizeof(I8s), 702);
                        tfread((I8s Fp) cr->c.io.io[k].d.d, sizeof(I8s),
                            cr->c.io.io[k].d.len, inf);
                    }
            }
#endif /* NET */
#ifdef IO
            ReadPGbf(&(cr->c.pg.pgb_put), inf);
            ReadPGbf(&(cr->c.pg.pgb_get), inf);
#endif /* IO */
            ReadThrdAnaDat(&(cr->cell_thrdanadat), &calvlintrvlkupid, inf);
            if(cr->cell_thrdanadat.mtad_rtcalvlint)
            {   for(l=0;l<(cr->c.n);l++)
                    cr->c.ar[l].cpu_crcalvlint=
                        calvlintrvlkupid.clid_array[
                        cr->c.ar[l].cpu_crcalvlid];
                FreeDynArr((DynArr *)(&calvlintrvlkupid), 2219, 0);
            }
        }
    }
    FreeMemry = (MemFr Fp) tcalloc(MaxFreeBlocks, sizeof(MemFr), 1907);
    tfread((I8s Hp) FreeMemry, sizeof(MemFr), MaxFreeBlocks, inf);
    tfread((I8s Fp) & tmpl, sizeof(TmpList), 1, inf);
    if (tmpl.n)
    {   tmpl.l = (I32s *) tcalloc(tmpl.n, sizeof(I32s), 1406);
        tfread((I8s Fp) tmpl.l, sizeof(I32s), tmpl.n, inf);
    }
#ifdef NET
    tfread((I8s Fp)   MonCom, sizeof(ComCount), 6, inf);
    tfread((I8s Fp) FEMonCom, sizeof(ComCount), 6, inf);
#endif /* NET */

    tfclose(inf);

    TopDummy = &cells[0][0];
    BottomDummy = &cells[0][1];
    BottomReap = &cells[cind[0].a][cind[0].i];
    TopReap = &cells[cind[1].a][cind[1].i];
    ce = &cells[cind[2].a][cind[2].i];
}

/*
 * ReadGeneBank - load rambank from gene bank archives for run resume
 *                called from GetOldSoup
 */
void ReadGeneBank()
{
#ifdef ERRORTIE
    I32s tgi;
#endif /* ERRORTIE */
    I32s si, ar, ci, oNumGenotypes, oNumSizes;
    Pcells cr;                  /* cell to be read */
    I32s old_hash, old_gi, gi, flag, old_origgi;
    Dem origdemo;

    oNumGenotypes = NumGenotypes;
    NumGenotypes = 0;
    oNumSizes = NumSizes;
    NumSizes = 0;
    for (ar = 0; ar < NumCelAr; ar++)
        for (ci = 0; ci < CelArSiz; ci++)
        {   cr = &cells[ar][ci];
            if (!cr->ld || (ar == 0 && ci < 2))
                continue;
            cr->d.genome = soup + cr->mm.p;
            if (!GeneBnker)
                continue;
            old_hash = cr->d.hash;
            old_gi = cr->d.gi;
            old_origgi = cr->d.origgi;
            cr->d.hash = Hash(cr->d.mg.s, cr->d.genome + cr->d.mg.p);
            if (DiskBank)
                flag = 15;
            else
                flag = 0;  /* check .gen & .tmp files */
            CheckGenotype(&(cr->d), flag, &siz_sl, &sl);

            if(old_gi!=old_origgi)
            {   if(DiskBank)
                {   memset((char *)(&origdemo), 0, sizeof(Dem));
                    origdemo.gen.size=cr->d.gen.size;
                    origdemo.gi=cr->d.origgi;
                    CheckGenotype(&origdemo, flag, &siz_sl, &sl);
                    cr->d.origgi=origdemo.gi;
                }
                else
                    cr->d.origgi=cr->d.gi;
            }
            else
                cr->d.origgi=cr->d.gi;

#ifdef ERRORTIE    /* and preserve bit 1 */
            if (cr->d.hash != old_hash)
            {   tsprintf((char *)(&(Fbuf[0])),
            "ReadGeneBank: (hash on disk = %d) != (computed hash = %d)\n",
                    old_hash, cr->d.hash);
                FEError(-1927, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
            if (DiskBank && (cr->d.gi != (tgi = Lbl2Int(cr->d.gen.label))))
            {   tsprintf((char *)(&(Fbuf[0])),
            "ReadGeneBank(): cr->d.gen.label = %s = %d, cr->d.gi = %hd\n",
                    cr->d.gen.label, tgi, cr->d.gi);
                FEError(-1928, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
            if (DiskBank && (old_gi != cr->d.gi))
            {   tsprintf((char *)(&(Fbuf[0])),
                    "ReadGeneBank(): cr->d.gi = %hd, gi from disk = %hd\n",
                    cr->d.gi, old_gi);
                FEError(-1929, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
#endif /* ERRORTIE */
            gi = cr->d.gi;
            si = cr->d.gen.size;
            strcpy((char *)(&(cr->d.gen.label[0])),
                (const char *)Int2Lbl(gi));
            if (!sl[si]->g[gi]->pop)
            {   NumGenotypes++;
                sl[si]->num_g++;

                /* update size class average */
                /* reproduction efficiency */
                /* daughter 1 */
                if (sl[si]->g[gi]->d1.instP)
                {   if (sl[si]->g[gi]->d1.mov_daught)
                    {   sl[si]->AvgRpdEff[0] = (((sl[si]->AvgRpdEff[0]) *
                            ((double) (sl[si]->AvgEffCnt[0]))) +
                            (((double) (sl[si]->g[gi]->d1.instP)) /
                            ((double) (sl[si]->g[gi]->d1.mov_daught)))) /
                            ((double) ((sl[si]->AvgEffCnt[0]) + 1));
                        (sl[si]->AvgEffCnt[0])++;
                    }
                }
                /* daughter 2 */
                if (sl[si]->g[gi]->d2.instP)
                {   if (sl[si]->g[gi]->d2.mov_daught)
                    {   sl[si]->AvgRpdEff[1] = (((sl[si]->AvgRpdEff[1]) *
                            ((double) (sl[si]->AvgEffCnt[1]))) +
                            (((double) (sl[si]->g[gi]->d2.instP)) /
                            ((double) (sl[si]->g[gi]->d2.mov_daught)))) /
                            ((double) ((sl[si]->AvgEffCnt[1]) + 1));
                        (sl[si]->AvgEffCnt[1])++;
                    }
                }
            }
            (sl[si]->g[gi]->pop)++;
            (sl[si]->g[cr->d.origgi]->origpop)++;
            if (!sl[si]->num_c)
                NumSizes++;
            sl[si]->num_c++;
        }
    if (!GeneBnker)
        return;
#ifdef ERRORTIE
    if (oNumGenotypes && oNumGenotypes != NumGenotypes)
    {   tsprintf((char *)(&(Fbuf[0])),
    "Tierra ReadGeneBank() error: oNumGenotypes = %d  NumGenotypes = %d\n",
            oNumGenotypes, NumGenotypes);
        FEError(-1930, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if (oNumSizes && oNumSizes != NumSizes)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra ReadGeneBank() error: oNumSizes = %d  NumSizes = %d\n",
            oNumSizes, NumSizes);
        FEError(-1931, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */
}

/*
 * SavGeneBank - save rambank contents for later run resumption
 *               called from WriteSoup
 *
 * description
 *
 * Returns:
 */
void SavGeneBank()
{   I32s si, gi;
    I8s path[80], tpath[80];
    GList *tgl;
    FILE *fp, *tf;
    head_t head, thead;
    indx_t *indx, *tindx;
    TmpList tmpt;

    tmpt.n = 0;
    tmpt.l = NULL;
    if (tmpl.n)
    {   tmpt.n = tmpl.n;
        tmpt.l = (I32s *) tcalloc(tmpt.n, sizeof(I32s), 1406);
        for (si = 0; si < tmpt.n; si++)
            tmpt.l[si] = tmpl.l[si];
    }
    for (si = 0; si < siz_sl; si++)
    {   if (!sl[si] || !sl[si]->num_c)
            continue;
        tsprintf((char *)(&(tpath[0])), "%s%.4d.tmp", GenebankPath, si);
        tsprintf((char *)(&(path[0])), "%s%.4d.gen", GenebankPath, si);
        if (tmpl.n)
            TmplRem(si, &tmpl);
        if (tmpt.n)
            TmplRem(si, &tmpt);
        fp = open_ar(path, si, -1);
        tf = open_ar(tpath, si, 1);
        head = read_head(fp);
#ifdef NET
        head.hdsucsiznslrat=sl[si]->sucsiznslrat;
        head.hdsvsiznsl=sl[si]->svsiznsl;
        head.hdsvsucsiznsl=sl[si]->svsucsiznsl;
#endif /* NET */
        thead = read_head(tf);
#ifdef NET
        thead.hdsucsiznslrat=sl[si]->sucsiznslrat;
        thead.hdsvsiznsl=sl[si]->svsiznsl;
        thead.hdsvsucsiznsl=sl[si]->svsucsiznsl;
#endif /* NET */
        indx = read_indx(fp, &head);
        tindx = read_indx(tf, &thead);
        /* for each genotype of this size */
        for (gi = 0; gi < sl[si]->a_num; gi++)
        {   if(!TNULL((tgl=sl[si]->g[gi])))
            {   if (IsBit(tgl->bits, 0))  /* if permanent genotype name */
                    add_gen(fp, &head, &indx, tgl);
                else if((tgl->pop)||(tgl->origpop))
                    add_gen(tf, &thead, &tindx, tgl);
            }
        }
        tfclose(fp);
        tfclose(tf);
        if (!head.n)
            tunlink(path);
        if (!thead.n)
            tunlink(tpath);
        else
            TmplAdd(si, &tmpl);
        if (indx)
        {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
            indx = NULL;
        }
        if (tindx)
        {   thfree((void *) tindx, thead.n_alloc * sizeof(indx_t), 600);
            tindx = NULL;
        }
    }
    if(!DiskBank)
        if (tmpt.n)
        {   for (si = 0; si < tmpt.n; si++)
            {   tsprintf((char *)(&(tpath[0])), "%s%.4d.tmp",
                    GenebankPath, tmpt.l[si]);
                tunlink(tpath);
            }
            tfree(tmpt.l, tmpt.n * sizeof(I32s), 1406);
            tmpt.l = NULL;
            tmpt.n = 0;
        }
}

/*
 * InitCell - initialize data structures for new cell
 *
 * ar, ci - coordinates of cell array element
 *          corresponding to cell to be initialized
 * cp - pointer to cell array element
 */
void InitCell(ar, ci, cp)
    I32s ar, ci;
    Pcells cp;
{   I32s i;
#ifdef SHADOW
    I32s j;
#endif /* SHADOW */

    FreeThrdAnaDatCell(cp);
    if (cp->c.sy.sy)
    {   tfree((void *) (cp->c.sy.sy), cp->c.sy.n * sizeof(Sync), 707);
        cp->c.sy.sy = NULL;
    }
    if (cp->c.ar)
    {   tfree((void *) (cp->c.ar), cp->c.n * sizeof(Cpu), 1902);
        cp->c.ar = NULL;
    }
    if (cp->c.sig.sig)
    {   tfree((void *) (cp->c.sig.sig), cp->c.sig.siz * sizeof(I8s), 706);
        cp->c.sig.siz = 0;
        cp->c.sig.sig = NULL;
    }
#ifdef NET
    if (cp->c.io.io)
    {   for (i = 0; i < cp->c.io.siz; i++)
        {   if (cp->c.io.io[i].d.d)
                tfree((void *) (cp->c.io.io[i].d.d),
                    cp->c.io.io[i].d.len * sizeof(I8s), 702);
            cp->c.io.io[i].d.d = NULL;
        }
        tfree((void *) (cp->c.io.io), cp->c.io.siz * sizeof(IOS), 700);
        cp->c.io.siz = 0;
        cp->c.io.io = NULL;
    }
#endif /* NET */
#ifdef IO
    FreePGbf(&(cp->c.pg.pgb_put));
    FreePGbf(&(cp->c.pg.pgb_get));
#endif /* IO */
    memset(cp, 0, sizeof(TCell));
    InitThrdAnaDat(&(cp->cell_thrdanadat));
    strcpy((char *)(&(cp->d.gen.label[0])),(const char *)"---");
    strcpy((char *)(&(cp->d.parent.label[0])),(const char *)"---");
    cp->d.RanID = tintrand();
    cp->d.origgi = cp->d.gi = (I32s) -1L;  /* index to genome in bank */
    cp->d.ploidy = (I8s) PLOIDY;  /* how many tracks */
    cp->q.htis.a = (I32u)ar;
    cp->q.htis.i = (I32u)ci;
    cp->d.ne = cp->q.n_time = cp->q.p_time = cp->q.n_reap
        = cp->q.p_reap = cp->q.htis;
    cp->c.n = PLOIDY;
    cp->c.threadct=0;
    cp->c.ar = (Cpu *) tcalloc(cp->c.n, sizeof(Cpu), 1902);
    for (i = 0; i < cp->c.n; i++)
    {   cp->c.ar[i].sp = (I32s) STACK_SIZE - 1;
        cp->c.ar[i].sy.G = -1;
        if (IDregs.Se.n)
            cp->c.ar[i].cf.Se.i = IDregs.Se.r[0];
        if (IDregs.De.n)
            cp->c.ar[i].cf.De.i = IDregs.De.r[0];
        if (IDregs.So.n)
            cp->c.ar[i].cf.So.i = IDregs.So.r[0];
#ifdef SHADOW
        for (j = 0; j < NUMREG; j++)
            cp->c.ar[i].re[NUMREG + j] = j;
#endif /* SHADOW */
#if PLOIDY > 1
        cp->c.ar[i].ex=cp->c.ar[i].so = cp->c.ar[i].de = 0;
#endif /* PLOIDY != 1 */
        InitCpu(&(cp->c.ar[i]));
    }
    cp->c.c = &(cp->c.ar[0]);

#ifdef NET
    cp->c.io.ipic = -1;
    cp->c.io.ipic1 = -1;
    cp->c.io.ipis = -1;
    cp->c.io.ipis1 = -1;
#endif /* NET */

}

void InitCpu(cpu)
Cpu *cpu;
{   cpu->threadid      = -1;
    cpu->parthrdid     = -1;
    cpu->prvgene       = -1;
    cpu->curgene       = -1;
    cpu->cpu_crcalvlid = -1;
    cpu->calltcmpstr   = -1;
}

/*
 * FreeDynMem - free memory allocated for structures kept in core_out
 *
 * description
 *
 * Returns:
 */
void FreeDynMem()
{  I32s i, j;
#ifdef NET
   I32s k;
#endif /* NET */
   Pcells cp;

    FreeGB(&sl, &siz_sl);
    if (soup)
    {   thfree((void *) soup,InstrXdrWrdSize(SoupSize)*sizeof(xdrwrd),1906);
        soup = NULL;
    }
    for (i = 0; i < NumCelAr; i++)
    {   if (cells[i])
        {   for (j = 0; j < CelArSiz; j++)
            {   cp = &cells[i][j];
                if (cp->c.sy.sy)
                {   tfree((void *) (cp->c.sy.sy),
                        cp->c.sy.n * sizeof(Sync), 707);
                    cp->c.sy.sy = NULL;
                }
                FreeThrdAnaDatCell(cp);
                if (cp->c.ar)
                {   tfree((void *) (cp->c.ar), cp->c.n * sizeof(Cpu), 1902);
                    cp->c.ar = NULL;
                }
                if (cp->c.sig.sig)
                {   tfree((void *) (cp->c.sig.sig),
                        cp->c.sig.siz * sizeof(I8s), 706);
                    cp->c.sig.siz = 0;
                    cp->c.sig.sig = NULL;
                }
#ifdef NET
                if (cp->c.io.io)
                {   for (k = 0; k < cp->c.io.siz; k++)
                    {   if (cp->c.io.io[k].d.d)
                            tfree((void *) (cp->c.io.io[k].d.d),
                                cp->c.io.io[k].d.len * sizeof(I8s), 702);
                        cp->c.io.io[k].d.d = NULL;
                    }
                    tfree((void *) (cp->c.io.io), cp->c.io.siz * sizeof(IOS),
                        700);
                    cp->c.io.siz = 0;
                    cp->c.io.io = NULL;
                }
#endif /* NET */
#ifdef IO
                FreePGbf(&(cp->c.pg.pgb_put));
                FreePGbf(&(cp->c.pg.pgb_get));
#endif /* IO */

#ifdef EXEINSTSIZTIMSLC
                FreeCellGenBits(cp);
#endif /* EXEINSTSIZTIMSLC */
            }
            tfree((void *) cells[i], CelArSiz * sizeof(TCell), 1901);
            cells[i] = NULL;
        }
    }
    if (cells)
    {   tfree((void *) cells, NumCelAr * sizeof(Pcells), 1900);
        cells = NULL;
    }
    if (FreeMemry)
    {   tfree((void *) FreeMemry, MaxFreeBlocks * sizeof(MemFr), 1907);
        FreeMemry = NULL;
    }
    if (tmpl.n)
    {   tfree(tmpl.l, tmpl.n * sizeof(I32s), 1406);
        tmpl.l = NULL;
        tmpl.n = 0;
    }
}

#ifdef _WIN32
void Win32Startup()
{   if(HookInstall())
    {   tsprintf((char *)(&(Fbuf[0])), "hook install failed");
        FEError(-1913, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
}
#endif /* _WIN32 */

#endif /* CLSTRSRVR */
