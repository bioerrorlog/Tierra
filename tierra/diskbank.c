/* @(#)diskbank.c 1.16 2/13/95 */
/* diskbank.c 30-3-98 disk genebank manager for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #04XX, in use: */
/* FEError labels: #04XX, in use: 0400-0401 */

#include "license.h"
#include "tierra.h"
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

static void GetSmpFilNam P_((I8s* dirnam,
    I32s size, I8s *label, I8s *nambuf));

/*
 * Inject - Injects a genome into the soup
 *
 * FpInst g;      pointer to genome
 * I32s size;     size of genome
 * Mem  mg;       genetic memory location and size
 * I32s sad;      suggested address for placement of genome
 * I32s tol;      tolerance placement of genome
 * I32s disk;     1 = this genome comes from the disk
 * float *rrpi;   reap rand prop for injection
 * I32s immigrant;    1 = immigrant
 * N32u immsrcnode; immigrant source node
 *
 * Returns: Pcells cp;
 * pointer to cell struct where injected creature will go
 */
Pcells Inject(g, size, mg, sad, tol, disk, rrpi,
    immigrant, immsrcnode)
FpInst  g;
I32s  size, sad, tol, disk, immigrant;
Mem  mg;
float * rrpi;
N32u immsrcnode;

{   float tReapRndProp = ReapRndProp;
    I32s j, flag, allocmemptr;
#if PLOIDY > 1
    I32s k;
#endif
    Pcells cp = NULL;
    /* pointer to cell struct where injected creature will go */
    FpInst si;

    if(size<=SoupSize)
    {   if ((allocmemptr = MemAlloc(size, sad, tol)) >= 0)
        {   cp = GetFreeCell();
            cp->ld = 1;
            cp->d.gen.size = cp->mm.s = size;
            cp->d.mg = mg;
            ReapRndProp = *rrpi;  /* allocate the needed memory */
            cp->mm.p = allocmemptr;
            ReapRndProp = tReapRndProp;
            si = cp->d.genome = soup + cp->mm.p;
            for (j = 0; j < cp->c.n; j++)
                cp->c.ar[j].ip = cp->mm.p;
            EntBotSlicer(cp);
            EntBotReaper(cp);
            cp->d.is = 1;

            memcpy(si, g, size*sizeof(Instruction));

            if (GeneBnker)  /* determine genotype, record in genebanker */
            {   cp->d.hash = Hash(cp->d.mg.s, cp->d.genome + cp->d.mg.p);
                if (DiskBank)  /* new hash due to mutation */
                {   flag = 21;
                    if (CumGeneBnk)
                        SetBit((I32u *) & flag, 3, I32UONE);
                }
                else
                    flag = 0;
                CheckGenotype(&(cp->d), flag, &siz_sl, &sl);
                cp->d.origgi = cp->d.gi;
                (sl[size]->g[cp->d.origgi]->origpop)++;
                cp->d.parent = sl[size]->g[cp->d.gi]->parent;
                strcpy((char *)(&(cp->d.gen.label[0])),
                    (const char *)Int2Lbl(cp->d.gi));

#ifdef NET
                if(immigrant){
#ifdef TIERRA
#if FRONTEND == BASIC
                    if(!HistoInEffect())
                    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                        tsprintf((char *)(&(Fbuf[0])),
                "%s: Genome %.4d%3s was injected from %.2d.%.2d.%.2d.%.2d\n",
                            LocalHostName, cp->d.gen.size, cp->d.gen.label,
                            ((I8u *)(&(immsrcnode)))[0],
                            ((I8u *)(&(immsrcnode)))[1],
                            ((I8u *)(&(immsrcnode)))[2],
                            ((I8u *)(&(immsrcnode)))[3]);

                        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#ifdef BGL
                        TBgl_make_femessages(&(Fbuf[0]),
                            strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */

#ifdef TIERRA
#if FRONTEND == BASIC
                    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                }
#endif /* NET */

                DivGenBook(cp, InstExe, reaped, 0, 0, disk, 0);
             }
#ifdef NET
            else
            {   if(immigrant){
#ifdef TIERRA
#if FRONTEND == BASIC
                    if(!HistoInEffect())
                    {
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                        tsprintf((char *)(&(Fbuf[0])),
                "%s: Genome %.4d was injected from %.2d.%.2d.%.2d.%.2d\n",
                            LocalHostName, cp->d.gen.size,
                            ((I8u *)(&(immsrcnode)))[0],
                            ((I8u *)(&(immsrcnode)))[1],
                            ((I8u *)(&(immsrcnode)))[2],
                            ((I8u *)(&(immsrcnode)))[3]);
                        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#ifdef BGL
                        TBgl_make_femessages(&(Fbuf[0]),
                            strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */

#ifdef TIERRA
#if FRONTEND == BASIC
                    }
#endif /* FRONTEND == BASIC */
#endif /* TIERRA */
                }
            }
#endif /* NET */

            OutDisk((I32s) 'b', cp->d.gen.size, cp->d.gen.label);
            BirthNum++;

#ifdef NET
            BirthInject++;
#endif /* NET */

            LastDiv = InstExe;
#if FRONTEND != STDIO
            FEStats();
#endif /* FRONTEND != STDIO */
            if (NumCells == 1)
            {   CalcAverages();
                AdjCtrlVars();
            }
#ifdef EXEINSTSIZTIMSLC
            AllocCellGenBits(cp);
#endif /* EXEINSTSIZTIMSLC */
        }
    }
    return cp;
}

/*
 * InjectFromBank - inject genome from disk bank
 *
 * crit      - genotype name "<size><label>"
 * sad       - suggested address for placement of genome
 * tol       - tolerance placement of genome
 * injectnum - number of copys to inject
 *
 * returns : actual number of copies injected
 *
 */
I32s InjectFromBank(crit, sad, tol, injectnum)
    I8s *crit;
    I32s sad;       /* suggested address for placement of genome */
    I32s tol;       /* tolerance placement of genome */
    I32s injectnum; /* number of creatures to be injected */
{   float rrpi;
    I32s size, i, actnuminject, tNumCellsMin;
    GList *g;
    Pcells cp;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    actnuminject = 0;
    g = GetAGen(crit);
    if (g == NULL)
        return actnuminject;

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
    sscanf((const char *)crit, (const char *)"%d", &size);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tNumCellsMin = NumCellsMin;
    NumCellsMin = 0;
    for (i = 0; i<injectnum; i++)
    {   rrpi = (float)1.0;
        while (!(cp=Inject(g->genome, size,
           g->mg, sad, tol, 1, &rrpi, 0,0)))
        {   if (reaper(1, -1, REAP_SOUP_FULL))
                break;
        }
        cp->d.birthtime = (I32s) tietime(NULL);

#ifdef EXEINSTSIZTIMSLC
        cp->d.PExeSegSiz=cp->d.mg.s;
#endif /* EXEINSTSIZTIMSLC */
        actnuminject++;
        if (sad >= 0 && sad < SoupSize)
        {   sad += size;
            sad = ad(sad);
        }
    }
    NumCellsMin = tNumCellsMin;
    FreeGen(g);
    return actnuminject;
}

/*
 * GetAGen - read genome from disk bank
 *           searches for both temporary and permanent archives
 *
 * crit      - genotype name "<size><label>"
 *
 * returns : pointer to GList structure with genome in it
 *           0 on failure
 *
 */
GList *GetAGen(crit)
    I8s *crit;
{   GList *g;

    if (!(g = GetAGenExt(crit, (I8s *)".gen")))
    {   if (!(g = GetAGenExt(crit, (I8s *)".tmp")))
        {   tsprintf((char *)(&(Fbuf[0])), "genome not found");
            FEError(-400, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
        }
    }
    return g;
}


/*
 * GetAGenExt - read genome from disk bank
 *              searches for specified archive type
 *
 * crit - genotype name "<size><label>"
 * ext  - ".gen" - check permanent archive
 *        ".tmp" - check temporary archive
 *
 *
 * returns : pointer to GList structure with genome in it
 *           0 on failure
 *
 */
GList *GetAGenExt(crit, ext)
    I8s *crit, *ext;
{   I32s size;
    I32s n;
    GList *g = NULL;
    char cpath[128], gen[4];
    FILE *fp = NULL;
    head_t head;
    indx_t *indx = NULL, *tindx;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet = TSigBlock(SigSet);
#endif /* SIGBLOCK */
    sscanf((const char *)crit, (const char *)"%d%3s", &size, gen);
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

    tsprintf(cpath, "%s%.4d%s", GenebankPath, size, ext);
    if ( (fp = open_ar((I8s *)(&(cpath[0])), size, 0)) != 0 )
    {   head = read_head(fp);
        if ( (indx = read_indx(fp, &head)) != 0 )
        {   if ((n = (I32s)find_gen(indx,
                (I8s *)(&(gen[0])), head.n)) != head.n)
            {   tindx = &indx[n];
                g = get_gen(fp, &head, tindx, n);
            }
            else
            {
#ifdef SIGBLOCK
                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                return NULL;
            }
            thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
        }
        tfclose(fp);
    }
    return g;
}

/*
 * extract - extract genome of given cell from soup into gene bank
 *
 * si - size of genome to be extracted
 * virtextr - 0 - real extraction
 *            1 - virtual extraction - already in gene bank
 * gi - genotype index of genome to be extracted
 * tadatrdy - 0 =  not called for "thread analysis ready"
 *            1 =  called for "thread analysis ready"
 *
 */
void extract(si, virtextr, gi, tadatrdy)
    I32s si;
    I32s virtextr, gi;
    I32s tadatrdy;
{   Pgl g;
    FILE *fp;
    head_t head;
    indx_t *indx;

    if (!DiskBank)
        virtextr = 1;
    isolate = 0;
    g = sl[si]->g[gi];
    if (g->pop)
    {   if (virtextr)
            tsprintf((char *)(&(ExtrG[0])), "%.4d%s @ %d v",
                g->gen.size, g->gen.label, (GeneBnker)?g->pop:0);
        else
            tsprintf((char *)(&(ExtrG[0])), "%.4d%s @ %d",
                g->gen.size, g->gen.label, (GeneBnker)?g->pop:0);
#if FRONTEND == STDIO
        tsprintf((char *)(&(Fbuf[0])), "extract: %s\n", ExtrG);
        FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#endif /* FRONTEND == STDIO */
#ifdef BGL
        tsprintf((char *)(&(Fbuf[0])), "extract: %s", ExtrG);
        TBgl_make_femessages(&(Fbuf[0]),
            strlen((const char *)(&(Fbuf[0]))));
#endif /* BGL */
        if (TierraLog)
            tfprintf(tfp_log, "ex = %s\n", ExtrG);
    }
    if (virtextr)
        return;

/* open an archive, if it does not exist, create it */

    if(tadatrdy)
    {   GetSmpFilNam(&(GenebankPath[0]),si,
            &(g->gen.label[0]), &(SmplNam[0]));
        tsprintf((char *)(&(Buff[0])),"%s.smp", &(SmplNam[0]));
    }
    else
#ifdef IBM3090
        tsprintf((char *)(&(Buff[0])),
            "%.4d.gen.d", si);
#else
        tsprintf((char *)(&(Buff[0])),
            "%s%.4d.gen", GenebankPath, si);
#endif
    
    if (!(fp = open_ar(&(Buff[0]), si, -1)))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra extract() Unable to open extract file %s", Buff);
        FEError(-401, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    head = read_head(fp);
    indx = read_indx(fp, &head);
    add_gen(fp, &head, &indx, g);
    if (indx)
    {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
        indx = NULL;
    }
    tfclose(fp);
    if(!tadatrdy)
       NumGenDG++;
#ifdef MONKIM
    tsprintf(MDFEStatsBuf.ExtrG_gen, "%.4d%s", g->gen.size, g->gen.label);
    MDFEStatsBuf.ExtrG_pop = g->pop;
#endif /* MONKIM */
}

/*
 * GetSmpFilNam - get sample file name
 *
 * dirnam - pointer gene bank directory name string
 * size   - genotype size
 * label  - pointer to genotype label string
 * nambuf - pointer to sample file name buffer
 */
static void GetSmpFilNam(dirnam, size, label, nambuf)
I8s *dirnam;
I32s size;
I8s *label;
I8s *nambuf;
{   I32s seq, bufpsn, i;
    struct stat statbuf;
    I8s smpnambuf[81];
    static char *extarr[]={"", ".smp", ".tmp", ".gen", ".tie"};

    for(seq=0;;seq++)
    {   tsprintf((char *)nambuf, "%s%.4d%s", dirnam, size, label);
        bufpsn=strlen((const char *)nambuf);
        if(seq)
            tsprintf((char *)(&(nambuf[bufpsn])), "_%d", seq);
        for(i=0;i<(sizeof(extarr)/sizeof(extarr[0]));i++)
        {   tsprintf((char *)(&(smpnambuf[0])),
                "%s%s", nambuf, extarr[i]);
            if(!tstat((char *)(&(smpnambuf[0])), &statbuf))
                break;
        }
        if(i==(sizeof(extarr)/sizeof(extarr[0])))
            break;
    }
}
