/* rambank.c 30-3-98 rambank manager for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #14XX, in use: 1400-1406 */
/* FEError labels: #14XX, in use: 1407-1436, 1438-1453 */

#include "license.h"
#include "tierra.h"
#include "globals.h"
#ifdef TIEAUDIO
#include "tieaudsrv.h"
#endif /* TIEAUDIO */

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

/*
 * CheckGenotype(ce, flags, lsiz_sl, lsl)
 * Check if cell ce is a new genotype.  If it is a new genotype, it will be
 * assigned a unique label.  If this genotype is not present in the RAM
 * genebank, it will be placed there, but there will be no demographic
 * information updated (this is not assumed to be a birth or death).
 * 
 * flags: bit 0  (1): check .gen files
 *        bit 1  (2): check .tmp files
 *        bit 2  (4): check files even if rambank does not indicate presence
 *                    of genotype on disk (used for startup of old or
 *                    new soups).
 *        bit 3  (8): use all files in the genebank to assemble the list in
 *                    the rambank.  Each time a new size is checked, all
 *                    genomes of that size in the genebank will become listed
 *                    in the rambank as being on the disk (used for startup
 *                    of old soups and for cumulative genebanks).  However,
 *                    the genomes will remain on disk until they are actually
 *                    accessed, at which time they will be placed in the
 *                    genebank in RAM.  If soup_in variable CumGeneBnk is
 *                    nonzero, bit 3 is set.
 *        bit 4 (16): when reading a file from the disk genebank, zero bit 1
 *                    of the tgl->bits field, otherwise preserve it.
 */
void CheckGenotype(cd, flags, lsiz_sl, lsl)
    Dem *cd;  /* demography structure of cell to be checked */
    I32s flags;
    I32s *lsiz_sl;
    SList Fp Fp Fp lsl;
{   I32s si;
    I32s gi;

    si = cd->gen.size;
    if (IsNewSize(si, *lsiz_sl, *lsl))
        NewSize(cd, flags, lsiz_sl, lsl);
    gi = IsInGenQueue(cd, cd->hash, *lsl, flags);
    if (gi >= 0)
    {   cd->gi = gi;
        return;
    }
    gi = IsInGenBank(cd, cd->hash, flags, *lsl);
    if (gi >= 0)
    {   cd->gi = gi;
        return;
    }

    /* register new genotype in the lists */
    cd->gi = NewGenotype(cd, cd->hash, *lsl, flags);

}

/*
 * DivGenBook - update g_list elements for mother and daughter on division
 *
 * cp - pointer to mother or daughter cell
 * InstExe - current time in serial instructions
 * reaped  - 1 = the reaper has acted
 * mom     - 1 = this is a mother cell, otherwise it it a daughter
 * same    - 1 = daughter is same genotype as mother
 * disk    - 1 = this creature came from the outside (Inject)
 * mutflag - 1 = called for mutation bookeeping
 */
void DivGenBook(cp, InstExe, reaped, mom, same, disk, mutflag)
    Pcells cp;
    Event InstExe; /* current time */
    I32s reaped;   /* 1 = the reaper has acted */
    I32s mom;      /* 1 = this is a mother cell, otherwise it it a daughter */
    I32s same;     /* 1 = daughter is same genotype as mother */
    I32s disk;     /* 1 = this creature came from the outside (Inject) */
    I32s mutflag;
{   GList *tgl, *tcgl, *otcgl=0;
    float maxp, maxi=0;
    I32s si, gi, gbupdflg, origgi;
    Pcells nc;

    if (mom) /* this code deals only with the mother of the cell being born */
    {   si = cp->d.gen.size;

        gi=cp->d.gi;
        tcgl=sl[si]->g[gi];  /* mother GList */
        if(tcgl&&TNULL(tcgl))
            tcgl=sl[si]->g[gi]=gq_read(si, gi);  /* mother GList */
        if(TNULL(tcgl))
            FEError(-1408, EXIT, WRITE,
                "Tierra DivGenBook() mother genotype missing");

        origgi=cp->d.origgi;
        if(origgi!=gi)
        {   otcgl=sl[si]->g[origgi];
            if(otcgl&&TNULL(otcgl))
                otcgl=sl[si]->g[origgi]=gq_read(si, origgi);
            if(TNULL(tcgl))
                FEError(-1407, EXIT, WRITE,
                    "Tierra DivGenBook() mother genotype missing");
        }

        if((cp->d.fecundity)==1)
        {   gbupdflg=(tcgl->d1.instP)?1:0;
            tcgl->d1=cp->d.d1;
            if(ThrdAnaCollTst()&&(gi!=origgi))
                otcgl->d1=tcgl->d1;
            /* update size class average reproduction efficiency */
            /* daughter 1 */
            if(!(gbupdflg)&& tcgl->d1.mov_daught)
            {   ((sl[si]->AvgRpdEff)[0])=(((sl[si]->AvgRpdEff[0])*
                        ((double)(sl[si]->AvgEffCnt[0])))+
                    (((double)(tcgl->d1.instP))/
                        ((double)(tcgl->d1.mov_daught))))/
                    ((double)((sl[si]->AvgEffCnt[0])+1));
                (sl[si]->AvgEffCnt[0])++;
            }
        }
        else if (cp->d.fecundity==2)
        {   gbupdflg=(tcgl->d2.instP)?1:0;
            (tcgl->d2.inst)=(cp->d.inst)+1-(cp->d.d1.inst);
            (tcgl->d2.instP)=(cp->d.instP)+1-(cp->d.d1.instP);
            (tcgl->d2.flags)=(cp->d.flags)-(cp->d.d1.flags);
            (tcgl->d2.mov_daught)=(cp->d.mov_daught);
            (tcgl->d2.BreedTrue)=same;
            if(ThrdAnaCollTst()&&(gi!=origgi))
                otcgl->d2=tcgl->d2;

            /* update size class average reproduction efficiency */
            /* daughter 2 */
            if(!gbupdflg && tcgl->d2.mov_daught)
            {   ((sl[si]->AvgRpdEff)[1])=(((sl[si]->AvgRpdEff[1])*
                    ((double)(sl[si]->AvgEffCnt[1])))+
                    (((double)(tcgl->d2.instP))/
                    ((double)(tcgl->d2.mov_daught)))) /
                    ((double)((sl[si]->AvgEffCnt[1])+1));
                (sl[si]->AvgEffCnt[1])++;
            }
        }
        if (tcgl->max_cpus<cp->d.max_cpus)
        {   tcgl->max_cpus=cp->d.max_cpus;
            if(ThrdAnaCollTst()&&(gi!=origgi))
                otcgl->max_cpus=tcgl->max_cpus;
        }
        return;
    }
    /* the following code deals only with the daughter cell */
    nc=cp;
    tgl=sl[si=nc->d.gen.size]->g[nc->d.gi];  /* new cell GList */
    if(!tgl->pop)
    {   NumGenotypes++;
        sl[si]->num_g++;

        if (disk)  /* this creature is an injection */
        {   NumGenDG++;
            SetBit(&tgl->bits, 0, 1);
        }
        if (IsBit(tgl->bits, 0))
            SetBit(&tgl->bits, 1, 1);
    }
    (tgl->pop)++;
    if (!sl[si]->num_c)
        NumSizes++;
    if(!mutflag)
    {   sl[si]->num_c++;
#ifdef TIEAUDIO
        SndTieAudDat(si);
#endif /* TIEAUDIO */
    }
#ifdef MICRO
    ChkBrkBd();
#endif /* MICRO */
#if defined(BGL)||(FRONTEND==BASIC)
    if((!mom)&&HistoInEffect())
        query_spec_d(si, nc->d.gi);
#endif /* defined(BGL)||(FRONTEND != STDIO) */
/* this might be a good place to keep track of multiple parental genotypes */
    if (reaped)
    {   maxp = (float) tgl->pop / (float) NumCells;
        if (maxp > tgl->MaxPropPop)
        {   tgl->MaxPropPop = maxp;
            tgl->mpp_time = InstExe;
        }
        maxi = (float) tgl->pop * nc->d.gen.size / (float) SoupSize;
        if (maxi > tgl->MaxPropInst)
            tgl->MaxPropInst = maxi;
        if (tgl->pop > tgl->MaxPop)
            tgl->MaxPop = tgl->pop;
    }

    /* criteria for saving genotype to disk */
    /* note: tgl->bit  0  permanent genotype name, saved in .gen file */
    if (reaped &&
        tgl->pop >= SavMinNum &&
        ((tgl->MaxPropInst > SavThrMem * .5) || (maxi > SavThrMem * .5)))
    {   if (!IsBit(tgl->bits, 0))
        {   SetBit(&tgl->bits, 0, 1);
            SetBit(&tgl->bits, 1, 0);
            extract(nc->d.gen.size, 0, nc->d.gi, 0);
        }
        if (IsBit(tgl->bits, 0) && IsBit(tgl->bits, 1))
        {   SetBit(&tgl->bits, 1, 0);
            extract(nc->d.gen.size, 1, nc->d.gi, 0);
        }
    }
    if(new_soup)
    {   /* calculate reproduction efficiency for daughter 1 */
        if(tgl->d1.mov_daught)
        {   ((sl[si]->AvgRpdEff)[0])=(((sl[si]->AvgRpdEff[0])*
                    ((double)(sl[si]->AvgEffCnt[0])))+
                (((double)(tgl->d1.instP))/
                    ((double)(tgl->d1.mov_daught))))/
                ((double)((sl[si]->AvgEffCnt[0])+1));
            (sl[si]->AvgEffCnt[0])++;
        }

        /* calculate reproduction efficiency for daughter 2 */
        if(tgl->d2.mov_daught)
        {   ((sl[si]->AvgRpdEff)[1])=(((sl[si]->AvgRpdEff[1])*
                ((double)(sl[si]->AvgEffCnt[1])))+
                ((!(tgl->d2.mov_daught))?(0.0):
                (((double)(tgl->d2.instP))/
                ((double)(tgl->d2.mov_daught)))))/
                ((double)((sl[si]->AvgEffCnt[1])+1));
            (sl[si]->AvgEffCnt[1])++;
        }
    }
}

/*
 * ReapGenBook - update g_list elements for cell at death.
 *
 * cp - pointer to cell
 * mutflag - 1 = called for mutation bookeeping
 */
void ReapGenBook(cp, mutflag)
    Pcells cp;
    I32s mutflag;
{   Pgl tgl;
    I32s si = cp->d.gen.size;
    I32s gi = cp->d.gi;
    I32s origgi = cp->d.origgi;

    tgl = sl[si]->g[gi];

#ifdef ERRORTIE
    if (gi >= sl[si]->a_num)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra ReapBookeep() genotype %hd out of range", gi);
        FEError(-1413, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    if(TNULL(tgl))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra ReapBookeep() genotype %hd not in genebank", gi);
        FEError(-1414, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
#endif /* ERRORTIE */
    
    (tgl->pop)--;

    if((gi==origgi)&&(!mutflag))
        (tgl->origpop)--;

    if(!tgl->pop)
    {   if(!tgl->origpop)
        {
            /* update size class average reproduction efficiency */
            /* daughter 1 */
            if (tgl->d1.instP)
            {   if (tgl->d1.mov_daught)
                {   sl[si]->AvgRpdEff[0]=
                        ((sl[si]->AvgEffCnt[0])>1)?
                        ((((sl[si]->AvgRpdEff[0])*
                        ((double)(sl[si]->AvgEffCnt[0]))) -
                        (((double)(tgl->d1.instP))/
                        ((double)(tgl->d1.mov_daught))))/
                        ((double)((sl[si]->AvgEffCnt[0])-1))):0.;
                        sl[si]->AvgEffCnt[0]=
                            ((sl[si]->AvgEffCnt[0])>0)?
                            ((sl[si]->AvgEffCnt[0])-1):0;
                }
            }
            /* daughter 2 */
            if (tgl->d2.instP)
            {   if (tgl->d2.mov_daught)
                {   sl[si]->AvgRpdEff[1]=
                        ((sl[si]->AvgEffCnt[1])>1)?
                        ((((sl[si]->AvgRpdEff[1])*
                        ((double)(sl[si]->AvgEffCnt[1])))-
                        (((double)(tgl->d2.instP))/
                        ((double)(tgl->d2.mov_daught))))/
                        ((double)((sl[si]->AvgEffCnt[1])-1))):0.;
                    sl[si]->AvgEffCnt[1]=
                        ((sl[si]->AvgEffCnt[1])>0)?
                        ((sl[si]->AvgEffCnt[1])-1):0;
                }
            }
            if(!TNULL(tgl))
            {   if (!IsBit(tgl->bits, 0))
                {   sl[si]->g[gi] = NULL;
                    sl[si]->hash[gi] = -1;
                }
                else
                {   extract(cp->d.gen.size,0,cp->d.gi,0);
                    sl[si]->g[gi] = (Pgl) 1;
                }
                FreeGen(tgl);
            }
        }
        NumGenotypes--;
        sl[si]->num_g--;
    }

    if((gi!=origgi)&&(!mutflag))
    {   tgl = sl[si]->g[origgi];
    
#ifdef ERRORTIE
        if (origgi >= sl[si]->a_num)
        {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra ReapBookeep() genotype %hd out of range", origgi);
            FEError(-1409, EXIT, WRITE, (char *)(&(Fbuf[0])));
        }
        if(TNULL(tgl))
        {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra ReapBookeep() genotype %hd not in genebank", origgi);
            FEError(-1410, EXIT, WRITE, (char *)(&(Fbuf[0])));
        }
#endif /* ERRORTIE */

        (tgl->origpop)--;

        if ((!tgl->pop)&&(!tgl->origpop))
        {   
            /* update size class average reproduction efficiency */
            /* daughter 1 */
            if (tgl->d1.instP)
            {   if (tgl->d1.mov_daught)
                {   sl[si]->AvgRpdEff[0]=
                        ((sl[si]->AvgEffCnt[0])>1) ?
                        ((((sl[si]->AvgRpdEff[0])*
                        ((double)(sl[si]->AvgEffCnt[0]))) -
                        (((double)(tgl->d1.instP))/
                        ((double)(tgl->d1.mov_daught))))/
                        ((double)((sl[si]->AvgEffCnt[0])-1))):0.;
                        sl[si]->AvgEffCnt[0]=
                            ((sl[si]->AvgEffCnt[0])>0)?
                            ((sl[si]->AvgEffCnt[0])-1):0;
                }
            }
            /* daughter 2 */
            if (tgl->d2.instP)
            {   if (tgl->d2.mov_daught)
                {   sl[si]->AvgRpdEff[1]=
                        ((sl[si]->AvgEffCnt[1])>1)?
                        ((((sl[si]->AvgRpdEff[1])*
                        ((double)(sl[si]->AvgEffCnt[1])))-
                        (((double)(tgl->d2.instP))/
                        ((double)(tgl->d2.mov_daught))))/
                        ((double)((sl[si]->AvgEffCnt[1])-1))):0.;
                    sl[si]->AvgEffCnt[1]=
                        ((sl[si]->AvgEffCnt[1])>0)?
                        ((sl[si]->AvgEffCnt[1])-1):0;
                }
            }
            if(!TNULL(tgl))
            {   if (!IsBit(tgl->bits, 0))
                {   sl[si]->g[origgi] = NULL;
                    sl[si]->hash[origgi] = -1;
                }
                else
                {   extract(cp->d.gen.size, 0, cp->d.origgi, 0);
                    sl[si]->g[origgi] = (Pgl) 1;
                }
                FreeGen(tgl);
            }
        }
    }
    if(!mutflag)
    {   sl[si]->num_c--;
#ifdef TIEAUDIO
        SndTieAudDat(si);
#endif /* TIEAUDIO */
    }
    if ((!sl[si]->num_c))
        NumSizes--;

#if defined(BGL)||(FRONTEND==BASIC)
    if(HistoInEffect())
        query_spec_d(si, gi);
#endif /* defined(BGL)||(FRONTEND==BASIC) */
}

/*
 * IsNewSize - check for new size class of creature
 *
 * Returns: 0 = size class already exists
 *          1 = new size class
 */
I8s IsNewSize(si, lsiz_sl, lsl)
    I32s si;
    I32s lsiz_sl;
    SList Fp Fp lsl;
{   if (si < lsiz_sl && lsl[si])
        return 0;
    return 1;
}

/*
 * NewSize - update rambank for new creature size class
 *
 * cd - demographic structure of cell
 * flags: bit 0  (1): check .gen files
 *        bit 1  (2): check .tmp files
 *        bit 2  (4): check files even if rambank does not indicate presence
 *                    of genotype on disk (used for startup of old or
 *                    new soups).
 *        bit 3  (8): use all files in the genebank to assemble the list in
 *                    the rambank.  Each time a new size is checked, all
 *                    genomes of that size in the genebank will become listed
 *                    in the rambank as being on the disk (used for startup
 *                    of old soups and for cumulative genebanks).  However,
 *                    the genomes will remain on disk until they are actually
 *                    accessed, at which time they will be placed in the
 *                    genebank in RAM.  If soup_in variable CumGeneBnk is
 *                    nonzero, bit 3 is set.
 *        bit 4 (16): when reading a file from the disk genebank, zero bit 1
 *                    of the tgl->bits field, otherwise preserve it.
 * lsiz_sl - size of size list array
 * lsl - pointer to size list array
 */
void NewSize(cd, flags, lsiz_sl, lsl)
    Dem *cd;
    I32s flags;
    I32s *lsiz_sl;
    SList Fp Fp Fp lsl;
{   static char *ext[] =
    {"gen", "tmp"};
    I32s i, si, exi;
    I32s gi;
    Pgl sgl = NULL;
    FILE *afp;
    head_t head;
    indx_t *indx, *tindx;

    si = cd->gen.size;
    if (si >= *lsiz_sl)
    {   (*lsl) = (SList Fp Fp) trecalloc((void *) (*lsl),
            sizeof(SList Fp) * (si + 1),
            sizeof(SList Fp) * *lsiz_sl, 1903);
        *lsiz_sl = si + 1;

#ifdef ERRORTIE

#if FRONTEND == BASIC
        if(!HistoInEffect())
        {
#endif /* FRONTEND == BASIC */
            tsprintf((char *)(&(Fbuf[0])), 
                "genebank: recalloc, *lsiz_sl = %d\n", (*lsiz_sl - 1) );
            FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
#if FRONTEND == BASIC
        }
#endif /* FRONTEND == BASIC */

#endif /* ERRORTIE */
    }
    (*lsl)[si] = (SList *) tcalloc(1, sizeof(SList), 1400);
    (*lsl)[si]->a_num = 20;
    (*lsl)[si]->g = (Pgl Fp) tcalloc(20, sizeof(Pgl), 1401);
    (*lsl)[si]->hash = (I32s Fp) tcalloc(20, sizeof(I32s), 1405);

    /* read gene definition table */
    ReadGenDef(&((*lsl)[si]->slst_gendef),
        &((*lsl)[si]->genelkup), si, (I8s *)"", &(GenebankPath[0]));

    if (IsBit(flags, 3))  /* use for old soups, and cumulative genebanks */
        for (exi = 0; exi < 2; exi++)
            if (IsBit(flags, exi))
            {   tsprintf((char *)(&(Buff[0])), "%s%.4d.%s",
                    GenebankPath, si, ext[exi]);
                if (!(afp = tfopen(Buff, (I8s *)"rb")))
                    continue;
                head = read_head(afp);
                indx = read_indx(afp, &head);
                for (i = head.n - 1; i >= 0; i--)
                {   tindx = &indx[i];
                    sgl = get_gen(afp, &head, tindx, i);
                    gi = Lbl2Int(sgl->gen.label);
                    if (gi >= (*lsl)[si]->a_num)
                    {   (*lsl)[si]->g = (Pgl Fp)
                            trecalloc((void *) ((*lsl)[si]->g),
                            sizeof(Pgl) * (gi + 1),
                            sizeof(Pgl) * (*lsl)[si]->a_num, 1401);
                        (*lsl)[si]->hash = (I32s Fp)
                            trecalloc((void *) ((*lsl)[si]->hash),
                            sizeof(I32s) * (gi + 1),
                            sizeof(I32s) * (*lsl)[si]->a_num, 1405);
                        (*lsl)[si]->a_num = gi + 1;
                    }
                    (*lsl)[si]->hash[gi] = sgl->hash;
                    if (!exi)
                    {   (*lsl)[si]->g[gi] = (Pgl) 1; /* permanent gen name */
                        FreeGen(sgl);
                    }
                    else
                        (*lsl)[si]->g[gi] = sgl;
                }
                tfclose(afp);
                if (exi)
                    TmplAdd(si, &tmpl);
                if (indx)
                {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
                    indx = NULL;
                }
            }
}

/*
 * TmplAdd - add size to TmpList size array
 *
 * si - size to add
 * tl - pointer to TmpList array
 */
void TmplAdd(si, tl)
    I32s si;
    TmpList *tl;
{   I32s i = 0;

    if (!(tl->n))
    {   tl->l = (I32s *) tcalloc(1, sizeof(I32s), 1406);
        tl->n = 1;
        tl->l[0] = si;
    }
    else
        for (i = 0; i < tl->n; i++)
            if (tl->l[i] == si)
                break;
    if (i == tl->n)
    {   tl->l = (I32s *) trecalloc((void *) tl->l,
            sizeof(I32s) * (tl->n + 1),
            sizeof(I32s) * tl->n, 1406);
        tl->l[tl->n] = si;
        tl->n++;
    }
}

/*
 * TmplRem - remove size from TmpList size array
 *
 * si - size to remove
 * tl - pointer to TmpList array
 */
void TmplRem(si, tl)
    I32s si;
    TmpList *tl;
{   I32s i, j, *p = NULL;

    if (!(tl->n))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra RemTmpl() error: si = %d, no tmp files in tmpl", si);
        FEError(-1415, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    for (i = 0; i < tl->n; i++)
        if (tl->l[i] == si)
            break;
    if (i == tl->n)
        return;
    else
    {   if (tl->n - 1 > 0)
        {   p = (I32s *) tcalloc((tl->n - 1), sizeof(I32s), 1406);
            for (i = 0, j = 0; j < tl->n; i++, j++)
            {   if (tl->l[j] == si)
                {   j++;
                    continue;
                }
                p[i] = tl->l[j];
            }
        }
        tfree(tl->l, tl->n * sizeof(I32s), 1406);
        tl->l = NULL;
        if (p)
            tl->l = p;
        tl->n--;
    }
}

/*
 * IsInGenQueue
 *
 * cd - pointer to demographic "Dem" structure
 * hash - demographic structure hash code
 * lsl - pointer to size list array
 * 
 * flags: bit 0  (1): check .gen files
 *        bit 1  (2): check .tmp files
 *        bit 2  (4): check files even if rambank does not indicate presence
 *                    of genotype on disk (used for startup of old or
 *                    new soups).
 *        bit 3  (8): use all files in the genebank to assemble the list in
 *                    the rambank.  Each time a new size is checked, all
 *                    genomes of that size in the genebank will become listed
 *                    in the rambank as being on the disk (used for startup
 *                    of old soups and for cumulative genebanks).  However,
 *                    the genomes will remain on disk until they are actually
 *                    accessed, at which time they will be placed in the
 *                    genebank in RAM.  If soup_in variable CumGeneBnk is
 *                    nonzero, bit 3 is set.
 *        bit 4 (16): when reading a file from the disk genebank, zero bit 1
 *                    of the tgl->bits field, otherwise preserve it.
 *
 * Returns: returns GList index of the genotype in the rambank
 */
I32s IsInGenQueue(cd, hash, lsl, flags)
    Dem *cd; /* returns the index of the genotype in the list */
    I32s hash;
    SList Fp Fp lsl;
    I32s flags;
{   I32s si = cd->gen.size, lhash, i;
    I32s gi = cd->gi;
    GList *tgl;

    if(((!IsBit(flags, 2))||IsBit(flags, 2))&&(gi>=0))
    {   if((gi<(lsl[si]->a_num))&&(!TNULL(lsl[si]->g[gi])))
            return gi;
        return -1;
    }
    for (i = 0; i < lsl[si]->a_num; i++)
    {   tgl = lsl[si]->g[i];
        lhash = lsl[si]->hash[i];
        if(!TNULL(tgl))
        {   if((cd->mg.s)==(tgl->mg.s)&&
                (cd->mg.p)==(tgl->mg.p)&&
                IsSameGen(cd->mg.s,
                cd->genome+cd->mg.p,
                tgl->genome+cd->mg.p))
            {   if(hash==lhash)
                    return (I32s)i;
#ifdef ERRORTIE
                else
                {   tsprintf((char *)(&(Fbuf[0])),
    "size = %d, index = %hd, label = %s, hash = %d and %d\n",
                        si, i, cd->gen.label, hash, lhash);
                    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
                    FEError(-1417, EXIT, WRITE,
    "Tierra IsInGenQueue() error: IsSameGen, but not same hash");
                }
#endif /* ERRORTIE */
            }
        }
    }
    return -1;
}

/*
 * Check to see if ce is in the disk genebank.  This will require the reading
 * of successive genotypes of this size from the .gen and .tmp files on disk.
 * Each genotype that is read will be placed in the genequeue and the complete
 * genome will be placed in the rambank.
 */
I32s IsInGenBank(cd, hash, flags, lsl)
    Dem *cd;
    I32s hash, flags;
    SList Fp Fp lsl;
{   I32s si = cd->gen.size;
    I32s j, n;
    I32s gi = cd->gi, gih = -1;
    Pgl g;
    FILE *afp;
    head_t head;
    indx_t *indx, *tindx;

    /*
     * return -1 if we are looking for a specific geneotype, and it does not
     * appear in the genequeue list, and we are not starting up a soup
     */
    if(gi>=0)
        if(((gi>=(lsl[si]->a_num))||(!(lsl[si]->g[gi]))))
            return -1;

    if (!IsBit(flags, 2))  /* we are not starting up a soup */
    {   for (j = 0; j < lsl[si]->a_num; j++)
        {   if(!((((I32u)((unsigned long)(lsl[si]->g[j])))>(I32u)0)&&
                (TNULL(lsl[si]->g[j]))))
                continue;
            if((lsl[si]->hash[j])==hash)
            {   gih=(I32s)j;
                break;
            }
        }
        if(gih==(-1))
            return -1;
    }

#ifdef IBM3090
    tsprintf((char *)(&(Buff[0])),
        (char *)"%.4d.gen.d", si);
#else
    tsprintf((char *)(&(Buff[0])),
        (char *)"%s%.4d.gen",GenebankPath, si);
#endif

    if((afp=tfopen(Buff, (I8s *)"rb"))!=0)
    {   head=read_head(afp);
        indx=read_indx(afp, &head);
    }
    else
        return -1;

    if(gi>=0)
    {   if (gi >= lsl[si]->a_num)
        {   lsl[si]->g = (Pgl Fp) trecalloc((void *) (lsl[si]->g),
                sizeof(Pgl) * (gi + 1), sizeof(Pgl) * lsl[si]->a_num, 1401);
            lsl[si]->hash = (I32s Fp) trecalloc((void *) (lsl[si]->hash),
                sizeof(I32s) * (gi + 1),
                sizeof(I32s) * lsl[si]->a_num, 1405);
            lsl[si]->a_num = gi + 1;
        }
        n=find_gen(indx, Int2Lbl(gi), head.n);
        if(n<(head.n))
        {   tindx = &indx[n];
            (lsl[si]->g[gi])=g=get_gen(afp, &head, tindx, n);
            if (IsBit(flags, 4))
                SetBit(&g->bits, 1, 0);

            /* if disk genotype matches soup genotype */
            /* name cell and put genotype in genequeue */
            if((((cd->mg.s)==(g->mg.s))&&
                ((cd->mg.p)==(g->mg.p))&&
                IsSameGen(cd->mg.s, (FpInst)
                ((cd->genome)+(cd->mg.p)),
                (g->genome)+(cd->mg.p)))||
                (!(cd->genome)))
            {   if((hash==(g->hash))||(!(cd->genome)))
                {   cd->gen = g->gen;
                    cd->gi = gi;
                    if (indx)
                    {   thfree((void *)indx, head.n_alloc*
                            sizeof(indx_t), 600);
                        indx = NULL;
                    }
                    tfclose(afp);
                    return gi;
                }
#ifdef ERRORTIE
                else
                    FEError(-1418, EXIT, WRITE,
        "Tierra IsInGenBank() error: IsSameGen, but not same hash");
#endif /* ERRORTIE */
            }
        }
    }
    else        /* we don't know what genotype we are looking for */
        /*
           * check only genotypes that are listed in the rambank as on
           * disk, but whose genomes are not held in the rambank
           * (0 < lsl[si]->g[j] <= 4); or which are not listed in the
           * rambank at all (!lsl[si]->g[j]), if bit 2 is set, which
           * means we are starting a new or old soup and don't have
           * a list of what is on disk. 
         */
        for (j = 0; j < lsl[si]->a_num; j++)
        {   if(!(((((I32u)((unsigned long)(lsl[si]->g[j])))>(I32u)0)&&
                (TNULL(lsl[si]->g[j])))
                ||((!(lsl[si]->g[j]))&&(IsBit(flags, 2)))))
                continue;

            if((lsl[si]->g[j])||(!(IsBit(flags, 2))))
                if(lsl[si]->hash[j]!=hash)
                    continue;

            n = find_gen(indx, Int2Lbl(j), head.n);
            if (n < head.n)
            {   tindx = &indx[n];
                g = get_gen(afp, &head, tindx, n);
#ifdef ERRORTIE
                if(((lsl[si]->g[j])||(!(IsBit(flags, 2))))&&
                    ((g->hash)!=(lsl[si]->hash[j])))
                {   tsprintf((char *)(&(Fbuf[0])),
                        "hash mismatch: g->hash = %d lsl[si]->hash[j] = %d",
                            g->hash, lsl[si]->hash[j]);
                    FEError(-1419, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
#endif /* ERRORTIE */
                lsl[si]->g[j]=g;
                lsl[si]->hash[j]=g->hash;
                if (IsBit(flags, 4))
                    SetBit(&g->bits, 1, 0);
                /* if disk genotype matches soup genotype */
                /* name cell and put genotype in genequeue */
                if (hash == g->hash && cd->mg.s == g->mg.s &&
                    cd->mg.p == g->mg.p &&
                    IsSameGen(cd->mg.s, (FpInst) (cd->genome + cd->mg.p),
                    g->genome + cd->mg.p))
                {   cd->gen = g->gen;
                    cd->gi = (I32s)j;
                    if (indx)
                    {   thfree((void *) indx, head.n_alloc * sizeof(indx_t),
                            600);
                        indx = NULL;
                    }
                    tfclose(afp);
                    return (I32s) j;
                }
            }
        }
    if (afp)
    {   if (indx)
        {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
            indx = NULL;
        }
        tfclose(afp);
    }
    return -1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/*
 * add a new genotype to the RAM list
 */
I32s NewGenotype(cd, hash, lsl, flags)
    Dem *cd;
    I32s hash;
    SList Fp Fp lsl;
    I32s flags;
{   Pgl tgl;
    I32s i, size = cd->gen.size;
#if PLOIDY > 1
    I32s j;
#endif /* PLOIDY > 1 */
    I32s gi=0;
    I8s found = 0;

    if((cd->gi)>=0)
    {       gi=cd->gi;
            found=1;
    }
    else
        for (i = 0; i < lsl[size]->a_num; i++)
            if (!lsl[size]->g[i])
            {   gi = (I32s)i;
                found = 1;
                break;
            }
    if (!found)
    {   gi = lsl[size]->a_num;
        lsl[size]->g = (Pgl Fp) trecalloc((void *) (lsl[size]->g),
            sizeof(Pgl) * (gi + 4), sizeof(Pgl) * gi, 1401);
        lsl[size]->hash = (I32s Fp) trecalloc((void *) (lsl[size]->hash),
            sizeof(I32s) * (gi + 4), sizeof(I32s) * gi, 1405);
        lsl[size]->a_num += 4;
    }
    lsl[size]->hash[gi] = hash;
    lsl[size]->g[gi] = tgl =
        (GList *) tcalloc(1, sizeof(GList), 1402);
    InitThrdAnaDat(&(tgl->glst_thrdanadat));
    tgl->gen.size = cd->gen.size = size;
    AllocGenome(tgl,size);
    AllocGenBits(tgl,size);
    for (i = 0; i < size; i++)
#if PLOIDY == 1
        tgl->genome[i] = cd->genome[i];
#else
        for (j = 0; j < PLOIDY; j++)
            tgl->genome[i][j] = cd->genome[i][j];
#endif
    strcpy((char *)(&(tgl->gen.label[0])), (const char *)Int2Lbl(gi));
    tgl->originC = tietime(NULL);
    tgl->originI = InstExe;
    tgl->pop = 0;
    tgl->parent = cd->parent;
    tgl->mg = cd->mg;
    tgl->hash = hash;
    tgl->bits = 0;
    tgl->max_cpus = 0;
    if (reaped)
    {   tgl->MaxPropPop = (float) 1 / (float) NumCells;
        tgl->MaxPropInst = (float) size / (float) SoupSize;
        tgl->MaxPop = 1;
        tgl->mpp_time = InstExe;
    }
    tgl->ploidy = cd->ploidy;
/*
   tgl->track = cd->tr;
 */
    return (I32s)gi;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
GList *gq_read(si, gi)
{   GList *p = sl[si]->g[gi];
    I32s n;
    FILE *fp;
    head_t head;
    indx_t *indx, *tindx;

    if(!TNULL(p))
        return p;
    tsprintf((char *)(&(Buff[0])),
#ifdef IBM3090
        "%.4d.%s.d",
#else
        "%s%.4d.%s", GenebankPath,
#endif
        si, (((I32u)((unsigned long)p))==(I32u)1)?"gen":"mem");
    if (!(fp = open_ar(Buff, si, 0)))
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra gq_read() unable to open genome file %s", Buff);
        FEError(-1420, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    head = read_head(fp);
    indx = read_indx(fp, &head);
    if ((n = (I32s)find_gen(indx, Int2Lbl(gi), head.n)) == head.n)
    {   tsprintf((char *)(&(Fbuf[0])), "genome not found");
        FEError(-1444, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    tindx = &indx[n];
    p = get_gen(fp, &head, tindx, n);
    tfclose(fp);
    if (indx)
    {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
        indx = NULL;
    }
    return p;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void VerifyGB(zeropopchk, cellgbgenchk)   /* verify genebank */
I32s zeropopchk, cellgbgenchk;
{   I32s gNumSizes = 0, cNumSizes = 0, cgNumSizes = 0;
    I32s gNumGenot = 0, cNumGenot = 0, cgNumGenot = 0;
    I32s gNumCells = 0, cNumCells = 0, cgNumCells = 0;
    I32s cgsNumGenot = 0, ggNumGenot = 0;
    I32s cgsNumCells = 0, ggNumCells = 0;
    I32s tsiz_sl = 1, si, ar, ci, i, j;
    I32s gi, origgi, maxgi;
    Cpu *tcpu;
    Pcells cp;
    GList Fp pgl;
    GList Fp oripgl;
    SList Fp Fp tsl, Fp psl;

    /* begin cells array check */

    tsl = (SList Fp Fp) tcalloc(1, sizeof(SList Fp), 1903);
    for (ar = 0; ar < NumCelAr; ar++)
        for (ci = 0; ci < CelArSiz; ci++)
        {   if (ar == 0 && ci < 2)
                continue;
            cp = &cells[ar][ci];
            if (cp->ld)
            {   cNumCells++;
                si = cp->d.gen.size;
                if (si >= siz_sl)
                {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra VerifyGB() size %d out of range in genebank", si);
                    FEError(-1421, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                psl = sl[si];
                if (!psl)
                {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra VerifyGB() sl[%d] not allocated in genebank", si);
                    FEError(-1422, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                gi = cp->d.gi;
                if (gi >= psl->a_num)
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra VerifyGB() genome %hd out of range in genebank", gi);
                    FEError(-1423, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                origgi = cp->d.origgi;
                if (origgi >= psl->a_num)
                {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra VerifyGB() genome %hd out of range in genebank", origgi);
                    FEError(-1411, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                maxgi=(origgi>gi)?origgi:gi;
                pgl = psl->g[gi];
                if(TNULL(pgl))
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra VerifyGB() gl[%hd] not allocated in genebank", gi);
                    FEError(-1424, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                oripgl = psl->g[origgi];
                if(TNULL(oripgl))
                {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra VerifyGB() gl[%hd] not allocated in genebank", origgi);
                    FEError(-1412, EXIT, WRITE, (char *)(&(Fbuf[0])));
                }
                if(cellgbgenchk)
                {   if (cp->d.mg.s != pgl->mg.s ||
                        cp->d.mg.p != pgl->mg.p ||
                        !IsSameGen(cp->d.mg.s, soup + cp->mm.p + cp->d.mg.p,
                        pgl->genome + cp->d.mg.p))
                        FEError(-1425, EXIT, WRITE,
                        "Tierra VerifyGB() cell and genebank do not match");
                }
                if (si >= tsiz_sl)
                {   tsl = (SList Fp Fp) trecalloc((void *) tsl,
                        (si + 1) * sizeof(SList Fp),
                        tsiz_sl * sizeof(SList Fp), 1903);
                    tsiz_sl = si + 1;
                }
                if (!tsl[si])
                {   tsl[si] = (SList Fp) tcalloc(1, sizeof(SList), 1400);
                    tsl[si]->g = (Pgl Fp) tcalloc(
                        maxgi+1, sizeof(Pgl), 1401);
                    tsl[si]->hash = (I32s Fp) tcalloc(
                        maxgi+1, sizeof(I32s), 1405);
                    tsl[si]->a_num = maxgi+1;
                    InitDynArr((DynArr *)(&(tsl[si]->
                        slst_gendef)), sizeof(GenDef), 0);
                }

                if (!tsl[si]->num_c)
                {   if (tsl[si]->num_g)
                        FEError(-1426, NOEXIT, NOWRITE,
                    "Tierra VerifyGB() !tsl[si]->num_c but tsl[si]->num_g");
                    cNumSizes++;
                }
                tsl[si]->num_c++;
                if (maxgi >= tsl[si]->a_num)
                {   tsl[si]->g = (Pgl Fp) trecalloc((void *) (tsl[si]->g),
                        (maxgi+1) * sizeof(Pgl),
                        tsl[si]->a_num * sizeof(Pgl), 1401);
                    tsl[si]->hash = (I32s Fp)
                        trecalloc((void *) (tsl[si]->hash),
                        (maxgi+1) * sizeof(I32s),
                        tsl[si]->a_num * sizeof(I32s), 1405);
                    tsl[si]->a_num = maxgi + 1;
                }
                if(TNULL(tsl[si]->g[gi]))
                {   tsl[si]->g[gi] = (GList Fp)
                        tcalloc(1, sizeof(GList), 1402);
                    InitThrdAnaDat(&(tsl[si]->
                        g[gi]->glst_thrdanadat));
                }
                if(!(tsl[si]->g[gi]->pop))
                {   cNumGenot++;
                    tsl[si]->num_g++;
                }

                tsl[si]->g[gi]->pop++;

                if(TNULL(tsl[si]->g[origgi]))
                {   tsl[si]->g[origgi] = (GList Fp)
                        tcalloc(1, sizeof(GList), 1402);
                    InitThrdAnaDat(&(tsl[si]->g[origgi]->glst_thrdanadat));
                }
                tsl[si]->g[origgi]->origpop++;

                /* begin check for bad cell struct data */

                if (ce->d.mov_daught)
                {   if (ce->d.MovOffMin < 0)
                    {   tsprintf((char *)(&(Fbuf[0])),
                            "Tierra movid() error: ce->d.MovOffMin = %d",
                            ce->d.MovOffMin);
                        FEError(-1448, EXIT, WRITE, (char *)(&(Fbuf[0])));
                    }
                    if (ce->d.MovOffMin >= ce->md.s)
                    {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMin = %d  ce->md.s = %d",
                            ce->d.MovOffMin, ce->md.s);
                        FEError(-1449, EXIT, WRITE, (char *)(&(Fbuf[0])));
                    }
                    if (ce->d.MovOffMax < 0)
                    {   tsprintf((char *)(&(Fbuf[0])),
                            "Tierra movid() error: ce->d.MovOffMax = %d",
                            ce->d.MovOffMax);
                        FEError(-1450, EXIT, WRITE, (char *)(&(Fbuf[0])));
                    }
                    if (ce->d.MovOffMax >= ce->md.s)
                    {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra movid() error: ce->d.MovOffMax = %d  ce->md.s = %d",
                            ce->d.MovOffMax, ce->md.s);
                        FEError(-1451, EXIT, WRITE, (char *)(&(Fbuf[0])));
                    }
                }

                /* end   check for bad cell struct data */

                /* begin check for corruption of CPU memory */

                if (ce->c.ac >= ce->c.n || ce->c.ac < 0)
                {   tsprintf((char *)(&(Fbuf[0])),
"Tierra VerifyGB() active cpu out of range: ce->c.ac = %d, ce->c.n = %d",
                        ce->c.ac, ce->c.n);
                    FEError(-1447, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                }
                for (i = 0; i < ce->c.n; i++)
                {   tcpu = &(ce->c.ar[i]);
                    for (j = 0; j < NUMREG; j++)
                    {   if (ShadowUsed)
                        {   if (tcpu->re[NUMREG + j] >= NUMREG ||
                                tcpu->re[NUMREG + j] < 0)
                            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra VerifyGB() shadow reg %d out of range: %d",
                                    j, tcpu->re[NUMREG + j]);
                                FEError(-1416, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                            }
                        }
#ifdef SHADOW
                        else if (tcpu->re[NUMREG + j] != j)
                        {   tsprintf((char *)(&(Fbuf[0])),
                            "Tierra VerifyGB() shadow reg %d corrupted: %d",
                                j, tcpu->re[NUMREG + j]);
                            FEError(-1445, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                        }
#endif /* SHADOW */
                    }
                    if (tcpu->sp >= STACK_SIZE || tcpu->sp < 0)
                    {   tsprintf((char *)(&(Fbuf[0])),
                            "Tierra VerifyGB() stack pointer corrupted: %d",
                            tcpu->sp);
                        FEError(-1446, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                    }
                }

                /*   end check for corruption of CPU memory */
            }
        }

    /* check and free temporary genebank */

    for (si = 0; si < tsiz_sl; si++)
    {   if (tsl[si])
        {   if (tsl[si]->num_c != sl[si]->num_c)
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra VerifyGB() tsl[%d]->num_c != sl[%d]->num_c",
                    si, si);
                FEError(-1427, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
            if (tsl[si]->num_g != sl[si]->num_g)
            {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra VerifyGB() tsl[%d]->num_g = %d != sl[%d]->num_g = %d",
                si, tsl[si]->num_g, si, sl[si]->num_g);
                FEError(-1428, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
            if (tsl[si]->num_c && tsl[si]->g)
            {   cgNumSizes++;
                cgsNumCells += tsl[si]->num_c;
                cgsNumGenot += tsl[si]->num_g;
                for (gi = 0; gi < tsl[si]->a_num; gi++)
                {   if(!TNULL(tsl[si]->g[gi]))
                    {   if (tsl[si]->g[gi]->pop != sl[si]->g[gi]->pop)
                        {   tsprintf((char *)(&(Fbuf[0])),
"Tierra VerifyGB() tsl[%d]->g[%hd]->pop = %d != sl[%d]->g[%hd]->pop = %d",
                                si, gi, tsl[si]->g[gi]->pop, si, gi,
                                sl[si]->g[gi]->pop);
                            FEError(-1429, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                        }
                        if (tsl[si]->g[gi]->origpop != sl[si]->g[gi]->origpop)
                        {   tsprintf((char *)(&(Fbuf[0])),
"Tierra VerifyGB() tsl[%d]->g[%hd]->origpop = %d != sl[%d]->g[%hd]->origpop = %d",
                                si, gi, tsl[si]->g[gi]->origpop, si, gi,
                                sl[si]->g[gi]->origpop);
                            FEError(-1453, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
                        }
                        if(tsl[si]->g[gi]->pop)
                            cgNumGenot++;
                        cgNumCells += tsl[si]->g[gi]->pop;
                        tfree((void *) (tsl[si]->g[gi]), sizeof(GList), 1402);
                    }
                }
                tfree((void *) (tsl[si]->g), tsl[si]->a_num * sizeof(Pgl),
                    1401);
                tfree((void *) (tsl[si]->hash), tsl[si]->a_num * sizeof(I32s),
                    1405);
                FreeGenDef(&(tsl[si]->slst_gendef), &(tsl[si]->genelkup), si);
                tfree((void *) tsl[si], sizeof(SList), 1400);
            }
        }
    }

    tfree((void *) tsl, tsiz_sl * sizeof(SList Fp), 1903);
    if (NumCells != cNumCells || NumCells != cgNumCells ||
        NumCells != cgsNumCells)
        FEError(-1430, NOEXIT, NOWRITE,
            "Tierra VerifyGB() NumCells cells array inconsistency");
    if (NumGenotypes != cNumGenot || NumGenotypes != cgNumGenot ||
        NumGenotypes != cgsNumGenot)
        FEError(-1431, NOEXIT, NOWRITE,
            "Tierra VerifyGB() NumGenot cells array inconsistency");
    if (NumSizes != cNumSizes || NumSizes != cgNumSizes)
    {   tsprintf((char *)(&(Fbuf[0])),
        "Tierra VerifyGB() NumSizes cells array inconsistency = %d, %d, %d",
            NumSizes, cNumSizes, cgNumSizes);
        FEError(-1432, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }

    /* end cells array check */

    /* begin genebank check */

    for (si = 0; si < siz_sl; si++)
    {   psl = sl[si];
        if (!psl)
            continue;
        if ((!psl->num_c || !psl->num_g)&&(zeropopchk))
            FEError(-1433, NOEXIT, NOWRITE,
                "Tierra VerifyGB() !sl[si]->num_c or !sl[si]->num_g");
        if (sl[si]->num_c)
        {   gNumSizes++;
            ggNumCells += sl[si]->num_c;
        }
        if (sl[si]->num_g)
            ggNumGenot += sl[si]->num_g;
        for (gi = 0; gi < sl[si]->a_num; gi++)
        {   pgl = psl->g[gi];
            if((TNULL(pgl))||(!pgl->pop))
                continue;
            if (pgl->hash != psl->hash[gi])
            {   tsprintf((char *)(&(Fbuf[0])),
    "hash values should match: pgl->hash = %d psl->hash[%hd] = %d",
                    pgl->hash, gi, psl->hash[gi]);
                FEError(-1452, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
            }
            gNumGenot++;
            gNumCells += pgl->pop;
        }
    }
    if (NumCells != gNumCells || NumCells != ggNumCells)
        FEError(-1434, NOEXIT, NOWRITE,
            "Tierra VerifyGB() NumCells genebank inconsistency");
    if (NumGenotypes != gNumGenot || NumGenotypes != ggNumGenot)
        FEError(-1435, NOEXIT, NOWRITE,
            "Tierra VerifyGB() NumGenot genebank inconsistency");
    if (NumSizes != gNumSizes)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra VerifyGB() NumSizes genebank inconsistency = %d, %d",
            NumSizes, gNumSizes);
        FEError(-1436, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    }
    /* end genebank check */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void GBGarbageCollect()
{   I32s i, j, maxsiz = 0, tail;
    GList Fp pgl;
    I8s path[80];
    FILE *fp;
    head_t head;
    indx_t *indx;

    for (i = siz_sl - 1; i >= 0; i--)  /* for each allocated size class */
    {   if (sl[i])
        {   if (sl[i]->num_c)
            {   if (!maxsiz)  /* find largest size class */
                    maxsiz = i;
                tail = -1;
                for (j = sl[i]->a_num - 1; j >= 0; j--)
                {   if((!TNULL(pgl=sl[i]->g[j])) &&
                        !pgl->pop && !pgl->origpop
                        && !IsBit(pgl->bits, 0))
                    {   if (pgl->genome)
                        {   FreeGenome(pgl);
                            pgl->genome = NULL;
                        }
                        if (pgl->gbits)
                        {   FreeGenBits(pgl);
                            pgl->gbits = NULL;
                        }
                        FreeThrdAnaDat(&(pgl->glst_thrdanadat));

                        tfree((void *) (sl[i]->g[j]), sizeof(GList), 1402);
                        sl[i]->g[j] = NULL;
                    }
                    if (tail < 0 && sl[i]->g[j])
                        tail = j;  /* skip empty geotypes at end of array */
                }
                if (tail < sl[i]->a_num - 1)
                {   if (tail < 0)  /* no genotypes in size class */
                    {   if (sl[i]->g)
                        {   tfree((void *) (sl[i]->g),
                                sl[i]->a_num * sizeof(Pgl), 1401);
                            sl[i]->g = NULL;
                        }
                        if (sl[i]->hash)
                        {   tfree((void *) (sl[i]->hash),
                                sl[i]->a_num * sizeof(I32s), 1405);
                            sl[i]->hash = NULL;
                        }
                        if (sl[i])
                        {   FreeGenDef(&(sl[i]->slst_gendef),
                                &(sl[i]->genelkup), i);
                            tfree((void *) sl[i], sizeof(SList), 1400);
                            sl[i] = NULL;
                        }
                    }
                    else
                        /* shorten g arrays to avoid empty tails */
                    {   sl[i]->g = (Pgl Fp) trecalloc((void *) (sl[i]->g),
                            (tail + 1) * sizeof(Pgl),
                            sl[i]->a_num * sizeof(Pgl), 1401);
                        sl[i]->hash = (I32s Fp)
                            trecalloc((void *) (sl[i]->hash),
                            (tail + 1) * sizeof(I32s),
                            sl[i]->a_num * sizeof(I32s), 1405);
                        sl[i]->a_num = tail + 1;
                    }
                }
            }
            else
                /* no creatures of this size, free sl[i] and sl[i]->g */
            {   tsprintf((char *)(&(path[0])),
                    "%s%.4d.gen", GenebankPath, i);
                fp = open_ar(path, i, -1);
                head = read_head(fp);
                indx = read_indx(fp, &head);
                for (j = sl[i]->a_num - 1; j >= 0; j--)
                    if(!TNULL(pgl=sl[i]->g[j]))
                    {   if ((!(pgl->pop))&&(!(pgl->origpop)))
                        {   if (IsBit(pgl->bits, 0))
                                add_gen(fp, &head, &indx, pgl);
                            FreeGen(pgl);
                            sl[i]->g[j] = NULL;
                        }
                    }
                tfclose(fp);
                if (!head.n)
                    tunlink(path);
                if (indx)
                {   thfree((void *) indx, head.n_alloc * sizeof(indx_t), 600);
                    indx = NULL;
                }
                if (sl[i]->g)
                {   tfree((void *) (sl[i]->g), sl[i]->a_num * sizeof(Pgl),
                        1401);
                    sl[i]->g = NULL;
                }
                if (sl[i]->hash)
                {   tfree((void *) (sl[i]->hash), sl[i]->a_num * sizeof(I32s),
                        1405);
                    sl[i]->hash = NULL;
                }
                if (sl[i])
                {   FreeGenDef(&(sl[i]->slst_gendef),
                        &(sl[i]->genelkup), i);
                    tfree((void *) sl[i], sizeof(SList), 1400);
                    sl[i] = NULL;
                }
            }
        }
    }
    if (maxsiz < (I32s) (.5 * (siz_sl - 1)))
    {   sl = (SList Fp Fp) trecalloc((void *) sl,
            (maxsiz + 1) * sizeof(SList Fp),
            siz_sl * sizeof(SList Fp), 1903);
        siz_sl = maxsiz + 1;
    }           /* end garbage collect for genebank */
}

/*
 * GenExTemp - collect data for the current instruction template
 *             and its complement
 *
 * adrt - address of template complement
 * tsize - template size
 * tmpcall - 1 = called for "call" with template
 * tmpcmplflg  - 1 = called for template complement
 *
 * Returns:
 */
void GenExTemp(adrt, tsize, tmpcall, tmpcmplflg)
    I32s adrt, tsize, tmpcall,tmpcmplflg;
{   I32s i;
    I32u who;   /* 0 same cell; 1 daughter cell; 2 other cell; */
    /* 3 daughter of other cell; 4 free memory */
    I32s dist, thrdanacolflg, watchflg, hoffset, tmplbytadr;
    Pgl tgl, ogl, anatgl;
    Pcells ct;

    tgl = sl[ce->d.gen.size]->g[ce->d.gi];
    anatgl=sl[ce->d.gen.size]->g[ce->d.origgi];
    thrdanacolflg=ThrdAnaCollTst();
    watchflg=((!((ce->d.flaw)||(ce->d.nonslfmut)))&&(WatchTem))?1:0;
    if(watchflg||thrdanacolflg)
    {   for (i = 0; i < tsize; i++)
        {   if((!tmpcall)||((tmpcall)&&(!tmpcmplflg)))
            {   /* who has templ pattern */
                switch(who=WhoIs(&ct, ad((ce->c.c->ip)+1+i), &hoffset))
                {   case 0:
                    case 1:
                    case 2:
                        if(watchflg||thrdanacolflg)
                            (tgl->bits)|=(I32UONE<<((I32u)(12+who)));
                        ThrdAnaTdtBits(thrdanacolflg, (I32u)(12+who));
                        break;
                    case 3:
                    default:
                        if(watchflg||thrdanacolflg)
                            (tgl->bits)|=(I32UONE<<((I32u)(11+who)));
                        ThrdAnaTdtBits(thrdanacolflg, (I32u)(11+who));
                        break;
                }
    
                switch(who)
                {   case 0:
                        tmplbytadr=ad((ce->c.c->ip)+1+i);
                        dist=tmplbytadr-(ce->mm.p);
                        dist=ad(dist);
                        if(watchflg||thrdanacolflg)
                        {   
#ifdef ERRORTIE
                            if((!(tgl->genome))||(dist<0)||
                                (dist>=(tgl->gen.size)))
                                FEError(-1438, EXIT, WRITE,
                                    "Tierra GenExTemp() error 0");
    
#endif /* ERRORTIE */
#if PLOIDY == 1
                            ((tgl->gbits)[dist])|=1;
#else /* PLOIDY == 1 */
                            ((tgl->gbits)[dist][is.oextrk])|=1;
#endif /* PLOIDY == 1 */
    
                            if(thrdanacolflg)
                            {   AllocThrdAnaCod(&(ce->
                                    cell_thrdanadat), tmplbytadr);
                                ThrdAnaGenExUsed(thrdanacolflg, tmplbytadr);
                                ThrdAnaDatColl(0,0,tmplbytadr);
                            }
                        }
                        break;
                    case 2:
                        if(watchflg||thrdanacolflg)
                        {   ogl=sl[ct->d.gen.size]->g[ct->d.gi];
                            (ogl->bits)|=(I32UONE<<((I32u)(12+4)));
                            dist=ad((ce->c.c->ip)+1+i)-(ct->mm.p);
                            dist=ad(dist);
#ifdef ERRORTIE
                            if((!(ogl->genome))||(dist<0)||
                                (dist>=(ogl->gen.size)))
                                FEError(-1439, EXIT, WRITE,
                                    "Tierra GenExTemp() error 1");
#endif /* ERRORTIE */
#if PLOIDY == 1
                            ((ogl->gbits)[dist])|=(1<<1);
#else /* PLOIDY == 1 */
                            ((ogl->gbits)[dist][is.oextrk])|=(1<<1);
#endif /* PLOIDY == 1 */
                        }
                        break;
                    default:
                        break;
                }
            }

            if((!tmpcall)||((tmpcall)&&(tmpcmplflg)&&(tsize)))
            {   /* who has complementary template */
                switch(who=WhoIs(&ct, ad(adrt+i), &hoffset))
                {   case 0:
                    case 1:
                    case 2:
                        if(watchflg||thrdanacolflg)
                            tgl->bits|=(I32UONE<<((I32u)(7+who)));
                        ThrdAnaTdtBits(thrdanacolflg, (I32u)(7+who));
                        break;
                    case 3:
                    default:
                        if(watchflg||thrdanacolflg)
                            (tgl->bits)|=(I32UONE<<((I32u)(6+who)));
                        ThrdAnaTdtBits(thrdanacolflg, (I32u)(6+who));
                        break;
                }
    
                switch(who)
                {   case 0:
                        tmplbytadr=ad(adrt+i);
                        dist=tmplbytadr-(ce->mm.p);
                        dist=ad(dist);
                        if(watchflg||thrdanacolflg)
                        {   
#ifdef ERRORTIE
                            if((!(tgl->genome))||(dist<0)||
                                (dist>=(tgl->gen.size)))
                                FEError(-1440, EXIT, WRITE,
                                    "Tierra GenExTemp() error 2");
#endif /* ERRORTIE */
#if PLOIDY == 1
                            ((tgl->gbits)[dist])|=1;
#else /* PLOIDY == 1 */
                            ((tgl->gbits)[dist][ce->c.c->de])|=1;
#endif /* PLOIDY == 1 */
                            if(thrdanacolflg)
                            {   AllocThrdAnaCod(&(ce->
                                    cell_thrdanadat), tmplbytadr);
                                ThrdAnaDatColl(0,0,tmplbytadr);
                                ThrdAnaGenExUsed(thrdanacolflg, tmplbytadr);
                            }
                        }
                        break;
                    case 2:
                        if(watchflg||thrdanacolflg)
                        {   dist=ad(adrt+i)-(ct->mm.p);
                            dist=ad(dist);
                            ogl=sl[ct->d.gen.size]->g[ct->d.gi];
                            (ogl->bits)|=(I32UONE<<((I32u)(7+4)));
#ifdef ERRORTIE
                            if((!(ogl->genome))||(dist<0)||
                                (dist>=(ogl->gen.size)))
                                FEError(-1441, EXIT, WRITE,
                                    "Tierra GenExTemp() error 3");
#endif /* ERRORTIE */
#if PLOIDY == 1
                            ((ogl->gbits)[dist])|=(1<<1);
#else /* PLOIDY == 1 */
                            ((ogl->gbits)[dist][ce->c.c->de])|=(1<<1);
#endif /* PLOIDY == 1 */
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void GenExMov(to, from, srccpysegofst, dstcpysegofst)
    I32s to, from, srccpysegofst, dstcpysegofst;
{   Pcells ct;
    Pgl tgl, ogl;
    I32u thrdanacolflg, watchflg, dist;
    I32u who;     /* 0 same cell; 1 daughter cell; 2 other cell; */
    /* 3 daughter of other cell; 4 free memory */
    I32s hoffset;

    tgl=sl[ce->d.gen.size]->g[ce->d.gi];
    thrdanacolflg=ThrdAnaCollTst();
    watchflg=((!((ce->d.flaw)||(ce->d.nonslfmut)))&&(WatchMov))?1:0;
    if((watchflg)||(thrdanacolflg))
    {
        /* the mov instruction being executed is within your own genome */
        if(((ce->mm.p)<=(ce->c.c->ip))&& 
            ((ce->c.c->ip)<((ce->mm.p)+(ce->mm.s))))
        {   dist=from-(ce->mm.p);
            /* who is it moved from */
            switch(who=WhoIs(&ct, from, &hoffset))
            {   case 0:
                case 1:
                case 2:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(17+who)));
                    ThrdAnaGenExMov(thrdanacolflg,
                        srccpysegofst, from, to, who);
                    break;
                case 3:
                default:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(16+who)));
                    ThrdAnaGenExMov(thrdanacolflg,
                        srccpysegofst, from, to, (I32u)(16+who));
                    break;
            }
            if(who==2)
                if(watchflg||thrdanacolflg)
                {   ogl=sl[ct->d.gen.size]->g[ct->d.gi];
                    ogl->bits|=(I32UONE<<((I32u)(17+4)));
                }
            switch(who=WhoIs(&ct, to, &hoffset)) /* who is it moved to */
            {   case 0:
                case 1:
                case 2:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(22+who)));
                    if(thrdanacolflg)
                        ce->cell_thrdanadat.mtad_thdatara.
                            mtda_thdatara[is.othreadid]->
                            mtdt_basthrdat.tdt_bits|=
                            (I32UONE<<((I32u)(22+who)));
                    break;
                case 3:
                default:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(21+who)));
                    if(thrdanacolflg)
                        ce->cell_thrdanadat.mtad_thdatara.
                            mtda_thdatara[is.othreadid]->
                            mtdt_basthrdat.tdt_bits|=
                            (I32UONE<<((I32u)(21+who)));
                    break;
            }
            if(who==2)
                if(watchflg||thrdanacolflg)
                {   ogl = sl[ct->d.gen.size]->g[ct->d.gi];
                    ogl->bits|=(I32UONE<<((I32u)(22+4)));
                }
        }
        else /* these are moved from while executing instructions that */
             /* are not your own */
        {   dist=from-(ce->mm.p);
            /* who is it moved from */
            switch(who=WhoIs(&ct, from, &hoffset))
            {   case 0:
                case 1:
                case 2:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(27+who)));
                    break;
                case 3:
                default:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(26+who)));
                    break;
            }
            /* ct is cell from which inst is moved */
            if(who==2)
            {   if(watchflg||thrdanacolflg)
                {   ogl=sl[ct->d.gen.size]->g[ct->d.gi];
                    ogl->bits|=(I32UONE<<((I32u)(27+4)));
                }
            }
        }
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void GenExExe(adrt)
    I32s adrt;
{   Pcells ct;
    Pgl tgl, ogl, anatgl;
    I32s watchflg, dist, thrdanacolflg, hoffset;
    I32u who;  /* 0 same cell; 1 daughter cell; 2 other cell; */
    /* 3 daughter of other cell; 4 free memory */

    if((GeneBnker&&WatchExe)||ThreadAnalysis)
    {   tgl = sl[ce->d.gen.size]->g[ce->d.gi];
        anatgl=sl[ce->d.gen.size]->g[ce->d.origgi];
        thrdanacolflg=ThrdAnaCollTst();
        watchflg=((!((ce->d.flaw)||(ce->d.nonslfmut)))&&(WatchExe))?1:0;
        if((watchflg)||(thrdanacolflg))
        {   switch(who=WhoIs(&ct, adrt, &hoffset))
            {   case 0 :
                case 1 :
                case 2 :
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(2+who)));
                    ThrdAnaTdtBits(thrdanacolflg, (I32u)(2+who));
                    break;
                case 3 :
                default:
                    if(watchflg||thrdanacolflg)
                        tgl->bits|=(I32UONE<<((I32u)(1+who)));
                    ThrdAnaTdtBits(thrdanacolflg, (I32u)(1+who));
                    break;
            }
            switch(who)
            {   case 0:
                    if(watchflg||thrdanacolflg)
                    {   
#ifdef ERRORTIE
                        if (!(tgl->gbits))
                            FEError(-1442, EXIT, WRITE,
                            "Tierra GenExExe() error 0");
#endif /* ERRORTIE */
    
                        if(((is.cipofst)>=(ce->d.mg.p))&&
                            ((is.cipofst)<((ce->d.mg.p)+(ce->d.mg.s))))
                        {
#if PLOIDY == 1
                            tgl->gbits[is.cipofst] |= 1;
#else /* PLOIDY == 1 */
                            tgl->gbits[is.cipofst][is.oextrk] |= 1;
#endif /* PLOIDY == 1 */
                        }
                    }
                    ThrdAnaGenExUsed(thrdanacolflg, is.oip);
                    break;
                case 2: /* is other cell */
                    if(watchflg||thrdanacolflg)
                    {   dist=adrt-(ct->mm.p);
                        ogl=sl[ct->d.gen.size]->g[ct->d.gi];
                        (ogl->bits)|=(I32UONE<<((I32u)(2+4)));
#ifdef ERRORTIE
                        if ((!(ogl->gbits))||(dist<0)||
                            (dist>=(ogl->gen.size)))
                            FEError(-1443, EXIT, WRITE,
                                "Tierra GenExExe() error 1");
#endif /* ERRORTIE */
#if PLOIDY == 1
                        ((ogl->gbits)[dist])|=(1<<1);
#else /* PLOIDY == 1 */
                        ((ogl->gbits)[dist][ce->c.c->ex])|=(1<<1);
#endif /* PLOIDY == 1 */
                    }
                    break;
                default:
                    break;
            }
        }
    }
#ifdef EXEINSTSIZTIMSLC
    MarkCellGenBitExe(ce, is.oip);
#endif /* EXEINSTSIZTIMSLC */
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void StartGeneBanker()
{   if (!GeneBnkUsage)
    {   GeneBnker=1;
        ChangeGeneBnker(GeneBnker);
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void StopGeneBanker()
{   if (!GeneBnkUsage)
    {   GeneBnker=0;
        ChangeGeneBnker(GeneBnker);
    }
    else
        GeneBnker=1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void OpenGeneBanker()
{   StartGeneBanker();
    GeneBnkUsage++;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void CloseGeneBanker()
{   I32s MinGeneBnkUsage;

    MinGeneBnkUsage=(GeneBnkerOvrd)?1:0;
    GeneBnkUsage=(GeneBnkUsage>MinGeneBnkUsage)?
        (GeneBnkUsage-1):MinGeneBnkUsage;
    StopGeneBanker();
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
/* rationale for the functioning of the genebank:

   The term ``rambank'' refers to a collection of genotypes maintained in RAM
   The term ``diskbank'' refers to a collection of genotypes maintained on disk
   The term ``genebank'' refers to both the rambank and the diskbank

   Genotype names have two parts: size-label, for example 0080aaa, 0045eat,
   6666god.

   1) When a creature is born its genotype will be compared to that of its
      parent.
   A) if they are the same, the daughter will be given the same name as the
   mother.
   B) if they are not the same, the genebank will be searched.
   a) if the daughter genotype is found in the genebank, it will be given
   the same name as the genotype that it matches in the bank.
   b) if the daughter genotype does not match any genotype in the bank,
   a new name will be created for it, and it will be entered into the
   rambank.
   2) For each birth and death a count of the population of both the genotype
   and the size class involved will be incremented or decremented, so that we
   have a count of the current population of each genotype and each size class.
   This information is maintained in rambank.
   3) If a genotype frequency crosses a critical threshold, the genotype name
   will become permanent and the genotype will be saved to the diskbank.
   There may be several types of thresholds: proportion of population
   (e.g., 2%), proportion of soup, or just numbers of creatures.
   4) When a genotype frequency drops to zero:
   A) If the genotype never crossed the thresholds, the genotype will be
   removed from the genebank, and its unique name will become available for
   reuse.
   B) If the genotype crossed the threshold, gaining a permanent name, it
   should be retained in the genebank.
   5) Periodically, Tierra saves the complete state of the machine (e.g., every
   100 million instructions executed).  At that time, the complete rambank
   is saved to the diskbank.  For this reason, 4 A applies also to genotypes
   which never became permanent, and these must be removed from the diskbank
   as well.  The bitfield in the genotype structure tells us if a genotype is
   saved to the diskbank, and if it is permanent.
   6) If the rambank becomes ``too full'', some relatively unused portion of it
   should be swapped to the diskbank.  In DOS, ``too full'' could be signaled
   by a malloc failure.  In unix, ``too full'' could be signaled by some
   specified limit on how big the rambank should get, if this seems wise.
   That portion of the rambank to be swapped to the diskbank might consist of
   the least recently accessed size class.  For this reason it might be
   worthwhile to maintain a queue of size classes, ordered by last use.
 */
