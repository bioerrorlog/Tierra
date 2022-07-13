/* bookeep.c 30-3-98 bookeeping functions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #01XX, in use: none */
/* FEError labels: #01XX, in use: 0100-0108 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

/*
 * n a m e
 *
 *  Pcells mc;     mc = mother cell
 *  Pcells nc;     nc = daughter cell
 *  I32s   eject;  -1 = daughter born into soup,
 *               > -1 = index into IPMap[index]
 *                 of node to which the daughter will be ejected
 *
 * Returns:
 */
void DivideBookeep(mc, nc, eject)
Pcells mc, nc;
I32s eject;
{   I8s same = 0;
    I32s flag;
#ifdef NET
#ifdef BGL
    BglTrackFlag migr;
    I32s size;
    GList *tgl;
    I8s               label[4];
#endif /* BGL */
#endif /* NET */

    LastDiv = InstExe;
    if (!mc->d.fecundity && !mc->d.nonslfmut && !mc->d.flaw)
    {   mc->d.d1.flags = mc->d.flags;  /* record metabolic data 1st repl */
        mc->d.d1.inst = mc->d.inst + 1;
        mc->d.d1.instP = mc->d.instP + 1;
        mc->d.d1.mov_daught = mc->d.mov_daught;
    }

    mc->d.fecundity++;
    nc->d.gen.size = nc->mm.s;
    if (GeneBnker)
    {   nc->d.genome = soup + nc->mm.p;
        if (mc->mm.s == nc->mm.s &&       /* if cell breeds true */
            mc->d.mg.s == nc->d.mg.s &&
            mc->d.mg.p == nc->d.mg.p &&
            IsSameGen(nc->d.mg.s, soup + nc->mm.p + nc->d.mg.p,
                soup + mc->mm.p + mc->d.mg.p))
        {   if (mc->d.fecundity == 1)
                nc->d.d1.BreedTrue = mc->d.d1.BreedTrue = 1;
            nc->d.parent = mc->d.parent;
            nc->d.gen = mc->d.gen;
            nc->d.gi = nc->d.origgi = mc->d.gi;
            (sl[nc->d.gen.size]->g[nc->d.origgi]->origpop)++;
            nc->d.hash = mc->d.hash;
            same = 1;
        }
        else /* if daughter is a new genotype (same = 0) */
        {   nc->d.parent = mc->d.gen;  /* this will assign a gen.label */
            nc->d.hash = Hash(nc->d.mg.s, nc->d.genome + nc->d.mg.p);
            flag=(DiskBank)?25:0;
            CheckGenotype(&(nc->d), flag, &siz_sl, &sl);
                /* check .gen files */
            nc->d.origgi = nc->d.gi;
            (sl[nc->d.gen.size]->g[nc->d.origgi]->origpop)++;
            strcpy((char *)(&(nc->d.gen.label[0])),
                (const char *)Int2Lbl(nc->d.gi));
        }
        DivGenBook(nc, InstExe, reaped, 0, same, 0, 0);
        DivGenBook(mc, InstExe, reaped, 1, same, 0, 0);
    }
    RepNum++;
    BirthNum++;
    if (eject != -1)
    {

#ifdef NET
        BirthEject++;
#endif /* NET */

        if(ThrdAnaCollTst())
            (ce->cell_thrdanadat.mtad_thdatara.
                mtda_thdatara[is.othreadid]->
                mtdt_basthrdat.tdt_nlocrepl)++;
    }
    else
    {
#ifdef NET
        BirthLocal++;
#endif /* NET */
        if(ThrdAnaCollTst())
            (ce->cell_thrdanadat.mtad_thdatara.
               mtda_thdatara[is.othreadid]->
               mtdt_basthrdat.tdt_locrepl)++;
    }
    nc->d.birthtime = (I32s) tietime(NULL);
#ifdef NET
#ifdef BGL
    if (MigrStartGet(&migr, &(nc->d.gen))) /* we need a new cell to track */
    {   nc->d.BglTrack.node    = migr.node;
        nc->d.BglTrack.portnb  = migr.portnb;
        nc->d.BglTrack.session = migr.session;
        strcpy((char *)(&(nc->d.BglTrack.param[0])),
            (const char *)(&(migr.param[0])));

        if (MigrStopGet(migr))
        {   MigrToolTerminateMesg(nc, M_normal);
            nc->d.BglTrack.node=0;
            nc->d.BglTrack.portnb=0;
            nc->d.BglTrack.session=0;
            nc->d.BglTrack.param[0]='\0';
        }
        else
        {   if((sscanf((const char *)(&(migr.param[0])),
                (const char *)"%d%3s", &size, label) == 2)&&
                (label[0] <= 'z') && (label[0] >= 'a')&&
                (label[1] <= 'z') && (label[1] >= 'a') &&
                (label[2] <= 'z') && (label[2] >= 'a'))
                if (TrackNormLike)
                {   tgl=sl[nc->d.gen.size]->g[nc->d.gi];
                    if (!IsBit(tgl->bits, 0)) /* if name is not permanent */
                    {   SetBit(&tgl->bits, 0, 1);
                        SetBit(&tgl->bits, 1, 0);
                        extract(nc->d.gen.size, 0, nc->d.gi, 0);
                    }
                }
            /* fprintf(stderr, "%d\n", nc->d.hash); */
            MigrationTrackCount++;
            if (eject != -1)
                MigrToolRemotbirthMesg(nc);
            else
                MigrToolBirthMesg(nc);
        }
    }
    else
    {   nc->d.BglTrack.node=0;
        nc->d.BglTrack.portnb=0;
        nc->d.BglTrack.session=0;
        nc->d.BglTrack.param[0]='\0';
    }
#endif /* BGL */
#endif /* NET */
    mc->d.repinst++;
    if(mc->d.mov_daught)
    {   RepInstEffSum += (double) mc->d.repinst / (double) mc->d.mov_daught;
        RepInstSum += mc->d.repinst;
    }
    mc->d.repinst = mc->d.mov_daught = mc->d.mut = 0;
    mc->d.MovOffMin = mc->d.MovOffMax = 0;
    OutDisk((I32s) 'b', nc->d.gen.size, nc->d.gen.label);
#if FRONTEND != STDIO
    FEStats();
#endif /* FRONTEND != STDIO */
}

/*
 * ReapBookeep - ReapCell gene bank bookeeping
 *
 *     cr - cell to be reaped
 */

void ReapBookeep(cr)
    Pcells cr;                  /* cell to be reaped */
{   I32s  i, j;

    OutDisk((I32s) 'd', cr->d.gen.size, cr->d.gen.label);
#ifdef NET
    if (cr->ld == 2)     /* don't sum fecundity for ejected daughters */
        FecunEjecSum++;
    else
#else /* NET */
    if (cr->ld != 2)
#endif /* NET */
    {
        if (cr->d.fecundity)
        {   FecunditySum += cr->d.fecundity;
#ifdef NET
            FecundYesSum++;
#endif /* NET */
        }
#ifdef NET
        else
            FecunZeroSum++;
#endif /* NET */
        AgeSum += cr->d.inst;
        DeathNum++;
    }
    if (GeneBnker)
        ReapGenBook(cr, 0);

#ifdef MICRO
    if((cr->ld)!=2)
        if (cr == BrkTrapCondBuf.curbrkdat.MicroSlice)
        {   GoBrkClr();
            MC_step=1;
            tsprintf((char *)(&(Fbuf[0])),
                "cell being monitored has died");
            FEError(-107, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            FEBanner();
            FEStats();
        }
#endif /*MICRO */

    NumCells--;
    InitCell(cr->q.htis.a, cr->q.htis.i, cr);
    if (!NumCells)
    {   if (FreeMemry)
        {   tfree((void *) FreeMemry, MaxFreeBlocks * sizeof(MemFr), 1907);
            FreeMemry = NULL;
        }
        FreeMemry = (MemFr Fp) tcalloc(MaxFreeBlocks, sizeof(MemFr), 1907);
        FreeMemCurrent = SoupSize;
        MemInit();
        for (i = 0; i < NumCelAr; i++)
            for (j = 0; j < CelArSiz; j++)
                InitCell(i, j, &cells[i][j]);
        TopDummy->ld = BottomDummy->ld = 1;
        ce = BottomReap = TopReap = &cells[0][2];
    }
#ifdef MICRO
    if((cr->ld)!=2)
        ChkBrkBd();
#endif
    reaped = 1;
}

/*
 * MutBookeep - do mutation bookeeping for mutation at soup address i
 *
 *     i - address relative to soup start
 *     instrflg - instruction currently executing
 *                (0 = no, non-zero = yes)
 */
void MutBookeep(i,instrflg,clist,nclist)
I32s i, instrflg, *nclist;
Pcells **clist;
{   I8s md;
    Pcells cm;                  /* cell to be mutated */

    if((!GeneBnker)||IsFree(i))
        return;

    WhichCell(i,&cm,&md);
    if((md=='d')||(i<((cm->mm.p)+(cm->d.mg.p)))||
        (i>=((cm->mm.p)+(cm->d.mg.p)+(cm->d.mg.s))))
        return;

    for(i=0;i<(*nclist);i++)
        if(((*clist)[i])==cm)
            break;

    if(i==(*nclist))
    {   (*clist)=(Pcells *)trecalloc((*clist),
            ((*nclist)+1)*sizeof(Pcells *),
            (*nclist)*sizeof(Pcells *),108);
        (*clist)[*nclist]=cm;
        (*nclist)++;
    }

    if(instrflg)
    {   if(cm!=ce)
        {   if((i>=((cm->mm.p)+(cm->d.mg.p)))&&
                (i<((cm->mm.p)+(cm->d.mg.p)+(ce->d.mg.s))))
                cm->d.nonslfmut=1;
        }
        else
            if(cm->d.srctrksw)
                if((i>=((cm->mm.p)+(cm->d.mg.p)))&&
                    (i<((cm->mm.p)+(cm->d.mg.p)+(ce->d.mg.s))))
                    cm->d.nonslfmut=1;
    }
    else
        cm->d.nonslfmut=1;
}

void CellMutBookeep(clist,nclist)
Pcells *clist;
I32s nclist;
{   I32s i,flag;
    for(i=0;i<nclist;i++)
    {   ReapGenBook(clist[i],1);
        OutDisk((I32s) 'd', clist[i]->
            d.gen.size, clist[i]->d.gen.label);
        clist[i]->d.parent = clist[i]->d.gen;
        clist[i]->d.gi = -1;
        strcpy((char *)(&(clist[i]->
            d.gen.label[0])), "---");
        clist[i]->d.hash=Hash(clist[i]->d.mg.s,
            clist[i]->d.genome+clist[i]->d.mg.p); 
        flag=(DiskBank)?25:0;
        CheckGenotype(&(clist[i]->d), flag, &siz_sl, &sl);
        strcpy((char *)(&(clist[i]->d.gen.label[0])),
            (const char *)Int2Lbl(clist[i]->d.gi));
        DivGenBook(clist[i], InstExe, reaped, 0, 0, 0, 1);
        OutDisk((I32s) 'b', clist[i]->d.gen.size, clist[i]->d.gen.label);
        clist[i]->d.mut++;
    }
    tfree(clist,nclist*sizeof(Pcells),108);
}

/*
 * OutDisk - birth/death log - active when OutDisk = 1
 *           writes one line to the break file log on each birth/death
 *
 *     bd   - 'b' - birth, 'd' - death
 *     size - size of creature
 *     label - genotype label
 */
void OutDisk(bd, size, label)
I32s bd, size;
I8s label[4];
{   I32s ttime;

    if (DiskOut)
    {   if (FirstOutDisk)
        {   FirstOutDisk = 0;
            BrkupCum = 0;
            BrkupCou = 1;
#ifdef IBM3090
            if (BrkupSiz)
                tsprintf(Buff, "break.1.d");
            else
                tsprintf(Buff, "tierra.run.d");
            oufr = tfopen(Buff, "w");
#else /* IBM3090 */
            if (BrkupSiz)
                tsprintf((char *)(&(Buff[0])),
                    "%sbreak.1", GenebankPath);
            else
                tsprintf((char *)(&(Buff[0])),
                    "%stierra.run", GenebankPath);
            oufr = tfopen(&(Buff[0]), (I8s *)"w");
#endif /* IBM3090 */
            if (oufr == NULL)
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra OutDisk() 1: file %s not opened, exiting\n", Buff);
                FEError(-100, EXIT, WRITE, (char *)(&(Fbuf[0])));
            }
#ifdef IBM3090
            Ascii2Ebcdic(label);
#endif /* IBM3090 */
            BrkupCum += tfprintf(oufr, "%d %c %d", InstExe.i,
                (I8s) bd, size);
            if (GeneBnker)
                BrkupCum += 1 + tfprintf(oufr, " %s\n", label);
            else
                BrkupCum += 1 + tfprintf(oufr, "\n");
        }
        else
        {   ttime = InstExe.i - lo.time;
            if (ttime < 0)
                ttime += 1000000L;
            TimePop += (double) ttime *(double) NumCells;
            BrkupCum += tfprintf(oufr, "%d", ttime);
            if (lo.bd != bd)
                BrkupCum += tfprintf(oufr, " %c", bd);
            if (lo.size != size)
                BrkupCum += tfprintf(oufr, " %d", size);
            if (GeneBnker && strcmp((const char *)(&(lo.label[0])),
                (const char *)(&(label[0]))))
            {
#ifdef IBM3090
                Ascii2Ebcdic(label);
#endif /* IBM3090 */
                BrkupCum += tfprintf(oufr, " %s", label);
            }
            BrkupCum += 1 + tfprintf(oufr, "\n");
            if (BrkupSiz && BrkupCum > BrkupSiz * 1024L)
            {   tfclose(oufr);
                BrkupCum = 0;
                BrkupCou++;
#ifdef IBM3090
                tsprintf(Buff, "break.%d.d", BrkupCou);
                oufr = tfopen(Buff, "w");
#else /* IBM3090 */
                tsprintf((char *)(&(Buff[0])),
                    "%sbreak.%d", GenebankPath, BrkupCou);
                oufr = tfopen(&(Buff[0]), (I8s *)"w");
#endif /* IBM3090 */
                if (oufr == NULL)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra OutDisk() 2: file %s not opened, exiting\n", Buff);
                    FEError(-101, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
            }
        }
    }
    else
    {   if (FirstOutDisk)
            FirstOutDisk = 0;
        else
        {   ttime = InstExe.i - lo.time;
            if (ttime < 0)
                ttime += 1000000L;
            TimePop += (double) ttime *(double) NumCells;
        }
    }
    lo.bd = (I8s)bd;
    lo.size = size;
    lo.time = InstExe.i;
    strcpy((char *)(&(lo.label[0])),(const char *)(&(label[0])));
}

/*
 * stats - called before plan() at end of million instruction interval
 *         clean up and verify gene bank
 *         update AverageSize, MaxGenPop MaxGenMem
 *         change subnet
 *         update map files on disk
 */

void stats()
{   if (GeneBnker)
    {   if (reaped)
        {   GBGarbageCollect();
#ifdef ERRORTIE
            VerifyGB(1,1);
#endif /* ERRORTIE */
        }
        CalcGBStats(sl, siz_sl);
    }
    else
        CalcSoupStats();
    CalcTimeSoup();  /* call this function only on Tierra nodes */
#ifdef NET
    if(!(LocalClusterAddr.node))
    {   if((SubNetCnt>1)&&(BasSubNetChgFrq))
        {   SubNetChgTime--;
            if(SubNetChgTime<=0)
                TieCommChgSubNet(TieSubNet, 0,0,0);
            else
                UpdateOwnMapNode();
        }
        else
            UpdateOwnMapNode();
    }
    else
        UpdateOwnMapNode();
#endif /* NET */
}

/*
 * plan - called before plan() at end of million instruction interval
 *        called after plan() at end of million instruction interval
 *        clean up cell structure
 *        update TPing log
 *        for next million instruction interval, update :
 *           flaw and mutation rates
 *           limits
 *              template search distance
 *              "put" buffer size
 *              daughter memory size
 *        update "plan mode" screen
 *        initialize data collection variables for 
 *           next million instruction interval
 */
void plan()
{   I32s OClock;
#ifdef NET
    I32s i;
#endif /* NET */

    OClock = TierraClock;
    TierraClock = (I32s) tietime(NULL);
    if (OClock)
    {   FESpeed = TierraClock - OClock;
        Speed = 1000000L / FESpeed;
        LastSpeedUpdate=TierraClock;
    }
    if (reaped)
        CellsGarbageCollect();  /* call this function only on Tierra nodes */
#ifdef NET
    if (PingDatLog)
    {   WriteTPingDat(0);
        WriteTPingDat(1);

#ifndef AMIGA
#ifndef DECVAX
#ifndef _WIN32
        tfflush(ping_fp);
#endif /* _WIN32 */
#endif /* DECVAX */
#endif /* AMIGA */
#ifdef _WIN32
        ToggleLog(0, &ping_fp, TPINGLOGNAME, &tpinglogpos);
        ToggleLog(1, &ping_fp, TPINGLOGNAME, &tpinglogpos);
#endif /* _WIN32 */

    }
#endif /* NET */
    /* adjust control variables */
    AdjCtrlVars();

#ifndef CLSTRSRVR
    if(IMode==PLN_STATS)
        FEPlan(1);
#if FRONTEND == BASIC
    else if(HistoInEffect())
        query_species(fe_lines);
#endif /* FRONTEND == BASIC */

#else /* CLSTRSRVR */
    srvrFEPlan(1);
#endif /* CLSTRSRVR */
    RepInstEffSum = TimePop = 0.;
    RepInstSum = RepNum = DeathNum = 0L;
    AgeSum = BirthNum = 0;

#ifdef NET
    BirthEject = BirthLocal = BirthInject = 0L;
    FecunditySum = FecunEjecSum = FecunZeroSum = FecundYesSum = 0L;
#endif /* NET */

    EjectToSelf = 0L;
#ifdef NET
    for (i = 0; i < 6; i++)
    {   MonCom[i].SNumMess = MonCom[i].SNumPack = MonCom[i].SNumByte =
        MonCom[i].RNumMess = MonCom[i].RNumPack = MonCom[i].RNumByte = 0;
    }
#endif /* NET */
    TimeStats = InstExe;
}

/*
 * CalcTimeSoup()
 *
 * calculates these variables for each million instructions elapsed:
 *
 * AvgPop       - average population over the time interval
 * Generations  - estimated number of generations elapsed
 * RepInst      - avg num of inst executed per replication
 * RepInstEff   - avg CPU-cycles / byte executed
 * FecundityAvg - avg fecundity at death
 *
 * Returns: void
 */
void CalcTimeSoup()
{   Event TimeSince; /* dummy variable, not used */
    double AvgBD, dt;
    Pcells cr;
    I32s  i;

    if (InstExe.m)
    {   dt=(double)SubEvent(&InstExe, &TimeStats, &TimeSince);
        AvgBD = (double) (BirthNum + DeathNum) / 2.;
        AvgPop = TimePop/dt;  /* average pop over time-interval */
        if (AvgPop)
            Generations += AvgBD / AvgPop;
        if (RepNum)
        {   RepInst = RepInstSum / RepNum;
            RepInstEff = RepInstEffSum / RepNum;
        }
        else
        {   RepInst=LazyTol*10000;
            RepInstEff=(double)RepInst;
        }
        if (!RepInst)
        {   RepInst=LazyTol*10000;
            RepInstEff=RepInst;
        }
        if (!DeathNum && NumCells)
        {   FecunditySum = AgeSum = 0;
            cr = TopReap;
            for (i = 0; i < NumCells; i++)
            {   FecunditySum += cr->d.fecundity;
                AgeSum += cr->d.inst;
                cr = &cells[cr->q.n_reap.a][cr->q.n_reap.i];
            }
        }
        if((DeathNum)?DeathNum:NumCells)
        {   if (FecunditySum < 0.1)
                FecundityAvg = 1.0;
            else
                FecundityAvg = ((double)FecunditySum)/
                    ((double)((DeathNum)?DeathNum:NumCells));
            AgeAvg = AgeSum / ((DeathNum)?DeathNum:NumCells);
        }
        else
        {   FecundityAvg = 1.0;
            AgeAvg = 0;
        }
    }

#ifdef NET
    FEBirthLocal = BirthLocal;
    FEBirthInject = BirthInject;
    FEBirthEject = BirthEject;
    FEFecundYesSum = FecundYesSum;
    FEFecunZeroSum = FecunZeroSum;
    FEFecunEjecSum = FecunEjecSum;
#endif /* NET */

    FEDeathNum = DeathNum;
    FEBirthNum = BirthNum;
    FEEjectToSelf = EjectToSelf;
#ifdef NET
    for (i = 0; i < 6; i++)
    {   FEMonCom[i].SNumMess = MonCom[i].SNumMess;
        FEMonCom[i].SNumPack = MonCom[i].SNumPack;
        FEMonCom[i].SNumByte = MonCom[i].SNumByte;
        FEMonCom[i].RNumMess = MonCom[i].RNumMess;
        FEMonCom[i].RNumPack = MonCom[i].RNumPack;
        FEMonCom[i].RNumByte = MonCom[i].RNumByte;
    }
#endif /* NET */
    if (MaxGenMem.size)
        MaxMemry = MaxMemry / MaxGenMem.size;
}

/*
 * CalcAverages - update AverageSize
 *
 */

void CalcAverages()             /* call this function only on Tierra nodes */
{   I32s tNumCells = 0, ar, ci;
    Pcells tc;                  /* temporary cell pointer */

    AverageSize = 0;
    for (ar = 0; ar < NumCelAr; ar++)
        for (ci = 0; ci < CelArSiz; ci++)
        {   if (ar == 0 && ci < 2)
                continue;
            tc = &cells[ar][ci];
            if (tc->ld)
            {   tNumCells++;
                AverageSize += tc->d.gen.size;
            }
        }
#ifdef ERRORTIE
    if (tNumCells != NumCells)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra CalcAverages() NumCells = %d  count of cells = %d\n",
            NumCells, tNumCells);
        FEError(-102, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */
    AverageSize /= tNumCells;
}

/*
 * CalcSoupStats()
 *
 * This function builds a local soup genebank from a scan of the soup
 * call this function only on Tierra nodes
 *
 * Returns: void
 */
void CalcSoupStats()
{   I32s lsiz_sl = 1, flag;
    SList Fp Fp lsl;

    flag=(DiskBank)?25:0;
    lsl = MkGBFromSoup(&lsiz_sl, &lsl, flag);
    CalcGBStats(lsl, lsiz_sl);
    FreeGB(&lsl, &lsiz_sl);
}

/*
 * CalcGBStats(lsl, lsiz_sl)
 *
 * uses data in the rambank to calculate 
 * AverageSize = (tNumCells) ? (AverageSize / tNumCells) : 0;
 *
 * Returns: void
 */
void CalcGBStats(lsl, lsiz_sl)
SList Fp Fp lsl;
I32s lsiz_sl;
{   I32s si, gi, pop, mem, tNumCells;
    Pgl pgl;

    tNumCells=AverageSize=MaxPop=MaxMemry=0;
    for(si=lsiz_sl-1;si>=0;si--)
    {   if(lsl[si])
        {   if(lsl[si]->num_c)
            {   if(GeneBnker)
                {   for(gi=lsl[si]->a_num-1;gi>=0;gi--)
                    {   if(!TNULL(pgl=lsl[si]->g[gi]))
                        {   if((pop=pgl->pop))
                            {   mem=pop*si;
                                if(pop>MaxPop)
                                {   MaxPop=pop;
                                    MaxGenPop.size=si;
                                    strcpy((char *)(&(MaxGenPop.
                                        label[0])), (const char *)
                                        Int2Lbl(gi));
                                }
                                if (mem>MaxMemry)
                                {   MaxMemry=mem;
                                    MaxGenMem.size=si;
                                    strcpy((char *)(&(MaxGenMem.label[0])),
                                        (const char *)Int2Lbl(gi));
                                }
                                AverageSize+=mem;
                                tNumCells+=pop;
                            }
#ifdef NET
                            if(((lsl[si]->siznsl)>=NodeSelSucRatPrec)||
                                (NodeSelSucRatPrec<0))
                            {   (pgl->sucgennslrat)=(pgl->gennsl)?
                                    (((double)(pgl->sucgennsl))/
                                    ((double)(pgl->gennsl))):0.0;
                                pgl->svgennsl=pgl->gennsl;
                                pgl->gennsl=0;
                                pgl->svsucgennsl=pgl->sucgennsl;
                                pgl->sucgennsl=0;
                            }
#endif /* NET */
                        }
                    }
                }
                else
                {   tNumCells+=lsl[si]->num_c;
                    AverageSize+=lsl[si]->num_c*si;
                }
            }
#ifdef NET
            if(((lsl[si]->siznsl)>=NodeSelSucRatPrec)||
                (NodeSelSucRatPrec<0))
            {   (lsl[si]->sucsiznslrat)=(lsl[si]->siznsl)?
                    (((double)(lsl[si]->sucsiznsl))/
                    ((double)(lsl[si]->siznsl))):0.0;
                lsl[si]->svsiznsl=lsl[si]->siznsl;
                lsl[si]->siznsl=0;
                lsl[si]->svsucsiznsl=lsl[si]->sucsiznsl;
                lsl[si]->sucsiznsl=0;
            }
#endif /* NET */
        }
    }
    if(tNumCells!=NumCells)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra CalcGBStats() NumCells = %d  count of cells = %d\n",
            NumCells, tNumCells);
        FEError(-103, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    AverageSize=(tNumCells)?(AverageSize/tNumCells):0;
}

/*
 * CalcGBMaxes - up MaxPop, MaxMemry, MaxGenPop, MaxGenMem
 *
 *     lsl - SList pointer array (rambank)
 *     lsiz_sl - size of SList pointer array
 *
 */
void CalcGBMaxes(lsl, lsiz_sl)
    SList Fp Fp lsl;
    I32s lsiz_sl;
{   I32s si, gi, pop, mem;
    Pgl pgl;

    MaxPop = MaxMemry = 0;
    for (si = lsiz_sl - 1; si >= 0; si--)  /* each size class */
    {   if (lsl[si] && lsl[si]->num_c)
        {   for (gi = lsl[si]->a_num - 1; gi >= 0; gi--)  /* each genotype */
            {   if((!TNULL(pgl=lsl[si]->g[gi]))&&(pop=pgl->pop))
                {   mem = pop * si;
                    if (pop > MaxPop)
                    {   MaxPop = pop;
                        MaxGenPop.size = si;
                        strcpy((char *)(&(MaxGenPop.label[0])),
                            (const char *)Int2Lbl(gi));
                    }
                    if (mem > MaxMemry)
                    {   MaxMemry = mem;
                        MaxGenMem.size = si;
                        strcpy((char *)(&(MaxGenMem.label[0])),
                            (const char *)Int2Lbl(gi));
                    }
                }
            }
        }
    }
}

/*
 * FreeGB - free ram bank (gene bank)
 *
 *     lsl - pointer to SList pointer array (rambank)
 *     lsiz_sl - pointer to size of SList pointer array
 *
 */
void FreeGB(lsl, lsiz_sl)
    SList Fp Fp Fp lsl;
    I32s *lsiz_sl;
{   I32s si;
    I32s gi;
    Pgl tgl;

    if(!(*lsl))
        return;

    for(si=0; si<(*lsiz_sl); si++)  /* for each size */
    {   if((*lsl)[si])
        {   if((*lsl)[si]->num_c)
                NumSizes--;
            if((*lsl)[si]->g)
            {   for(gi=0; gi<((*lsl)[si]->a_num); gi++)  /* for each gen */
                {   if(!TNULL(tgl=(*lsl)[si]->g[gi]))
                    {   if(tgl->pop)
                            NumGenotypes--;
                        FreeThrdAnaDat(&(tgl->glst_thrdanadat));
                        if(tgl->genome)
                        {   FreeGenome(tgl);
                            tgl->genome=NULL;
                        }
                        if(tgl->gbits)
                        {   FreeGenBits(tgl);
                            tgl->gbits=NULL;
                        }
                        tfree((void *)((*lsl)[si]->g[gi]),
                            sizeof(GList), 1402);
                        (*lsl)[si]->g[gi]=NULL;
                    }
                }
                tfree((void *)((*lsl)[si]->g),
                    (*lsl)[si]->a_num * sizeof(Pgl), 1401);
                (*lsl)[si]->g=NULL;
            }
            if((*lsl)[si]->hash)
            {   tfree((void *)((*lsl)[si]->hash),
                    ((*lsl)[si]->a_num)*sizeof(I32s), 1405);
                (*lsl)[si]->hash=NULL;
            }
            FreeGenDef(&((*lsl)[si]->slst_gendef),
                &((*lsl)[si]->genelkup), si);
            tfree((void *)(*lsl)[si], sizeof(SList), 1400);
            (*lsl)[si]=NULL;
        }
    }
    tfree((void *)(*lsl), (*lsiz_sl)*sizeof(SList Fp), 1903);
    (*lsl)=NULL;
    *lsiz_sl=0;
    if(NumGenotypes||NumSizes)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra FreeGB(): NumGenotypes = %d  NumSizes = %d\n",
            NumGenotypes, NumSizes);
        FEError(-104, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
}

/*
 * MkGBFromSoup - make gene bank from soup
 *
 *  lsl - pointer to SList pointer array (rambank)
 *  lsiz_sl - pointer to size of SList pointer array
 *  flag - arg to CheckGenotype
 *      : bit 0 : check .gen files
 *      : bit 1 : check .tmp files
 *      : bit 2 : check files even if rambank does not indicate presence
 *                of genotype on disk (used for startup of old or new soups).
 *      : bit 3 : use all files in the genebank to assemble the list in
 *                the rambank.  Each time a new size is checked, all genomes of 
 *                that size in the genebank will become listed in the rambank 
 *                as being on the disk (used for startup of old soups and for 
 *                cumulative genebanks).  However, the genomes will remain on 
 *                disk until they are actually accessed, at which time they 
 *                will be placed in the genebank in RAM.
 *      : bit 4 : when reading a file from the disk genebank, zero bit 1 of the
 *                tgl->bits field, otherwise preserve it.
 *
 */
SList Fp Fp MkGBFromSoup(lsiz_sl, lsl, flag)
    I32s *lsiz_sl, flag;
    SList Fp Fp Fp lsl;
{   I32s si, ar, ci, i, tNumCells=0;
#if PLOIDY > 1
    I32s j;
#endif
    I32s gi;
    Pgl tgl;
    Pcells tc;                  /* temporary cell pointer */

    (*lsiz_sl)=1;
    NumSizes=NumGenotypes=0;
    (*lsl)=(SList Fp Fp)tcalloc(1, sizeof(SList Fp), 1903);
    for(ar=0; ar<NumCelAr; ar++)
        for(ci=0; ci<CelArSiz; ci++)
        {   if((ar==0)&&(ci<2))
                continue;
            tc=&cells[ar][ci];
            if(tc->ld)
            {   si=tc->d.gen.size;
                gi=tc->d.gi;
                tNumCells++;
                if(GeneBnker&&(gi<0))
                {   tc->d.hash=Hash(tc->d.mg.s, (tc->d.genome)+(tc->d.mg.p));
                    CheckGenotype(&(tc->d), flag, lsiz_sl, lsl);
                    strcpy((char *)(&(tc->d.gen.label[0])),
                        (const char *)Int2Lbl(tc->d.gi));
                    gi=tc->d.gi;
                }
                tc->d.origgi=tc->d.gi;
                if(si>=(*lsiz_sl))
                {   (*lsl)=(SList Fp Fp)trecalloc((void *)(*lsl),
                        (si+1)*sizeof(SList Fp),
                        (*lsiz_sl)*sizeof(SList Fp), 1903);
                    (*lsiz_sl)=si+1;
                }
                if(!(*lsl)[si])
                {   (*lsl)[si]=(SList Fp)tcalloc(1, sizeof(SList), 1400);
                    if(GeneBnker)
                    {   ((*lsl)[si]->g)=(Pgl Fp)
                            tcalloc(gi+1, sizeof(Pgl), 1401);
                        ((*lsl)[si]->hash)=(I32s Fp)
                            tcalloc(gi+1, sizeof(I32s), 1405);
                        ((*lsl)[si]->a_num)=gi+1;
                    }
                }
#ifdef ERRORTIE
                if(!(*lsl)[si]->num_c)
                {   if ((*lsl)[si]->num_g)
                        FEError(-105, NOEXIT, NOWRITE,
           "Tierra MkGBFromSoup() !(*lsl)[si]->num_c but (*lsl)[si]->num_g");
                }
#endif /* ERRORTIE */
                if(!(*lsl)[si]->num_c)
                    NumSizes++;
                ((*lsl)[si]->num_c)++;
                if(GeneBnker)
                {   if(gi>=((*lsl)[si]->a_num))
                    {   ((*lsl)[si]->g)=(Pgl Fp)
                            trecalloc((void *)((*lsl)[si]->g),
                            (gi+1)*sizeof(Pgl),
                            ((*lsl)[si]->a_num)*sizeof(Pgl), 1401);
                        ((*lsl)[si]->hash)=(I32s Fp)
                            trecalloc((void *)((*lsl)[si]->hash),
                            (gi+1)*sizeof(I32s),
                            ((*lsl)[si]->a_num)*sizeof(I32s), 1405);
                        ((*lsl)[si]->a_num)=gi+1;
                        InitDynArr((DynArr *)(&((*lsl)[si]->
                            slst_gendef)), sizeof(GenDef), 0);
                    }
                    if(TNULL((*lsl)[si]->g[gi]))
                    {   ((*lsl)[si]->g[gi])=(Pgl)
                            tcalloc(1, sizeof(GList), 1402);
                        InitThrdAnaDat(&((*lsl)[si]->
                            g[gi]->glst_thrdanadat));
                    }
                    tgl=(*lsl)[si]->g[gi];
                    if(!tgl->genome)
                    {   AllocGenome(tgl,si);
                        for (i=0; i<si; i++)
#if PLOIDY == 1
                            tgl->genome[i]=tc->d.genome[i];
#else
                            for (j=0; j<PLOIDY; j++)
                                (tgl->genome[i][j])=(tc->d.genome[i][j]);
#endif
                    }
                    if(!tgl->gbits)
                        AllocGenBits(tgl,si);
                    if(!tgl->pop)
                    {   (*lsl)[si]->num_g++;
                        NumGenotypes++;
                    }

                    /* update size class average */
                    /* reproduction efficiency */
                    /* daughter 1 */
                    if(tgl->d1.instP)
                    {   if(tgl->d1.mov_daught)
                        {   (sl[si]->AvgRpdEff[0])=
                                (((sl[si]->AvgRpdEff[0])*
                                ((double)(sl[si]->AvgEffCnt[0])))+
                                (((double)(tgl->d1.instP))/
                                ((double)(tgl->d1.mov_daught))))/
                                ((double)((sl[si]->AvgEffCnt[0])+1));
                            (sl[si]->AvgEffCnt[0])++;
                        }
                    }
                    /* daughter 2 */
                    if(tgl->d2.instP)
                    {   if(tgl->d2.mov_daught)
                        {   (sl[si]->AvgRpdEff[1])=
                                (((sl[si]->AvgRpdEff[1])*
                                ((double)(sl[si]->AvgEffCnt[1])))+
                                (((double)(tgl->d2.instP))/
                                ((double)(tgl->d2.mov_daught))))/
                                ((double)((sl[si]->AvgEffCnt[1])+1));
                            (sl[si]->AvgEffCnt[1])++;
                        }
                    }

                    (tgl->hash)=(*lsl)[si]->hash[gi]=tc->d.hash;
                    (tgl->gen.size)=(tc->d.gen.size);
                    strcpy((char *)(&(tgl->gen.label[0])),
                        (const char *)Int2Lbl(gi));
                    tgl->originC=tietime(NULL);
                    tgl->originI=InstExe;
                    tgl->parent=tc->d.parent;
                    tgl->bits=0;
                    tgl->max_cpus=0;
                    if(reaped)
                    {   tgl->MaxPropPop=1.0/((float)NumCells);
                        tgl->MaxPropInst=((float)si)/((float)SoupSize);
                        tgl->MaxPop=1;
                        tgl->mpp_time=InstExe;
                    }
                    tgl->ploidy=tc->d.ploidy;
                    tgl->pop++;
                    ((*lsl)[si]->g[tc->d.origgi]->origpop)++;
                }
            }
        }
#ifdef ERRORTIE
    if (tNumCells!=NumCells)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra MkGBFromSoup() NumCells = %d  count of cells = %d\n",
            NumCells, tNumCells);
        FEError(-106, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */
    return *lsl;
}

/*
 * CellsGarbageCollect - clean up cell structure
 *
 */

void CellsGarbageCollect()
{   I32s ar, ci;
#ifdef NET
    I32s i;
#endif /* NET */
    I8s LInUse = 0, PInUse = 0;
    Pcells tc;                  /* temporary cell pointer */
    CellInd htis, mtis;

    htis.a = ce->q.htis.a;
    htis.i = ce->q.htis.i;
    if(BrkTrapCondBuf.curbrkdat.MicroSlice)
    {   mtis.a=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.a;
        mtis.i=BrkTrapCondBuf.curbrkdat.MicroSlice->q.htis.i;
    }

    if (NumCelAr <= 2)
        return;
    ar = NumCelAr - 2;  /* check for use in penultimate cells array */
    for (ci = 0; ci < CelArSiz; ci++)
    {   if (cells[ar][ci].ld)
        {   PInUse = 1;
            break;
        }
    }
    if (PInUse)
        return;
    ar = NumCelAr - 1;  /* check for use in last cells array */
    for (ci = 0; ci < CelArSiz; ci++)
    {   if (cells[ar][ci].ld)
        {   LInUse = 1;
            break;
        }
    }
    if (LInUse)
        return;

    if (cells[ar])  /* free the last cells array */
    {   for (ci = 0; ci < CelArSiz; ci++)
        {   tc = &cells[ar][ci];
            if (tc->c.ar)
            {   tfree((void *) (tc->c.ar), tc->c.n * sizeof(Cpu), 1902);
                tc->c.ar = NULL;
            }
            if (tc->c.sig.sig)
            {   tfree((void *) (tc->c.sig.sig), tc->c.sig.siz * sizeof(I8s),
                    706);
                tc->c.sig.siz = 0;
                tc->c.sig.sig = NULL;
            }
#ifdef NET
            if (tc->c.io.io)
            {   for (i = 0; i < tc->c.io.siz; i++)
                {   if (tc->c.io.io[i].d.d)
                        tfree((void *) (tc->c.io.io[i].d.d),
                            tc->c.io.io[i].d.len * sizeof(I8s), 702);
                    tc->c.io.io[i].d.d = NULL;
                }
                tfree((void *) (tc->c.io.io), tc->c.io.siz * sizeof(IOS),
                    700);
                tc->c.io.siz = 0;
                tc->c.io.io = NULL;
            }
#endif /* NET */
#ifdef IO
            FreePGbf(&(tc->c.pg.pgb_put));
            FreePGbf(&(tc->c.pg.pgb_get));
#endif /* IO */
        }
        tfree((void *) cells[ar], CelArSiz * sizeof(TCell), 1901);
        cells[ar] = NULL;
    }
    NumCelAr--;
    cells = (Pcells Fp) trecalloc((void *) cells,
        (I32u) NumCelAr * sizeof(Pcells Fp),
        (I32u) (NumCelAr + 1) * sizeof(Pcells Fp), 1900);
    CellsSize = NumCelAr * CelArSiz;
    ce = &cells[htis.a][htis.i];
    if(BrkTrapCondBuf.curbrkdat.MicroSlice)
        BrkTrapCondBuf.curbrkdat.MicroSlice=&cells[mtis.a][mtis.i];
}

/*
 * WhoIs - identify relation of given soup address to given cell
 *
 * input
 *    cp      - pointer to pointer to cell
 *    target  - address relative to soup start
 *    hoffset - pointer to offset relative to host start
 *
 * output
 *    returns (address is in):
 *       0 - same cell
 *       1 - own daughter
 *       2 - another cell
 *       3 - daughter of another cell
 *       4 - free memory
 *    *cp - pointer to cell containing address
 *    *hoffset - offset relative to host start
 *
 */
I32u WhoIs(cp, target, hoffset)
    Pcells Fp cp;               /* pointer to pointer to cell */
    I32s target;
    I32s *hoffset;
{   I32s mmp=ce->mm.p;
    I32s mdp=ce->md.p;
    I32s mds=ce->md.s;
    I32s notcurceldgtr=0;
    I32u hofsttyp=-1;
    I32s hosttype;

    if((target>=mmp)&&(target<(mmp+(ce->mm.s))))
    {   (*hoffset)=target-mmp;
        hofsttyp=0;  /* same cell */
        (*cp)=ce;
    }
    else
    {   if(mds)
            if((target>=mdp)&&(target<(mdp+mds)))
            {   (*hoffset)=target-mdp;
                hofsttyp=1;  /* daughter cell */
                (*cp)=ce;
            }
            else
                notcurceldgtr=1;
        else
            notcurceldgtr=1;
        if(notcurceldgtr)
        {   (*cp)=FindHost(target, &hosttype);
            switch(hosttype)
            {   case 1: /* adult */
                    hofsttyp=2;
                    (*hoffset)=target-((*cp)->mm.p);
                    break;
                case 2: /* daughter */
                    hofsttyp=3;
                    (*hoffset)=target-((*cp)->md.p);
                    break;
                default: /* free memory */
                    hofsttyp=4;
                    (*hoffset)=target-mmp;
                    break;
            }
        }
    }
    (*hoffset)=ad(*hoffset); 
    return hofsttyp;
}

/*
 * IsSameGen - compare two genomes of the same size
 *
 *  size - size of genomes
 *  g1   - genome 1
 *  g2   - genome 2
 *
 * returns : 0 - genomes are different
 *           1 - genomes are identical
 *
 */
I8s IsSameGen(size, g1, g2)     /* compare two genomes */
    I32s size;
    FpInst g1, g2;
{   I32s i;
#if PLOIDY > 1
    I32s j;
#endif

    for (i = 0; i < size; i++)
#if PLOIDY == 1
        if (*(g1 + i) != *(g2 + i))
#else /* PLOIDY > 1 */
        for (j = 0; j < PLOIDY; j++)
            if (g1[i][j] != g2[i][j])
#endif /* PLOIDY > 1 */
                return 0;
    return 1;
}

/*
 * AdjCtrlVars - for next million instruction interval, update :
 *                  flaw and mutation rates
 *                  limits
 *                     template search distance
 *                     "put" buffer size
 *                     daughter memory size
 */
void AdjCtrlVars()              /* adjust control variables */
{   CalcFlawRates();
    if (JmpSouTra)
        RateJmpSou = (I32s) (2 * JmpSouTra * AverageSize);
    Search_limit = (I32s) (SearchLimit * AverageSize);
    Search_limit=(AbsSearchLimit>0)?((AbsSearchLimit>
        Search_limit)?AbsSearchLimit:Search_limit):Search_limit;
    Put_limit = (I32s) (PutLimit * AverageSize);
    MalLimit = MalTol * AverageSize;
    if (MalLimit >= SoupSize)
        MalLimit = SoupSize - 1;
}

/*
 * AdjCtrlVars - for next million instruction interval, update 
 *                  flaw and mutation rates
 */
void CalcFlawRates()
{   I32s pop_gen_time;
    double prob_of_hit;

    if (GenPerMovMut)
        RateMovMut = (I32s) 2L * GenPerMovMut * AverageSize * PLOIDY;
    if (InstExe.m)
        pop_gen_time=(I32s)(AvgPop*RepInst);
    else
    {   RepInst = 10L * AverageSize;
        pop_gen_time = (AverageSize) ?
            (RepInst * (SoupSize / (4L * AverageSize))) : 0;
    }
    prob_of_hit = (double) AverageSize / (double) SoupSize;
    if (GenPerBkgMut)
        RateMut=(I32s)(((double)pop_gen_time)*2.0*
            ((double)GenPerBkgMut)*prob_of_hit);
    if (GenPerFlaw)
        RateFlaw = (I32s) RepInst * GenPerFlaw * 2L;
}
#ifdef EXEINSTSIZTIMSLC
void AllocCellGenBits(nc)
Pcells nc;
{   (nc->d.cellgenbits)=(CellGenBits *)
        thcalloc(nc->mm.s, sizeof(CellGenBits), 109);
}
void FreeCellGenBits(cp)
Pcells cp;
{   if(cp->d.cellgenbits)
        tfree((void *)(cp->d.cellgenbits),
            (cp->mm.s)*sizeof(CellGenBits), 109);
}
void MarkCellGenBitExe(cp, addr)
Pcells cp;
I32s addr;
{   I32s curcelofst;
    CellGenBits InstExedBit=1<<INSTEXECD;
    CellGenBits *cellgenbits;
    I32s mgp=ce->d.mg.p;
    curcelofst=addr-(ce->mm.p);
    if((curcelofst>=mgp)&&((curcelofst)<(mgp+(ce->d.mg.s))))
    {   cellgenbits=&((cp->d.cellgenbits)[curcelofst]);
        if(!((*cellgenbits)&InstExedBit))
        {   (*cellgenbits)|=InstExedBit;
            (cp->d.ExeSegSiz)++;
        }
    }
}
void ReadCellGenBits(inf, cp)
FILE *inf;
Pcells cp;
{   AllocCellGenBits(cp);
    tfread((I8s Fp)(cp->d.cellgenbits),
        sizeof(CellGenBits), cp->mm.s, inf);
}
void SaveCellGenBits(ouf, cp)
FILE *ouf;
Pcells cp;
{   tfwrite((I8s Fp)(cp->d.cellgenbits),
        sizeof(CellGenBits), cp->mm.s, ouf);
    FreeCellGenBits(cp);
}
#endif /* EXEINSTSIZTIMSLC */
