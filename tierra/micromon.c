/* @(#) micromon.c */
/* memalloc.c 30-3-98 debug monitor routines */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* FEError labels: #22XX, in use: 2200-2232 */
#include "license.h"
#include "tierra.h"

#ifdef __GUI__
#define WEXTERN_H
#endif /* __GUI__ */
#include "globals.h"
#if INTERFACE == BASICD
#include <gppconio.h>
#endif /* INTERFACE == BASICD */

#ifdef __GUI__
#include "wntierra.h"
#endif /* __GUI__ */

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*************************************/
/* Clear step debugger break point   */
/*************************************/
void GoBrkClr()
{   if (BrkTrapCondBuf.curbrkdat.BrkSet)
    {   switch (BrkTrapCondBuf.curbrkdat.TrapType)
        {   case BRKTYP_GIVENGEN_OFFSET:
            case BRKTYP_GIVENPARAGIVENHOST:
            case BRKTYP_PCTCHGPOP:
            case BRKTYP_ANYPARAANYHOSTTIME:
            case BRKTYP_ANYGENGIVENTIME:
            case BRKTYP_GIVENPARAANYHOST:
            case BRKTYP_ANYPARAGIVENHOST:
            case BRKTYP_ANYGENSIZERANGE:
            case BRKTYP_ANYPARA_HOSTSIZERANGE:
            case BRKTYP_ANYHOST_PARASIZERANGE:
            case BRKTYP_THRDANADATRDY:
                CloseGeneBanker();
                break;
            default:
                break;
        }
    }

    /* untrack cell */
    UnTrkCell();

#ifdef MICRO
    memset(&(BrkTrapCondBuf.curbrkdat),
        0, sizeof(BrkTrapDat));
    BrkTrapCondBuf.curbrkdat.pctchg=0.0;
    BrkTrapCondBuf.NewCpuI=-1;
    BrkTrapCondBuf.curbrkdat.MicroCpuI = -1;
#endif /* MICRO */
}

/*
 * SavBrkDat - copy break trap parameters for reuse by
 *             break trap repeat
 */
void SavBrkDat()
{   BrkTrapCondBuf.savbrkdat=BrkTrapCondBuf.curbrkdat;
}

/*
 * TrkCPU - track cpu of given cell
 *
 * cp - pointer to given cell
 * newcpu - 1 = track newest cpu from most recent split
 *              overrides thrdid
 * thrdid - track cpu with given threadid
 *
 */
void TrkCPU(cp, newcpu, thrdid)
    Pcells cp;
    I32s newcpu, thrdid;

{   TrkCell(cp);
    BrkTrapCondBuf.NewCpuI=-1;
    if(newcpu)
        if((cp->c.ar[(cp->c.n)-1].threadid)>=0)
            BrkTrapCondBuf.curbrkdat.MicroCpuI=
                cp->c.ar[(cp->c.n)-1].threadid;
        else
            BrkTrapCondBuf.NewCpuI=(cp->c.n)-1;
    else
        if(thrdid<0)
            BrkTrapCondBuf.curbrkdat.MicroCpuI=
                cp->c.c->threadid;
        else
            BrkTrapCondBuf.curbrkdat.MicroCpuI=thrdid;
}

/*
 * UnTrkCPU - stop any cpu tracking
 */
void UnTrkCPU()
{
    BrkTrapCondBuf.curbrkdat.MicroCpuI = -1;
}

/*
 * TrkCell - track given cell in debugger
 *
 * cp - pointer to given cell
 */
void TrkCell(cp)
    Pcells cp;
{   BrkTrapCondBuf.curbrkdat.MicroSlice = cp;
}

/*
 * UnTrkCell - stop cell tracking by debugger
 */
void UnTrkCell()
{   BrkTrapCondBuf.curbrkdat.MicroSlice = 0;
    UnTrkCPU();
}

/*
 * EnterLongVal - enter I32s value for break trap parameter
 *
 * promptline - line on which to display prompt
 * prompttext - prompt text
 * abort - pointer to abort flag
 *
 * returns : I32s value for break trap parameter
 *           *abortflag - 1 = entry aborted
 */
I32s EnterLongVal(promptline, prompttext, abortflag)
    I32s promptline, *abortflag;
    char *prompttext;
{   int scanfrtn;
    I32s rtnlongint;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    rtnlongint = 0;
    (*abortflag) = 0;
    for(;;)
    {
#if FRONTEND == BASIC
        FECeol(promptline);
#endif /* FRONTEND == BASIC */
        tsprintf((char *)(&(Fbuf[0])),"%s", prompttext);
        FEPrintf(PLN_X, promptline, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(PLN_X + strlen(prompttext), promptline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            scanfrtn=sscanf((const char *)(&(FGSbuf[0])), "%d", &rtnlongint);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            if (scanfrtn == 1)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2200, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
        {   (*abortflag) = 1;
            break;
        }
    }
    return rtnlongint;
}

/*
 * EnterFloatVal - enter floating point  value for break trap parameter
 *
 * promptline - line on which to display prompt
 * prompttext - prompt text 
 * abort - pointer to abort flag
 *
 * returns : floating point value for break trap parameter
 *           *abortflag - 1 = entry aborted
 */
double EnterFloatVal(promptline, prompttext, abortflag)
    I32s promptline, *abortflag;
    char *prompttext;
{   int scanfrtn;
    double rtnfloatval;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    rtnfloatval = 0;
    (*abortflag) = 0;
    for(;;)
    {
#if FRONTEND == BASIC
        FECeol(promptline);
#endif /* FRONTEND == BASIC */
        tsprintf((char *)(&(Fbuf[0])),"%s", prompttext);
        FEPrintf(PLN_X, promptline, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(PLN_X + strlen(prompttext), promptline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            scanfrtn=sscanf((const char *)(&(FGSbuf[0])), "%lf", &rtnfloatval);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            if (scanfrtn == 1)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2201, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
        {   (*abortflag) = 1;
            break;
        }
    }
    return rtnfloatval;
}

/*
 * EnterGenotype - enter genotype name for break trap parameter
 *
 * promptline - line on which to display prompt
 * prompttext - prompt text 
 * abort - pointer to abort flag
 * genchk - 1 = check gene bank for existence of genotype
 *
 * returns : GlIndex structure for break trap parameter
 *           *abortflag - 1 = entry aborted
 */
GlIndex EnterGenotype(promptline, prompttext, abortflag, genchk)
I32s promptline, *abortflag, genchk;
char *prompttext;
{   char gen[4];
    I32s size;
    int scanfrtn;
    I32s gidx=0;
    GlIndex rtngenome;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    (*abortflag) = 0;
    rtngenome.si = 0;
    rtngenome.gi = 0;
    /* enter parasite genome name */
    for(;;)
    {
#if FRONTEND == BASIC
        FECeol(promptline);
#endif /* FRONTEND == BASIC */
        tsprintf((char *)(&(Fbuf[0])),"%s", prompttext);
        FEPrintf(PLN_X, promptline, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(PLN_X + strlen(prompttext), promptline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            scanfrtn=sscanf((const char *)(&(FGSbuf[0])), "%d%3s", &size, &(gen[0]));
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            if (scanfrtn == 2)
            {   gidx = Lbl2Int((I8s *)(&(gen[0])));
                if (genchk)
                {   if(!((size<1)||(size>(siz_sl-1))||(!(sl[size]))))
                    {   if (gidx < sl[size]->a_num)
                        {   if (sl[size]->g[gidx])
                                break;
                            else
                            {   tsprintf((char *)(&(Fbuf[0])),
                                    "genome not found");
                                FEError(-2202, NOEXIT, NOWRITE,
                                    (char *)(&(Fbuf[0])));
                            }
                        }
                        else
                        {   tsprintf((char *)(&(Fbuf[0])),
                                "genome not found");
                            FEError(-2203, NOEXIT, NOWRITE,
                                (char *)(&(Fbuf[0])));
                        }
                    }
                    else
                    {   tsprintf((char *)(&(Fbuf[0])),
                            "genome not found");
                        FEError(-2204, NOEXIT, NOWRITE,
                            (char *)(&(Fbuf[0])));
                    }
                }
                else
                    break;
            }
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2205, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
        {   (*abortflag) = 1;
            break;
        }
    }
    if (!(*abortflag))
    {   rtngenome.si = size;
        rtngenome.gi = gidx;
    }
    return (rtngenome);
}

/*
 * GoMicroToggle - toggle step mode for debugger
 *
 * returns: MC_step = -1 = free running
 *                     0 = 1 second delay between steps
 *                     1 = manual step mode
 */
#ifdef MICRO

void GoMicroToggle()
{
    GoBrkClr();
    if (MC_step == -1L)
    {   MC_step = 0L;
#ifdef __GUI__
        CtrBufStr(&(Fbuf[0]),COLS,
            "Click Mouse to Switch Mode");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
    }
    else if (MC_step == 0L)
        MC_step = 1L;
    else
    {   MC_step = -1L;

#if FRONTEND == BASIC
        IMode=PLN_STATS;
        FEVidInit((I32s)0);
#endif /* FRONTEND == BASIC */

        if (tfp_log != NULL)
            TierraLog = 1L;
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*************************************/
/* GoBrkMenu - menu - set, clear, or */
/*             repeat break trap     */
/*             settings              */
/*************************************/
void GoBrkMenu()
{   I8s answer;

#if FRONTEND == BASIC
    FEClrmsg(MSG_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    tsprintf((char *)(&(Fbuf[0])),
        "b-Set Brk  c-Clr Brk  r-Rpt Brk->");
    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
    answer = (I8s)FEGetch(HLP_X + strlen((
        const char *)(&(Fbuf[0]))), HLP_Y);
    switch (answer)
    {   case 'b':  /* Break Set */
        GoBrkSet();
        break;
    case 'c':  /* Break Clear */
        GoBrkClr();
        break;
    case 'r':  /* Repeat Break */
        GoRepeatBrk();
        break;
    default:
        break;
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*************************************/
/* GoBrkSet - Break Set Menu         */
/*************************************/
void GoBrkSet()
{   int menuopt, abortflag, menuline, scanfrtn;
    char *menutitles[15];
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
    I32s loopflag;

    menutitles[0]="Given Genotype / Offset";
    menutitles[1]="Current cell / Offset";
    menutitles[2]="Abs Time (Instr)";
    menutitles[3]="Rel Time (Instr)";
    menutitles[4]="Given Parasite / Given Host";
    menutitles[5]="Pct Change in Avg Pop";
    menutitles[6]="Any Parasite / Any Host / Time";
    menutitles[7]="Any Genotype / Given Time";
    menutitles[8]="Given Parasite / Any Host";
    menutitles[9]="Any Parasite / Given Host";
    menutitles[10]="Any Genotype / Given Size Range";
    menutitles[11]="Any Parasite / Host Size Range";
    menutitles[12]="Any Host / Parasite Size Range";
    menutitles[13]="Thread Analysis Ready";
    menutitles[(sizeof(menutitles)/sizeof(char *))-1]=0;

    abortflag=0;
    loopflag=1;
    while(loopflag)
    {   DisplayMenu(2, menutitles, 77, 3, 2, 1, &menuline);
        FEGetStr(PLN_X+strlen((const char *)
            (&Fbuf[0])), PLN_Y+menuline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   if (!strcmp((const char *)(&(FGSbuf[0])), "q"))
            {   abortflag=1;
                break;
            }
            else
            {

#ifdef SIGBLOCK
                SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
                scanfrtn=sscanf((const char *)(&(FGSbuf[0])), "%d", &menuopt);
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

                if (scanfrtn == 1)
                {   switch (menuopt)
                    {   case 1:  /* Genome/offset */
                            Brk_GivenGen_Offset();
                            loopflag=0;
                            break;

                        case 2:  /* Current cell/offset */
                            Brk_CurCell_Offset();
                            loopflag=0;
                            break;

                        case 3:  /* Absolute Time (Instr Exec) */
                            Brk_AbsTimeInst();
                            loopflag=0;
                            break;

                        case 4:  /* Time Interval (Instr Exec) */
                            Brk_IntrvlTimeInst();
                            loopflag=0;
                            break;

                        case 5:  /* Parasite genome / Host genome */
                            Brk_GivenParaGivenHost();
                            loopflag = 0;
                            break;

                        case 6:  /* Percent change in average population */
                            Brk_PctChgPop();
                            loopflag = 0;
                            break;

                        case 7:  /* Any Parasite / Any Host / Time */
                            Brk_AnyParaAnyHostTime();
                            loopflag = 0;
                            break;

                        case 8:  /* genome - given time */
                            Brk_AnyGenGivenTime();
                            loopflag = 0;
                            break;

                        case 9:  /* given parasite / host */
                            Brk_GivenParaAnyHost();
                            loopflag = 0;
                            break;

                        case 10:  /* permanent parasite / given host */
                            Brk_AnyParaGivenHost();
                            loopflag = 0;
                            break;
    
                        case 11:  /* any genome in given size range */
                            Brk_AnyGenSizeRange();
                            loopflag = 0;
                            break;

                        case 12:  /* permanent parasite, host size range */
                            Brk_AnyPara_HostSizeRange();
                            loopflag = 0;
                            break;

                        case 13:  /* permanent host, parasite size range */
                            Brk_AnyHost_ParaSizeRange();
                            loopflag = 0;
                            break;

                        case 14:  /* thread analysis ready */
                            Brk_ThrdAnaDatRdy();
                            loopflag = 0;
                            break;

                        default:
                            tsprintf((char *)(&(Fbuf[0])),
                                "invalid menu option");
                            FEError(-2206, NOEXIT, NOWRITE,
                                (char *)(&(Fbuf[0])));
                            break;
                    }
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])), "invalid menu option");
                    FEError(-2207, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
            }
        }
        else
        {   abortflag = 1;
            loopflag = 0;
        }
    }
}

/*
 * Micro_Spy - step/delay mode monitor and debugger
 *
 * cp - pointer to cell to be tracked if any
 *
 */
void Micro_Spy(cp)
    Pcells cp;
{   I32s tlog;
    I8s cc;
    I32s MicroSpyLoop;
    I32s dbgentry;
#ifdef __GUI__
    DWORD tick;
#endif /* __GUI__ */

    tlog = TierraLog;
    TierraLog = 0;    /*note: file not closed, we just wont scream at it */

    dbgentry=0;
    if(BrkTrapCondBuf.curbrkdat.BrkSet)
    {   if(!ChkBrk(cp))
            dbgentry=1;
    }
    else
        if(BrkTrapCondBuf.curbrkdat.MicroSlice)
        {   if(cp==BrkTrapCondBuf.curbrkdat.MicroSlice)
            {   if(BrkTrapCondBuf.curbrkdat.MicroCpuI>=0)
                {   if(cp->c.c->threadid ==
                        BrkTrapCondBuf.curbrkdat.MicroCpuI)
                        dbgentry=1;
                }
                else if((BrkTrapCondBuf.NewCpuI)<0)
                    dbgentry=1;
            }
        }
        else
            if((MC_step==0)||(MC_step==1))
                dbgentry=1;

    if(dbgentry)
    {   if (IMode != PLN_STATS)
            GoPlan();

        TierraLog = tlog;
        MicroSpyLoop = 1;
        while (MicroSpyLoop)
        {
#if FRONTEND == BASIC
            if(IMode!=SCRN_DEBUG)
            {   IMode=SCRN_DEBUG;
                FEVidInit((I32s)0);
            }
#endif /* FRONTEND == BASIC */
            DispDebuggerScrn(cp);
            if (MC_step==1)
            {   FEBanner();
                FEStats();
                tsprintf((char *)(&(Fbuf[0])),
#ifdef __GUI__
                    "%s  %s  Click-Mode Switch  n-Next step",
#else /* __GUI__ */
                    "%s  %s  r-Rpt Brk  q-Main d-displ a-disasm",
#endif /* __GUI__ */
                    (BrkTrapCondBuf.curbrkdat.MicroSlice) ?
                    "t-Untrk cell" : "T-Trk cell",
                    (BrkTrapCondBuf.curbrkdat.MicroCpuI>=0) ?
                    "u-Untrk thd" : "Trk U-cur N-New thd");
                FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
#if FRONTEND == STDIO
                tfprintf(stdout, "\n");
#endif /* FRONTEND == STDIO */
#ifdef __GUI__
                while ((cc = FEGetch(HLP_X + strlen(
                    (const char *)(&(Fbuf[0]))), HLP_Y)) == ' ')
                    ;
                if (cc == 'z')
                {   GoMicroToggle();  /* turn off microSpy on mouse click */
                    CtrBufStr(&(Fbuf[0]),COLS,
                        "Use Menu for Mode Selection");
                    FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
                }
#else /* __GUI__ */
                cc = (I8s)FEGetch(HLP_X + strlen(
                    (char *)(&(Fbuf[0]))), HLP_Y);
                if (cc == 'q')
                {   FEMenu();
                }
#endif /* __GUI__ */
                switch (cc)
                {   case 'T':  /* track cell */
                        GoBrkClr();
                        TrkCell(cp);
                        break;
                    case 't':  /* untrack cell */
                        GoBrkClr();
                        break;
                    case 'U':  /* track cpu */
                        GoBrkClr();
                        TrkCPU(cp,0,-1);
                        break;
                    case 'N':  /* track newest cpu */
                        GoBrkClr();
                        TrkCPU(cp,1,-1);
                        if(cp->c.c->threadid != BrkTrapCondBuf.
                            curbrkdat.MicroCpuI)      
                            MicroSpyLoop=0;
                        break;
                    case 'u':  /* untrack cpu */
                        GoBrkClr();
                        TrkCell(cp);
                        break;                             
                    case 'r':  /* Repeat Break */          
                        GoRepeatBrk();                     
                        if(BrkTrapCondBuf.savbrkdat.BrkSet)
                            MicroSpyLoop = 0;
                        break;
#ifndef __GUI__
                    case 'd':  /* debugger display */
                        DbgDisplCtrl(cp);
                        break;
                    case 'a':  /* disassembly menu */
                        DisasmMenu(cp);
                        break;
#endif /* __GUI__ */
                    default:
                        MicroSpyLoop = 0;
                        break;
                }
            }
            else if(MC_step==0)
            {
#ifndef __GUI__
                if (KEYHIT()){
                    FEMenu();
                    MicroSpyLoop = 0;
                }
                else
                {
#ifdef _WIN32
                    Sleep(1000);
#else /* _WIN32 */
                    sleep(1);
#endif /* _WIN32 */
                    MicroSpyLoop = 0;
                }
#else /* __GUI__ */
                if ((cc = FEGetch(HLP_X + strlen(
                    (char *)(&(Fbuf[0]))), HLP_Y)) != ' ')
                    GoMicroToggle();  /* go to keypress mode */
                else
                {   tick = GetTickCount();
                    while (GetTickCount() - tick < 1000);
                }
#endif /* __GUI__ */

            }
        }
    }
}

/*
 * DisplayHostCell - fill display buffers for debugger screen from given cell
 *
 * cp - pointer to given cell
 * display buffers
 *    hostbuf1
 *    hostbuf2
 *    hostbuf3
 *    hostbuf4
 *    hostbuf5
 *    hostbuf6
 */
void DisplayHostCell(cp, hostbuf1, hostbuf2, hostbuf3, hostbuf4,
    hostbuf5, hostbuf6)
    Pcells cp;
    char *hostbuf1, *hostbuf2, *hostbuf3, *hostbuf4, *hostbuf5, *hostbuf6;
{   int hostbuflen;
    char hostbuf2a[20], hostbuf3a[10], hostbuf4a[7];

    /* initialize title */
    tsprintf(hostbuf1, "----------- host ------------");

    ViewSoupHost(cp->c.c->ip, &(hostbuf2a[0]),
        &(hostbuf3a[0]), &(hostbuf4a[0]));

    tsprintf(&(hostbuf2[0]), "    id : ");
    hostbuflen = strlen(&(hostbuf2[0]));
    tsprintf(&(hostbuf2[hostbuflen]), "%s", hostbuf2a);

    tsprintf(&(hostbuf3[0]), "status : ");
    hostbuflen = strlen(&(hostbuf3[0]));
    tsprintf(&(hostbuf3[hostbuflen]), "%s", hostbuf3a);

    tsprintf(&(hostbuf4[0]), "offset : ");
    hostbuflen = strlen(&(hostbuf4[0]));
    tsprintf(&(hostbuf4[hostbuflen]), "%s", hostbuf4a);

    tsprintf(&(hostbuf5[0]), "inst   : %d", cp->d.inst);

    tsprintf(&(hostbuf6[0]), "instP  : %d", cp->d.instP);
}

/*
 * ViewSoupHost - fill display buffers with host data for given address
 *
 * soupviewaddr - soup address
 * display buffers
 *    hostbuf1
 *    hostbuf2
 *    hostbuf3
 */
void ViewSoupHost(soupviewaddr, hostbuf1, hostbuf2, hostbuf3)
    I32s soupviewaddr;
    char *hostbuf1, *hostbuf2, *hostbuf3;
{   Pcells scncp;
    I32s hosttype, hostoffset;

    scncp = FindHost(soupviewaddr, &hosttype);

    switch (hosttype)
    {   case 1:    /* live host */

            /* display host identity */
            tsprintf(hostbuf1, "%.4d%3s",
                  scncp->d.gen.size, Int2Lbl(scncp->d.gi));

            /* display host status */
            tsprintf(hostbuf2, "%s", (scncp->ld) ? "alive" : "dead");

            /* display host offset */
            hostoffset = soupviewaddr - (scncp->mm.p);
            tsprintf(hostbuf3, "%6d", hostoffset);

            break;

        case 2:    /* host is daughter */

            /* display host identity */
            tsprintf(hostbuf1, "daughter of %.4d%3s",
                  scncp->d.gen.size, Int2Lbl(scncp->d.gi));

            /* display host status */
            tsprintf(hostbuf2, "%s", "embryonic");

            /* display host offset */
            hostoffset = soupviewaddr - (scncp->md.p);
            tsprintf(hostbuf3, "%6d", hostoffset);

            break;

        default:   /* free memory */

            /* display host identity */
            tsprintf(hostbuf1, "free memory");

            /* display host status */
            tsprintf(hostbuf2, " ");

            /* display host offset */
            tsprintf(hostbuf3, " ");

            break;
    }
}

/*
 * DebugDisplay - display debugger screen for given cell
 * cp - pointer to cell
 */
void DebugDisplay(cp)
    Pcells cp;
{   I8s off_char = ' ', d_data[80];
    char hostbuf1[30];
    char hostbuf2[30];
    char hostbuf3[30];
    char hostbuf4[30];
    char hostbuf5[30];
    char hostbuf6[30];
    I32s off_int;
    char regcol[NUMSPECREG+1];

#ifdef BGL
    TBglDebugDisplayClr();
#endif /* BGL */

    off_int = cp->c.c->ip - cp->mm.p;
    DisplayHostCell(cp, &(hostbuf1[0]),
        &(hostbuf2[0]), &(hostbuf3[0]),
        &(hostbuf4[0]), &(hostbuf5[0]), &(hostbuf6[0]));
    if (!(off_int < cp->mm.s))
        off_char = '+';
    if (cp->md.s >= MinCellSize)
        tsprintf((char *)(&(d_data[0])),
            "Daughter @ %-6d + %-6d",
            cp->md.p, cp->md.s);
    else
        tsprintf((char *)(&(d_data[0])), "NO Daughter");



    tsprintf((char *)(&(Fbuf[0])),
#if STACK_SIZE > 0
        "  Cell %2d:%3d %6.4d%3s @ %7d Slice=%4d Stack [%11d]  "
#else /* STACK_SIZE > 0 */
        "  Cell %2d:%3d %6.4d%3s @ %7d Slice=%4d                      "
#endif /* STACK_SIZE > 0 */
        ,cp->q.htis.a, cp->q.htis.i
        ,cp->mm.s, Int2Lbl(cp->d.gi), cp->mm.p
        ,cp->c.ib
#if STACK_SIZE > 0
        ,cp->c.c->st[0]
#endif /* STACK_SIZE > 0 */
        );
#if STACK_SIZE > 0
    if (cp->c.c->sp == 0)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 0 */
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));



    tsprintf((char *)(&(Fbuf[0])),
#if STACK_SIZE > 1
        "    IP [%11d] (%c%-7d) = 0x%.2x %9.9s   [%11d]  "
#else /* STACK_SIZE > 1 */
        "    IP [%11d] (%c%-7d) = 0x%.2x %9.9s                  "
#endif /* STACK_SIZE > 1 */
        ,cp->c.c->ip, off_char, off_int
#if PLOIDY == 1
        ,soup[ad(cp->c.c->ip)]
        ,(I8u *)(id[soup[ad(cp->c.c->ip)]%InstNum].mn)
#else /* PLOIDY == 1 */
        ,soup[ad(cp->c.c->ip)][cp->c.c->ex]
        ,(I8u *)(id[(soup[ad(cp->c.c->ip)]
        [cp->c.c->ex])%InstNum].mn)
#endif /* PLOIDY == 1 */
#if STACK_SIZE > 1
        ,cp->c.c->st[1]
#endif /* STACK_SIZE > 1 */
        );
#if STACK_SIZE > 1
    if (cp->c.c->sp == 1)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 1 */
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 0
    MrkSpecRegCol(&(regcol[0]), 0);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.regcol0[0])),
        (const char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol0[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 0 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 0
#if STACK_SIZE > 2
        "%3s AX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 2 */
        "%3s AX [%11d] %-29s                "
#endif /* STACK_SIZE > 2 */
#else /* NUMREG > 0 */
#if STACK_SIZE > 2
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 2 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 2 */
#endif /* NUMREG > 0 */
        ,&(regcol[0])
#if NUMREG > 0
        ,cp->c.c->re[0]
#endif /* NUMREG > 0 */
        ,&(hostbuf1[0])
#if STACK_SIZE > 2
        ,cp->c.c->st[2]
#endif /* STACK_SIZE > 2 */
        );
#if STACK_SIZE > 2
    if (cp->c.c->sp == 2)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 2 */
    FEPrintf(PLN_X, PLN_Y + 2, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 1
    MrkSpecRegCol(&(regcol[0]), 1);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.
        regcol1[0])), (char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol1[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 1 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 1
#if STACK_SIZE > 3
        "%3s BX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 3 */
        "%3s BX [%11d] %-29s                "
#endif /* STACK_SIZE > 3 */
#else /* NUMREG > 1 */
#if STACK_SIZE > 3
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 3 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 3 */
#endif /* NUMREG > 1 */
        ,&(regcol[0])
#if NUMREG > 1
        ,cp->c.c->re[1]
#endif /* NUMREG > 1 */
        ,&(hostbuf2[0])
#if STACK_SIZE > 3
        ,cp->c.c->st[3]
#endif /* STACK_SIZE > 3 */
        );
#if STACK_SIZE > 3
    if (cp->c.c->sp == 3)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 3 */
    FEPrintf(PLN_X, PLN_Y + 3, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 2
    MrkSpecRegCol(&(regcol[0]), 2);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.regcol2[0])),
        (char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol2[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 2 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 2
#if STACK_SIZE > 4
        "%3s CX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 4 */
        "%3s CX [%11d] %-29s                "
#endif /* STACK_SIZE > 4 */
#else /* NUMREG > 2 */
#if STACK_SIZE > 4
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 4 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 4 */
#endif /* NUMREG > 2 */
        ,&(regcol[0])
#if NUMREG > 2
        ,cp->c.c->re[2]
#endif /* NUMREG > 2 */
        ,&(hostbuf3[0])
#if STACK_SIZE > 4
        ,cp->c.c->st[4]
#endif /* STACK_SIZE > 4 */
        );
#if STACK_SIZE > 4
    if (cp->c.c->sp == 4)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 4 */
    FEPrintf(PLN_X, PLN_Y + 4, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 3
    MrkSpecRegCol(&(regcol[0]), 3);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.regcol3[0])),
        (char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol3[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 3 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 3
#if STACK_SIZE > 5
        "%3s DX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 5 */
        "%3s DX [%11d] %-29s                "
#endif /* STACK_SIZE > 5 */
#else /* NUMREG > 3 */
#if STACK_SIZE > 5
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 5 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 5 */
#endif /* NUMREG > 3 */
        ,&(regcol[0])
#if NUMREG > 3
        ,cp->c.c->re[3]
#endif /* NUMREG > 3 */
        ,&(hostbuf4[0])
#if STACK_SIZE > 5
        ,cp->c.c->st[5]
#endif /* STACK_SIZE > 5 */
        );
#if STACK_SIZE > 5
    if (cp->c.c->sp == 5)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 5 */
    FEPrintf(PLN_X, PLN_Y + 5, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 4
    MrkSpecRegCol(&(regcol[0]), 4);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.regcol4[0])),
        (char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol4[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 4 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 4
#if STACK_SIZE > 6
        "%3s EX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 6 */
        "%3s EX [%11d] %-29s                "
#endif /* STACK_SIZE > 6 */
#else /* NUMREG > 4 */
#if STACK_SIZE > 6
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 6 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 6 */
#endif /* NUMREG > 4 */
        ,&(regcol[0])
#if NUMREG > 4
        ,cp->c.c->re[4]
#endif /* NUMREG > 4 */
        ,&(hostbuf5[0])
#if STACK_SIZE > 6
        ,cp->c.c->st[6]
#endif /* STACK_SIZE > 6 */
        );
#if STACK_SIZE > 6
    if (cp->c.c->sp == 6)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 6 */
    FEPrintf(PLN_X, PLN_Y + 6, 0, 0, (char *)(&(Fbuf[0])));



#if NUMREG > 5
    MrkSpecRegCol(&(regcol[0]), 5);
#ifdef BGL
    strncpy((char *)(&(BglMCSpyBuf.regcol5[0])),
        (char *)(&(regcol[0])), NUMSPECREG);
    BglMCSpyBuf.regcol5[NUMSPECREG]='\0';
#endif /* BGL */
#endif /* NUMREG > 5 */
    tsprintf((char *)(&(Fbuf[0])),
#if NUMREG > 5
#if STACK_SIZE > 7
        "%3s FX [%11d] %-29s [%11d]  "
#else /* STACK_SIZE > 7 */
        "%3s FX [%11d] %-29s                "
#endif /* STACK_SIZE > 7 */
#else /* NUMREG > 5 */
#if STACK_SIZE > 7
        "%3s                  %-29s [%11d]  "
#else /* STACK_SIZE > 7 */
        "%3s                  %-29s                "
#endif /* STACK_SIZE > 7 */
#endif /* NUMREG > 5 */
        ,&(regcol[0])
#if NUMREG > 5
        ,cp->c.c->re[5]
#endif /* NUMREG > 5 */
        ,&(hostbuf6[0])
#if STACK_SIZE > 7
        ,cp->c.c->st[7]
#endif /* STACK_SIZE > 7 */
        );
#if STACK_SIZE > 7
    if (cp->c.c->sp == 7)
        Fbuf[65] = '<';
#endif /* STACK_SIZE > 7 */
    FEPrintf(PLN_X, PLN_Y + 7, 0, 0, (char *)(&(Fbuf[0])));



    tsprintf((char *)(&(Fbuf[0])),
#if PLOIDY > 1
#if STACK_SIZE > 8
        "    Flag: %2d   Stk: %2d      Tracks: Ex So De       [%11d]  "
#else /* STACK_SIZE > 8 */
        "    Flag: %2d   Stk: %2d      Tracks: Ex So De                      "
#endif /* STACK_SIZE > 8 */
#else /* PLOIDY > 1 */
#if STACK_SIZE > 8
        "    Flag: %2d   Stk: %2d                             [%11d]  "
#else /* STACK_SIZE > 8 */
        "    Flag: %2d   Stk: %2d                                            "
#endif /* STACK_SIZE > 8 */
#endif /* PLOIDY == 1 */
        ,(int) cp->c.c->fl.E, cp->c.c->sp
#if STACK_SIZE > 8
        ,cp->c.c->st[8]
#endif /* STACK_SIZE > 8 */
        );
#if STACK_SIZE > 8
    if (cp->c.c->sp == 8)
        Fbuf[65] = '<';
    FEPrintf(PLN_X, PLN_Y + 8, 0, 0, (char *)(&(Fbuf[0])));
#endif /* STACK_SIZE > 8 */



    tsprintf((char *)(&(Fbuf[0])),
#if PLOIDY > 1
#if STACK_SIZE > 9
        "    %-30.30s  %2d %2d %2d       [%11d]  "
#else /* STACK_SIZE > 9 */
        "    %-30.30s  %2d %2d %2d                      "
#endif /* STACK_SIZE > 9 */
#else /* PLOIDY > 1 */
#if STACK_SIZE > 9
        "    %-30.30s                 [%11d]  "
#else /* STACK_SIZE > 9 */
        "    %-30.30s                                "
#endif /* STACK_SIZE > 9 */
#endif /* PLOIDY > 1 */
        ,d_data
#if PLOIDY > 1
        ,ce->c.c->ex
        ,ce->c.c->so
        ,ce->c.c->de
#endif /* PLOIDY > 1 */
#if STACK_SIZE > 9
        ,cp->c.c->st[9]
#endif /* STACK_SIZE > 9 */
        );
#if STACK_SIZE > 9
    if (cp->c.c->sp == 9)
        Fbuf[65] = '<';
    FEPrintf(PLN_X, PLN_Y + 9, 0, 0, (char *)(&(Fbuf[0])));
#endif /* STACK_SIZE > 9 */



        tsprintf((char *)(&(Fbuf[0])),
        "           %3d of %3d Cpus  thread [%6d]                        ",
               cp->c.ac,  cp->c.n, cp->c.c->threadid);
    if (cp->c.c->sp == 10)
        Fbuf[65] = '<';
    FEPrintf(PLN_X, PLN_Y + 10, 0, 0, (char *)(&(Fbuf[0])));

#ifdef BGL
    TBglDebugDisplay(cp, d_data, off_char, off_int,
        (I8s *)(&(hostbuf1[0])), (I8s *)(&(hostbuf2[0])),
        (I8s *)(&(hostbuf3[0])), (I8s *)(&(hostbuf4[0])),
        (I8s *)(&(hostbuf5[0])), (I8s *)(&(hostbuf6[0])));
#endif /* BGL */

}

/*
 * ChkBrk - Check for break trap condition for given cell
 *
 * cp - pointer to cell
 *
 * BrkTrapCondBuf.curbrkdat.BrkFnd = 0 - break trap condition not found
 *                                   1 - "     "    "         found
 */
int ChkBrk(cp)
    Pcells cp;
{   I32s waitforbrk, hosttype, off_int;
    Pcells hostcp;

    waitforbrk = 0;
    off_int = (cp->c.c->ip) - (cp->mm.p);
    if (BrkTrapCondBuf.curbrkdat.BrkSet)
    {   switch (BrkTrapCondBuf.curbrkdat.TrapType)
        {   case BRKTYP_GIVENGEN_OFFSET:  /* Given Genotype / Offset */
                if ((cp->d.gi == BrkTrapCondBuf.curbrkdat.prim_genome.gi) &&
                    (cp->d.gen.size == BrkTrapCondBuf.
                    curbrkdat.prim_genome.si) &&
                    (off_int == BrkTrapCondBuf.curbrkdat.genoffset))
                    BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                break;

            case BRKTYP_CURCELL_OFFSET:
                if ((cp == BrkTrapCondBuf.curbrkdat.MicroSlice) &&
                    (off_int == BrkTrapCondBuf.curbrkdat.genoffset))
                {   if(BrkTrapCondBuf.curbrkdat.MicroCpuI>=0)
                    {   if(cp->c.c->threadid==
                            BrkTrapCondBuf.curbrkdat.MicroCpuI)
                            BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                    else
                        BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                }
                break;

            case BRKTYP_ABSTIMEINST:
            case BRKTYP_INTRVLTIMEINST:
                if (CmpEvents(InstExe, BrkTrapCondBuf.
                        curbrkdat.AbsTime) > (-1))
                    BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                break;

            case BRKTYP_GIVENPARAGIVENHOST:  /* Given Parasite / Given Host */
                if ((cp->d.gi == BrkTrapCondBuf.
                    curbrkdat.prim_genome.gi) &&
                    (cp->d.gen.size == BrkTrapCondBuf.
                    curbrkdat.prim_genome.si))
                    if (((off_int < 0) || (off_int >
                        ((cp->mm.s) - 1))) && (!cp->d.flaw))
                    {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                        if (hosttype == 1)
                            if ((hostcp->d.gi == BrkTrapCondBuf.
                                curbrkdat.host_genome.gi) &&
                                (hostcp->d.gen.size == BrkTrapCondBuf.
                                curbrkdat.host_genome.si))
                                BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                break;

            case BRKTYP_ANYPARAANYHOSTTIME:/* Any Parasite / Any Host / Time */
                if (ChkPermTemp(0, cp))
                    if (CmpEvents(sl[cp->d.gen.size]->g[cp->d.gi]->
                        originI, BrkTrapCondBuf.curbrkdat.AbsTime) > (-1))
                        if (((off_int < 0) || (off_int >
                            ((cp->mm.s) - 1))) && (!cp->d.flaw))
                        {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                            if (hosttype == 1)
                                if (ChkPermTemp(1, hostcp))
                                    if (CmpEvents(sl[hostcp->
                                        d.gen.size]->g[hostcp->d.
                                        gi]->originI, BrkTrapCondBuf.
                                        curbrkdat.AbsTime)>(-1))
                                        BrkTrapCondBuf.curbrkdat.BrkFnd=1;
                        }
                break;

            case BRKTYP_ANYGENGIVENTIME:  /* Any Genotype / Given Time */
                if (ChkPermTemp(0, cp))
                    if (CmpEvents(sl[cp->d.gen.size]->g[cp->d.gi]->
                        originI, BrkTrapCondBuf.curbrkdat.AbsTime) > (-1))
                        BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                break;

            case BRKTYP_GIVENPARAANYHOST:  /* Given Parasite / Any Host */
                if ((cp->d.gi == BrkTrapCondBuf.
                    curbrkdat.prim_genome.gi) &&
                    (cp->d.gen.size == BrkTrapCondBuf.
                    curbrkdat.prim_genome.si))
                    if (((off_int < 0) || (off_int >
                        ((cp->mm.s) - 1))) && (!cp->d.flaw))
                    {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                        if (hosttype == 1)
                            if (ChkPermTemp(0, hostcp))
                                BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                break;

            case BRKTYP_ANYPARAGIVENHOST:  /* Any Parasite / Given Host */
                if (ChkPermTemp(0, cp))
                    if (((off_int < 0) || (off_int >
                        ((cp->mm.s) - 1))) && (!cp->d.flaw))
                    {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                        if (hosttype == 1)
                            if ((hostcp->d.gi == BrkTrapCondBuf.
                                curbrkdat.host_genome.gi) &&
                                (hostcp->d.gen.size == BrkTrapCondBuf.
                                curbrkdat.host_genome.si))
                                BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                break;

            case BRKTYP_ANYGENSIZERANGE:  /* Any Genotype / Given Size Range */
                if (ChkPermTemp(0, cp))
                    if ((cp->d.gen.size >= BrkTrapCondBuf.curbrkdat.lowlim)&&
                        (cp->d.gen.size <= BrkTrapCondBuf.curbrkdat.hilim)&&
                        (off_int == BrkTrapCondBuf.curbrkdat.genoffset))
                    {   if(BrkTrapCondBuf.curbrkdat.MicroCpuI>=0)
                        {   if(cp->c.c->threadid==
                                BrkTrapCondBuf.curbrkdat.MicroCpuI)
                                BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                        }
                        else
                            BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                break;

            case BRKTYP_ANYHOST_PARASIZERANGE:/* Any Host/Parasite Siz Range */
                if (ChkPermTemp(0, cp))
                    if (((off_int < 0) || (off_int >
                        ((cp->mm.s) - 1))) && (!cp->d.flaw))
                        if ((cp->d.gen.size >=
                            BrkTrapCondBuf.curbrkdat.lowlim) &&
                            (cp->d.gen.size <=
                            BrkTrapCondBuf.curbrkdat.hilim))
                        {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                            if (hosttype == 1)
                                if (ChkPermTemp(1, hostcp))
                                    BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                        }
                break;

            case BRKTYP_ANYPARA_HOSTSIZERANGE:/* Any Parasite-Host Size Rng */
                if (ChkPermTemp(0, cp))
                    if (((off_int < 0) || (off_int >
                        ((cp->mm.s) - 1))) && (!cp->d.flaw))
                    {   hostcp = FindHost(cp->c.c->ip, &hosttype);
                        if (hosttype == 1)
                            if ((hostcp->d.gen.size >=
                                BrkTrapCondBuf.curbrkdat.lowlim) &&
                                (hostcp->d.gen.size <=
                                BrkTrapCondBuf.curbrkdat.hilim))
                                if (ChkPermTemp(1, hostcp))
                                    BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                    }
                break;

            case BRKTYP_THRDANADATRDY:
                break;

            default:
                break;
        }
        if (!BrkTrapCondBuf.curbrkdat.BrkFnd)
            waitforbrk = 1;
        else
        {   MC_step=1;
            SavBrkDat();
            GoBrkClr();
            TrkCPU(cp,0,-1);
        }
    }
    return waitforbrk;
}

/*
 * ChkBrkBd - Check for break trap at cell birth/death 
 *
 * BrkTrapCondBuf.curbrkdat.BrkFnd = 0 - break trap condition not found
 *                                   1 - "     "    "         found
 */
void ChkBrkBd()
{   double curavg, dlta_avg, dlta_pct;

    if (BrkTrapCondBuf.curbrkdat.BrkSet)
    {   switch (BrkTrapCondBuf.curbrkdat.TrapType)
        {   case BRKTYP_PCTCHGPOP:
                curavg = CalcAvgPopSizRng();
                dlta_avg = curavg - BrkTrapCondBuf.curbrkdat.avgpoprng;
                dlta_avg = (dlta_avg < 0) ? (-1 * dlta_avg) : dlta_avg;
                if (BrkTrapCondBuf.curbrkdat.avgpoprng)
                {   dlta_pct = ((dlta_avg / BrkTrapCondBuf.
                            curbrkdat.avgpoprng) * ((double) 100));
                    if (dlta_pct > BrkTrapCondBuf.curbrkdat.pctchg)
                        BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                }
                else
                    if (dlta_avg)
                        BrkTrapCondBuf.curbrkdat.BrkFnd = 1;
                break;
            default:
                break;
        }
    }
}

/*
 * Str2Event - convert string to Event structure value
 *
 * eventbuf - pointer to string
 *
 * returns : event - Event structure
 *
 */
Event Str2Event(eventbuf)
char *eventbuf;
{   Event event;
    int numlen, mlen;
    char mbuf[10];
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    event.m = 0;
    event.i = 0;
    numlen = strlen(eventbuf);
    if (numlen)
    {   mlen = (numlen > 6) ? (numlen - 6) : 0;
        if (mlen)
        {   strncpy(&(mbuf[0]), eventbuf, mlen);
            mbuf[mlen] = '\0';

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf(&(mbuf[0]), "%d", &(event.m));
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        }

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanf(&(eventbuf[mlen]), "%d", &(event.i));
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    }
    return event;
}

/*
 * RclBrkDat - restore previous break trap data for break trap repeat
 */
void RclBrkDat()
{   I32s result1, svBrkSet;
    Event result;

    svBrkSet=BrkTrapCondBuf.curbrkdat.BrkSet;
    BrkTrapCondBuf.curbrkdat=BrkTrapCondBuf.savbrkdat;
    switch (BrkTrapCondBuf.savbrkdat.TrapType)
    {   case BRKTYP_CURCELL_OFFSET:  /* Current cell/offset */
            BrkTrapCondBuf.curbrkdat.MicroSlice = ce;
            if((BrkTrapCondBuf.savbrkdat.MicroCpuI)>=0)
                if(BrkTrapCondBuf.savbrkdat.trkcurthrd)
                    BrkTrapCondBuf.curbrkdat.
                        MicroCpuI=ce->c.c->threadid;
            break;

        case BRKTYP_ANYGENSIZERANGE: /* Any Genotype / Given Size Range */
            if((BrkTrapCondBuf.savbrkdat.MicroCpuI)>=0)
                if(BrkTrapCondBuf.savbrkdat.trkcurthrd)
                    BrkTrapCondBuf.curbrkdat.
                        MicroCpuI=ce->c.c->threadid;
            break;

        case BRKTYP_INTRVLTIMEINST:  /* relative Time (Instr Exec) */
            result1=AddEvent(&InstExe, &(BrkTrapCondBuf.
                curbrkdat.IntervalTime), &result);
            break;

        case BRKTYP_PCTCHGPOP:
            BrkTrapCondBuf.curbrkdat.avgpoprng = CalcAvgPopSizRng();

        default:
            break;
    }
    switch (BrkTrapCondBuf.savbrkdat.TrapType)
    {   case BRKTYP_GIVENGEN_OFFSET:  /* Given Genotype / Offset */
        case BRKTYP_GIVENPARAGIVENHOST:  /* Given Parasite / Given Host */
        case BRKTYP_PCTCHGPOP:
        case BRKTYP_ANYPARAANYHOSTTIME:  /* Any Parasite/Any Host/Time */
        case BRKTYP_ANYGENGIVENTIME:  /* Any Genotype / Given Time */
        case BRKTYP_GIVENPARAANYHOST:  /* Given Parasite / Any Host */
        case BRKTYP_ANYPARAGIVENHOST:  /* Any Parasite / Given Host */
        case BRKTYP_ANYGENSIZERANGE:  /* Any Genotype / Given Size Range */
        case BRKTYP_ANYPARA_HOSTSIZERANGE:  /* Any Parasite/Host Size Range */
        case BRKTYP_ANYHOST_PARASIZERANGE:  /* Any Host/Parasite Size Range */
        case BRKTYP_THRDANADATRDY:
            if (!svBrkSet)
                OpenGeneBanker();
            break;
        default:
            break;
    }
    MC_step=1;
    BrkTrapCondBuf.curbrkdat.BrkFnd = 0;
}

/*
 * FindHost - find resident of given soup address
 *
 * soupaddr - given soup address
 *
 * returns: scncp - pointer to resident if any at given address
 *          hosttype - 0 - no resident
 *                     1 - adult
 *                     2 - daughter
 */
Pcells FindHost(soupaddr, hosttype)
    I32s soupaddr;
    I32s *hosttype;
{   Pcells scncp;
    I32s hostend, daughtend;

    (*hosttype)=0;

    /* select currently active cell */
    scncp = ce;

    for(scncp=ce; !(*hosttype);)
    {   hostend=(scncp->mm.p)+(scncp->mm.s)-1;
        if(!((soupaddr<(scncp->mm.p))||(soupaddr>hostend)))
        {   (*hosttype) = 1;
            break;
        }
        if(scncp->md.s)
        {   daughtend=(scncp->md.p)+(scncp->md.s)-1;
            if(!((soupaddr<(scncp->md.p))||(soupaddr>daughtend)))
            {   (*hosttype) = 2;
                break;
            }
        }
        scncp=&cells[scncp->q.n_time.a][scncp->q.n_time.i];
        if(scncp==ce)
            break;
    }
    return scncp;
}

/*
 * GoRepeatBrk - Repeat previous break trap setting
 */
void GoRepeatBrk()
{   GoBrkClr();
    if (BrkTrapCondBuf.savbrkdat.BrkSet)
    {   MC_step = 1;
        RclBrkDat();
    }
    else
    {   tsprintf((char *)(&(Fbuf[0])),
            "no previous break settings available");
        FEError(-2208, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
}

/*
 * Enter start address for debugger soup view screen
 *
 * returns: soupviewstrt - global
 */
void ViewSoupStartAddr()
{   I32s tsoupviewstrt;
    I32s abortflag;

    abortflag = 0;
    soupviewstrt = 0;

    for(;;)
    {

#if FRONTEND == BASIC
        FEClrmsg(PLN_Y, (I8s *)"");
        FEBanner();
        FEStats();
#endif /* FRONTEND == BASIC */

        /* display soup limits */
        tsprintf((char *)(&(Fbuf[0])),
            "soup address limits = 0 - %d\n", SoupSize-1);
        FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));

        /* enter view start address */
        tsoupviewstrt = EnterLongVal(PLN_Y + 1,
            "Enter view start address (enter to abort)->",
            &abortflag);

        if (!abortflag)
        {   if ((tsoupviewstrt >= 0) && (tsoupviewstrt < SoupSize))
            {   soupviewstrt = tsoupviewstrt;
                break;
            }
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid soup address");
                FEError(-2209, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
            break;
    }
#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
}

/*
 * DisplaySoupPage - display soup memory page for debugger soup view screen
 */
void DisplaySoupPage()
{

    I32s soupviewaddr, maxviewstrt;
    int souppglnct,viewsouplines=LINES-(PLN_Y+VIEWSOUPHDRLINES+1);
    char hostbuf1[20], hostbuf2[10], hostbuf3[7];

    maxviewstrt=(viewsouplines>SoupSize)?0:SoupSize-viewsouplines;
    soupviewstrt=(soupviewstrt<0)?0:((soupviewstrt>
        maxviewstrt)?maxviewstrt:soupviewstrt);
    soupviewaddr = ad(soupviewstrt);

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    tsprintf((char *)(&(Fbuf[0])),
"        soup      soup                    current          host      host\n");
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"       address    byte    opcode         resident         status    offset\n");
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
"      ---------  ------  ---------  -------------------  ---------  ------\n");
    FEPrintf(PLN_X, PLN_Y + 2, 0, 0, (char *)(&(Fbuf[0])));

    souppglnct = 0;
    for(;;)
    {   if (souppglnct<viewsouplines)
        {   if (soupviewaddr < SoupSize)
            {   ViewSoupHost(soupviewaddr, &(hostbuf1[0]),
                    &(hostbuf2[0]), &(hostbuf3[0]));
                tsprintf((char *)(&(Fbuf[0])),
                    "      [%7d]  [0x%.2x]  %-9s  %-19s  %-9s  %-6s\n",
                    soupviewaddr,
#if PLOIDY == 1
                    soup[soupviewaddr],
                    (soup[soupviewaddr]>=InstNum)?(I8u *)"data" :
                    (I8u *)(id[soup[soupviewaddr]].mn),
#else /* PLOIDY == 1 */
                    soup[soupviewaddr][ce->c.c->ex],
                    ((soup[soupviewaddr][ce->c.c->ex])>=
                    InstNum)?((I8u *)"data"):((I8u *)
                    (id[soup[soupviewaddr][ce->c.c->ex]].mn)),
#endif /* PLOIDY == 1 */
                    &(hostbuf1[0]), &(hostbuf2[0]), &(hostbuf3[0]));
                FEPrintf(PLN_X,PLN_Y+3+souppglnct,
                    0,0,(char *)(&(Fbuf[0])));
                souppglnct++;
                soupviewaddr = ad(soupviewaddr + 1);
            }
            else
                break;
        }
        else
            break;
    }
}

/*
 * CalcAvgPopSizRng - calc average pop over size range
 *
 * BrkTrapCondBuf.curbrkdat.lowlim = low limit
 * BrkTrapCondBuf.curbrkdat.hilim  = high limit
 *
 * return: avgpoprng - average pop over size range
 *
 */
double CalcAvgPopSizRng()
{   int i, j;
    I32s totsizpoprng, totsizcntrng;
    double avgpoprng;

    totsizpoprng = 0;
    totsizcntrng = 0;

    i = BrkTrapCondBuf.curbrkdat.lowlim;
    for(;;)
        if (i < siz_sl)
            if (i > BrkTrapCondBuf.curbrkdat.hilim)
                break;
            else
            {   if (sl[i])
                {   if (BrkTrapCondBuf.curbrkdat.permtemp[0] == 'b')
                        totsizpoprng += sl[i]->num_c;
                    else
                        for(j=0;j<sl[i]->a_num;j++)
                            if(!TNULL(sl[i]->g[j]))
                            {   if (BrkTrapCondBuf.curbrkdat.
                                    permtemp[0] == 'p')
                                {   if (IsBit(sl[i]->g[j]->bits, 0))
                                        totsizpoprng+=sl[i]->g[j]->pop;
                                }
                                else
                                    if(!IsBit(sl[i]->g[j]->bits, 0))
                                        totsizpoprng += sl[i]->g[j]->pop;
                            }
                    totsizcntrng++;
                }
                i++;
            }
        else
            break;
    if (totsizcntrng)
        avgpoprng = ((double) totsizpoprng) / (double) (totsizcntrng);
    else
        avgpoprng = 0;
    return avgpoprng;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
#ifndef __GUI__
/*
 * DbgDisplCtrl - debugger display control
 *                scroll up/down 1 line
 *                page up/down 1 screen
 *                toggle between step and soup view debug displays
 * cp - cell to be displayed in step mode
 *
 */
void DbgDisplCtrl(cp)
    Pcells cp;
{   char cc;
    I32s loopflag=0,viewsouplines=LINES-(PLN_Y+VIEWSOUPHDRLINES+1);

    loopflag=1;
    while(loopflag)
    {   FEBanner();
        FEStats();
        if (dbgdisptype == 0)
            tsprintf((char *)(&(Fbuf[0])),
                "t-toggle display  q-quit->\n");
        else
            tsprintf((char *)(&(Fbuf[0])),
                "%s%s", "a-addr  U/D-page  u/d-scroll",
                "  i-IP  t-toggle display  q-quit->");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        cc = (I8s)FEGetch(HLP_X + strlen(
            (const char *)(&(Fbuf[0]))), HLP_Y);
        switch (cc)
        {   case 'a':
            if (dbgdisptype == 1)
            {   ViewSoupStartAddr();
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 'U':
            if (dbgdisptype == 1)
            {   soupviewstrt+=viewsouplines;
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 'D':
            if (dbgdisptype==1)
            {   soupviewstrt-=viewsouplines;
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 'u':
            if (dbgdisptype == 1)
            {   soupviewstrt++;
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 'd':
            if (dbgdisptype == 1)
            {   soupviewstrt--;
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 'i':
            if (dbgdisptype == 1)
            {   soupviewstrt = ce->c.c->ip;
                FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DisplaySoupPage();
            }
            break;
        case 't':
            if (dbgdisptype == 0)
            {   FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                dbgdisptype = 1;
                soupviewstrt = ce->c.c->ip;
                DisplaySoupPage();
            }
            else
            {   FEClrmsg(MSG_Y, (I8s *)"");
                FEBanner();
                FEStats();
                DebugDisplay(cp);
                dbgdisptype = 0;
            }
            break;
        case 'q':
            loopflag = 0;
            break;
        default:
            break;
        }
    }
}
#endif /* __GUI__ */

/*
 * CmpEvents - compare two Event structures
 *
 * event1, event2 - Event structures to be compared
 *
 * return: -1 - event1 < event2
 *          0 - event1 = event2
 *          1 - event1 > event2
 */
int CmpEvents(event1, event2)
Event event1, event2;
{   int eventcmpcd;

    if ((event1.m) < (event2.m))
        eventcmpcd = -1;
    else
        if ((event1.m) == (event2.m))
            if ((event1.i) < (event2.i))
                eventcmpcd = -1;
            else
                if ((event1.i) == (event2.i))
                    eventcmpcd = 0;
                else
                    eventcmpcd = 1;
        else
            eventcmpcd = 1;
    return eventcmpcd;
}

/*
 * DisasmMenu - disassembly menu
 *
 * cp - pointer to cell to be disassembled
 */
void DisasmMenu(cp)
    Pcells cp;
{   int menuline, menuopt, abortflag;
    char *menutitles[5];
    int scanfrtn;
    I32s loopflag;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    menutitles[0]="current creature"; 
    menutitles[1]="specific genome";
    menutitles[2]="resident of specific address";
    menutitles[3]="most recent thrd analysis data trapped";
    menutitles[(sizeof(menutitles)/sizeof(char *))-1]=0;

    OpenGeneBanker();
    abortflag = 0;
    loopflag = 1;
    while (loopflag)
    {   DisplayMenu(1, menutitles, 77, 3, 2, 1, &menuline);
        FEGetStr(MSG_X + strlen((const char *)
            (&(Fbuf[0]))), MSG_Y + menuline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   if (!strcmp((const char *)(&(FGSbuf[0])), "q"))
            {   abortflag = 1;
                loopflag = 0;
            }
            else
            {

#ifdef SIGBLOCK
                SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
                scanfrtn=sscanf((const char *)(&(FGSbuf[0])), "%d", &menuopt);
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

                if (scanfrtn == 1)
                {   switch (menuopt)
                    {   case 1:  /* current creature */
                            Disasm_Creature(cp, menuline+1);
                            loopflag = 0;
                            break;
                        case 2:  /* specific genome */
                            Disasm_SpecGenome(menuline+1);
                            loopflag = 0;
                            break;
                        case 3:  /* resident of specific address */
                            Disasm_Resident(menuline+1);
                            loopflag = 0;
                            break;
                        case 4:  /* most recent thrd analysis data trapped */
                            Disasm_ThrdAnaDatRdy(menuline+1);
                            loopflag = 0;
                            break;
                        default:
                            tsprintf((char *)(&(Fbuf[0])),
                                "invalid menu option");
                            FEError(-2210, NOEXIT, NOWRITE,
                                (char *)(&(Fbuf[0])));
                            break;
                    }
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])),
                        "invalid menu option");
                    FEError(-2211, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
            }
        }
        else
        {   abortflag = 1;
            loopflag = 0;
        }
    }
    CloseGeneBanker();
}

/*
 * Disasm_Creature - disassemble creature
 *
 * cp - pointer to creature to be disassembled
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 *
 */
void Disasm_Creature(cp, lineoffset)
Pcells cp;
I32s lineoffset;
{   I32s tarpt, abortflag, curlineoffset, dtlrpt;
    char curorig, ynspec, file[80], sumdtl;
    I32s gidx;
    FILE *srcfilptr;

    tarpt=0;
    abortflag=0;
    curorig='o';
    dtlrpt=0;
    curlineoffset=lineoffset;
    if((cp->d.gi)!=(cp->d.origgi))
        curorig=CurrOrigSpec(curlineoffset++, &abortflag);
    if(!abortflag)
    {   gidx=(curorig=='o')?(cp->d.origgi):(cp->d.gi);
        tsprintf((char *)(&(file[0])), "%s%.4d%3s",
            &(GenebankPath[0]), cp->d.gen.size,
            sl[cp->d.gen.size]->g[gidx]->gen.label);
        if((srcfilptr=tfopen((I8s *)(&(file[0])), (I8s *)"rb")))
        {   ynspec=YNSpec(MSG_Y+(curlineoffset++), MSG_X,
                "disassembly exists (replace) ?", &abortflag);
            if((!abortflag)&&(ynspec=='n'))
                abortflag=1;
            fclose(srcfilptr);
        }
        if(!abortflag)
        {   if((curorig=='o')&&(ThreadAnalysis))
            {   ynspec=YNSpec(MSG_Y+(curlineoffset++), MSG_X,
                    "include thread analysis ?", &abortflag);
                if(!abortflag)
                    tarpt=(ynspec=='y')?1:0;
            }
            if(!abortflag)
            {   if(tarpt)
                {   sumdtl=SummDtlSpec(curlineoffset++, &abortflag);
                    if(!abortflag)
                        dtlrpt=(sumdtl=='d')?1:0;
                }
                if(!abortflag)
                {   WritAscFile(sl[cp->d.gen.size]->
                        g[gidx], (I8s *)(&(file[0])), tarpt,
#ifdef NET
                        sl[cp->d.gen.size]->sucsiznslrat,
                        sl[cp->d.gen.size]->svsucsiznsl,
                        sl[cp->d.gen.size]->svsiznsl
#else /* NET */
                        0.0, 0, 0
#endif /* NET */
                        , dtlrpt, 0, 0, 0, NULL, 0,
                        sl[cp->d.gen.size]->genelkup,
                        &(sl[cp->d.gen.size]->slst_gendef),
                        SplitTissueAna);
                }
            }
        }
    }
}

/*
 * Disasm_SpecGenome - disassemble specific genome
 *
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 */
void Disasm_SpecGenome(lineoffset)
I32s lineoffset;
{   GlIndex tmpgen;
    I32s tarpt, abortflag, curlineoffset, dtlrpt,n;
    char ynspec, file[80], sumdtl;
    FILE *srcfilptr, *afp;
    indx_t *indx, *tindx;
    I32u errflag;
    head_t head;

    tarpt=0;
    abortflag=0;
    errflag=0;
    dtlrpt=0;

    for(;;)
    {   curlineoffset=lineoffset;
        tmpgen = EnterGenotype(MSG_Y+(curlineoffset++),
            "Enter genotype to disassemble (eg 0080aaa) (enter to abort)->",
            &abortflag, 1);
        if(!abortflag)
        {   tsprintf((char *)(&(file[0])), "%s%.4d%3s",
                &(GenebankPath[0]), tmpgen.si, Int2Lbl(tmpgen.gi));
            if((srcfilptr=tfopen((I8s *)(&(file[0])), (I8s *)"rb")))
            {   ynspec=YNSpec(MSG_Y+(curlineoffset++), MSG_X,
                    "disassembly exists (replace) ?", &abortflag);
                if((!abortflag)&&(ynspec=='n'))
                    abortflag=1;
                fclose(srcfilptr);
            }
            if(!abortflag)
            {   if(ThreadAnalysis)
                {   ynspec=YNSpec(MSG_Y+(curlineoffset++), MSG_X,
                        "include thread analysis ?", &abortflag);
                    if(!abortflag)
                        tarpt=(ynspec=='y')?1:0;
                }
                if(!abortflag)
                {   if(((I32u)((unsigned long)(
                        sl[tmpgen.si]->g[tmpgen.gi])))==(I32u)1)
                    {   tsprintf((char *)(&(Buff[0])),
                            "%s%.4d.gen",
                            GenebankPath, tmpgen.si);
                        if((afp=tfopen(Buff, (I8s *)"rb")))
                        {   head = read_head(afp);
                            indx = read_indx(afp, &head);
                            if(indx)
                            {   n=find_gen(indx, Int2Lbl(tmpgen.gi), head.n);
                                if(n<head.n)
                                {   tindx = &indx[n];
                                    sl[tmpgen.si]->g[tmpgen.gi]=
                                        get_gen(afp, &head, tindx, n);
                                    thfree((void *)indx,
                                        head.n_alloc*sizeof(indx_t), 600);
                                    indx = NULL;
                                    tfclose(afp);
                                }
                                else
                                    errflag=1;
                            }
                            else
                                errflag=1;
                        }
                        else
                            errflag=1;
                    }
                    if(!errflag)
                    {   if(tarpt)
                        {   sumdtl=SummDtlSpec(curlineoffset++, &abortflag);
                            if(!abortflag)
                                dtlrpt=(sumdtl=='d')?1:0;
                        }
                        if(!abortflag)
                        {   WritAscFile(sl[tmpgen.si]->
                                g[tmpgen.gi], (I8s *)(&(file[0])), tarpt,
#ifdef NET
                                sl[tmpgen.si]->sucsiznslrat,
                                sl[tmpgen.si]->svsucsiznsl,
                                sl[tmpgen.si]->svsiznsl
#else /* NET */
                                0.0, 0, 0
#endif /* NET */
                            , dtlrpt, 0, 0, 0, NULL, 0,
                            sl[tmpgen.si]->genelkup,
                            &(sl[tmpgen.si]->slst_gendef),
                            SplitTissueAna);
                        }
                        break;
                    }
                    errflag=0;
                }
                else
                   break;
            }
            else
               break;
        }
        else
            break;
    }
}

/*
 * Disasm_Resident - disassemble soup resident
 *
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 */
void Disasm_Resident(lineoffset)
I32s lineoffset;
{   I32s disasmaddr, tarpt, abortflag, curlineoffset;
    I32s hosttype, dtlrpt, loopflag;
    Pcells rescp;
    char curorig, ynspec, file[80], sumdtl;
    FILE *srcfilptr;
    I32s gidx;

    tarpt=0;
    abortflag=0;
    curorig='o';
    dtlrpt=0;
    loopflag=1;
    while(loopflag)
    {

#if FRONTEND == BASIC
        FEClrmsg(MSG_Y, (I8s *)"");
        FEBanner();
        FEStats();
#endif /* FRONTEND == BASIC */

        curlineoffset=lineoffset;

        /* display soup limits */
        tsprintf((char *)(&(Fbuf[0])),
            "soup address limits = 0 - %d\n", SoupSize-1);
        FEPrintf(MSG_X, MSG_Y+(curlineoffset++),
            0, 0, (char *)(&(Fbuf[0])));

        /* enter view start address */
        disasmaddr = EnterLongVal(MSG_Y+(curlineoffset++),
            "address of resident to disassemble (enter to abort)->",
            &abortflag);

        if (!abortflag)
        {   if ((disasmaddr >= 0) && (disasmaddr < SoupSize))
            {   rescp = FindHost(disasmaddr, &hosttype);

                switch (hosttype)
                {   case 1:  /* occupied */

                        /* display host identity */
                        if (rescp->ld)
                        {   if((rescp->d.gi)!=(rescp->d.origgi))
                                curorig=CurrOrigSpec(
                                    curlineoffset++, &abortflag);
                            if(!abortflag)
                            {   gidx=(curorig=='o')?
                                    (rescp->d.origgi):(rescp->d.gi);
                                tsprintf((char *)(&(file[0])), "%s%.4d%3s",
                                    &(GenebankPath[0]), rescp->d.gen.size,
                                    sl[rescp->d.gen.size]->
                                    g[gidx]->gen.label);
                                if((srcfilptr=tfopen((I8s *)
                                    (&(file[0])), (I8s *)"rb")))
                                {   ynspec=YNSpec(
                                        MSG_Y+(curlineoffset++),
                                        MSG_X,
                                        "disassembly exists (replace) ?",
                                        &abortflag);
                                    if((!abortflag)&&(ynspec=='n'))
                                        abortflag=1;
                                    fclose(srcfilptr);
                                }
                                if(!abortflag)
                                {   if((curorig=='o')&&(ThreadAnalysis))
                                    {   ynspec=YNSpec(
                                            MSG_Y+(curlineoffset++),
                                            MSG_X,
                                            "include thread analysis ?",
                                            &abortflag);
                                        if(!abortflag)
                                            tarpt=(ynspec=='y')?1:0;
                                    }
                                    if(!abortflag)
                                    {   if(tarpt)
                                        {   sumdtl=SummDtlSpec(
                                                curlineoffset++, &abortflag);
                                            if(!abortflag)
                                                dtlrpt=(sumdtl=='d')?1:0;
                                        }
                                        if(!abortflag)
                                        {   WritAscFile(sl[rescp->d.gen.size]->
                                                g[gidx], (I8s *)(&(file[0])), tarpt,
#ifdef NET
                                                sl[rescp->d.gen.size]->
                                                sucsiznslrat,

                                                sl[rescp->d.gen.size]->
                                                svsucsiznsl,

                                                sl[rescp->d.gen.size]->
                                                svsiznsl

#else /* NET */
                                                0.0, 0, 0
#endif /* NET */
                                                , dtlrpt, 0, 0, 0, NULL, 0,
                                                sl[rescp->d.gen.size]->
                                                    genelkup,
                                                &(sl[rescp->d.gen.size]->
                                                slst_gendef),
                                                SplitTissueAna);
                                        }
                                    }
                                }
                            }
                            loopflag=0;
                        }
                        else
                        {   tsprintf((char *)(&(Fbuf[0])),
                                "resident is dead");
                            FEError(-2215, NOEXIT, NOWRITE,
                                (char *)(&(Fbuf[0])));
                        }
                        break;

                    case 2:  /* embryonic resident */
                        tsprintf((char *)(&(Fbuf[0])),
                            "resident is embryonic");
                        FEError(-2216, NOEXIT, NOWRITE,
                            (char *)(&(Fbuf[0])));
                        break;

                    default:  /* free memory */
                        tsprintf((char *)(&(Fbuf[0])),
                            "location is in free memory");
                        FEError(-2217, NOEXIT, NOWRITE,
                            (char *)(&(Fbuf[0])));
                        break;
                }
            }
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid soup address");
                FEError(-2218, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
            loopflag = 0;
    }
}

/*
 * Disasm_ThrdAnaDatRdy - disassembly for most recent
 *                        thrd analysis data trapped
 *
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 */
void Disasm_ThrdAnaDatRdy(lineoffset)
I32s lineoffset;
{   GList *tgl;
    char sumdtl;
    I32s dtlrpt, abortflag, curlineoffset;

    abortflag=0;
    curlineoffset=lineoffset;
    dtlrpt=0;
    if((rcnttardy.si)>=0)
    {   if((tgl=sl[rcnttardy.si]->g[rcnttardy.gi]=
            gq_read(rcnttardy.si, rcnttardy.gi)))
        {   tsprintf((char *)(&(Fbuf[0])),
                "genotype %s trapped\n", &(SmplNam[0]));
            FEPrintf(MSG_X, MSG_Y+(curlineoffset++),
                0, 0, (char *)(&(Fbuf[0])));

            sumdtl=SummDtlSpec(curlineoffset++, &abortflag);
            if(!abortflag)
            {   dtlrpt=(sumdtl=='d')?1:0;
                WritAscFile(sl[rcnttardy.si]->
                    g[rcnttardy.gi], &(SmplNam[0]), 1,
#ifdef NET
                    sl[rcnttardy.si]->sucsiznslrat,
                    sl[rcnttardy.si]->svsucsiznsl,
                    sl[rcnttardy.si]->svsiznsl
#else /* NET */
                    0.0, 0, 0
#endif /* NET */
                    , dtlrpt, 0, 0, 0, NULL, 0,
                    sl[rcnttardy.si]->genelkup,
                    &(sl[rcnttardy.si]->slst_gendef),
                    SplitTissueAna);
            }
        }
    }
    else
    {   tsprintf((char *)(&(Fbuf[0])),
            "no thread analysis data trapped");
        FEError(-2213, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
}

/*
 * PermTempBothSpec - perm/temp/both genotype specifications
 *                    case insensitive
 *
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 * speclbl - specification label for prompt
 *           (ex. "genotype", "parasite", "host")
 * abortflag - pointer to abort flag
 *
 * returns: *abortflag - 0 - entry not aborted
 *                       1 - entry aborted
 *
 *          permtemp = p - permanent genotypes
 *                     t - temporary genotypes
 *                     b - both permanent and temporary genotypes
 */
char PermTempBothSpec(lineoffset, speclbl, abortflag)
    I32s lineoffset, *abortflag;
    char *speclbl;
{   int loopflag;
    char permtemp;

    permtemp = 0;
    loopflag = 1;                   
    while (loopflag)
    {   tsprintf((char *)(&(Fbuf[0])),
            "%s - p-permanent, t-temporary, b-both->", speclbl);
        FEPrintf(PLN_X,PLN_Y+lineoffset,
            0,0,(char *)(&(Fbuf[0])));
        FEGetStr(PLN_X+strlen((char *)
            (&(Fbuf[0]))),PLN_Y+lineoffset);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   switch (FGSbuf[0])
            {   case 'p':
                case 'P':
                    loopflag=0;
                    permtemp='p';
                    break;
                case 't':
                case 'T':
                    loopflag=0;
                    permtemp='t';
                    break;
                case 'b':
                case 'B':
                    loopflag=0;
                    permtemp='b';
                    break;
            default:
                tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2219, NOEXIT, NOWRITE,
                    (char *)(&(Fbuf[0])));
            }
        }
        else
        {   loopflag = 0;
            (*abortflag) = 1;
        }
    }
    return permtemp;
}

/*
 * YNSpec - y/n specification - case insensitive
 *
 * line, col - prompt coordinates
 * prompt - pointer to prompt string
 * abortflag - pointer to abort flag
 *
 * returns: *abortflag - 0 - entry not aborted
 *                       1 - entry aborted
 *          ynspec - 'y' - yes
 *                   'n' - no
 */
char YNSpec(line, col, prompt, abortflag)
I32s line, col, *abortflag;
    char *prompt;
{   int loopflag;
    char ynspec;

    ynspec = 'n';
    loopflag = 1;
    while (loopflag)
    {   tsprintf((char *)(&(Fbuf[0])),
            "%s - specify (y/n)->", prompt);
        FEPrintf(col, line, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(col + strlen((char *)(&(Fbuf[0]))), line);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   switch (FGSbuf[0])
            {   case 'y':
                case 'Y':
                    ynspec='y';
                    loopflag=0;
                    break;
                case 'n':
                case 'N':
                    ynspec='n';
                    loopflag=0;
                    break;
                default:
                    tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-2230, NOEXIT, NOWRITE,
                        (char *)(&(Fbuf[0])));
            }
        }
        else
        {   loopflag = 0;
            (*abortflag) = 1;
        }
    }
    return ynspec;
}

/*
 * CurrOrigSpec - curr/orig genotype specification
 *                case insensitive
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 * abortflag - pointer to abort flag
 *
 * returns: *abortflag - 0 - entry not aborted
 *                       1 - entry aborted
 *          cospec - 'c' - current
 *                   'o' - original
 */
char CurrOrigSpec(lineoffset, abortflag)
I32s lineoffset, *abortflag;
{   int loopflag;
    char cospec;

    cospec = 'o';
    loopflag = 1;
    while (loopflag)
    {   tsprintf((char *)(&(Fbuf[0])),
            "current/original genotype - specify (c/o)->");
        FEPrintf(MSG_X,MSG_Y+lineoffset,0,0,(char *)(&(Fbuf[0])));
        FEGetStr(MSG_X+strlen((const char *)
            (&(Fbuf[0]))), MSG_Y+lineoffset);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   switch (FGSbuf[0])
            {   case 'c':
                case 'C':
                    cospec='c';
                    loopflag=0;
                    break;
                case 'o':
                case 'O':
                    cospec='o';
                    loopflag=0;
                    break;
                default:
                    tsprintf((char *)(&(Fbuf[0])),
                        "invalid entry");
                    FEError(-2212, NOEXIT, NOWRITE,
                        (char *)(&(Fbuf[0])));
            }
        }
        else
        {   loopflag = 0;
            (*abortflag) = 1;
        }
    }
    return cospec;
}

/*
 * SummDtlSpec - summary/detail rpt specification
 *
 * lineoffset - offset relative to MSG_Y at which to start dialog prompts
 * abortflag - pointer to abort flag
 *
 * returns: *abortflag - 0 - entry not aborted
 *                       1 - entry aborted
 */
char SummDtlSpec(lineoffset, abortflag)
I32s lineoffset, *abortflag;
{   int loopflag;
    char sdspec;

    sdspec = 's';
    loopflag = 1;
    while (loopflag)
    {   tsprintf((char *)(&(Fbuf[0])),"summary/detail - specify (s/d)->");
        FEPrintf(MSG_X,MSG_Y+lineoffset,0,0,(char *)(&(Fbuf[0])));
        FEGetStr(MSG_X + strlen((const char *)
            (&(Fbuf[0]))), MSG_Y+lineoffset);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   switch (FGSbuf[0])
            {   case 's':
                case 'S':
                    sdspec='s';
                    loopflag=0;
                    break;
                case 'd':
                case 'D':
                    sdspec='d';
                    loopflag=0;
                    break;
                default:
                    tsprintf((char *)(&(Fbuf[0])),
                        "invalid entry");
                    FEError(-2214, NOEXIT, NOWRITE,
                        (char *)(&(Fbuf[0])));
            }
        }
        else
        {   loopflag = 0;
            (*abortflag) = 1;
        }
    }
    return sdspec;
}

/*
 * ChkPermTemp - check for permanent or temporary genotype in trap
 *
 * permtempsub - subscript in break trap specification of genotype
 *               to be checked (for break traps involving multiple
 *                              genotypes)
 * cp - pointer to cell to be checked
 *
 * returns: permtempfnd - 0 - specified kind of genotype not found
 *                        1 - "         "    "  "        found
 */
int ChkPermTemp(permtempsub, cp)
    int permtempsub;
    Pcells cp;
{   int permtempfnd;

    permtempfnd = 0;
    if (BrkTrapCondBuf.curbrkdat.permtemp[permtempsub] == 'b')
    {   permtempfnd = 1;
    }
    else
    {   if (BrkTrapCondBuf.curbrkdat.permtemp[permtempsub] == 'p')
        {   if (IsBit(sl[cp->d.gen.size]->g[cp->d.gi]->bits, 0))
            {   permtempfnd = 1;
            }
        }
        else
        {   if (!IsBit(sl[cp->d.gen.size]->g[cp->d.gi]->bits, 0))
            {   permtempfnd = 1;
            }
        }
    }
    return permtempfnd;
}

/*
 * SzClsGenSpec - specification to break on size class or genotype
 *                (ex. thread analysi data ready for specified size
 *                     class or specified genotype)
 *
 * promptline - line number for prompt
 * abortflag - pointer to abort flag
 *
 * returns: *abortflag - 0 - entry not aborted
 *                       1 - entry aborted
 *          szclsgn - 's' - size class
 *                    'g' - genotype
 */
I8s SzClsGenSpec(promptline, abortflag)
I32s promptline, *abortflag;
{   I32s loopflag;
    I8s szclsgn;

    (*abortflag)=0;
    szclsgn='s';
    loopflag=1;
    while(loopflag)
    {
#if FRONTEND == BASIC
        FEClrmsg(promptline, (I8s *)"");
        FEBanner();
        FEStats();
#endif /* FRONTEND == BASIC */

        tsprintf((char *)(&(Fbuf[0])),
            "specify size class or genotype (s/g)->");
        FEPrintf(MSG_X, promptline, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(MSG_X+strlen((const char *)
            (&(Fbuf[0]))), promptline);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   switch(FGSbuf[0])
            {   case 's':
                case 'S':
                    szclsgn='s';
                    loopflag=0;
                    break;
                case 'g':
                case 'G':
                    szclsgn='g';
                    loopflag=0;
                    break;
                default:
                    tsprintf((char *)(&(Fbuf[0])),
                        "invalid entry");
                    FEError(-2232, NOEXIT, NOWRITE,
                        (char *)(&(Fbuf[0])));
            }
        }
        else
        {   loopflag=0;
            (*abortflag)=1;
        }
    }
    return szclsgn;
}

/******* Break Trap Settings *********/

/*************************************/
/* Given Genotype / Offset           */
/*************************************/
void Brk_GivenGen_Offset()
{   GlIndex brkgen;
    I32s offset=0;
    I32s abortflag;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    /* enter genotype */
    brkgen = EnterGenotype(PLN_Y,
        "Enter genotype (eg 0080aaa) (enter to abort)->", &abortflag, 1);

    if (!abortflag)
    {
        /* enter creature offset */
        offset = EnterLongVal(PLN_Y + 1,
            "Enter creature offset (enter to abort)->", &abortflag);
    }
    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_GIVENGEN_OFFSET;
        BrkTrapCondBuf.curbrkdat.prim_genome.gi = brkgen.gi;
        BrkTrapCondBuf.curbrkdat.prim_genome.si = brkgen.si;
        BrkTrapCondBuf.curbrkdat.genoffset = offset;
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Current cell/offset               */
/*************************************/
void Brk_CurCell_Offset()
{   I32s offset;
    I32s abortflag;
    char thrdtrk;
    I32s trkthrdid=0;

    abortflag = 0;
    GoBrkClr();
    thrdtrk='n';

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    tsprintf((char *)(&(Fbuf[0])), "current cell is %.4d%3s\n",
        ce->d.gen.size, Int2Lbl(ce->d.gi));
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));

    /* enter creature offset */
    offset = EnterLongVal(PLN_Y+1,
        "Enter creature offset (enter to abort)->", &abortflag);

    if(!abortflag)
        thrdtrk=YNSpec(PLN_Y+2, PLN_X, "track thread ?", &abortflag);
  
    if(!abortflag)
        if(thrdtrk=='y')
            trkthrdid=EnterLongVal(PLN_Y+3,
                "Enter thread id (-1=curr thrd, enter to abort)->",
                &abortflag);

    if(!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_CURCELL_OFFSET;
        if(thrdtrk=='y')
            if(trkthrdid<0)
            {   BrkTrapCondBuf.curbrkdat.trkcurthrd=1;
                TrkCPU(ce,0,-1);
            }
            else
                TrkCPU(ce,0,trkthrdid);
        else
            TrkCell(ce);
        BrkTrapCondBuf.curbrkdat.genoffset=offset;
    }
    else
        GoBrkClr();
}

/*************************************/
/* Absolute Time (Instr Exec)        */
/*************************************/
void Brk_AbsTimeInst()
{   Event stoptime;
    I32s abortflag;

    abortflag = 0;
    GoBrkClr();

    /* enter absolute time in instructions */
    for(;;)
    {
#if FRONTEND == BASIC
        FEClrmsg(PLN_Y, (I8s *)"");
        FEBanner();
        FEStats();
#endif /* FRONTEND == BASIC */
        if (InstExe.m)
            tsprintf((char *)(&(Fbuf[0])),
                "current time (instructions executed) = %d%.6d\n",
                InstExe.m, InstExe.i);
        else
            tsprintf((char *)(&(Fbuf[0])),
                "current time (instructions executed) = %6d\n",
                InstExe.i);
        FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
        tsprintf((char *)(&(Fbuf[0])),
            "Enter break time (enter to abort)->");
        FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
        FEGetStr(PLN_X + strlen((const char *)
            (&(Fbuf[0]))), PLN_Y+1);
        if (strlen((const char *)(&(FGSbuf[0]))))
        {   stoptime = Str2Event((char *)(&(FGSbuf[0])));
            if (CmpEvents(stoptime, InstExe) == 1)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid start time");
                FEError(-2220, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
        {   abortflag = 1;
            break;
        }
    }
    if (!abortflag)
    {

        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ABSTIMEINST;
        BrkTrapCondBuf.curbrkdat.AbsTime.m = stoptime.m;
        BrkTrapCondBuf.curbrkdat.AbsTime.i = stoptime.i;
    }
    else
        GoBrkClr();
}

/*************************************/
/* Time Interval (Instr Exec)        */
/*************************************/
void Brk_IntrvlTimeInst()
{   Event stoptime, intervaltime;
    I32s abortflag;

    abortflag = 0;
    GoBrkClr();

    /* enter interval time in instructions */
#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */
    if (InstExe.m)
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %d%.6d\n",
            InstExe.m, InstExe.i);
    else
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %6d\n",
            InstExe.i);
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Enter interval time (enter to abort)->");
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    FEGetStr(PLN_X + strlen((const char *)(&(Fbuf[0]))), PLN_Y + 1);
    if (strlen((const char *)(&(FGSbuf[0]))))
    {   intervaltime = Str2Event((char *)(&(FGSbuf[0])));
        stoptime.m = 0;
        stoptime.i = InstExe.i + intervaltime.i;
        if (stoptime.i > 999999)
        {   stoptime.i = stoptime.i - 1000000;
            stoptime.m = 1;
        }
        stoptime.m += InstExe.m + intervaltime.m;
    }
    else
    {   abortflag = 1;
    }

    if (!abortflag)
    {

        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_INTRVLTIMEINST;
        BrkTrapCondBuf.curbrkdat.AbsTime.m = stoptime.m;
        BrkTrapCondBuf.curbrkdat.AbsTime.i = stoptime.i;
        BrkTrapCondBuf.curbrkdat.IntervalTime.m = intervaltime.m;
        BrkTrapCondBuf.curbrkdat.IntervalTime.i = intervaltime.i;
    }
    else
        GoBrkClr();
}

/*************************************/
/* Given Parasite / Given Host       */
/*************************************/
void Brk_GivenParaGivenHost()
{   GlIndex para_gen, host_gen;
    I32s abortflag;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    if(!(BrkTrapCondBuf.curbrkdat.BrkSet))
        OpenGeneBanker();

    /* enter parasite genome name */
    para_gen = EnterGenotype(PLN_Y,
        "Enter parasite genotype (eg 0080aaa) (enter to abort)->",
        &abortflag, 1);

    if (!abortflag)
        host_gen = EnterGenotype(PLN_Y + 1,
            "Enter host genotype (eg 0080aaa) (enter to abort)->",
            &abortflag, 1);
    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_GIVENPARAGIVENHOST;
        BrkTrapCondBuf.curbrkdat.prim_genome.gi = para_gen.gi;
        BrkTrapCondBuf.curbrkdat.prim_genome.si = para_gen.si;
        BrkTrapCondBuf.curbrkdat.host_genome.gi = host_gen.gi;
        BrkTrapCondBuf.curbrkdat.host_genome.si = host_gen.si;
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Pct Chg in Population             */
/*************************************/
void Brk_PctChgPop()
{   I32s abortflag;
    I32s lowlim=0, hilim=0;
    double pctchg=0;
    char permtemp=0;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    for(;;)
    {
        /* Enter percent change in population */
        pctchg = EnterFloatVal(PLN_Y,
            "Enter pct change in population (pct > 0)->", &abortflag);
        if (!abortflag)
        {   if (pctchg > 0)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2221, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        }
        else
            break;
    }
    if (!abortflag)
    {   for(;;)
        {
            /* Enter lower limit of size range */
            lowlim = EnterLongVal(PLN_Y + 1,
                "Enter lower limit of size range ->", &abortflag);
            if (!abortflag)
            {   if (lowlim > 0)
                    break;
                else
                {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-2222, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
            }
            else
                break;
        }
        if (!abortflag)
        {   for(;;)
            {
                /* Enter upper limit of size range */
                hilim = EnterLongVal(PLN_Y + 2,
                    "Enter upper limit of size range ->", &abortflag);
                if (!abortflag)
                    if (hilim < lowlim)
                    {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                        FEError(-2223, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                    }
                    else
                        break;
                else
                    break;
            }
            if (!abortflag)
                permtemp = PermTempBothSpec(3, "genotype", &abortflag);
        }
    }

    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_PCTCHGPOP;
        BrkTrapCondBuf.curbrkdat.pctchg = pctchg;
        BrkTrapCondBuf.curbrkdat.lowlim = lowlim;
        BrkTrapCondBuf.curbrkdat.hilim = hilim;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp;
        BrkTrapCondBuf.curbrkdat.avgpoprng = CalcAvgPopSizRng();
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Parasite / Any Host / Time    */
/*************************************/
void Brk_AnyParaAnyHostTime()
{   Event begtime;
    I32s abortflag;
    char permtemp[2];

    abortflag = 0;
    GoBrkClr();

    /* enter beginning time in instructions */

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();
    if (InstExe.m)
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %d%.6d\n",
            InstExe.m, InstExe.i);
    else
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %6d\n",
            InstExe.i);
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Enter earliest origin time to look for (enter to abort)->");
    FEPrintf(PLN_X, PLN_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    FEGetStr(PLN_X+strlen((const char *)(&(Fbuf[0]))), PLN_Y+1);
    if (strlen((const char *)(&(FGSbuf[0]))))
        begtime = Str2Event((char *)(&(FGSbuf[0])));
    else
        abortflag = 1;
    if (!abortflag)
        permtemp[0] = PermTempBothSpec(2, "parasite", &abortflag);
    if (!abortflag)
        permtemp[1] = PermTempBothSpec(3, "host", &abortflag);
    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYPARAANYHOSTTIME;
        BrkTrapCondBuf.curbrkdat.AbsTime.m = begtime.m;
        BrkTrapCondBuf.curbrkdat.AbsTime.i = begtime.i;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp[0];
        BrkTrapCondBuf.curbrkdat.permtemp[1] = permtemp[1];
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Genotype / Given Time         */
/*************************************/
void Brk_AnyGenGivenTime()
{   Event begtime;
    I32s abortflag;
    char permtemp=0;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    /* enter absolute time in instructions */

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    if (InstExe.m)
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %d%.6d\n",
            InstExe.m, InstExe.i);
    else
        tsprintf((char *)(&(Fbuf[0])),
            "current time (instructions executed) = %6d\n",
            InstExe.i);
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    tsprintf((char *)(&(Fbuf[0])),
        "Enter earliest origin time to look for (enter to abort)->");
    FEPrintf(PLN_X,PLN_Y+1,0,0, (char *)(&(Fbuf[0])));
    FEGetStr(PLN_X+strlen((const char *)(&(Fbuf[0]))), PLN_Y+1);
    if (strlen((const char *)(&(FGSbuf[0]))))
        begtime = Str2Event((char *)(&(FGSbuf[0])));
    else
        abortflag = 1;
    if (!abortflag)
        permtemp = PermTempBothSpec(2, "genotype", &abortflag);
    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYGENGIVENTIME;
        BrkTrapCondBuf.curbrkdat.AbsTime.m = begtime.m;
        BrkTrapCondBuf.curbrkdat.AbsTime.i = begtime.i;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp;
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Given Parasite / Any Host         */
/*************************************/
void Brk_GivenParaAnyHost()
{   GlIndex para_gen;
    I32s abortflag;
    char permtemp=0;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    /* enter parasite genome name */
    para_gen = EnterGenotype(PLN_Y,
        "Enter parasite genotype (eg 0080aaa) (enter to abort)->",
        &abortflag, 1);

    if (!abortflag)
        permtemp = PermTempBothSpec(1, "host", &abortflag);

    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_GIVENPARAANYHOST;
        BrkTrapCondBuf.curbrkdat.prim_genome.gi = para_gen.gi;
        BrkTrapCondBuf.curbrkdat.prim_genome.si = para_gen.si;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp;
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Parasite / Given Host         */
/*************************************/
void Brk_AnyParaGivenHost()
{   GlIndex host_gen;
    I32s abortflag;
    char permtemp;

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    /* enter parasite genome name */
    host_gen = EnterGenotype(PLN_Y,
        "Enter host genotype (eg 0080aaa) (enter to abort)->",
        &abortflag, 1);

    if (!abortflag)
        permtemp = PermTempBothSpec(1, "parasite", &abortflag);

    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYPARAGIVENHOST;
        BrkTrapCondBuf.curbrkdat.host_genome.gi = host_gen.gi;
        BrkTrapCondBuf.curbrkdat.host_genome.si = host_gen.si;
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Genotype / Size Range         */
/*************************************/
void Brk_AnyGenSizeRange()
{   I32s abortflag, lowlim=0, hilim=0, offset=0, trkthrdid=0;
    char permtemp=0, thrdtrk;

    abortflag = 0;
    GoBrkClr();
    thrdtrk='n';

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();

    for(;;)
    {   /* Enter lower limit of size range */
        lowlim = EnterLongVal(PLN_Y,
            "Enter lower limit of size range ->", &abortflag);
        if(!abortflag)
            if(lowlim>0)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2224, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        else
            break;
    }

    if(!abortflag)
    {   for(;;)
        {   /* Enter upper limit of size range */
            hilim = EnterLongVal(PLN_Y+1,
                "Enter upper limit of size range ->", &abortflag);
            if(!abortflag)
                if(hilim<lowlim)
                {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-2225, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
                else
                    break;
            else
                break;
        }
    }

    if(!abortflag)
        offset = EnterLongVal(PLN_Y+2,
            "Enter creature offset (enter to abort)->",
            &abortflag);

    if(!abortflag)
        permtemp = PermTempBothSpec(3, "genotype", &abortflag);

    if(!abortflag)
        thrdtrk=YNSpec(PLN_Y+6, PLN_X, "track thread ?", &abortflag);

    if(!abortflag)
        if(thrdtrk=='y')
            trkthrdid=EnterLongVal(PLN_Y+7,
                "Enter thread id (-1=curr thrd, enter to abort)->",
                &abortflag);

    if(!abortflag)
    {   /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYGENSIZERANGE;
        BrkTrapCondBuf.curbrkdat.lowlim = lowlim;
        BrkTrapCondBuf.curbrkdat.hilim = hilim;
        BrkTrapCondBuf.curbrkdat.genoffset = offset;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp;

        if(thrdtrk=='y')
        {   if(trkthrdid<0)
            {   BrkTrapCondBuf.curbrkdat.trkcurthrd=1;
                BrkTrapCondBuf.curbrkdat.MicroCpuI=
                    ce->c.c->threadid;
            }
            else
                BrkTrapCondBuf.curbrkdat.MicroCpuI=trkthrdid;
        }
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Parasite / Host Size Range    */
/*************************************/
void Brk_AnyPara_HostSizeRange()
{   I32s abortflag;
    I32s lowlim, hilim=0;
    char permtemp[2];

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();
    for(;;)
    {
        /* Enter lower limit of size range */
        lowlim = EnterLongVal(PLN_Y,
            "Enter lower limit of size range ->", &abortflag);
        if (!abortflag)
            if (lowlim > 0)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2226, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        else
            break;
    }
    if (!abortflag)
    {   for(;;)
        {
            /* Enter upper limit of size range */
            hilim = EnterLongVal(PLN_Y + 1,
                "Enter upper limit of size range ->", &abortflag);
            if (!abortflag)
                if (hilim < lowlim)
                {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-2227, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
                else
                    break;
            else
                break;
        }
    }
    if (!abortflag)
        permtemp[0] = PermTempBothSpec(2, "parasite", &abortflag);
    if (!abortflag)
        permtemp[1] = PermTempBothSpec(3, "host", &abortflag);

    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYPARA_HOSTSIZERANGE;
        BrkTrapCondBuf.curbrkdat.lowlim = lowlim;
        BrkTrapCondBuf.curbrkdat.hilim = hilim;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp[0];
        BrkTrapCondBuf.curbrkdat.permtemp[1] = permtemp[1];
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* Any Host / Parasite Size Range    */
/*************************************/
void Brk_AnyHost_ParaSizeRange()
{   I32s abortflag, lowlim, hilim=0;
    char permtemp[2];

    abortflag = 0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    OpenGeneBanker();
    for(;;)
    {
        /* Enter lower limit of size range */
        lowlim = EnterLongVal(PLN_Y,
            "Enter lower limit of size range ->", &abortflag);
        if (!abortflag)
            if (lowlim > 0)
                break;
            else
            {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                FEError(-2228, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
        else
            break;
    }
    if (!abortflag)
    {   for(;;)
        {
            /* Enter upper limit of size range */
            hilim = EnterLongVal(PLN_Y + 1,
                "Enter upper limit of size range ->", &abortflag);
            if (!abortflag)
                if (hilim < lowlim)
                {   tsprintf((char *)(&(Fbuf[0])), "invalid entry");
                    FEError(-2229, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
                else
                    break;
            else
                break;
        }
    }
    if (!abortflag)
        permtemp[0] = PermTempBothSpec(2, "parasite", &abortflag);
    if (!abortflag)
        permtemp[1] = PermTempBothSpec(3, "host", &abortflag);

    if (!abortflag)
    {
        /* set trap parameters. */
        MC_step = 1;
        BrkTrapCondBuf.curbrkdat.BrkSet = 1;
        BrkTrapCondBuf.curbrkdat.TrapType = BRKTYP_ANYHOST_PARASIZERANGE;
        BrkTrapCondBuf.curbrkdat.lowlim = lowlim;
        BrkTrapCondBuf.curbrkdat.hilim = hilim;
        BrkTrapCondBuf.curbrkdat.permtemp[0] = permtemp[0];
        BrkTrapCondBuf.curbrkdat.permtemp[1] = permtemp[1];
    }
    else
    {   CloseGeneBanker();
        GoBrkClr();
    }
}

/*************************************/
/* thread analysis data ready        */
/*************************************/
void Brk_ThrdAnaDatRdy()
{   GlIndex brkgen;
    I32s abortflag, sizecls=0;
    I8s szclsgn;

    abortflag=0;
    GoBrkClr();

#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, (I8s *)"");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    if(ThreadAnalysis)
    {   OpenGeneBanker();
        szclsgn=SzClsGenSpec(PLN_Y, &abortflag);
        if(!abortflag)
        {   switch(szclsgn)
            {   case 's':
                    /* enter size class */
                    sizecls=EnterLongVal(PLN_Y+1,
                        "Enter size class (enter to abort)->",
                        &abortflag);
                    break;
                case 'g':
                    /* enter genome name */
                    brkgen=EnterGenotype(PLN_Y,
                        "Enter genotype (enter to abort)->",
                        &abortflag, 1);
                    break;
                default :
                    break;
            }
        }
        if (!abortflag)
        {
            /* set trap parameters. */
            MC_step=1;
            BrkTrapCondBuf.curbrkdat.BrkSet=1;
            BrkTrapCondBuf.curbrkdat.TrapType=BRKTYP_THRDANADATRDY;
            BrkTrapCondBuf.curbrkdat.szclsgn=szclsgn;
            switch(szclsgn)
            {   case 's':
                    BrkTrapCondBuf.curbrkdat.sizecls=sizecls;
                    break;
                case 'g':
                    BrkTrapCondBuf.curbrkdat.
                        prim_genome.gi=brkgen.gi;
                    BrkTrapCondBuf.curbrkdat.
                        prim_genome.si=brkgen.si;
                    break;
                default :
                    break;
            }
        }
        else
        {   CloseGeneBanker();
            GoBrkClr();
        }
    }
    else
    {
        tsprintf((char *)(&(Fbuf[0])), "thread analysis not in effect");
        FEError(-2231, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
}

/*
 * MrkSpecRegCol - mark special register columns on 
 *                 step debugger display
 * regcol - display buffer for special register type
 * regno - register number
 */
void MrkSpecRegCol(regcol, regno)
char *regcol;
int regno;
{
   int i;

   /* clear special register debugger display columns */
   for(i=0;i<NUMSPECREG;i++){
      regcol[i]=' ';
   }
   regcol[NUMSPECREG]='\0';

   /* if AX register is used as an offset register */
   if((ce->c.c->cf.Se.i)==regno){
      /* mark AX offset register column */
      regcol[0] = 'O';
   }

   /* if AX register is used as an source register */
   if((ce->c.c->cf.So.i)==regno){
      /* mark AX source register column */
      regcol[1] = 'S';
   }

   /* if AX register is used as an destination register */
   if((ce->c.c->cf.De.i)==regno){
      /* mark AX destination register column */
      regcol[2] = 'D';
   }
}

/*
 * DispDebuggerScrn - debugger display
 *
 * cp - pointer to cell to be displayed by debugger
 */
void DispDebuggerScrn(cp)
Pcells cp;
{   if(!dbgdisptype)
        DebugDisplay(cp);
    else
    {   soupviewstrt=ce->c.c->ip;
        DisplaySoupPage();
    }
}

#endif /* MICRO */
