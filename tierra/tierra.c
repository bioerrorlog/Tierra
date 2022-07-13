/* @(#)tierra.c    1.28 2/13/95 */
/* tierra.c 30-3-98 main module of Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #00XX, in use: 0001-0002 */
/* FEError labels: #00XX, in use: 0001-0002,0005-0011 */

#include "license.h"
#include "tierra.h"

#define EXTERN
#include "globals.h"
#ifdef TIEAUDIO
#include "tieaudsrv.h"
#endif /* TIEAUDIO */
#undef   EXTERN

static void Deconstruct P_((I32s di, I32s lip, I32s cellstrt,
    I32s threadid, Dem *celldem));
static void GenName P_((I32s  offset, char  *gbuf));

static time_t KbdMseSlpStrt P_((void));

#include "soup_in.h"
#ifdef TIEAUDIO
#include "tieaudsrvprm.h"
#endif /* TIEAUDIO */

#ifdef BGL
#define BGL_GLOBAL_VALUE_DEFINE 1
#include "tbgl.h"
#include "tbgl_fsm.h"
#endif /* BGL */

#if defined(TIERRA)
static I16s GetCurGene P_((void));
#endif /* defined(TIERRA) */

I32s FindCell;
I32s itime, mtime;

int t = 432432423;

#ifdef _WIN32
#pragma data_seg(".tierra")
extern __declspec(dllimport) time_t tmlstact;
#pragma data_seg()
#pragma comment(linker, "-section:.tierra,rws")
#endif /* _WIN32 */

#ifndef __GUI__

/*
 * main - Tierra main program
 *
 * argc - number of arguments including program name
 * argv - pointer to array of argument string pointers
 *
 */
int main(argc, argv)
    int argc;
    char *argv[];
{   int errflag;

#ifdef __MWERKS__
#if FRONTEND == STDIO  /* get command line and parse it if on Mac */
    I8s *p1 =
    "\nPlease input arguments for tierra separated by spaces:\n\ntierra ";
#endif /* FRONTEND == STDIO */
#endif /* __MWERKS__ */

    errflag=0;
    errno=0;

#ifdef SIGBLOCK
    SetSigSet();
#endif /* SIGBLOCK */

    tiestrtme=tietime(NULL);
    CpuLoadLimitLstSlp=tiestrtme;

#ifdef __MWERKS__
#if FRONTEND == STDIO  /* get command line and parse it if on Mac */
    I8s *p1 =
    "\nPlease input arguments for tierra separated by spaces:\n\ntierra ";
    tfprintf(stdout, p1);
    argc = cmdline("tierra ", &argv);
#else /* FRONTEND == STDIO */
    SIOUXSettings.standalone = FALSE;
    SIOUXSettings.setupmenus = FALSE;
    SIOUXSettings.initializeTB = FALSE;
    SIOUXSettings.showstatusline = FALSE;
    SIOUXSettings.autocloseonquit = FALSE;
    SIOUXSettings.asktosaveonclose = FALSE;
    SIOUXSettings.rows = 10;
    SIOUXSettings.columns = 80;
    SIOUXSettings.toppixel = windowHeight + 64;
    SIOUXSettings.leftpixel = 5;
    FEInitToolbox();
    Alert(128, nil);
    argc = ccommand(&argv);
#ifdef NET
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif /* NET */
    wp = FECreateNewWindow();
#endif /* FRONTEND == STDIO */
#endif /* __MWERKS__ */

    initvar();
    GoBrkClr();
    SavBrkDat();

#ifdef NET
    TieCommInit(&globtcibuf);
#endif /* NET */

    FEStartup();

    GetSoup(argc, argv);
    if (argc > 2)
        FEMenu();
#if FRONTEND == BASIC
    else
    {
#if INTERFACE != BASICW
#ifndef __linux__
#ifndef DJGPP
        ttouchwin();
#endif /* DJGPP */
#endif /* __linux__ */
#ifndef DJGPP
        trefresh();
#endif /* DJGPP */
#endif /* INTERFACE != BASICW */

        FEVidInit((I32s)0);
    }
#endif /* FRONTEND == BASIC */

    life();
    WriteSoup(1);
    if (hangup)
        FEMenu();

#ifdef NET
    if (ping_fp)
    {   tfclose(ping_fp);
        ping_fp=NULL;
    }
#endif /* NET */

    FEExit(0);
    return 0;
}

/*
 * life - doles out time slices and death
 *
 */
void life()
{
#ifdef NET
   I32s  LClock;
#endif /* NET */

    while((!alive)||(((AliveGen)&&(Generations<alive))||
        ((!AliveGen)&&(InstExe.m<alive))))
    {
#ifdef AMIGA
#if FRONTEND == STDIO
        chkabort();
#endif /* FRONTEND == STDIO */
#endif /* AMIGA */
        if (KEYHIT())
            FEMenu();
        else
        {   if(TierraSleep)
                TieSleep(KbdMseSlpStrt, TierraSleep);
        }
#ifdef BGL
        if (TBglOneCellContFlg == 1)
        {
#endif /* BGL */
            (*slicer)();
            ReapCheck();
            if(CpuLoadLimitSleepNow)
            {   tsleep(CpuLoadLimitSleepTime);
                CpuLoadLimitSleepNow=0;
                CpuLoadLimitLstSlp=tietime(NULL);
            }
#ifdef BGL
        }
#endif /* BGL */

#ifdef NET
        STPingUpdateReq();
        if(ClusterServerUp)
            CleanIPMap(1);
#endif /* NET */

#ifdef BGL
    if (MC_step == -1L)   TBglRecvFunc(BGL_NONBLOCK);
#endif /* BGL */

#ifdef NET
        NetRecvFunc();

        LClock = (I32s) tietime(NULL);
        if (LClock - LastSpeedUpdate > SpeedUpdate)
        {   if (NumCells)
                Speed = InstExe.i / (LClock - TierraClock);
            else
                Speed=(MinSpeed<ARBLRGSPEED)?ARBLRGSPEED:MinSpeed;
            LastSpeedUpdate=LClock;
        }
#endif /* NET */
    }
}

#endif /* __GUI__ */

/*
 * TimeSlice - execute a given number of instructions for each cpu
 *             of the current creature
 *
 * size_slice - given number of instructions to be executed for each cpu
 *
 * Returns:
 */
void TimeSlice(size_slice)
    I32s size_slice;
{   I32s c, di, newintrvl;

/*        USE FOR DEBUGGING PURPOSES  
    I8s a = 0, b = 0;
         END FOR DEBUGGING PURPOSES */

    ce->c.ib += (I32s)size_slice;
    for (is.ts = ce->c.ib; is.ts > 0;)  /* do each inst of the slice */
    {   CellHalt=0;
        /* do each cpu of the cell */
        for (c = ce->c.n - 1; c >= 0; c--)
        {   CpuHalt=0;
            ce->c.c = &(ce->c.ar[c]);
            ce->c.ac = c;

#ifdef EXECPROT
            if (PrivExec(ce, ce->c.c->ip))
            {
#endif /* EXECPROT */
                if (ce->c.c->slicexit ||
                    (ce->c.sy.sy && (ce->c.c->sy.G > -1) &&
                    (ce->c.sy.sy[ce->c.c->sy.G].sync && ce->c.c->sy.sync)))
                    continue;

/*  BEGIN FOR DEBUGGING PURPOSES
    if (debug && InstExe.m >= FindTimeM && InstExe.i >= FindTimeI)
        b++;
    END FOR DEBUGGING PURPOSES
 */

                newintrvl=0;
                if((ce->c.c->threadid)<0)
                {   (ce->c.c->threadid)=(ce->c.threadct)++;
                    newintrvl=1;
                }

                if((BrkTrapCondBuf.NewCpuI)>=0)
                {   if((ce->c.ar[BrkTrapCondBuf.NewCpuI].threadid)>=0)
                    {   BrkTrapCondBuf.curbrkdat.MicroCpuI=
                            ce->c.ar[BrkTrapCondBuf.NewCpuI].threadid;
                        BrkTrapCondBuf.NewCpuI=-1;
                    }
                }

                di = FetchDecode();

                if(ce->c.c->prvcallins)
                {   newintrvl=1;
                    (ce->c.c->curcallvlcell)++;
                    (ce->c.c->curcallvlthrd)++;
                }
                if(ce->c.c->prvretins)
                {   if((ce->c.c->curcallvlcell)>0)
                        (ce->c.c->curcallvlcell)--;
                    if((ce->c.c->curcallvlthrd)>0)
                    {   extcallvlintrvl(ce, ce->c.c);
                        (ce->c.c->curcallvlthrd)--;
                    }
                }

                if(ThrdAnaCollTst())
                {   /* thread code analysis memory allocation */
                    AllocThrdAnaCod(&(ce->cell_thrdanadat), is.oip);
                    if(newintrvl)
                        entcallvlintrvl();
                    CallLvlIntrvlDevChk();
                }
                is.crcalvlint=ce->c.c->cpu_crcalvlint;
                if(ce->c.c->prvcallins)
                    if(ce->c.c->prvcalltcmplen)
                        if((GeneBnker)&&(WatchTem))
                            GenExTemp(ce->c.c->prvcalltcmpstr,
                                ce->c.c->prvcalltcmplen, 1, 1);

                (*id[di].execute)();

                Deconstruct((I32s)di, is.oip, is.ocellmem.p,
                    is.othreadid, &is.odem);

                if((!CellHalt)&&(!CpuHalt)&&(!TierraWait))
                    AllocThrdAnaEvent(&(ce->cell_thrdanadat));

#ifdef BGL
                /*
                 * this part of the code had been added with a view to stop
                 * on current instruction when thread analysis is ready or
                 * or one cell beeing monitored has halted
                 */
                if(((BrkTrapCondBuf.curbrkdat.BrkSet) &&
                    (BrkTrapCondBuf.curbrkdat.TrapType ==
                    BRKTYP_THRDANADATRDY) &&
                    (!BrkTrapCondBuf.curbrkdat.BrkFnd))||
                    ((!BrkTrapCondBuf.curbrkdat.MicroSlice) &&
                    (BrkTrapCondBuf.curbrkdat.MicroCpuI < 0) &&
                    ((MC_step==0) || (MC_step==1))))
                    TBgl_Micro_Spy(ce);
#endif /* BGL */

                if(CellHalt)
                    return;
                else
                {   if(!CpuHalt)
                        GenExExe(is.oip);
                    ThrdAnaDatColl(0,0,is.oip);
                }

#ifdef BGL
                /*
                 * this part of the code had been added with a view to stop
                 * on current instruction when thread analysis is ready or
                 * or one cell beeing monitored has halted
                 */
                if(((BrkTrapCondBuf.curbrkdat.BrkSet) &&
                    (BrkTrapCondBuf.curbrkdat.TrapType==
                    BRKTYP_THRDANADATRDY) &&
                    (!BrkTrapCondBuf.curbrkdat.BrkFnd)) ||
                    ((!BrkTrapCondBuf.curbrkdat.MicroSlice) &&
                    (BrkTrapCondBuf.curbrkdat.MicroCpuI < 0) &&
                    ((MC_step==0) || (MC_step==1))))
                    TBgl_Micro_Spy(ce);
#endif /* BGL */

                if (CpuHalt)
                {   ce->d.inst += is.dib;
                    ce->d.repinst += is.dib;
                    continue;
                }
                if (TierraWait)
                     continue;
                IncrementIp();

/*              BEGIN FOR DEBUGGING PURPOSES
                if(debug&&InstExe.m>=FindTimeM&&InstExe.i>=FindTimeI)
                    a=b;
                a=++a-b;
                END FOR DEBUGGING PURPOSES */

                SystemWork();

#ifdef EXECPROT
            } /* if (PrivExec) */
            else
                IncrNPrIp();
#endif /* EXECPROT */
        }  /* do each cpu of the cell */

#ifdef NET
    if(BklogProcFreq)
        if((NumCells<=NumCellsMin)||((tietime(NULL)-
            LastBklogProc)>=BklogProcFreq))
        {   TieRcvIncMsg();
            LastBklogProc=tietime(NULL);
        }
    if(NetRcvUpdFreq)
        if((NumCells<=NumCellsMin)||((tietime(NULL)-
            LstRcvTime)>=NetRcvUpdFreq))
            NetRecvFunc();
#endif /* NET */
#ifdef TIEAUDIO
    if(AudConnProcFreq)
        if((tietime(NULL)-LastAudConnProc)>=AudConnProcFreq)
        {   AcceptClient();
            LastAudConnProc=tietime(NULL);
        }
#endif /* TIEAUDIO */

        ce->c.ib -= is.dib;
        is.ts -= is.dib;
        ce->d.instP++;
    }  /* do each inst of the slice */
}

/*
 * GenName - put name of gene of instruction at given cell
 *           offset in string buffer
 *
 * offset - given cell offset
 * gbuf   - pointer to gene name buffer
 */
static void GenName(offset, gbuf)
I32s  offset;
char  *gbuf;
{
/*  BEGIN: for 8313aaa
    if (0 <= offset && offset <= 31)
        tsprintf(gbuf,"sel");
    else if (32 <= offset && offset <= 66)
        tsprintf(gbuf,"int");
    else if (67 <= offset && offset <= 79)
        tsprintf(gbuf,"repS");
    else if (80 <= offset && offset <= 108)
        tsprintf(gbuf,"repL");
    else if (109 <= offset && offset <= 267)
        tsprintf(gbuf,"int");
    else if (268 <= offset && offset <= 289)
        tsprintf(gbuf,"copS");
    else if (290 <= offset && offset <= 301)
        tsprintf(gbuf,"copL");
    else if (302 <= offset && offset <= 313)
        tsprintf(gbuf,"int");
    else if (314 <= offset && offset <= 326)
        tsprintf(gbuf,"dev");
    else if (327 <= offset && offset <= 747)
        tsprintf(gbuf,"int");
    else if (748 <= offset && offset <= 750)
        tsprintf(gbuf,"mig");
    else if (751 <= offset && offset <= 2923)
        tsprintf(gbuf,"int");
    else if (2924 <= offset && offset <= 2924)
        tsprintf(gbuf,"div");
    END: for 8313aaa  */

/*  BEGIN: for 0960aaa
    if (0 <= offset && offset <= 20)
        tsprintf(gbuf,"sel");
    else if (21 <= offset && offset <= 38)
        tsprintf(gbuf,"dif");
    else if (39 <= offset && offset <= 51)
        tsprintf(gbuf,"repS");
    else if (52 <= offset && offset <= 94)
        tsprintf(gbuf,"repL");
    else if (95 <= offset && offset <= 116)
        tsprintf(gbuf,"copS");
    else if (117 <= offset && offset <= 128)
        tsprintf(gbuf,"copL");
    else if (129 <= offset && offset <= 140)
        tsprintf(gbuf,"copC");
    else if (141 <= offset && offset <= 154)
        tsprintf(gbuf,"dev");
    else if (155 <= offset && offset <= 195)
        tsprintf(gbuf,"senS");
    else if (196 <= offset && offset <= 212)
        tsprintf(gbuf,"senC");
    else if (213 <= offset && offset <= 224)
        tsprintf(gbuf,"senY");
    else if (225 <= offset && offset <= 276)
        tsprintf(gbuf,"senA");
    else if (277 <= offset && offset <= 298)
        tsprintf(gbuf,"senR");
    else if (299 <= offset && offset <= 319)
        tsprintf(gbuf,"pad");
    else if (320 <= offset && offset <= 959)
        tsprintf(gbuf,"dat");
  END: for 0960aaa  */
    tsprintf(gbuf,"   ");
}

/*
 * Deconstruct - log the execution of selected instructions for tracked 
 *               threads, tracked cells, or all cells.
 *
 * di       = current instruction opcode
 * lip      = current execution address
 * cellstrt = current cell memory start
 * threadid = current thread id
 * celldem  = cell demographic structure before execution of current
 *            instruction
 *
 */
static void Deconstruct(di, lip, cellstrt, threadid, celldem)
I32s  di, lip, cellstrt, threadid;
Dem *celldem;
{   I32s offset, ooffset, loip, logtrace;
    I8s  buf[100], gbuf[8], ogbuf[8], tgbuf[20];

    logtrace=0;
    if(DeconstructOn)
    {
        if(!(ce->c.c->fl.E))
        {   if(BrkTrapCondBuf.curbrkdat.MicroSlice)
            {   if(ce==BrkTrapCondBuf.curbrkdat.MicroSlice)
                {   if(BrkTrapCondBuf.curbrkdat.MicroCpuI>=0)
                    {   if(ce->c.c->threadid==BrkTrapCondBuf.
                            curbrkdat.MicroCpuI)
                            logtrace=1;
                    }
                    else
                        logtrace=1;
                }
            }
            else
                logtrace=1;
        }
        if(logtrace)
        {   loip = is.oip;
            if (lip == loip)
            {   lip += is.iip;
                lip = ad(lip);
            }
            offset = lip - cellstrt;
            GenName(offset, (char *)(&(gbuf[0])));
            ooffset = loip - cellstrt;
            GenName(ooffset, (char *)(&(ogbuf[0])));
            if (!strcmp((const char *)&(id[di].mn[0]),(const char *)"call") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"csync") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"divide") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"getipp") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"getippf") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"halt") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"ifgrtr") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"mal") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"movdi4") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"offAACD") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"offBBCD") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"ret") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"split") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"surf") ||
                !strcmp((const char *)&(id[di].mn[0]),(const char *)"surff") ||
                strcmp((const char *)gbuf,(const char *)ogbuf))
            {   if (strcmp((const char *)gbuf,(const char *)ogbuf)&&
                    strcmp((const char *)&(id[di].mn[0]),(const char *)"halt"))
                    tsprintf((char *)(&(tgbuf[0])),
                        " -> %d %s\n", offset, &(gbuf[0]));
                else
                    tsprintf((char *)(&(tgbuf[0])),"\n");
                tsprintf((char *)(&(buf[0])),
                    "%-7s %7d %7d %5hd %5hd %7d %7d %10d %5s %5s",
                    id[di].mn, ooffset, threadid, is.oac,
                    is.oncpu, celldem->inst, celldem->instP,
                    celldem->RanID, ogbuf, tgbuf);
                tfprintf(sgfile,(char *)(&(buf[0])));
            }
        }
    }
}

/*
 * FetchDecode - prepare to execute next instruction
 */
I32s FetchDecode()
{   I32s di;
    Pcells cp;

#if PLOIDY == 1
    is.eins = &soup[ce->c.c->ip];
#else /* PLOIDY > 1 */
    is.eins = &soup[ce->c.c->ip][ce->c.c->ex];
#endif /* PLOIDY > 1 */
    di = (*(is.eins)) % InstNum;
    if (di < 0)
        di = -di;
    is.oncpu = ce->c.n;
    is.oac = ce->c.ac;
    is.oip = ce->c.c->ip;
    is.cipofst = ce->c.c->ip-ce->mm.p;
    is.hipofsttype=(I8u)WhoIs(&cp, ce->c.c->ip, &(is.hipofst));
    is.othreadid = ce->c.c->threadid;
    ce->c.c->prvgene=ce->c.c->curgene;
    ce->c.c->curgene=GetCurGene();

    ce->c.c->prvcallins=ce->c.c->callins;
    ce->c.c->callins=0;

    ce->c.c->prvcalltcmpstr=ce->c.c->calltcmpstr;
    ce->c.c->calltcmpstr=-1;

    ce->c.c->prvcalltcmplen=ce->c.c->calltcmplen;
    ce->c.c->calltcmplen=0;

    ce->c.c->prvretins=ce->c.c->retins;
    ce->c.c->retins=0;

#if PLOIDY == 1
    is.oextrk=0;
#else
    is.oextrk=ce->c.c->ex;
#endif /* PLOIDY > 1 */

    is.odem=ce->d;
    is.odstoffsettype=0;
    is.odstoffset=0;
    is.oi32a=0;
    is.oport=0;
    is.divide=0;
#ifdef NET
    is.onodetype=0;
    is.oipic1=(-1);
    is.oipis1=(-1);
#endif /* NET */

    is.ocellmem=ce->mm;

    ce->c.d = id + di;
    is.pdi = is.di;
    is.di = di;
    is.dmod=is.dran=0;
    (*id[di].decode) ();
    is.dib = (I8u)(id[di].cyc);

#ifdef BGL
#ifdef MICRO
    TBgl_Micro_Spy(ce);
#endif
#else /* BGL */
#ifdef MICRO
    if (MC_step > -1L)
    {
        Micro_Spy(ce);
    }
#endif
#endif /* BGL */
    TierraWait = 0;
    return di;
}

/*
 * IncrementIp - increment current instruction pointer for current cpu
 */
void IncrementIp()
{   ce->c.c->ip += is.iip;
    ce->c.c->ip = ad(ce->c.c->ip);
#if PLOIDY > 1  /* if there is an error, switch tracks & repeat instruction */
    if (ce->c.c->fl.E && !flaw())
    {   if (ce->c.c->wc < PLOIDY - 1)
        {   ce->c.c->ip -= is.iip;
            ce->c.c->ip = ad(ce->c.c->ip);
            (ce->c.c->wc)++;
        }
        else
            ce->c.c->wc = 0;
        ChangeTrack(&(ce->c.c->ex));
    }
#endif /* PLOIDY > 1 */
}

/*
 * IncrNPrIp - increment current instruction pointer for current cpu
 *             if cell does not have execution privilege
 */
void IncrNPrIp()
{   ce->c.c->ip += 1;
    ce->c.c->ip = ad(ce->c.c->ip);
}

/*
 * SystemWork - post instruction execution processing
 *
 */
void SystemWork()
{   CellInd htis, mtis;

    ce->d.inst += is.dib;
    ce->d.repinst += is.dib;
    is.iip = 1;
    if (ce->c.c->fl.E)
    {   ce->d.flags++;
        if (!(ce->d.dm))
            UpRprIf(ce);
    }
    if (RateMut && ++CountMutRate >= RateMut)
    {   mutate();
        TotMut++;
        CountMutRate = tlrand() % RateMut;
    }
    if (isolate)
        extract(cells[extr.a][extr.i].d.gen.size, 0,
            cells[extr.a][extr.i].d.gi, 0);

#ifdef NET
    if (OnLineStat)
    {   InstExeConnect.i++;
        if (InstExeConnect.i > 999999L)
        {   InstExeConnect.i %= 1000000L;
            InstExeConnect.m++;
        }
    }
#endif /* NET */

#if FRONTEND == BASIC
    if(InstExeUpdFreq)
        if(!(InstExeUpdNow=(InstExeUpdNow>0)?(InstExeUpdNow-1):0))
            FEStats();
#endif /* FRONTEND == BASIC */

    InstExe.i++;
    if (InstExe.i > 999999L)
    {   InstExe.i %= 1000000L;
        InstExe.m++;
        if (DropDead && (InstExe.m > LastDiv.m + DropDead))
            FEError(-6, EXIT, NOWRITE,
                "Tierra SystemWork() soup has died");
        if ((MinSpeed>=0)&&(Speed<MinSpeed))
            FEError(-5, EXIT, NOWRITE, "Speed too slow");
        if (SaveFreq && !(InstExe.m % SaveFreq))
        {   WriteSoup(0);
            if (SavRenewMem)
            {   htis.a = ce->q.htis.a;
                htis.i = ce->q.htis.i;
                if(BrkTrapCondBuf.curbrkdat.MicroSlice)
                {   mtis.a=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.a;
                    mtis.i=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.i;
                }
                FreeDynMem();
#ifdef MEM_PROF
                MemStatCleanUp();
#endif /* MEM_PROF */
                ReadDynMem();
                ReadGeneBank();
                ce = &cells[htis.a][htis.i];
                if(BrkTrapCondBuf.curbrkdat.MicroSlice)
                    BrkTrapCondBuf.curbrkdat.MicroSlice=
                        &cells[mtis.a][mtis.i];
            }
        }
#ifdef NET
        ApocalypseNowFreq--;
        if (ApocalypseFreq && ApocalypseNowFreq <= 0)
            ApocalypseNow = 1;
#endif /* NET */
        stats();
        plan();
        if(TierraSleep>0)
        {   if((tietime(NULL)-CpuLoadLimitLstSlp)>
                CpuLoadLimitRunTime)
                CpuLoadLimitSleepNow=1;
        }
    }
}

/*
 * ReapCheck - trigger disturbance at the appropriate time
 *
 */

void ReapCheck()
{   I32s i, t, dtime;
    Event result;
#ifdef NET
    I32s srvapcslpdur;
#endif /* NET */

#ifdef NET
    if (ApocalypseNow)
    {   srvapcslpdur=(SrvrApocFlag)?SrvrApocSleep:0;
        TieSleep(0, srvapcslpdur);
        return;
    }
#endif /* NET */
    if (DistFreq < -.00001 || !reaped || (!DistNext.m && !DistNext.i))
        return;
    dtime = SubEvent(&InstExe, &DistNext, &result);
    if (dtime > 0)
    {   Disturb = InstExe;
        DistNext.m = DistNext.i = 0L;
        t = (I32s) (DistProp * (float) NumCells);
        while (t >= NumCells - NumCellsMin)
            t--;
        for (i = 0; i < t; i++)
            reaper(0, -1, REAP_DISTURB);
    }
}

#ifdef NET
/*
 * Apocalypse - kill the current soup population
 *
 */
void Apocalypse()
{   I32s   i,k,nclist, mgp;
#if PLOIDY > 1
    I32s j;
#endif /* PLOIDY > 1 */
    Pcells cr,*clist;

    InitDeadMem(0, SoupSize, (!DeadMemInit)?2:DeadMemInit);
    while (NumCells > NumCellsMin)
    {
#ifdef NET
#ifdef BGL
        if (TopReap->d.BglTrack.node) /* this cell currently being tracked */
            MigrApocalypseMesg(TopReap);
#endif /* BGL */
#endif /* NET */
        ReapCell(TopReap, REAP_APOCALYPSE);
    }

    for (cr=TopReap,i=0;i<NumCells;i++)
    {   mgp=(cr->mm.p)+(cr->d.mg.p);
        nclist=0;
        clist=NULL;
        for(k=mgp;k<(mgp+(cr->d.mg.s));k++)
            MutBookeep(k,0,&clist,&nclist);
        CellMutBookeep(clist,nclist);
#ifdef NET
#ifdef BGL
        if (cr->d.BglTrack.node) /* this cell currently being tracked */
            MigrApocalypseMesg(cr);
#endif /* BGL */
#endif /* NET */
        cr = &cells[cr->q.n_reap.a][cr->q.n_reap.i];
    }
    ApocalypseNowFreq=((I32s)
        (ApocFixFrac*((double)ApocalypseFreq)))+
        (tlrand()%(((I32s)(ApocRanFrac*
        ((double)ApocalypseFreq)))+1));
    SrvrApocFlag=0;
    ApocalypseNow = 0;
}
#endif /* NET */

/*
 * reaper - decide which cell in reaper queue to kill and kill it.
 *
 * ex - 0        = no creature executing now
 *      non-zero = creature is executing now
 * sad           = suggested address for reaping
 * extrmcod      = cell termination code for mtad_extrnterm
 *
 * returns: 0 - reaping done
 *          1 - no reaping done, suitable cell could not be located
 *
 */
I32s reaper(ex, sad, extrmcod)
    I32s ex;                    /* is a creature executing now ? */
    I32s sad;                   /* suggested address for reaping */
    I32s extrmcod;              /* cell term code for mtad_extrnterm */
{   Pcells cr=NULL;             /* cell to be reaped */
    Event result;
    I32s reap_range, i, j, ll, ul, goon, rtime, found = 0;

    if (NumCells <= NumCellsMin)
        return 1;
    /* try to find a cell within MalReapTol of sad */
    if (MalReapTol && (sad >= 0 && sad < SoupSize))
    {   cr = TopReap;
        i = 1;
        goon = 1;
        ll = sad - MalLimit;
        ul = sad + MalLimit + 1;
        while (goon)
        {   goon = 0;
            if (ex && cr == ce)
                goon = 1;
            if (cr->md.s)
            {   if ((((cr->mm.p + cr->mm.s) < ll) || (cr->mm.p > ul))
                    && (((cr->md.p + cr->md.s) < ll) || (cr->md.p > ul)))
                    goon = 1;
            }
            else
            {   if (((cr->mm.p + cr->mm.s) < ll)
                    || (cr->mm.p > ul))
                    goon = 1;
            }
            if (goon)
            {   cr = &cells[cr->q.n_reap.a][cr->q.n_reap.i];
                i++;
            }
            if (i > NumCells || (!cr)
                || ((cr->q.htis.a == 0) && (cr->q.htis.i < 2)))
                break;
            if (!goon)
            {   found = 1;
                break;
            }
        }
    }
    /* This is default: if we didn't locate a cell using the above, then: */
    if (!found)
    {   reap_range = (I32s)(ReapRndProp * NumCells);
        if (reap_range < 2)
            cr = TopReap;
        else
            /* pick rnd cell in top reap_range */
        {   j = tlrand() % reap_range;
            for (cr = TopReap, i = 0; i < j; i++)
            {   cr = &cells[cr->q.n_reap.a][cr->q.n_reap.i];
#ifdef ERRORTIE
                if ((!cr) || ((cr->q.htis.a == 0) && (cr->q.htis.i < 2)))
                    FEError(-7, EXIT, WRITE,
                        "Tierra reaper() error, queues corrupted!");
#endif /* ERRORTIE */
            }
        }
    }
    /* now we have found the cell to reap, it is cr */
    if (ex && cr == ce) /* if cr is executing now */
    {   /* if cr is not the last cell, we will reap the next cell */
        if (NumCells != 1)
            cr=(cr==TopReap)?(&(cells[cr->q.n_reap.a][cr->q.n_reap.i])):
                (&(cells[cr->q.p_reap.a][cr->q.p_reap.i]));
    }
    if (cr->mm.s) /* if cr is a cell, we call ReapCell() */
    {   if (ex && DistFreq > -.00001 && !DistNext.m && !DistNext.i)
        {   rtime = SubEvent(&InstExe, &Disturb, &result);
            rtime = (I32s) (DistFreq * (float) rtime);
            DistNext = Disturb = InstExe;
            DistNext.m += rtime / 1000000L;
            DistNext.i += rtime % 1000000L;
            DistNext.m += DistNext.i / 1000000L;
            DistNext.i %= 1000000L;
        }
/*      if (NumCells == 1)
            FEError(-8, EXIT, WRITE,
                "Tierra reaper() error 0, attempt to reap last creature");
 */
#ifdef NET
#ifdef BGL
        if (cr->d.BglTrack.node) /* this cell currently being tracked */
            MigrDeathMesg(cr);
#endif /* BGL */
#endif /* NET */
        ReapCell(cr, extrmcod);
        return 0;
    }
    else
        return 1;
}


/*
 * ReapCell - kill the given cell
 *
 * cr - cell to be killed
 * extrmcod      = cell termination code for mtad_extrnterm
 *
 */
void ReapCell(cr, extrmcod)
Pcells cr;
I32s extrmcod;
{   Pcells nc;                  /* daughter of cell to be reaped */

#ifdef ERRORTIE
    if (!cr->ld || !NumCells || (!cr->mm.s && !cr->md.s))
        FEError(-9, EXIT, WRITE,
            "Tierra ReapCell() error 1, attempt to reap non-existant cell");
#endif /* ERRORTIE */
    extcelcallvlintrvl(cr);
    cr->cell_thrdanadat.mtad_base.btad_extrnterm=extrmcod;
    cr->cell_thrdanadat.mtad_base.btad_termInstP=cr->d.instP;
    if(((cr->d.gi)>=0)&&((cr->d.gen.size)>0)&&
        (ThrdAnaCollDone(cr, 1, 0)))
            ThrdAnaDatSave(cr);
    if (cr->mm.s)
    {
#ifdef ERRORTIE
        if (cr->mm.p < 0 || cr->mm.p >= SoupSize)
            FEError(-10, EXIT, WRITE,
"Tierra ReapCell() error 2: attemp to deallocate mother memory not in soup");
#endif /* ERRORTIE */
        MemDealloc(cr->mm.p, cr->mm.s);
        InitDeadMem(cr->mm.p, cr->mm.s, DeadMemInit);
    }

#ifdef EXEINSTSIZTIMSLC
    FreeCellGenBits(cr);
#endif /* EXEINSTSIZTIMSLC */

    if (cr->md.s && (cr->md.p > -1)) /* cleanup daughter, if there is one */
    {
#ifdef ERRORTIE
        if (cr->md.p < 0 || cr->md.p >= SoupSize)
            FEError(-11, EXIT, WRITE,
"Tierra ReapCell() error 3: attemp to deallocate daughter memory not in soup");
#endif /* ERRORTIE */
        if (cr->d.ne.a != cr->q.htis.a || cr->d.ne.i != cr->q.htis.i)
            /* cleanup daughter cpu */
        {   nc = &cells[cr->d.ne.a][cr->d.ne.i];
            if (nc->d.is)  /* cleanup daughter instruction pointer */
                RmvFrmSlicer(nc);
            NumCells--;  /* daughter cell */
            InitCell(nc->q.htis.a, nc->q.htis.i, nc);
        }
        MemDealloc(cr->md.p, cr->md.s);
        InitDeadMem(cr->md.p, cr->md.s, DeadMemInit);
    }
    if (cr == ce) /* if the cell to be killed is the currently active cell */
    {   is.ts = 0;  /* get mom out of the queues */
        DecrSliceQueue();
        CellHalt=1;
    }
    RmvFrmSlicer(cr);
    RmvFrmReaper(cr);
    ReapBookeep(cr);  /* does InitCell(ci); and NumCells-- */
}

#ifdef NET

/*
 * NetRecvFunc - process beagle connections and outstanding
 *               tierra message backlog
 */
void NetRecvFunc()
{
#ifndef BGL
    I32s nselect;
    struct timeval *pWait;
    struct sockaddr_in fsin;
#ifdef AMIGA
    LONG alen = sizeof(fsin);
#else
    int alen = sizeof(fsin);
#endif

    pWait = &tvWait;
    memcpy((char *) &Rfds, (char *) &Afds, sizeof(Rfds));
#ifdef __MWERKS__
    if ((nselect = tselect(FD_SETSIZE, &Afds, &Rfds, (fd_set *) 0, pWait)) > 0)
#else /* __MWERKS__ */
#ifdef __linux__
    if ((nselect = tselect(FD_SETSIZE, &Afds, &Rfds, (fd_set *) 0, pWait)) > 0)
#else /* __linux__ */
#ifdef DECVAX
    if ((nselect = tselect(Nfds, &Rfds, &Wfds, &Efds, pWait)) > 0)
#else
    if((nselect = (I32s)tselect(Nfds, &Rfds, (fd_set *)0,
        (fd_set *) 0, pWait)) > 0)
#endif /* DECVAX */
#endif /* __linux__ */
#endif /* __MWERKS__ */
    {
        if (FD_ISSET(MonSockTCP, &Rfds))
        {   if (SuMonFlg == 0)
                MonTCPNid = 0;
            if((MonSockTCP_ID[MonTCPNid].fd=accept(
                MonSockTCP, (struct sockaddr *) &fsin, &alen))>=0)
            {   MonSockTCP_ID[MonTCPNid].flg = 'T';
                SuMonFlg = 1;
                if (MonInitMesFunc((I32s)(MonSockTCP_ID[MonTCPNid].fd))==0)
                {   tclosesocket(MonSockTCP_ID[MonTCPNid].fd);
                    FD_CLR((I32u)MonSockTCP_ID[MonTCPNid].fd, &Afds);
                    MonSockTCP_ID[MonTCPNid].flg = 'F';
                }
            }   /* End of "if (MonSockTCP_ID[MonTCPNid].fd < 0) " */
        }       /* End of "if(FD_ISSET(MonTCPNid, &Rfds))" */
    }          /* END of "if ((nselect = select(Nfds..." */
#endif /* BGL */
    if(OnLineStat)
        TieCommProcMsgBkLog(&globtcibuf, ProcTierraMsg);
    LstRcvTime=tietime(NULL);
}

#endif /* NET */

/*
 * KbdMseSlpStrt - return time of last keyboard/mouse access
 */
static time_t KbdMseSlpStrt()
{
#ifdef unix
    struct stat devacc;
    tstat("/dev/kbd", &devacc);
    return devacc.st_atime;
#endif /* unix */
#ifdef _WIN32
    return tmlstact;
#endif /* _WIN32 */
}

#ifdef NET
/*
 * ProcTierraMsg - process incoming Tierra message
 *
 * mesg - pointer to TMessage structure containing incoming message
 * msgsrc - source address of message
 */
int ProcTierraMsg(mesg,msgsrc,msgid)
TMessage *mesg;
NetAddr *msgsrc;
I32u msgid;
{   I32s result, tpingrcvrtn, tpingreplyrtn, tNumCellsMin, index;
    NetAddr address;
    Pcells cp; /* pointer to cell struct where injected creature will go */

    switch ((int) mesg->info.tag)
    {   case SENDADDRTAG:
        {   Addr_recv(mesg, msgsrc);
            TreatTPingData(mesg, msgsrc, 0, &index);
            if(index>=0)
                TPing_reply(mesg, index, msgsrc);
            break;
        }
        case EMIGRATETAG:
        {   TreatTPingData(mesg, msgsrc, 0, &index);
            if(index>=0)
            {   TPing_reply(mesg, index, msgsrc);
                if(EmigSubNetChk(mesg->PrivPingData.subnet,
                    TieSubNet, mesg->dstmap))
                {   tNumCellsMin = NumCellsMin;
                    NumCellsMin = 0;
                    while (!(cp = Inject((FpInst)
                        (FpInst)(mesg->info.Data_u.
                            Creature.genome.genome_val),
                        mesg->info.Data_u.Creature.genome_siz,
                        mesg->info.Data_u.Creature.mg,
                        -1, 0, 0, &ReapRndProp, 1, msgsrc->node)))
                    {   if (reaper(1, -1, REAP_SOUP_FULL))
                            break;
                    }
                    if(cp)
                    {   cp->d.birthtime= (I32s) tietime(NULL);
                        cp->d.RanID = mesg->info.Data_u.Creature.ranID;
#ifdef EXEINSTSIZTIMSLC
                        cp->d.PExeSegSiz=mesg->info.
                            Data_u.Creature.PExeSegSiz;
#endif /* EXEINSTSIZTIMSLC */
#ifdef BGL
                        if (mesg->info.Data_u.Creature.BglTrack.node)
                            MigrArrivalMesg(cp, mesg);
#endif /* BGL */
                    }
                    NumCellsMin = tNumCellsMin;
                }
            }
            break;
        }
        case TPINGREQTAG:
        {   mesg->info.Data_u.TPingReq.source.address=*msgsrc;
            address = mesg->info.Data_u.TPingReq.source.address;
            TreatTPingData(mesg, &address, 0, &index);
            if(index>=0)
            {   tpingreplyrtn=TPing_reply(mesg, index, msgsrc);
                if (tpingreplyrtn < 0)
                {
#ifdef TIERRA
#if FRONTEND == BASIC
                    if(!HistoInEffect())
                    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                        tsprintf((char *)(&(Fbuf[0])),
                            "Tping request could not be processed\n");
                        FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
#ifdef TIERRA
#if FRONTEND == BASIC
                    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                }
                else
                {
#ifdef TIERRA
#if FRONTEND == BASIC
                    if(!HistoInEffect())
                    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                        tsprintf((char *)(&(Fbuf[0])),
                            "TPing request processed\n");
                        FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
#ifdef TIERRA
#if FRONTEND == BASIC
                    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                }
            }
            break;
        }
        case TPINGREPTAG:
        {   mesg->info.Data_u.TPingRep.source.address=*msgsrc;
            address = mesg->info.Data_u.TPingRep.source.address;
            TreatTPingData(mesg, &address, 1, &index);
            if(index>=0)
                tpingrcvrtn=TPing_recv(mesg);
            break;
        }
        case BYETAG:
        {   if((result=Bye_recv(mesg, msgsrc))>=0)
            {
#ifdef TIERRA
#if FRONTEND == BASIC
                if(!HistoInEffect())
                {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                    tsprintf((char *)(&(Fbuf[0])),
                        "Node no. %d erased from IPMap\n",
                        result);
                    FEPrintf(MSG_X,MSG_Y,0,0,
                        (char *)(&(Fbuf[0])));
#ifdef TIERRA
#if FRONTEND == BASIC
                }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
#ifdef BGL
                TBgl_make_femessages(&(Fbuf[0]),
                    strlen((const char *)&(Fbuf[0])));
#endif /* BGL */
            }
            break;
        }
        default:
        {
#ifdef TIERRA
#if FRONTEND == BASIC
            if(!HistoInEffect())
            {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                if (IMode == PLN_STATS)
                {   tsprintf((char *)(&(Fbuf[0])),
                        "Unknown message type\n");
                    FEPrintf(MSG_X, MSG_Y, 0, 0, &(Fbuf[0]));
                }
#ifdef TIERRA
#if FRONTEND == BASIC
            }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
            break;
        }
    }
    return 0;
}
#endif /* NET */

#if defined (TIERRA)
/*
 * GetCurGene - get gene index for current instruction
 */
static I16s GetCurGene()
{   I16s curgene=-1;
    I32s gensize=ce->d.gen.size;
    I16s *genelkup;
    I32s mgp=ce->d.mg.p;
    I32s mgs=ce->d.mg.s;
    
    if(!(is.hipofsttype))
        if(((is.hipofst)>=mgp)&&((is.hipofst)<(mgp+mgs)))
            if(GeneBnker)
            {   genelkup=sl[gensize]->genelkup;
                if(genelkup)
                    curgene=genelkup[is.hipofst];
            }
    return curgene;
}

#ifdef SIGBLOCK
/**********************************************************************
 * TieSigIO - SIGIO interrupt signal handler
 **********************************************************************/
#ifdef __TURBOC__
int TieSigIO(void)
#endif /* __TURBOC__ */
#ifdef __MWERKS__
void TieSigIO(sig)
I32s sig;
#endif /* __MWERKS__ */
#ifdef unix
void TieSigIO(sig)
int sig;
#endif /* unix */
#ifdef DECVAX
void TieSigIO(int sig)
#endif /* DECVAX */
#ifdef _WIN32
void TieSigIO(I32s sig)
#endif /* _WIN32 */
#ifdef AMIGA
void TieSigIO(int sig)
#endif /* AMIGA */
{
#ifdef NET
    TieRcvIncMsg();
#endif /* NET */
#ifdef TIEAUDIO
    AcceptClient();
#endif /* TIEAUDIO */
}
#endif /* SIGBLOCK */
#endif /* TIERRA */
