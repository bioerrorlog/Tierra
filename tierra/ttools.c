/* @(#)ttools.c    1.1 11/15/94 */
/* ttools.c 30-3-98 histogram routines for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Dan Pirone, Tom Ray & Virtual Life, originally written by Dan Pirone
 * MemProf labels: #21XX, in use: 2100-2101
 * FEError labels: #21XX, in use: 2102-2110 2113
 */

#include "tierra.h"
#ifdef __GUI__
#define WEXTERN_H
#endif /* __GUI__ */
#include "globals.h"

#ifdef BGL
#include "tbgl.h"
#endif /* BGL */

static int bg_compare P_((const void *i, const void *j));
static int hs_compare P_((const void *i, const void *j));
static int hg_compare P_((const void *i, const void *j));

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/* routines for query_species */
/*-----------------------------------------------------------------------*/

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static int hg_compare(i, j)
const void *i, *j;
{   return((HistSortOrder==FREQ_GEN)?(((IMode==GEN_EFF)?
        ((((HistType *)j)->dblcount)==(((HistType *)i)->
        dblcount)):((((HistType *)j)->count)==((
        (HistType *)i)->count)))?(((((HistType *)i)->
        size)==(((HistType *)j)->size))?((((
        (HistType *)i)->lbl)<(((HistType *)j)->lbl))?
        (-1):1):(((((HistType *)i)->size)<((
        (HistType *)j)->size))?(-1):1)):(
        (IMode==GEN_EFF)?(((((HistType *)i)->dblcount)<
        (((HistType *)j)->dblcount))?(-1):1):((
        (((HistType *)j)->count)<(((HistType *)i)->count))?
        (-1):1))):(((((HistType *)i)->size)==((
        (HistType *)j)->size))?(((((HistType *)i)->lbl)<
        (((HistType *)j)->lbl))?(-1):1):((((
        (HistType *)i)->size)<(((HistType *)j)->size))?(-1):1)));
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static int hs_compare(i, j)
const void *i, *j;
{   return((HistSortOrder==FREQ_SIZE)?(((IMode==SIZ_EFF)?
        ((((HistType *)i)->dblcount)==(((HistType *)j)->
        dblcount)):((((HistType *)i)->count)==(((
        HistType *)j)->count)))?(((((HistType *)i)->
        size)<(((HistType *)j)->size))?(-1):1):((IMode==
        SIZ_EFF)?(((((HistType *)i)->dblcount)<(((
        HistType *)j)->dblcount))?(-1):1):(((((
        HistType *)j)->count)<(((HistType *)i)->count))?
        (-1):1))):(((((HistType *)i)->size)==(((
        HistType *)j)->size))?((IMode==SIZ_EFF)?(((((
        HistType *)i)->dblcount)<(((HistType *)j)->
        dblcount))?(-1):1):(((((HistType *)j)->count)<
        (((HistType *)i)->count))?(-1):1)):(((((
        HistType *)i)->size)<(((HistType *)j)->size))?(-1):1)));
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
static int bg_compare(i, j)
const void *i, *j;
{   return ((((size_buf *)i)->lbl)<(((size_buf *)j)->lbl))?(-1):1;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
size_buf *InitSizeQuery(size_class, num_spec)
    I32s size_class;
    int num_spec;
{   size_buf *buf;
    int qbufidx, genidx;

    buf = (size_buf *) thcalloc(num_spec, sizeof(size_buf), 2100);
    qbufidx = 0;
    for (genidx = 0; genidx < sl[size_class]->a_num; genidx++)
    {   if (sl[size_class]->g[genidx])
        {   if (sl[size_class]->g[genidx]->pop)
            {   buf[qbufidx].lbl = genidx;
                buf[qbufidx].count =
                    sl[size_class]->g[genidx]->pop;
                buf[qbufidx].moves =
                    sl[size_class]->g[genidx]->d1.mov_daught;
                buf[qbufidx].flags =
                    sl[size_class]->g[genidx]->d1.flags;
                buf[qbufidx].bits = sl[size_class]->g[genidx]->bits;
                qbufidx++;
            }
        }
    }

    /* sort by genome */
    qsort(buf, qbufidx, sizeof(size_buf), bg_compare);

    return buf;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void DispSizeQuery(buf, num_spec, size_class)
    size_buf *buf;
    int num_spec;
    I32s size_class;
{   int querypgidx, loopflag, displinecnt;
    char cc;

    displinecnt=LINES-PLN_Y-3;
    querypgidx = 0;
    loopflag = 1;
    while (loopflag)
    {

        /* display query page */
        DispQueryPage(&querypgidx, buf, displinecnt, num_spec, size_class);

        /* menu bar - page up, page down, quit */
        tsprintf((char *)(&(Fbuf[0])), "U/D-page up/down  q-quit->");
        FEPrintf(HLP_X, HLP_Y, 1, 0, (char *)(&(Fbuf[0])));
        cc = (I8s)FEGetch(HLP_X+strlen((
            const char *)(&(Fbuf[0]))), HLP_Y);

        switch (cc)
        {   case 'U':
            querypgidx += displinecnt;
            break;
        case 'D':
            querypgidx -= displinecnt;
            break;
        case 'q':
            loopflag = 0;
            break;
        default:
            break;
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
void DispQueryPage(querypgidx, buf, displinecnt, num_spec, size_class)
    int *querypgidx;
    size_buf *buf;
    int displinecnt;
    int num_spec;
    I32s size_class;
{   int qbufidx, displine, maxquerypgidx, scrnline;

    maxquerypgidx = (displinecnt > num_spec) ? 0 : num_spec - displinecnt;
    (*querypgidx) = ((*querypgidx) < 0) ?
        0 : (((*querypgidx) > maxquerypgidx) ?
        maxquerypgidx : (*querypgidx));

    /* clear display area */
#if FRONTEND == BASIC
    FEClrmsg(PLN_Y, "");
    FEBanner();
    FEStats();
#endif /* FRONTEND == BASIC */

    /* display query heading */
    tsprintf((char *)(&(Fbuf[0])), "Gene:    #  Mem Errs   Move  Bits\n");
    FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
    if (TierraLog && HistPrint)
        tfprintf(tfp_log, (char *)(&(Fbuf[0])));

    /* display query lines */
    displine = 0;
    for(;;)
    {   qbufidx = (*querypgidx) + displine;
        scrnline = (PLN_Y + displine) + 1;
        if (qbufidx < num_spec)
        {   if (displine < displinecnt)
            {   WritEcoB(buf[qbufidx].bits, Buff);
                tsprintf((char *)(&(Fbuf[0])),
                    "%-3s %6d  %3d  %3d    %3d  %s\n",
                    Int2Lbl(buf[qbufidx].lbl), buf[qbufidx].count,
                    (100*buf[qbufidx].count*size_class)/SoupSize,
                    buf[qbufidx].flags, buf[qbufidx].moves, Buff);
                FEPrintf(PLN_X, scrnline, 0, 0, (char *)(&(Fbuf[0])));
                if (TierraLog && HistPrint)
                    tfprintf(tfp_log, (char *)(&(Fbuf[0])));
                displine++;
            }
            else
                break;
        }
        else
            break;
    }
    if (TierraLog && HistPrint)
    {
#ifndef AMIGA
#ifndef DECVAX
        tfflush(tfp_log);
#endif /* DECVAX */
#endif /* AMIGA */
    }
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void query_species(num)
    I32s num;
{   I32s num_ent = 0, nnn = 0, ci, t, n_star = 0;
    I32s maxhistostrt;
    double reprodeff=0.0;

#ifdef __GUI__
    char *ttbuff[80];
#endif /* __GUI__ */
    int i;

    if ((NumSizes < 1) || (num < 2))
        return;
    if (Hist)
    {   thfree((void *) Hist, HistAlocSiz, 2101);
        Hist = (HistType *) NULL;
        HistAlocSiz = 0;
    }
    Max_hits = -1;
    if ((IMode == GEN_HIST) || (IMode == GEN_EFF))
        num_ent = NumGenotypes;
    else
        num_ent = NumSizes;
    Hist = (HistType *) thcalloc((num_ent + 1), sizeof(HistType), 2101);
    HistAlocSiz = (num_ent + 1) * sizeof(HistType);
#ifdef BGL
    if (FD_ISSET(BGL_MASK_INFO_S, &BglMask))
    {   if ( BglSizHistBuf )
        {   thfree( (void*)BglSizHistBuf, BglSizHistAllocSiz, 12202 );
            BglSizHistBuf = (BglSizHistSt*)NULL;
            thfree( (void*)BglTblSizHist, BglTblSizHistAllocSiz, 12202 );
        }
        BglSizHistBuf = (BglSizHistSt*)thcalloc(
            num_ent + 1, sizeof(BglSizHistSt), 12323 );
        BglSizHistAllocSiz = (num_ent + 1) * sizeof(BglSizHistSt);
        BglTblSizHist = (I8s*)thcalloc( num_ent + 1, sizeof(I8s), 12324 );
        BglTblSizHistAllocSiz = (num_ent + 1) * sizeof(I32s);
    }
    else if (FD_ISSET(BGL_MASK_INFO_G, &BglMask))
    {   if ( BglGenHistBuf )
        {   thfree( (void*)BglGenHistBuf, BglGenHistAllocSiz, 12204 );
            BglGenHistBuf = (BglGenHistSt*)NULL;
            thfree( (void*)BglTblGenHist, BglTblGenHistAllocSiz, 12205 );
        }
        BglGenHistBuf=(BglGenHistSt*)thcalloc(
            num_ent + 1, sizeof(BglGenHistSt), 12325 );
        BglGenHistAllocSiz = (num_ent + 1) * sizeof(BglGenHistSt);
        BglTblGenHist = (I8s*)thcalloc( num_ent + 1, sizeof(I8s), 12326 );
        BglTblGenHistAllocSiz = (num_ent + 1) * sizeof(I32s);
    }
    else if (FD_ISSET(BGL_MASK_INFO_M, &BglMask))
    {   if ( BglMemHistBuf )
        {   thfree( (void*)BglMemHistBuf, BglMemHistAllocSiz, 12206 );
            BglMemHistBuf = (BglMemHistSt*)NULL;
            thfree( (void*)BglTblMemHist, BglTblMemHistAllocSiz, 12207 );
        }
        BglMemHistBuf=(BglMemHistSt*)thcalloc(
            num_ent + 1, sizeof(BglMemHistSt), 12326 );
        BglMemHistAllocSiz = (num_ent + 1) * sizeof(BglMemHistSt);
        BglTblMemHist = (I8s*)thcalloc( num_ent + 1, sizeof(I8s), 12327 );
        BglTblMemHistAllocSiz = (num_ent + 1) * sizeof(I32s);
    }
    else if (FD_ISSET(BGL_MASK_INFO_ES, &BglMask))
    {   if ( BglEfficSHistBuf )
        {   thfree( (void*)BglEfficSHistBuf, BglEfficSHistAllocSiz, 12202 );
            BglEfficSHistBuf = (BglEfficSHistSt*)NULL;
            thfree( (void*)BglTblEfficSHist, BglTblEfficSHistAllocSiz, 12202 );
        }
        BglEfficSHistBuf = (BglEfficSHistSt*)thcalloc(num_ent + 1,
            sizeof(BglEfficSHistSt), 12323);
        BglEfficSHistAllocSiz = (num_ent + 1) * sizeof(BglEfficSHistSt);
        BglTblEfficSHist = (I8s*)thcalloc(num_ent + 1, sizeof(I8s), 12324);
        BglTblEfficSHistAllocSiz = (num_ent + 1) * sizeof(I32s);
    }
    else if (FD_ISSET(BGL_MASK_INFO_EG, &BglMask))
    {   if ( BglEfficGHistBuf )
        {   thfree( (void*)BglEfficGHistBuf, BglEfficGHistAllocSiz, 12202 );
            BglEfficGHistBuf = (BglEfficGHistSt*)NULL;
            thfree( (void*)BglTblEfficGHist,
                BglTblEfficGHistAllocSiz, 12202 );
        }
        BglEfficGHistBuf = (BglEfficGHistSt*)thcalloc(num_ent + 1,
            sizeof(BglEfficGHistSt), 12323);
        BglEfficGHistAllocSiz = (num_ent + 1) * sizeof(BglEfficGHistSt);
        BglTblEfficGHist = (I8s*) thcalloc(num_ent + 1, sizeof(I8s), 12324);
        BglTblEfficGHistAllocSiz = (num_ent + 1) * sizeof(I32s);
    }
#endif /* BGL */
    for (nnn = 0, ci = 1; (ci < siz_sl && nnn < num_ent); ci++)
    {   if (!TNULL(sl[ci]) && sl[ci]->num_c)
        {   if ((IMode == GEN_HIST) || (IMode == GEN_EFF))
            {   for (t = 0; t < sl[ci]->a_num; t++)
                {   if (IMode == GEN_EFF)
                    {   if (TNULL(sl[ci]->g[t]))
                            continue;
                        if (!EffDaught)
                        {   reprodeff = (sl[ci]->g[t]->d1.mov_daught) ?
                                (((double) (sl[ci]->g[t]->d1.instP)) /
                                ((double) (sl[ci]->g[t]->
                                        d1.mov_daught))) : 0.0;
                        }
                        else
                        {   reprodeff = (sl[ci]->g[t]->d2.mov_daught) ?
                                (((double) (sl[ci]->g[t]->d2.instP)) /
                                ((double) (sl[ci]->g[t]->
                                        d2.mov_daught))) : 0.0;
                        }
                        if (reprodeff == 0.0)
                            continue;
                    }
                    else
                    {   if ((TNULL(sl[ci]->g[t])) || (sl[ci]->g[t]->pop < 1))
                            continue;
                    }
                    Hist[nnn].size = (I32u)ci;
                    Hist[nnn].lbl = (I32s)t;  /* int form of label */
                    if (IMode == GEN_EFF)
                    {   Hist[nnn].count = (I32s) reprodeff;
                        Hist[nnn].dblcount = reprodeff;
                    }
                    else
                        Hist[nnn].count = sl[ci]->g[t]->pop;
                    if (Hist[nnn].count >= Max_hits)
                        Max_hits = Hist[nnn].count;
                    nnn++;
                }
            }
            else
            {   if (IMode == SIZ_EFF)
                {   reprodeff = sl[ci]->AvgRpdEff[EffDaught];
                    if (reprodeff == 0.0)
                        continue;
                }
                Hist[nnn].size = (I32u)ci;
                if (IMode == SIZ_EFF)
                {   Hist[nnn].count = (I32s) reprodeff;
                    Hist[nnn].dblcount = reprodeff;
                }
                else
                    Hist[nnn].count = sl[ci]->num_c;
                if (Hist[nnn].count >= Max_hits)
                    Max_hits = Hist[nnn].count;
                nnn++;
            }
        }       /* end of valid size class */
    }
    if ((Max_hits) < 1)
        return;
    histoentcnt = nnn;
    nnn = ((nnn > num_ent) ? num_ent : nnn);
    nnn = ((nnn > num) ? num : nnn);
    nnn = ((nnn > fe_lines - PLN_Y) ? fe_lines - PLN_Y : nnn);
    histodsplnct = nnn;

    maxhistostrt = histoentcnt - histodsplnct;
    histostrtidx = (histostrtidx < 0) ?
        0 : ((histostrtidx > maxhistostrt) ?
        maxhistostrt : histostrtidx);

    if (IMode == SIZM_HIST)
#ifdef BGL
        for (t = 0; t < histoentcnt; t++)
#else /* BGL */
        for (t = histostrtidx; t < histostrtidx + histodsplnct; t++)
#endif /* BGL */
        {   Hist[t].count *= Hist[t].size;
            if (Hist[t].count >= Max_hits)
                Max_hits = Hist[t].count;
        }
    if ((IMode == GEN_HIST) || (IMode == GEN_EFF))  /* sort top num, by size */
        qsort(Hist, histoentcnt, sizeof(HistType), hg_compare);
    else        /* sort top num, by size */
        qsort(Hist, histoentcnt, sizeof(HistType), hs_compare);

#ifdef BGL
    if (FD_ISSET(BGL_MASK_INFO_S, &BglMask))
    {   BglHistClearBuf.flg = BGL_UNREAD;
        BglTblHistClear = BGL_UNREAD;
        for ( Bgl_nnn = 0; Bgl_nnn < histoentcnt; Bgl_nnn++ )
        {   BglSizHistBuf[Bgl_nnn].index = Bgl_nnn;
            BglSizHistBuf[Bgl_nnn].size = Hist[Bgl_nnn].size;
            BglSizHistBuf[Bgl_nnn].num = sl[Hist[Bgl_nnn].size]->num_g;
            BglSizHistBuf[Bgl_nnn].count = Hist[Bgl_nnn].count;
            BglTblSizHist[Bgl_nnn] = BGL_UNREAD;
        }
    }
    else if (FD_ISSET(BGL_MASK_INFO_G, &BglMask))
    {   BglHistClearBuf.flg = BGL_UNREAD;
        BglTblHistClear = BGL_UNREAD;
        for ( Bgl_nnn = 0; Bgl_nnn < histoentcnt; Bgl_nnn++ )
        {   BglGenHistBuf[Bgl_nnn].index = Bgl_nnn;
            BglGenHistBuf[Bgl_nnn].size = Hist[Bgl_nnn].size;
            strncpy((char *)(&(BglGenHistBuf[Bgl_nnn].lbl[0])),
                (const char *)Int2Lbl(Hist[Bgl_nnn].lbl), 3);
            BglGenHistBuf[Bgl_nnn].count = Hist[Bgl_nnn].count;
            BglTblGenHist[Bgl_nnn] = BGL_UNREAD;
        }
    }
    else if (FD_ISSET(BGL_MASK_INFO_M, &BglMask))
    {   BglHistClearBuf.flg = BGL_UNREAD;
        BglTblHistClear = BGL_UNREAD;
        for ( Bgl_nnn = 0; Bgl_nnn < histoentcnt; Bgl_nnn++ )
        {   BglMemHistBuf[Bgl_nnn].index = Bgl_nnn;
            BglMemHistBuf[Bgl_nnn].size = Hist[Bgl_nnn].size;
            BglMemHistBuf[Bgl_nnn].num = sl[Hist[Bgl_nnn].size]->num_g;
            BglMemHistBuf[Bgl_nnn].count = Hist[Bgl_nnn].count;
            BglTblMemHist[Bgl_nnn] = BGL_UNREAD;
        }
    }
    else if (FD_ISSET(BGL_MASK_INFO_ES, &BglMask))
    {   BglHistClearBuf.flg = BGL_UNREAD;
        BglTblHistClear = BGL_UNREAD;
        for ( Bgl_nnn = 0; Bgl_nnn < histoentcnt; Bgl_nnn++ )
        {   BglEfficSHistBuf[Bgl_nnn].index = Bgl_nnn;
            BglEfficSHistBuf[Bgl_nnn].size = Hist[Bgl_nnn].size;
            BglEfficSHistBuf[Bgl_nnn].num = sl[Hist[Bgl_nnn].size]->num_g;
            BglEfficSHistBuf[Bgl_nnn].dblcount = Hist[Bgl_nnn].dblcount;
            BglTblEfficSHist[Bgl_nnn] = BGL_UNREAD;
        }
    }
    else if (FD_ISSET(BGL_MASK_INFO_EG, &BglMask))
    {   BglHistClearBuf.flg = BGL_UNREAD;
        BglTblHistClear = BGL_UNREAD;
        for ( Bgl_nnn = 0; Bgl_nnn < histoentcnt; Bgl_nnn++ )
        {   BglEfficGHistBuf[Bgl_nnn].index = Bgl_nnn;
            BglEfficGHistBuf[Bgl_nnn].size = Hist[Bgl_nnn].size;
            strncpy((char *)(&(BglEfficGHistBuf[Bgl_nnn].lbl[0])),
                (const char *)Int2Lbl(Hist[Bgl_nnn].lbl), 3);
            BglEfficGHistBuf[Bgl_nnn].dblcount = Hist[Bgl_nnn].dblcount;
            BglTblEfficGHist[Bgl_nnn] = BGL_UNREAD;
        }
    }
#endif /* BGL */

#ifndef BGL
    Hist = (HistType *) threcalloc((void *) Hist,
        (histostrtidx + histodsplnct + 1) * sizeof(HistType),
        (num_ent + 1) * sizeof(HistType), 2101);
    HistAlocSiz = (histostrtidx + histodsplnct + 1) * sizeof(HistType);
#endif /* BGL */
    HistSize = nnn;
    HistNStars = ((float) (fe_width - 20) / (float) Max_hits);
#ifndef __GUI__
    FEClrmsg(PLN_Y,
        (FRONTEND==STDIO)?((I8s *)"") :
        (((INTERFACE == BASICB) || (INTERFACE == BASICM) ||
        (INTERFACE == BASICW)) ?(I8s *)
"                       Press Any Key for Menu ...                           "
:(I8s *)
"                    Press Interrupt Key for Menu ...                        "
        ));
#else /* __GUI__ */
    FEClrmsg(PLN_Y, "");
#endif /* __GUI__ */

    for (t = histostrtidx, i = 0; t < histostrtidx + histodsplnct; t++, i++)
    {   if (Hist[t].count < 1)
            continue;
#ifdef __GUI__
        strcpy(ttbuff, "*");
#else /* __GUI__ */
        Buff[0] = '*';
        Buff[1] = '\0';
#endif /* __GUI__ */
        n_star = (I32s)(HistNStars * Hist[t].count);
              Hist[t].nstar = (I8u) n_star;
        if ((IMode == GEN_HIST) || (IMode == GEN_EFF))
        {   if ((t > 0) && (Hist[t].size == Hist[t - 1].size))
            {   if (IMode == GEN_HIST)
                {   tsprintf((char *)(&(Fbuf[0])), "      %3s %6d | ",
                        Int2Lbl(Hist[t].lbl), Hist[t].count);
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])), "      %3s %6.3f | ",
                        Int2Lbl(Hist[t].lbl), Hist[t].dblcount);
                }
#ifndef __GUI__
                FEPrintf(PLN_X, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
                if (TierraLog && HistPrint)
                    tfprintf(tfp_log, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
            }
            else
            {   if (IMode == GEN_HIST)
                {   tsprintf((char *)(&(Fbuf[0])), " %5hd%3s %6d | ",
                        Hist[t].size, Int2Lbl(Hist[t].lbl),
                        Hist[t].count);
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])), " %5hd%3s %6.3f | ",
                        Hist[t].size, Int2Lbl(Hist[t].lbl),
                        Hist[t].dblcount);
                }
#ifndef __GUI__
                FEPrintf(PLN_X, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
                if (TierraLog && HistPrint)
                    tfprintf(tfp_log, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
            }
        }
        else
        {   if ((IMode == SIZ_HIST) || (IMode == SIZM_HIST))
            {   tsprintf((char *)(&(Fbuf[0])), "%5hd %3hd %6d | ",
                    Hist[t].size, sl[Hist[t].size]->num_g,
                    Hist[t].count);
            }
            else
            {   tsprintf((char *)(&(Fbuf[0])), "%5hd %3hd %6.3f | ",
                    Hist[t].size, sl[Hist[t].size]->num_g,
                    Hist[t].dblcount);
            }
#ifndef __GUI__
            FEPrintf(PLN_X, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
            if (TierraLog && HistPrint)
                tfprintf(tfp_log, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
        }

#ifdef __GUI__
        for(ci=0;ci<n_star;ci++)
            strcat(ttbuff, "*");
        strcat((char *)(&(Fbuf[0])), ttbuff);
        strcat((char *)(&(Fbuf[0])), "\n");
        FEPrintf(PLN_X, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
#else /* __GUI__ */
        for(ci=0;ci<n_star;ci++)
            strcat((char *)(&(Buff[0])),"*");
        tsprintf((char *)(&(Fbuf[0])), "%s\n", Buff);
        FEPrintf((PLN_X + 19), PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
        if (TierraLog && HistPrint)
            tfprintf(tfp_log, (char *)(&(Fbuf[0])));
    }
    if (TierraLog && HistPrint)
    {   tfprintf(tfp_log, "\n");
#ifndef AMIGA
#ifndef DECVAX
        tfflush(tfp_log);
#endif /* DECVAX */
#endif /* AMIGA */
    }
}               /* end of query_species */

/*-----------------------------------------------------------------------*/

#if FRONTEND == BASIC || BGL

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void query_spec_d(size, lbl)
    I32s size, lbl;
{   I32s l, n, tcount, maxhistostrt, pinflag;
#ifndef BGL
    I32s i;
#else /* BGL */
    I32s matchFlag=0;
#endif /* BGL */
    double dbltcount=0.0;
    I32s empty = 0;
    I8s old_n_star, ns_dlta;
    int maxbarlen;

    maxbarlen = fe_width - 20;
    pinflag = 0;
    if ((size > (siz_sl - 1)) || (size < 0))
        return;
    if ((Hist == NULL) || (!histodsplnct))
    {   query_species(fe_lines);
        return;
    }

    maxhistostrt = histoentcnt - histodsplnct;
    histostrtidx = (histostrtidx < 0) ?
        0 : ((histostrtidx > maxhistostrt) ?
        maxhistostrt : histostrtidx);

    Buff[0] = '*';
    Buff[1] = '\0';
#ifdef BGL
    for (n = 0; n < histoentcnt; n++)
#else /* BGL */
    for (n = histostrtidx, i = 0; n < histostrtidx + histodsplnct; n++, i++)
#endif /* BGL */
    {   if((((I32s)(Hist[n].size))>=0)&&(((I32s)(Hist[n].size))<siz_sl))
        {   if (TNULL(sl[Hist[n].size]))
            {   empty++;
                continue;
            }
        }
        else
        {   empty++;
            continue;
        }
        if ((IMode == GEN_HIST) || (IMode == GEN_EFF))
        {   if(((Hist[n].lbl)>=0)&&
                ((Hist[n].lbl)<(sl[Hist[n].size]->a_num)))
            {   if (TNULL(sl[Hist[n].size]->g[Hist[n].lbl]))
                {   empty++;
                    tcount = 0;
                    dbltcount = 0.0;
                }
                else
                {   if (IMode == GEN_EFF)
                    {   if (!EffDaught)
                        {   dbltcount = (sl[Hist[n].size]->
                                g[Hist[n].lbl]->d1.mov_daught) ?
                                (((double) (sl[Hist[n].size]->
                                        g[Hist[n].lbl]->d1.instP)) /
                                ((double) (sl[Hist[n].size]->
                                        g[Hist[n].lbl]->d1.mov_daught))) : 0.0;
                        }
                        else
                        {   dbltcount = (sl[Hist[n].size]->
                                g[Hist[n].lbl]->d2.mov_daught) ?
                                (((double) (sl[Hist[n].size]->
                                        g[Hist[n].lbl]->d2.instP)) /
                                ((double) (sl[Hist[n].size]->
                                        g[Hist[n].lbl]->d2.mov_daught))) : 0.0;
                        }
                        tcount =  (I32s)dbltcount;
                    }
                    else
                    {   tcount = sl[Hist[n].size]->g[Hist[n].lbl]->pop;
                    }
                }
            }
            else
            {   empty++;
                tcount = 0;
                dbltcount = 0.0;
            }
        }
        else if (IMode == SIZM_HIST)
        {   tcount = size * sl[Hist[n].size]->num_c;
        }
        else
        {   if (IMode == SIZ_EFF)
            {   dbltcount = sl[Hist[n].size]->AvgRpdEff[EffDaught];
                tcount =  (I32s)dbltcount;
            }
            else
            {   tcount = sl[Hist[n].size]->num_c;
            }
        }
        if(tcount<1)
        {   if((++empty)>3)
            {   query_species(fe_lines);
                return;
            }
        }
        if (Hist[n].size == (I32u)size)
        {   if (((IMode == GEN_HIST) || (IMode == GEN_EFF)) &&
                (Hist[n].lbl != lbl))
                continue;

#ifdef BGL
            matchFlag = 1;
#endif /* BGL */
            old_n_star = Hist[n].nstar;
            l=(I32s)(HistNStars*tcount);
            if (l > maxbarlen)
            {   Hist[n].nstar = maxbarlen;
                pinflag = 1;
            }
            else
            {   Hist[n].nstar = (I8u)l;
            }
            ns_dlta = Hist[n].nstar - old_n_star;
            if (ns_dlta < 0)
                for(l=0;l<((-1)*ns_dlta);l++,strcat(
                    (char *)(&(Buff[0])),(const char *)" "));
            if ((IMode == GEN_HIST) || (IMode == GEN_EFF))
            {   if (IMode == GEN_HIST)
                {   tsprintf((char *)(&(Fbuf[0])), "%6d", tcount);
#ifdef BGL
                    if (FD_ISSET(BGL_MASK_INFO_G, &BglMask))
                    {   BglGenHistBuf[n].count = tcount;
                        BglTblGenHist[n] = BGL_UNREAD;
                    }
#endif /* BGL */
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])), "%6.3f", dbltcount);
#ifdef BGL
                    if (FD_ISSET(BGL_MASK_INFO_EG, &BglMask))
                    {   BglEfficGHistBuf[n].dblcount = dbltcount;
                        BglTblEfficGHist[n] = BGL_UNREAD;
                    }
#endif /* BGL */
                }
#ifndef BGL
#ifdef __GUI__
                l = strlen((const char *)(&(Fbuf[0])));
                while (l--)
                    *((FEw + PLN_Y)->szDesc + PLN_X + 10 + l)
                        = *(Fbuf + l);
                FEPrintf(PLN_X, PLN_Y, 0, 0, (FEw + PLN_Y)->szDesc);
#else /* __GUI__ */
                FEPrintf(PLN_X + 10, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
#endif /* BGL */
            }
            else
            {   if ((IMode == SIZ_HIST) || (IMode == SIZM_HIST))
                {   tsprintf((char *)(&(Fbuf[0])), "%3hd %6d",
                        sl[Hist[n].size]->num_g, tcount);
#ifdef BGL
                    if ( FD_ISSET(BGL_MASK_INFO_S, &BglMask) )
                    {   BglSizHistBuf[n].count = tcount;
                        BglTblSizHist[n] = BGL_UNREAD;
                    }
                    else if (FD_ISSET(BGL_MASK_INFO_M, &BglMask) )
                    {   BglMemHistBuf[n].count = tcount;
                        BglTblMemHist[n] = BGL_UNREAD;
                    }
#endif /* BGL */
                }
                else
                {   tsprintf((char *)(&(Fbuf[0])), "%3hd %6.3f",
                        sl[Hist[n].size]->num_g, dbltcount);
#ifdef BGL
                    if (FD_ISSET(BGL_MASK_INFO_ES, &BglMask))
                    {   BglEfficSHistBuf[n].dblcount = dbltcount;
                        BglTblEfficSHist[n] = BGL_UNREAD;
                    }
#endif /* BGL */
                }
#ifndef BGL
#ifdef __GUI__
                l = strlen((const char *)(&(Fbuf[0])));
                while (l--)
                    *((FEw + PLN_Y)->szDesc + PLN_X + 6 + l)
                        = *(Fbuf + l);
                FEPrintf(PLN_X, PLN_Y, 0, 0, (FEw + PLN_Y)->szDesc);
#else /* __GUI__ */
                FEPrintf(PLN_X + 6, PLN_Y + i, 0, 0, (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
#endif /* BGL */
            }
            if (Hist[n].nstar && !ns_dlta && !pinflag)
                break;
            if (ns_dlta >= 0)
            {   for(l=0;l<ns_dlta;l++,strcat((char *)
                    (&(Buff[0])),(const char *)"*"));
                if (pinflag)
                    Buff[(int)ns_dlta] = '+';
                ns_dlta = 0;
            }
#ifndef BGL
#ifdef __GUI__
            l = strlen(Buff);
            while (l--)
                *((FEw + PLN_Y)->szDesc + PLN_X + 19 + old_n_star + ns_dlta)
                    = *(Buff + l);
            tsprintf((char *)(&(Fbuf[0])), "%s\n", (FEw + PLN_Y)->szDesc);
            FEPrintf(PLN_X, PLN_Y, 0, 0, (char *)(&(Fbuf[0])));
#else /* __GUI__ */
            tsprintf((char *)(&(Fbuf[0])), "%s\n", Buff);
            FEPrintf((PLN_X + 19 + old_n_star + ns_dlta), PLN_Y + i, 0, 0,
                (char *)(&(Fbuf[0])));
#endif /* __GUI__ */
#endif /* BGL */
        }
    }
#ifdef BGL
    if ( !matchFlag )   query_species(fe_lines);
#endif /* BGL */
}

#endif /* FRONTEND == BASIC || BGL */

#ifndef THINK_C
#ifdef SIGBLOCK

/*-----------------------------------------------------------------------*/
#ifdef unix
/*-----------------------------------------------------------------------*/
/* THIS CODE IS NOT FINISHED  */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */

#ifdef __TURBOC__
int T_sig_read(void)
#endif /* __TURBOC__ */

#ifdef __MWERKS__
void T_sig_read(sig)
I32s sig;
#endif /* __MWERKS__ */

#ifdef unix
void T_sig_read(sig)
int sig;
#endif /* unix */

#ifdef DECVAX
void T_sig_read(int sig)
#endif /* DECVAX */

{   FILE *inf;
    I8s data[85];
    sigset_t SigSaveSet;

    FEError(-2102, NOEXIT, NOWRITE,
        "==========================================================");
    tsprintf((char *)(&(Fbuf[0])),
        "TIERRA: re-read soup_in (%s) ...", soup_fn);
    FEError(-2103, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
    inf = tfopen(soup_fn, (I8s *)"r");
    if (inf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra T_sig_read() GetSoupIn: file %s not opened, exiting",
            soup_fn);
        FEError(-2104, EXIT, WRITE, (char *)(&(Fbuf[0])));
    }
    tfgets(data, 84, inf);

    SigSaveSet = TSigBlock(SigSet);
    sscanf((const char *)data, (const char *)"tierra core:");
    TSigRelease(SigSaveSet);

    tfgets(data, 84, inf);  /* blank line */
    tfgets(data, 84, inf);
    while (strlen((const char *)data) > 3)
    {   if (!GetAVar(data, 2, 0, sizeof(data)))
        {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra T_sig_read() bad soup_in line: %s", data);
            FEError(-2105, NOEXIT, NOWRITE, (char *)(&(Fbuf[0])));
        }
        tfgets(data, 84, inf);
    }

    FEError(-2106, NOEXIT, NOWRITE,
        "==========================================================");
    ToggleLog(TierraLog, &tfp_log, (I8s *)TIERRALOGNAME, &tielogpos);
#ifdef NET
    ToggleLog(PingDatLog, &ping_fp, (I8s *)TPINGLOGNAME, &tpinglogpos);
#endif /* NET */
}
#endif /* unix */

/*-----------------------------------------------------------------------*/
#ifdef unix
/*-----------------------------------------------------------------------*/
/* THIS CODE IS NOT FINISHED  */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */

#ifdef __TURBOC__
int T_sig_write(void)
#endif /* __TURBOC__ */

#ifdef __MWERKS__
void T_sig_write(sig)
I32s sig;
#endif /* __MWERKS__ */

#ifdef unix
void T_sig_write(sig)
int sig;
#endif /* unix */

#ifdef DECVAX
void T_sig_write(int sig)
#endif /* DECVAX */

{   FEError(-2107, NOEXIT, NOWRITE,
        "==========================================================");
    FEError(-2108, NOEXIT, WRITE, "TIERRA: writing soup ...");
    FEError(-2109, NOEXIT, NOWRITE,
        "==========================================================");
}
#endif /* unix */

/*-----------------------------------------------------------------------*/
#ifdef unix
/*-----------------------------------------------------------------------*/
/* THIS CODE IS NOT FINISHED  */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */

#ifdef __TURBOC__
int T_sig_info(void)
#endif /* __TURBOC__ */

#ifdef __MWERKS__
void T_sig_info(sig)
I32s sig;
#endif /* __MWERKS__ */

#ifdef unix
void T_sig_info(sig)
int sig;
#endif /* unix */

#ifdef DECVAX
void T_sig_info(int sig)
#endif /* DECVAX */


{   I32s t_hist_print = HistPrint;

    if (!HistPrint)
        HistPrint = 1L;
    tsprintf((char *)(&(Fbuf[0])),
        "InstExe.m    = %6d  InstExec.i = %6d  NumCells = %4d\n",
        InstExe.m, InstExe.i, NumCells);
    FEPrintf(MSG_X, MSG_Y, 0, 0, (char *)(&(Fbuf[0])));
    if (GeneBnker)
    {   tsprintf((char *)(&(Fbuf[0])),
            "NumGenotypes =   %4d  NumSizes   =   %4d\n",
            NumGenotypes, NumSizes);
        FEPrintf(MSG_X, MSG_Y + 1, 0, 0, (char *)(&(Fbuf[0])));
    }
    query_species(20);
    HistPrint = t_hist_print;
}
#endif /* unix */
#endif /* SIGBLOCK */
#endif /* THINK_C */

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void FESoupImage(fp, x, y, n)
    FILE *fp;
    I32s x, y, n;
{   I32s addr, fsz, sz;
    Pcells cp;
    I8s tmd;
    float cpb;

    if(!fp&&((fp=tfopen((I8s *)"soup.img", (I8s *)"w")) == NULL))
        FEError(-2113, NOEXIT, NOWRITE,
            "FESoupImage - can't open file soup.img\n");
    addr = 0;
    cpb = (float) (x * y) / (float) SoupSize;  /* char per byte */
    fsz = (I32s)(cpb * (MinCellSize - 1));  /* chars per Free step */
    while (1)
    {   if (IsFree(addr))
        {   FEPrintSI(fp, addr, x, n, fsz, 0);  /* 0 is char for Free */
            addr += MinCellSize - 1;
        }
        else
        {   WhichCell(addr, &cp, &tmd);  /*chars for this size */
            sz = cpb * (tmd == 'm') ? cp->mm.s : cp->md.s;
            FEPrintSI(fp, addr, x, n, sz, tmd);
            addr += cp->mm.s + 1;
        }
        if (addr >= SoupSize)
            break;
        addr = ad(addr);
    }
    tfclose(fp);
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void FEPrintSI(fp, addr, x, n, sz, md)
    FILE *fp;
    I32s x, addr, n, sz, md;
{   I32s l;
    I8s ch;

    ch = (md) ? ((md == 'm') ? ('A' + (sz % n)) : ('a' + (sz % n))) : '.';
    for (l = 0; l < sz; l++)
        tfprintf(fp, "%c", ch);
    if (((addr % x) + sz) > x)
        tfprintf(fp, "\n");
}
