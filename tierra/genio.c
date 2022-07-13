/* @(#)genio.c 1.15 2/13/95 */
/* genio.c 30-3-98 genebank input/output routines */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */
/* MemProf labels: #06XX, in use: 0600-0617,0620-0621 */

/* FEError labels: #06XX, in use: 0600-0628, 0630-0632 */

#include "license.h"
#include "tierra.h"
#include "globals.h"
#ifdef __MWERKS__
#ifndef NET
#include <stat.h>
#endif /* NET */
#endif /* __MWERKS__ */

#define WritEcoS(bits) WritEcoB(bits, Buff)

#if defined(TIERRA)||defined(ARGTIE)
static void WriteGBGenome P_((head_t *head, Pgl gen, XDR *xdrhndl));
static void WriteGBGenBits P_((head_t *head, Pgl gen, XDR *xdrhndl));
static void write_genome_genbits P_((head_t *head, Pgl gen, FILE *fp));
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(PROBE)||defined(ARGTIE)
static void ExePtrnHead P_((FILE *fp, I32s exeptrn,
    I32s *expttsarr, I32s expttsarrcnt));
static I8s IncLineChk P_((Pgl g, I32s t, I32s trkidx,
    I32s *lstmrk, I32s exeptrn, I32s *expttsarr,
    I32s expttsarrcnt, ThdTsTyArr *codthdtstyarr));
static void ExePtrnMrkLine P_((FILE *fp, Pgl g, I32s t,
    I32s trkidx, I32s exeptrn, I32s *expttsarr,
    I32s expttsarrcnt, ThdTsTyArr *codthdtstyarr));
#endif /* defined(TIERRA)||defined(PROBE)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)
static void read_genome_genbits P_((head_t *head, Pgl gen, FILE *fp));
static I32s GenomeGenBitsSize P_((head_t *head));
static void ReadGBGenome P_((head_t *head, Pgl gen, XDR *xdrhndl));
static void ReadGBGenBits P_((head_t *head, Pgl gen, XDR *xdrhndl));
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if TIERRA
#ifdef IBM3090

static I8u a2e[] =
{   0000, 0001, 0002, 0003, 0067, 0055, 0056, 0057, 0026, 0005, 0045, 0013, 0014, 0015, 0016,
    0017, 0020, 0021, 0022, 0023, 0074, 0075, 0062, 0046, 0030, 0031, 0077, 0047, 0034, 0035,
    0036, 0037, 0100, 0117, 0177, 0173, 0133, 0154, 0120, 0175, 0115, 0135, 0134, 0116, 0153,
    0140, 0113, 0141, 0360, 0361, 0362, 0363, 0364, 0365, 0366, 0367, 0370, 0371, 0172, 0136,
    0114, 0176, 0156, 0157, 0174, 0301, 0302, 0303, 0304, 0305, 0306, 0307, 0310, 0311, 0321,
    0322, 0323, 0324, 0325, 0326, 0327, 0330, 0331, 0342, 0343, 0344, 0345, 0346, 0347, 0350,
    0351, 0112, 0340, 0132, 0137, 0155, 0171, 0201, 0202, 0203, 0204, 0205, 0206, 0207, 0210,
    0211, 0221, 0222, 0223, 0224, 0225, 0226, 0227, 0230, 0231, 0242, 0243, 0244, 0245, 0246,
    0247, 0250, 0251, 0300, 0152, 0320, 0241, 0007, 0040, 0041, 0042, 0043, 0044, 0025, 0006,
    0027, 0050, 0051, 0052, 0053, 0054, 0011, 0012, 0033, 0060, 0061, 0032, 0063, 0064, 0065,
    0066, 0010, 0070, 0071, 0072, 0073, 0004, 0024, 0076, 0341, 0101, 0102, 0103, 0104, 0105,
    0106, 0107, 0110, 0111, 0121, 0122, 0123, 0124, 0125, 0126, 0127, 0130, 0131, 0142, 0143,
    0144, 0145, 0146, 0147, 0150, 0151, 0160, 0161, 0162, 0163, 0164, 0165, 0166, 0167, 0170,
    0200, 0212, 0213, 0214, 0215, 0216, 0217, 0220, 0232, 0233, 0234, 0235, 0236, 0237, 0240,
    0252, 0253, 0254, 0255, 0256, 0257, 0260, 0261, 0262, 0263, 0264, 0265, 0266, 0267, 0270,
    0271, 0272, 0273, 0274, 0275, 0276, 0277, 0312, 0313, 0314, 0315, 0316, 0317, 0332, 0333,
    0334, 0335, 0336, 0337, 0352, 0353, 0354, 0355, 0356, 0357, 0372, 0373, 0374, 0375, 0376,
    0377};

static I8u e2a[] =
{   0000, 0001, 0002, 0003, 0234, 0011, 0206, 0177, 0227, 0215, 0216, 0013, 0014, 0015, 0016,
    0017, 0020, 0021, 0022, 0023, 0235, 0205, 0010, 0207, 0030, 0031, 0222, 0217, 0034, 0035,
    0036, 0037, 0200, 0201, 0202, 0203, 0204, 0012, 0027, 0033, 0210, 0211, 0212, 0213, 0214,
    0005, 0006, 0007, 0220, 0221, 0026, 0223, 0224, 0225, 0226, 0004, 0230, 0231, 0232, 0233,
    0024, 0025, 0236, 0032, 0040, 0240, 0241, 0242, 0243, 0244, 0245, 0246, 0247, 0250, 0133,
    0056, 0074, 0050, 0053, 0041, 0046, 0251, 0252, 0253, 0254, 0255, 0256, 0257, 0260, 0261,
    0135, 0044, 0052, 0051, 0073, 0136, 0055, 0057, 0262, 0263, 0264, 0265, 0266, 0267, 0270,
    0271, 0174, 0054, 0045, 0137, 0076, 0077, 0272, 0273, 0274, 0275, 0276, 0277, 0300, 0301,
    0302, 0140, 0072, 0043, 0100, 0047, 0075, 0042, 0303, 0141, 0142, 0143, 0144, 0145, 0146,
    0147, 0150, 0151, 0304, 0305, 0306, 0307, 0310, 0311, 0312, 0152, 0153, 0154, 0155, 0156,
    0157, 0160, 0161, 0162, 0313, 0314, 0315, 0316, 0317, 0320, 0321, 0176, 0163, 0164, 0165,
    0166, 0167, 0170, 0171, 0172, 0322, 0323, 0324, 0325, 0326, 0327, 0330, 0331, 0332, 0333,
    0334, 0335, 0336, 0337, 0340, 0341, 0342, 0343, 0344, 0345, 0346, 0347, 0173, 0101, 0102,
    0103, 0104, 0105, 0106, 0107, 0110, 0111, 0350, 0351, 0352, 0353, 0354, 0355, 0175, 0112,
    0113, 0114, 0115, 0116, 0117, 0120, 0121, 0122, 0356, 0357, 0360, 0361, 0362, 0363, 0134,
    0237, 0123, 0124, 0125, 0126, 0127, 0130, 0131, 0132, 0364, 0365, 0366, 0367, 0370, 0371,
    0060, 0061, 0062, 0063, 0064, 0065, 0066, 0067, 0070, 0071, 0372, 0373, 0374, 0375, 0376,
    0377};

/*
 * Ascii2Ebcdic - convert ASCII string to EBCDIC
 *
 * s - pointer to ASCII string buffer
 *
 */
void Ascii2Ebcdic(s)
    char *s;
{   while (*s = a2e[*s])
        s++;
}

/*
 * Ebcdic2Ascii - convert EBCDIC string to ASCII
 *
 * s - pointer to EBCDIC string buffer
 *
 */
void Ebcdic2Ascii(s)
    char *s;
{   while (*s = e2a[*s])
        s++;
}

#endif /* IBM3090 */

#endif /* TIERRA */

#if TIERRA

/*
 * SetBit - set or clear specified bit in given integer
 *
 * seed - integer with bit to be updated
 * bit  - bit number
 * value - 0 - clear bit, <>0 - set bit
 */
void SetBit(seed, bit, value)
    I32u *seed, bit, value;
{   if (value)
        (*seed) |= (I32UONE << bit);
    else
        (*seed) &= (~(I32UONE << bit));
}
#endif /* TIERRA */

#ifdef ARGTIE
/*
 * GetAscGen - read ascii instruction mnemonic list ("source file")
 *             of genome
 *
 * g - pointer to GList structure to be filled out for genome
 * ifile - pointer to "source" file name
 *
 */
I32s GetAscGen(g, ifile)
    Pgl g;
    I8s ifile[];
{   I8s bit[4], buf[81], *gdat, inst[9];
    I32s j, k, stl, GenSiz, GenStart;
#if PLOIDY > 1  /* diploid conditions are out of date */
    I32s p;
#endif
    I8u ti;
    I32u datti;
    FILE *inf;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
    I32s sscanfrtn;

    if ((inf = tfopen((I8s *)ifile, (I8s *)"r"))== NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra GetAscGen() file %s not opened, exiting", ifile);
        porterrmsg(608,(char *)(&(Fbuf[0])),1);
    }
    gdat = (I8s *) tcalloc(85, sizeof(I8s), 605);
    g->ploidy = (I8s) 1;  /* default ploidy */
    g->gen.size = 80;
    tsprintf((char *)(&((g->gen.label)[0])), "aaa");
    g->parent.size = 666;
    tsprintf((char *)(&((g->parent.label)[0])), "god");

/* BEGIN PRESCAN */

    while (1)
    {   if (tfgets(gdat, 84, inf) == NULL)  /* exit on end of file */
        {   tsprintf((char *)(&(Fbuf[0])),
                "GetAscGen(): CODE directive missing from %s\n",
                ifile);
            porterrmsg(609,(char *)(&(Fbuf[0])),1);
        }
#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanfrtn=sscanf((char *)gdat, "%s", buf);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if(sscanfrtn<=0)
            continue;
        if (!strncmp((char *)(&(buf[0])), "CODE", 4))
            break;
        if (strlen((const char *)gdat) < 3)  /* eat blank line */
            continue;
        if (buf[0] == ';')  /* eat comment line */
            continue;
        if (!strcmp((const char *)(&(buf[0])), (const char *)"genotype:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%d%s%*s%d,%d%*s%*s%d%s",
                &g->gen.size, g->gen.label,
                &g->mg.p, &g->mg.s,
                &g->parent.size, g->parent.label);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])), (char *)"1st_daughter_a:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%*s%d%*s%d",
                   &g->d1.inst, &g->d1.instP);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"1st_daughter_b:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%*s%d%*s%d%*s%d",
                   &g->d1.flags, &g->d1.mov_daught,
                   &g->d1.BreedTrue);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"2nd_daughter_a:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%*s%d%*s%d",
                   &g->d2.inst, &g->d2.instP);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"2nd_daughter_b:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%*s%d%*s%d%*s%d",
                   &g->d2.flags, &g->d2.mov_daught,
                   &g->d2.BreedTrue);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"InstExe.m:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%d%%*s%d%*s%d",
                &g->originI.m, &g->originI.i,
                &g->originC);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"MaxCpus:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%d", &g->max_cpus);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"MaxPropPop:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%lf%*s%lf%*s%d%*s%d,%d",
                &g->MaxPropPop, &g->MaxPropInst, &g->MaxPop,
                &g->mpp_time.m, &g->mpp_time.i);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"Origin:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%d,%d%*s%d",
                &g->originI.m, &g->originI.i,
                &g->originC);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            continue;
        }
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"ploidy:"))
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((char *)gdat, "%*s%d%*s%d", &j, &k);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            g->ploidy = (I8s) j;
/*
   g->track = (I8s) k;
 */
            continue;
        }
    }
    j = GenSiz = 0;
    GenStart = -1;
    while (1)
    {   if (tfgets(gdat, 84, inf) == NULL)  /* break on end of file */
            break;
        if (strlen((const char *)gdat) < 3)  /* eat blank line */
            continue;

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanf((char *)gdat, "%s", buf);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if (buf[0] == ';')  /* eat comment line */
            continue;
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"track"))  /* eat track line */
            continue;
        if (strcmp((const char *)(&(buf[0])),
            (const char *)"data"))    /* count only code lines */
        {   GenSiz++;
            if (GenStart == -1)
                GenStart = j;
        }
        j++;
    }
    if (j != g->gen.size)
    {   tsprintf((char *)(&(Fbuf[0])),
    "GetAscGen(): different number of inst, %d, than stated in header %d\n",
        j, g->gen.size);
        porterrmsg(610,(char *)(&(Fbuf[0])),0);
        g->gen.size = j;
    }
    if (GenStart != g->mg.p)
    {   tsprintf((char *)(&(Fbuf[0])),
    "GetAscGen(): different genetic start, %d, than stated in header %d\n",
        GenStart, g->mg.p);
        porterrmsg(611,(char *)(&(Fbuf[0])),0);
        g->mg.p = GenStart;
    }
    if (GenSiz != g->mg.s)
    {   tsprintf((char *)(&(Fbuf[0])),
    "GetAscGen(): different genetic size, %d, than stated in header %d\n",
        GenSiz, g->mg.s);
        porterrmsg(612,(char *)(&(Fbuf[0])),0);
        g->mg.s = GenSiz;
    }

/* END PRESCAN */

    tfclose(inf);
    inf = tfopen(ifile, (I8s *)"r");
    if (inf == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "Tierra GetAscGen() file %s not opened, exiting", ifile);
        porterrmsg(613,(char *)(&(Fbuf[0])),1);
    }

    AllocGenome(g,g->gen.size);
    AllocGenBits(g,g->gen.size);
    while (1)
    {   if (tfgets(gdat, 84, inf) == NULL)  /* break on end of file */
            break;

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanf((char *)gdat, "%s", buf);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if (!strncmp((const char *)(&(buf[0])),
            (const char *)"CODE", 4))
            break;
    }
    j = -1;
#if PLOIDY == 1
    while (1)
    {   if (tfgets(gdat, 84, inf) == NULL)  /* break on end of file */
            break;
        if (strlen((const char *)gdat) < 3)  /* eat blank line */
            continue;

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanf((char *)gdat, "%s", buf);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        if (buf[0] == ';')  /* eat comment line */
            continue;
        if (!strcmp((const char *)(&(buf[0])),
            (const char *)"track"))  /* eat track line */
            continue;
        j++;

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        stl=sscanf((char *)gdat, "%8s%*s%3s%2x", inst, bit, &datti);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
        if(stl<3)
            datti=InstNum;

#ifdef ASMGBITS
   if (stl > 1 && strlen(bit) == 3)
   {   if (bit[0] - '0') g->gbits[j] |= (I8s) 1;
   if (bit[1] - '0') g->gbits[j] |= (I8s) (1 << 1);
   if (bit[2] - '0') g->gbits[j] |= (I8s) (1 << 2);
   }
#endif /* ASMGBITS */
        ti=0;
        if (!strcmp((const char *)(&(inst[0])),
            (const char *)"data"))
        {   ti=(I8u)datti;
            k=0;
        }
        else for (k = 0; k < InstNum; k++)
        {   if (!strcmp((const char *)inst,
                (const char *)(&(id[k].mn[0]))))
            {   ti = id[k].op;
                break;
            }
        }
        if (k == InstNum)
        {   tsprintf((char *)(&(Fbuf[0])),
                "Tierra GetAscGen() mnemonic %s not recognized\n", inst);
            porterrmsg(614,(char *)(&(Fbuf[0])),0);
            ti = 0;
        }
        g->genome[j] = ti;
    }
#else /* PLOIDY > 1 */
    for (p = 0; p < PLOIDY; p++)
    {   if (p)
            tfgets(gdat, 84, inf);
        tfgets(gdat, 84, inf);
        tfgets(gdat, 84, inf);
        for (j = 0; j < g->gen.size; j++)
        {   tfgets(gdat, 84, inf);

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            stl = sscanf((char *)gdat, "%s%*s%s", inst, bit);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

/*
   if (stl > 1 && strlen(bit) == 3)
   {   if (bit[0] - '0') g->gbits[j][p] |= (I8s) 1;
   if (bit[1] - '0') g->gbits[j][p] |= (I8s) (1 << 1);
   if (bit[2] - '0') g->gbits[j][p] |= (I8s) (1 << 2);
   }
 */
            for (k = 0; k < InstNum; k++)
            {   if (!strcmp((const char *)inst,
                    (const char *)(&(id[k].mn[0]))))
                {   ti = id[k].op;
                    break;
                }
            }
            if (k == InstNum)
            {   tsprintf((char *)(&(Fbuf[0])),
                    "Tierra GetAscGen() mnemonic %s not recognized\n", inst);
                porterrmsg(615,(char *)(&(Fbuf[0])),0);
                ti = 0;
            }
            g->genome[j][p] = ti;
        }
    }
#endif /* PLOIDY >  1 */
    tfclose(inf);
    if (j + 1 != g->gen.size)
    {   tsprintf((char *)(&(Fbuf[0])),
            "GetAscGen(): j = %d, g->gen.size = %d\n",
            j + 1, g->gen.size);
        porterrmsg(616,(char *)(&(Fbuf[0])),0);
    }
    g->hash = Hash(g->mg.s, g->genome + g->mg.p);
#ifdef IBM3090
    Ebcdic2Ascii(g->gen.label);
    Ebcdic2Ascii(g->parent.label);
#endif
    if (gdat)
    {   tfree((void *) gdat, 85 * sizeof(I8s), 605);
        gdat = NULL;
    }
    return 1;
}
#endif /* ARGTIE */

#if defined(TIERRA)||defined(ARGTIE)

/*
 * add_gen - replace or add a genotype to end of genebank archive
 *
 *     fp - pointer to open archive file
 *     head - header of archive
 *     indx - index of archive
 *     gen - genotype to be added
 *
 * reformats the genotype and replaces it in the archive, or adds it to
 * the end if not found. args head & indx are modified by this fn.
 * returns 0 on add, and 1 on replace.
 *
 */

I32s add_gen(fp, head, indx, gen)
    FILE *fp;
    head_t *head;
    indx_t **indx;
    Pgl gen;
{   I32s hiofstused, n, ret, olddatstrt=0, shiftflag;
    I32s datsize=0, newdatstrt=0, indxincr=0, bytincr=0;
    I32s i, newdatsiz, ixupdstrt=0, olddatsiz, xdrbuflen;
    indx_t *tp_indx;
    I8s *buf;

    /* reserve genotype space */
    xdrbuflen=GenomeGenBitsSize(head)+(prcencsize.
        pec_FilThrdAnaDat)+GTaDatSize(&(gen->glst_thrdanadat));
    n=find_gen(*indx, gen->gen.label, head->n);
    ret=(n==head->n);

    shiftflag=0;
    if(ret)
    {   if(head->n==head->n_alloc)
        {   olddatstrt=(prcencsize.pec_head_t)+
                (((head->n_alloc)*(prcencsize.
                pec_indx_t))+sizeof(u_int));
            indxincr=1024/(prcencsize.pec_indx_t);
            bytincr=indxincr*(prcencsize.pec_indx_t);
            datsize=(head->g_off)-olddatstrt;
            newdatstrt=olddatstrt+bytincr;
            ixupdstrt=0;
            shiftflag=1;
        }
    }
    else
    {   if(n<((head->n)-1))
        {   newdatsiz=xdrbuflen;
            ixupdstrt=n+1;
            olddatsiz=((*indx)[ixupdstrt].
                g_off)-((*indx)[n].g_off);
            bytincr=newdatsiz-olddatsiz;
            olddatstrt=(*indx)[ixupdstrt].g_off;
            datsize=(head->g_off)-olddatstrt;
            newdatstrt=olddatstrt+bytincr;
            if(bytincr!=0)
                shiftflag=1;
        }
    }
    if(shiftflag)
    {   buf=(I8s *)thcalloc(datsize, 1, 604);
        tseek(fp, olddatstrt, 0);
        tfread(buf, datsize, 1, fp);
        tseek(fp, newdatstrt, 0);
        tfwrite(buf, datsize, 1, fp);
        thfree((void *)buf, datsize, 604);

        for(i=ixupdstrt; i<head->n; i++)
            (*indx)[i].g_off+=bytincr;
        if(ret&&(head->n==head->n_alloc))
        {   *indx=(indx_t *)threcalloc((void *)*indx,
                (prcencsize.pec_indx_t)*((head->
                    n_alloc)+indxincr),
                (prcencsize.pec_indx_t)*(head->n_alloc), 600);
            head->n_alloc+=indxincr;
        }
        head->g_off+=bytincr;
    }

    /* write genotype data */
    if(ret)
        tseek(fp, head->g_off, 0);
    else
        tseek(fp, (*indx)[n].g_off, 0);

    write_genome_genbits(head, gen, fp);

    WriteThrdAnaDat(&(gen->glst_thrdanadat),fp);

    hiofstused=tftell(fp);

    tp_indx=&((*indx)[n]);
    strncpy((char *)(&((tp_indx->gen)[0])), (const char *)
        (&((gen->gen.label)[0])), 3);
    tp_indx->psize = gen->parent.size;
    strncpy((char *)(&((tp_indx->pgen)[0])), (const char *)
        (&((gen->parent.label)[0])), 3);
    (tp_indx)->MaxPop  = gen->MaxPop;
    tp_indx->mg = gen->mg;
    tp_indx->bits = gen->bits;
    tp_indx->hash = gen->hash;
    tp_indx->d1 = gen->d1;
    tp_indx->d2 = gen->d2;
    tp_indx->originI = gen->originI;
    tp_indx->originC = gen->originC;
    tp_indx->mpp = (short) (gen->MaxPropPop * 10000);
    tp_indx->mpi = (short) (gen->MaxPropInst * 10000);
    tp_indx->max_cpus = gen->max_cpus;
    tp_indx->mppT = gen->mpp_time;
    tp_indx->pt = (gen->ploidy << 4) /* + gen->track */ ;

#ifdef NET
    tp_indx->ixsucgennslrat=gen->sucgennslrat;
    tp_indx->ixsvgennsl=gen->svgennsl;
    tp_indx->ixsvsucgennsl=gen->svsucgennsl;
#endif /* NET */

    if(ret)
        tp_indx->g_off=head->g_off;

#ifdef ERRORTIE
    for (i=0; i<head->n; i++)
        if(!(*indx)[i].gen[0])
            porterrmsg(607,"Tierra add_gen() indx array corrupted",1);
#endif /* ERRORTIE */

    write_indx(fp, head, *indx);
    if(hiofstused>(head->g_off))
        head->g_off=hiofstused;
    head->n+=ret;
    write_head(fp, head);

    return !ret;
}

/*
 * write_genome_genbits - write genome and Genbits to gene bank archive
 *
 * head - pointer to head_t structure containing gene bank archive header
 * gen - pointer to genotype label
 * fp - handle for gene bank archive to be updated
 */
static void write_genome_genbits(head, gen, fp)
head_t *head;
Pgl gen;
FILE *fp;
{   I8u *xdrbuf;
    I32s xdrbuflen;
    XDR xdrhndl;

    xdrbuflen=GenomeGenBitsSize(head);
    xdrbuf=(I8u *)thcalloc(xdrbuflen, 1, 614);
    txdrmem_create(&xdrhndl, (I8s *)xdrbuf,
        xdrbuflen, XDR_ENCODE);
    WriteGBGenome(head, gen, &xdrhndl);
    WriteGBGenBits(head, gen, &xdrhndl);
    tfwrite((I8s *)xdrbuf, xdrbuflen, 1, fp);
    txdr_destroy(&xdrhndl);
    tfree(xdrbuf, xdrbuflen, 614);
}

/*
 * WriteGBGenome - write genome to genebank archive
 *
 * head - pointer to head_t structure 
 * fp - "FILE *" for genebank archive file
 */
static void WriteGBGenome(head, gen, xdrhndl)
head_t *head;
Pgl gen;
XDR *xdrhndl;
{   GBGenome GBGenome_buf;
    
    (GBGenome_buf.array.array_len)=InstrXdrWrdSize(head->size);
    GBGenome_buf.array.array_val=(xdrwrd *)(gen->genome);
    if(!(txdr_GBGenome(xdrhndl, &GBGenome_buf)))
        porterrmsg(625,"WriteGBGenome() failed",1);
}

/*
 * WriteGBGenBits - write GenBits to genebank archive
 *
 * head - pointer to head_t structure 
 * gen - pointer to g_list structure for genotype
 * xdrhndl - pointer to XDR handle
 */
static void WriteGBGenBits(head, gen, xdrhndl)
head_t *head;
Pgl gen;
XDR *xdrhndl;
{   GBGenBits GBGenBits_buf;
    (GBGenBits_buf.array.array_len)=GenBitsXdrWrdSize(head->size);
    GBGenBits_buf.array.array_val=(xdrwrd *)(gen->gbits);
    if(!(txdr_GBGenBits(xdrhndl, &GBGenBits_buf)))
        porterrmsg(627,"WriteGBGenBits() failed",1);
}

/*
 * Hash - calculate genome hash code
 *
 * size - size of genome
 * v    - pointer to genome buffer (FpInst)
 *
 * return: genome hash code
 */
I32s Hash(size, v)
    I32s size;
    FpInst v;
{   I32s h = 0;
    I32s i;
#if PLOIDY > 1
    I32s j;
#endif

    for (i = 0; i < size; i++)
#if PLOIDY == 1
        h = (3 * h + *(v + i)) % 277218551L;  /* 277218551 is prime */
#else /* PLOIDY > 1 */
        for (j = 0; j < PLOIDY; j++)
            h = (3 * h + v[i][j]) % 277218551L;
#endif /* PLOIDY > 1 */
    return h;
}

/*
 * write_head - write header to a genebank archive
 *
 * fp - "FILE *" for gene bank archive file to be updated
 * head - pointer to head_t structure
 *
 */

void write_head(fp, head)
FILE *fp;
head_t *head;
{   XDR xdrhndl;
    I8u *xdrmsgbuf;
    I32s rtnval;
    
    xdrmsgbuf=(I8u *)thcalloc(1, prcencsize.pec_head_t, 606);
    txdrmem_create(&xdrhndl, (I8s *)xdrmsgbuf,
        prcencsize.pec_head_t, XDR_ENCODE);
    if((rtnval=txdr_head_t(&xdrhndl, head)))
    {   if (!tseek(fp, 0, 0))
            tfwrite((I8s Hp)xdrmsgbuf, prcencsize.pec_head_t, 1, fp);
        else
            porterrmsg(602,"Tierra write_head() file access failed",1);
    }
    else
        porterrmsg(622,"write_head() file access failed",1);
    txdr_destroy(&xdrhndl);
    tfree(xdrmsgbuf, prcencsize.pec_head_t, 606);
}

/*
 * write_indx - update the index of a genebank archive
 *
 * fp - "FILE *" for genebank archive file to be updated
 * head - pointer to head_t structure containing gene bank archive header
 * indx - pointer to indx_t structure array containing archive index
 */
void write_indx(fp, head, indx)
FILE *fp;
head_t *head;
indx_t *indx;
{
#ifdef ERRORTIE
    I32s i;
#endif /* ERRORTIE */
    I8u *xdrmsgbuf;
    XDR xdrhndl;
    GBindx_t GBindx_t_buf;
    I32s xdrbuflen;

#ifdef ERRORTIE
    for (i = 0; i < head->n; i++)
        if (!(indx[i]).gen[0])
            porterrmsg(605,"Tierra write_indx() indx array corrupted",1);
#endif /* ERRORTIE */
    if (!tseek(fp, prcencsize.pec_head_t, 0))
    {   (GBindx_t_buf.array.array_len)=head->n_alloc;
        xdrbuflen=((GBindx_t_buf.array.array_len)*
            (prcencsize.pec_indx_t))+sizeof(u_int);
        xdrmsgbuf=(I8u *)thcalloc(xdrbuflen, 1, 616);
        txdrmem_create(&xdrhndl, (I8s *)xdrmsgbuf,
            xdrbuflen, XDR_ENCODE);
        GBindx_t_buf.array.array_val=(indx_t *)indx;
        if((txdr_GBindx_t(&xdrhndl, &GBindx_t_buf)))
            tfwrite((I8s Hp)xdrmsgbuf, txdr_getpos(&xdrhndl), 1, fp);
        else
            porterrmsg(630,"write_indx_t() failed",1);
        txdr_destroy(&xdrhndl);
        tfree(xdrmsgbuf, xdrbuflen, 616);
    }
    else
        porterrmsg(606,"write_indx failed",1);
}
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

/*
 * enc_size_precomp - precompute encoded structure sizes
 */
void enc_size_precomp()
{   XDR xdrhndl;
    I8u *encbuf;
    I32s rtnval;
    union {
        head_t             un_head_t;
        indx_t             un_indx_t;
        FilThrdAnaDat      un_FilThrdAnaDat;
        BaseCallLvlIntrv   un_BaseCallLvlIntrv;
        IdxThreadDat       un_IdxThreadDat;
        ThrdAnaEvent       un_ThrdAnaEvent;
        SoupUpdEvent       un_SoupUpdEvent;
        FilGenThrdDat      un_FilGenThrdDat;
        FilInstrThrd       un_FilInstrThrd;
#ifdef NET
#if defined(TIERRA)
#if NETTYPE == UDPASM
        pkthdr             un_pkthdr;
#endif /* NETTYPE == UDPASM */
#endif /* defined(TIERRA) */
#endif
    } strctbuf;
    
    encbuf=(I8u *)thcalloc(XDRBufMaxSize, 1, 607);



    txdrmem_create(&xdrhndl, (I8s *)encbuf,
        XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(head_t));
    if((rtnval=txdr_head_t(&xdrhndl, (head_t *)(&strctbuf))))
        (prcencsize.pec_head_t)=txdr_getpos(&xdrhndl);
    else
        porterrmsg(623,"head_t encoded size precomputation failed",1);
    txdr_destroy(&xdrhndl);



    txdrmem_create(&xdrhndl, (I8s *)encbuf,
        XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(indx_t));
    if((rtnval=txdr_indx_t(&xdrhndl, (indx_t *)(&strctbuf))))
        (prcencsize.pec_indx_t)=txdr_getpos(&xdrhndl);
    else
        porterrmsg(626,"indx_t encoded size precomputation failed",1);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(FilThrdAnaDat));
    txdr_FilThrdAnaDat(&xdrhndl, (FilThrdAnaDat *)(&strctbuf));
    (prcencsize.pec_FilThrdAnaDat)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(BaseCallLvlIntrv));
    txdr_BaseCallLvlIntrv(&xdrhndl, (BaseCallLvlIntrv *)(&strctbuf));
    (prcencsize.pec_BaseCallLvlIntrv)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(IdxThreadDat));
    txdr_IdxThreadDat(&xdrhndl, (IdxThreadDat *)(&strctbuf));
    (prcencsize.pec_IdxThreadDat)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(ThrdAnaEvent));
    txdr_ThrdAnaEvent(&xdrhndl, (ThrdAnaEvent *)(&strctbuf));
    (prcencsize.pec_ThrdAnaEvent)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(SoupUpdEvent));
    txdr_SoupUpdEvent(&xdrhndl, (SoupUpdEvent *)(&strctbuf));
    (prcencsize.pec_SoupUpdEvent)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(FilGenThrdDat));
    txdr_FilGenThrdDat(&xdrhndl, (FilGenThrdDat *)(&strctbuf));
    (prcencsize.pec_FilGenThrdDat)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(FilInstrThrd));
    txdr_FilInstrThrd(&xdrhndl, (FilInstrThrd *)(&strctbuf));
    (prcencsize.pec_FilInstrThrd)=txdr_getpos(&xdrhndl);
    txdr_destroy(&xdrhndl);


#ifdef NET
#if defined(TIERRA)
#if NETTYPE == UDPASM
    txdrmem_create(&xdrhndl, (I8s *)encbuf, XDRBufMaxSize, XDR_ENCODE);
    memset(&strctbuf, 0, sizeof(pkthdr));
    if((rtnval=txdr_pkthdr(&xdrhndl,
        (pkthdr *)(&strctbuf))))
        (prcencsize.pec_pkthdr)=txdr_getpos(&xdrhndl);
    else
        porterrmsg(628,"pkthdr encoded size precomputation failed",1);
    txdr_destroy(&xdrhndl);
#endif /* NETTYPE == UDPASM */
#endif /* defined(TIERRA) */
#endif /* NET */


    tfree(encbuf, XDRBufMaxSize*sizeof(I8u), 607);
}

/*
 * GetAMap - read opcode map file
 *
 * file - pointer to name of opcode map file
 *
 * return: id - InstDef array buffer containing opcode map
 *
 */
void GetAMap(file)
I8s *file;
{   FILE *afp;
    I8s mn[9], regs[9], flags[12], fmtbuf[MAXPRINTFMTSIZ];
    I8s Nop1Done = 0, Nop0Done = 0;
    I32s i = 0, j, opc = -1, oInstNum = 2, len;
    I32s  cyc, scanfrtn;

#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

    regs[0] = flags[0] = 0;
    IDregs.Se.n = IDregs.De.n = IDregs.So.n = 0;
    IDregs.Se.r = IDregs.De.r = IDregs.So.r = NULL;
    if ((afp = tfopen(file, (I8s *)"r")) == NULL)
    {   tsprintf((char *)(&(Fbuf[0])),
            "unable to open IMapFile  - %s", file);
        porterrmsg(618,(char *)(&(Fbuf[0])),1);
    }
    InstBitNum = 1;
    InstNum = 2;
    id = (InstDef *) tcalloc(InstNum, sizeof(InstDef), 608);
    while (tfgets(Buf85b, 119, afp) != NULL)
    {   Buf85a[0] = '\0';
        len = 0;

#ifdef SIGBLOCK
        SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
        sscanf((const char *)(&(Buf85b[0])), "%84s%*s%84s", Buf85, Buf85a);
#ifdef SIGBLOCK
        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

        len = strlen((const char *)(&(Buf85a[0])));
        if (!strncmp((const char *)(&(Buf85[0])), (const char *)"Se", 2))
        {   IDregs.Se.n = (I8s)len;
            if (len > 0)
                IDregs.Se.r = tcalloc(len + 1, sizeof(I8s), 609);
            for (i = 0; i < len; i++)
                IDregs.Se.r[i] = Buf85a[i] - 'a';
        }
        else if (!strncmp((const char *)(&(Buf85[0])), (const char *)"De", 2))
        {   IDregs.De.n = (I8s)len;
            if (len > 0)
                IDregs.De.r = tcalloc(len + 1, sizeof(I8s), 610);
            for (i = 0; i < len; i++)
                IDregs.De.r[i] = Buf85a[i] - 'a';
        }
        else if (!strncmp((const char *)(&(Buf85[0])), (const char *)"So", 2))
        {   IDregs.So.n = (I8s)len;
            if (len > 0)
                IDregs.So.r = tcalloc(len + 1, sizeof(I8s), 611);
            for (i = 0; i < len; i++)
                IDregs.So.r[i] = Buf85a[i] - 'a';
        }
        else if (Buf85b[0] == '\n')
        {   continue;
        }
/* NOTE: this sscanf fails in the case that one of the scanned strings
   is empty.  For example if the regs string is empty, then it fails
   to properly scan the flags string.  Needs to be fixed */
        else
        {

#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */

            sprintf((char *)(&(fmtbuf[0])),
                (const char *)"%s%s%s%d%s%s%d%s%s%d%s",
                "%*[^,],",
                "%d%*[^\"]\"",                                      /* cyc   */
                "%", (I32s)(sizeof(mn)-1), "[^\"]\",%*[^\"]\"",     /* mn    */
                "%", (I32s)(sizeof(regs)-1), "[^\"]\",%*[^\"]\"",   /* regs  */
                "%", (I32s)(sizeof(flags)-1), "[^\"]");             /* flags */

            scanfrtn=sscanf((const char *)(&(Buf85b[0])),
                (const char *)(&(fmtbuf[0])), &cyc, mn, regs, flags);

#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */

            if (scanfrtn>=1)
            {   opc++;
                if (opc >= InstNum)
                {   oInstNum = InstNum;
                    InstNum = oInstNum * 2;
                    InstBitNum++;
                    id = (InstDef *) trecalloc((void *) id,
                        InstNum * sizeof(InstDef),
                        oInstNum * sizeof(InstDef), 608);
                }
#if defined(ARGTIE)||defined(PROBE)||defined(THREADTREE)||\
    defined(GENALIGN)||defined(SOUPUPDTRC)
                strcpy((char *)(&(id[opc].mn[0])),
                    (const char *)(&(mn[0])));
#else /* defined(ARGTIE)||defined(PROBE)||
         defined(THREADTREE)||defined(GENALIGN)||
         defined(SOUPUPDTRC) */
                for (i = 0; strcmp((const char *)(&(mn[0])),
                    (const char *)(&(idt[i].mn[0]))); i++)
                    if (!strcmp((const char *)"END",
                       (const char *)(&(idt[i].mn[0]))))
                    {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra GetAMap() mnemonic \"%s\" not recognized", mn);
                        porterrmsg(619,(char *)(&(Fbuf[0])),1);
                    }
                    id[opc] = idt[i];
#endif /* defined(ARGTIE)||defined(PROBE)||
          defined(THREADTREE)||defined(GENALIGN)||
          defined(SOUPUPDTRC) */
                id[opc].op = (I8s)opc;
                id[opc].cyc = cyc;
                for (j = 0; j < (I32s)strlen((char *)(&(regs[0]))); j++)
                {   if (regs[j] >= 'a' && regs[j] <= 'a' + NUMREG)
                        id[opc].re[j] = regs[j] - 'a';
                    else if (regs[j] == ' ')
                    {   id[opc].re[j] = -1;
                        id[opc].idf.C = 1;
                    }
#if !(defined(ARGTIE)||defined(PROBE)||defined(THREADTREE)||\
    defined(GENALIGN)||defined(SOUPUPDTRC))
                    else
                    {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra GetAMap() regs argument '%c' not recognized",
                            regs[j]);
                        porterrmsg(620,(char *)(&(Fbuf[0])),1);
                    }
#endif /* !(defined(ARGTIE)||defined(PROBE)||defined(GENALIGN)) */
                }
                for (j = 0; j < (I32s)strlen((const char *)
                    (&(flags[0]))); j++)
                {   switch (flags[j])
                    {   case 'O':  /* Offset Segment */
                        {   id[opc].idf.Se = 1;
                            break;
                        }
                        case 'B':  /* Bits */
                        {   id[opc].idf.B = 1;
                            break;
                        }
                        case 'D':  /* Destination */
                        {   id[opc].idf.De = 1;
                            break;
                        }
                        case 'S':  /* Source */
                        {   id[opc].idf.So = 1;
                            break;
                        }
                        case 'I':  /* dIrection */
                        {   id[opc].idf.D = 1;
                            break;
                        }
#ifdef SHADOW
                        case 'H':  /* Shadow Registers */
                        {   id[opc].idf.H = 1;
                            ShadowUsed = 1;
                            break;
                        }
#endif /* SHADOW */
                        case 'P':  /* Reverse Polish */
                        {   id[opc].idf.P = 1;
                            break;
                        }
                        case 'C':  /* Special */
                        {   id[opc].idf.C = 1;
                            break;
                        }
#if !(defined(ARGTIE)||defined(PROBE)||defined(THREADTREE)||\
    defined(GENALIGN)||defined(SOUPUPDTRC))
                        default:
                        {   tsprintf((char *)(&(Fbuf[0])),
                        "Tierra GetAMap() flags argument '%c' not recognized",
                                flags[j]);
                            porterrmsg(621,(char *)(&(Fbuf[0])),1);
                        }
#endif /* !(defined(ARGTIE)||defined(PROBE)||
            defined(THREADTREE)||defined(GENALIGN))||
            defined(SOUPUPDTRC) */
                    }
                }
                if (!strcmp((const char *)(&(id[opc].mn[0])),
                    (const char *)"nop0") && !Nop0Done)
                {   Nop0 = id[opc].op; 
                    NopS = Nop0 + Nop1;
                    Nop0Done = 1;
                }
                if (!strcmp((const char *)(&(id[opc].mn[0])),
                    (const char *)"nop1") && !Nop1Done)
                {   Nop1 = id[opc].op;
                    NopS = Nop0 + Nop1;
                    Nop1Done = 1;
                }
            }
        }
        regs[0] = 0;
        flags[0] = 0;
    }
    tfclose(afp);
    for (i = opc + 1; i < InstNum; i++)
    {   id[i] = id[i - oInstNum];
        id[i].op = (I8s)i;
    }
}

void FreeAMap()
{   if(IDregs.Se.n)
       tfree(IDregs.Se.r,((IDregs.Se.n)+1)*sizeof(I8s), 609);
    if(IDregs.De.n)
       tfree(IDregs.De.r,((IDregs.De.n)+1)*sizeof(I8s), 610);
    if(IDregs.So.n)
       tfree(IDregs.So.r,((IDregs.So.n)+1)*sizeof(I8s), 611);
    tfree(id,InstNum*sizeof(InstDef), 608);
}

/*
 * read_head - read header from a genebank archive
 * 
 * fp - "FILE *" for gene bank archive file
 *
 * returns : archive header in "head_t" structure
 */
head_t read_head(fp)
    FILE *fp;
{   head_t t;
    I8u *decbuf;
    XDR xdrhndl;

    if(fp&&(!tseek(fp, 0, 0)))
    {   decbuf=(I8u *)thcalloc(1, prcencsize.pec_head_t, 612);
        tfread((I8s *)decbuf, prcencsize.pec_head_t, 1, fp);
        txdrmem_create(&xdrhndl, (I8s *)decbuf,
            prcencsize.pec_head_t, XDR_DECODE);
        if(!(txdr_head_t(&xdrhndl, &t)))
            porterrmsg(624,"read_head() file access failed",1);
        txdr_destroy(&xdrhndl);
        tfree(decbuf, prcencsize.pec_head_t, 612);
    }
    else
        porterrmsg(601,"read_head() file access failed",1);
    return t;
}

/*
 * read_indx - read the index from a genebank archive
 *
 * fp - "FILE *" for genebank archive file
 * head - pointer to head_t structure
 *
 * returns: pointer to indx_t structure array buffer containing archive index
 */

indx_t *read_indx(fp, head)
    FILE *fp;
    head_t *head;
{
#ifdef ERRORTIE
    I32s i;
#endif /* ERRORTIE */
    indx_t *t=0;
    I8u *xdrmsgbuf;
    I32s xdrbuflen;
    XDR xdrhndl;
    GBindx_t GBindx_t_buf;

    memset((char *)(&GBindx_t_buf), 0, sizeof(GBindx_t_buf));
    if(!tseek(fp, prcencsize.pec_head_t, 0))
    {   t=(indx_t *)thcalloc(head->n_alloc, sizeof(indx_t), 600);
        if(head->n)
        {   xdrbuflen=((head->n_alloc)*(prcencsize.
                pec_indx_t))+sizeof(u_int);
            xdrmsgbuf=(I8u *)thcalloc(xdrbuflen, 1, 621);
            txdrmem_create(&xdrhndl, (I8s *)xdrmsgbuf,
                xdrbuflen, XDR_DECODE);
            tfread((I8s Hp)xdrmsgbuf, xdrbuflen, 1, fp);
            GBindx_t_buf.array.array_val=t;
            if(!txdr_GBindx_t(&xdrhndl, &GBindx_t_buf))
                porterrmsg(600,"read_indx_t() failed",1);
            txdr_destroy(&xdrhndl);
            tfree(xdrmsgbuf, xdrbuflen, 621);
        }
    }
    else
        porterrmsg(603,"read_indx() failed",1);
#ifdef ERRORTIE
    if (t != NULL)
        for (i = 0; i < head->n; i++)
            if (!(t + i)->gen[0])
                porterrmsg(604,"read_indx() indx array corrupted",1);
#endif /* ERRORTIE */

    return t;
}

/*
 * get_gen - read a genome from genebank archive and return a pointer
 *     to a struct g_list containing all saved info.
 *
 *     fp - pointer to open archive file
 *     head - archive header
 *     indxn - index entry of desired genome
 *     n - position of desired genome in archive
 *
 * reads the genome and reformats its other args into the form used
 * internally by tierra. the genotype must be in archive. n can be
 * determined by find_gen(). currently no error checking
 */
Pgl get_gen(fp, head, indxn, n)
FILE *fp;
indx_t *indxn;
head_t *head;
I32s n;
{   CalLvlIntByIDArr calvlintrvlkupid;

    Pgl t = (Pgl) tcalloc(1, sizeof(GList), 1402);
    InitThrdAnaDat(&(t->glst_thrdanadat));
    tseek(fp, indxn->g_off, 0);
    read_genome_genbits(head, t, fp);
    t->gen.size = head->size;
    strncpy((char *)(&((t->gen.label)[0])),
        (const char *)(&((indxn->gen)[0])), 3);
    strncpy((char *)(&(t->parent.label[0])),
        (const char *)(&(indxn->pgen[0])), 3);
    t->parent.size = indxn->psize;
    t->mg = indxn->mg;
    t->hash = indxn->hash;
    t->bits = indxn->bits;
    t->d1 = indxn->d1;
    t->d2 = indxn->d2;
    t->originI = indxn->originI;
    t->originC = indxn->originC;
    t->MaxPropPop = (float) indxn->mpp / (float)10000.;
    t->MaxPropInst = (float) indxn->mpi / (float)10000.;
    t->MaxPop = indxn->MaxPop;
    t->mpp_time = indxn->mppT;
    t->ploidy = (indxn->pt & 0360) >> 4;  /* 0360 is not portable, octal */
    t->max_cpus = indxn->max_cpus;
/*
   t->track = indxn->pt & 017; 017 is not portable, octal
 */

#ifdef NET
    t->sucgennslrat=indxn->ixsucgennslrat;
    t->svgennsl=indxn->ixsvgennsl;
    t->svsucgennsl=indxn->ixsvsucgennsl;
#endif /* NET */

    /* read thread analysis data */
    ReadThrdAnaDat(&(t->glst_thrdanadat), &calvlintrvlkupid, fp);
    if(t->glst_thrdanadat.mtad_rtcalvlint)
        FreeDynArr((DynArr *)(&calvlintrvlkupid), 2219, 0);

    return t;
}

/*
 * open_ar - open gene bank archive read/write
 *
 * file - the filename;
 * size - the creature size
 * mode
 *    <0  : if archive does not exist
 *        :    create empty archive
 *        :
 *    ==0 : archive must exist
 *        :
 *    >0  :    create archive with number of allocated
 *        : genotype slots given by <mode>(will be
 *        : rounded to the next highest # such that
 *        : index + header is a multiple of 1K)
 *
 * returns a pointer to a file opened for update, or NULL if unsuccessful.
 * open_ar fails if size is incompatible with the archive.
 */
FILE *open_ar(file, size, mode)
I8s *file;
I32s size, mode;
{   FILE *fp = NULL;
    head_t head;
#if defined(TIERRA)||defined(ARGTIE)
    struct stat buf;
#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)
    if((mode>0)||(mode<0&&(tstat((char *)file, &buf) == -1)))
    {   if (mode < 0)  /* if file doesn't exist, no entries */
            mode = 0;
        if((fp=tfopen(file, (I8s *)"w+b")))
        {   memset((char *)(&head), 0, sizeof(head_t));
            head.size = size;
            head.n_alloc=(((int)(((prcencsize.
                pec_head_t)+mode*(prcencsize.pec_indx_t))/
                1024.0)+1)*1024-(prcencsize.pec_head_t))/
                (prcencsize.pec_indx_t);
            head.g_off=(prcencsize.pec_head_t)+
                (((head.n_alloc)*(prcencsize.
                pec_indx_t))+sizeof(u_int));
            write_head(fp, &head);
        }
    }
    else
#endif /* defined(TIERRA)||defined(ARGTIE) */
        if(mode<1&&(fp=tfopen((I8s *)file, (I8s *)"r+b")))
        {   head = read_head(fp);
            if (head.size != size)
            {   tfclose(fp);
                fp = NULL;
                errno = EINVAL;
            }
        }
    return fp;
}

/*
 * read_genome_genbits - read genome and Genbits from gene bank archive
 *
 * head - pointer to head_t structure containing gene bank archive header
 * fp - file handle for genebank archive
 */
static void read_genome_genbits(head, gen, fp)
head_t *head;
FILE *fp;
Pgl gen;
{   I32s xdrbuflen;
    I8u *xdrbuf;
    XDR xdrhndl;

    xdrbuflen=GenomeGenBitsSize(head);
    xdrbuf=(I8u *)thcalloc(xdrbuflen, 1, 613);
    txdrmem_create(&xdrhndl, (I8s *)xdrbuf, xdrbuflen, XDR_DECODE);
    tfread((I8s *)xdrbuf, xdrbuflen, 1, fp);
    ReadGBGenome(head, gen, &xdrhndl);
    ReadGBGenBits(head, gen, &xdrhndl);
    txdr_destroy(&xdrhndl);
    tfree(xdrbuf, xdrbuflen, 613);
}

/*
 * GenomeGenBitsSize - return total encoded size of the following for a 
 *               genotype: genome, GenBits
 */
static I32s GenomeGenBitsSize(head)
head_t *head;
{   return(((InstrXdrWrdSize(head->size)*
        sizeof(xdrwrd))+sizeof(u_int))+
        ((GenBitsXdrWrdSize(head->size)*
        sizeof(xdrwrd))+sizeof(u_int)));
}

/*
 * ReadGBGenome - read the index from a genebank archive
 *
 * head - pointer to head_t structure
 * gen - pointer to g_list structure for genotype
 * xdrhndl - pointer to XDR handle
 *
 * returns: pointer to Genome structure array buffer containing archive index
 */
static void ReadGBGenome(head, gen, xdrhndl)
head_t *head;
Pgl gen;
XDR *xdrhndl;
{   GBGenome GBGenome_buf;

    memset((char *)(&GBGenome_buf), 0, sizeof(GBGenome_buf));
    AllocGenome(gen,head->size);
    GBGenome_buf.array.array_val=(xdrwrd *)gen->genome;
    if(!txdr_GBGenome(xdrhndl, &GBGenome_buf))
        porterrmsg(631,"ReadGBGenome() failed",1);
}

/*
 * ReadGBGenBits - read the index from a genebank archive
 *
 * head - pointer to head_t structure
 * gen - pointer to g_list structure for genotype
 * xdrhndl - pointer to XDR handle
 *
 * returns: pointer to GenBits structure array buffer containing archive index
 */
static void ReadGBGenBits(head, gen, xdrhndl)
head_t *head;
Pgl gen;
XDR *xdrhndl;
{   GBGenBits GBGenBits_buf;

    memset((char *)(&GBGenBits_buf), 0, sizeof(GBGenBits_buf));
    AllocGenBits(gen,head->size);
    GBGenBits_buf.array.array_val=(xdrwrd *)gen->gbits;
    if(!txdr_GBGenBits(xdrhndl, &GBGenBits_buf))
        porterrmsg(632,"ReadGBGenBits() failed",1);
}

/*
 * ReadGenDef - read gene definition table and link to GList structure
 *
 * g - pointer to GList structure
 *
 */
void ReadGenDef(gendef, genelkup, size, label, defdir)
GeneDefArr *gendef;
I16s **genelkup;
I32s size;
I8s *defdir,*label;
{
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
    I8s genname[MAXGENDEFLINLEN+1],gendefline[MAXGENDEFLINLEN+1];
    I8s gendefnam[MAXGENDEFNAMLEN+1];
    FILE *gendefptr;
    I32s linelen,gdfstart,gdfend,gdftyp,i,cmntline;

    InitDynArr((DynArr *)gendef, sizeof(GenDef), 0);
    (*genelkup)=NULL;
    tsprintf((char *)(&(gendefnam[0])),
        "%s%.4d%s.gdf", defdir, size, label);
    if((gendefptr=tfopen(&(gendefnam[0]), (I8s *)"r")))
    {   while(tfgets(&(gendefline[0]), MAXGENDEFLINLEN, gendefptr))
        {   if((linelen=strlen((const char *)(&(gendefline[0])))))
                while((gendefline[linelen-1]=='\n')||
                    (gendefline[linelen-1]=='\r'))
                    gendefline[--linelen]='\0';
#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            sscanf((const char *)(&(gendefline[0])),
                (const char *)"%255s %d %d %d",
                &(genname[0]), &gdfstart, &gdfend, &gdftyp);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
            cmntline=1;
            for(i=0;i<(I32s)strlen((const char *)(&(gendefline[0])));i++)
                if(gendefline[i]==';')
                    break;
                else if((gendefline[i]!=' ')&&
                        (gendefline[i]!='\t')&&
                        (gendefline[i]!='\r')&&
                        (gendefline[i]!='\n'))
                        cmntline=0;
            if(!cmntline)
                AddGenDef(gendef,&(genname[0]),
                    (i<linelen)?((I8s *)&(gendefline[i])):(I8s *)"",
                    gdfstart,gdfend,gdftyp);
        }
        BldGenLkup(gendef,genelkup,size);
        fclose(gendefptr);
    }
}

/*
 * FreeGenDef - free gene definition lookup table
 */
void FreeGenDef(gendef, genelkup, size)
GeneDefArr *gendef;
I16s **genelkup;
I32s size;
{   GenDef *gdfelm;
    I32s i;

    if(genelkup)
        if(*genelkup)
        {   thfree((void *)(*genelkup), size*sizeof(**genelkup), 603);
            (*genelkup)=NULL;
        }
    for(i=0;i<(gendef->gdf_ctrl.dync_elmused);i++)
    {   gdfelm=&(gendef->gdf_array[i]);
        tfree(gdfelm->gdf_cmnt,(strlen(
            (const char *)(gdfelm->
            gdf_cmnt))+1)*sizeof(I8s), 617);
        if(gdfelm->gdf_name)
            tfree(gdfelm->gdf_name,(strlen(
                (const char *)(gdfelm->
                gdf_name))+1)*sizeof(I8s), 602);
    }
    FreeDynArr((DynArr *)gendef, 601, 0);
    InitDynArr((DynArr *)gendef, sizeof(GenDef), 0);
}

void AddGenDef(gendef,genname,gdfcmnt, gdfstart,gdfend,gdftyp)
GeneDefArr *gendef;
I8s *genname,*gdfcmnt;
I32s gdfstart,gdfend,gdftyp;
{   GenDef *gdfelm;

    gdfelm=&(gendef->gdf_array[AddDynArrEl(
        (DynArr *)(gendef), -1, GENDEFARRINC, 601, 0)]);
    (gendef->gdf_ctrl.dync_elmused)++;
    if(genname)
    {   gdfelm->gdf_name=tcalloc(strlen(
            (const char *)genname)+1, sizeof(I8s), 602);
        strcpy((char *)(gdfelm->gdf_name),
            (const char *)(&(genname[0])));
    }
    gdfelm->gdf_start=gdfstart;
    gdfelm->gdf_end=gdfend;
    gdfelm->gdf_typ=gdftyp;
    gdfelm->gdf_cmnt=tcalloc(strlen((
        const char *)gdfcmnt)+1,sizeof(I8s), 617);
#ifdef GENALIGN
    gdfelm->gdf_cnt=0;
#endif
    strcpy((char *)(gdfelm->gdf_cmnt),(const char *)gdfcmnt);
}

void BldGenLkup(gendef,genelkup,size)
GeneDefArr *gendef;
I16s **genelkup;
I32s size;
{   int i,j;
    GenDef *gdfelm;

    (*genelkup)=(I16s *)thcalloc(size, sizeof(**genelkup), 603);
    for(i=0;i<size;i++)
       (*genelkup)[i]=(-1);

    /* construct gene look table in g_list structure */
    for(i=0;i<size;i++)
        for(j=0;j<(gendef->gdf_ctrl.dync_elmused);j++)
        {   gdfelm=&(gendef->gdf_array[j]);
            if((i>=(gdfelm->gdf_start))&&(i<=(gdfelm->gdf_end)))
            {   (*genelkup)[i]=(I16s)j;
                break;
            }
        }
}

/*
 *  find_gen - find genotype archive index by label
 *
 * indx - pointer to indx_t structure array containing archive index
 * gen - pointer to genotype label
 * n - number of genotypes
 *
 * return : number of genotypes(n) if not found, otherwise the position
 *          (0 - [n-1]) in archive
 */

I32s find_gen(indx, gen, n)
    indx_t *indx;
    I8s *gen;
    I32s n;
{   I32s i;

    for (i = 0; i < n; i++)
        if (!strncmp((const char *)(&(((indx + i)->
            gen)[0])), (const char *)gen, 3))
            break;
    return i;
}

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
void FreeGen(g)
GList * g;
{   if (!g)
        return;
    if (g->genome)
    {   FreeGenome(g);
        g->genome = NULL;
    }
    if (g->gbits)
    {   FreeGenBits(g);
        g->gbits = NULL;
    }
    FreeThrdAnaDat(&(g->glst_thrdanadat));
    tfree((void *) g, sizeof(GList), 1402);
    g = NULL;
}

/*
 * InstrXdrWrdSize - convert length in instructions to length in
 *                   xdrwrd structures
 *
 * instrlen - length in instructions
 */
I32s InstrXdrWrdSize(instrlen)
I32s instrlen;
{   return(ByteXdrWrdSize(instrlen*sizeof(Instruction)));
}

/*
 * GenBitsXdrWrdSize - convert length in GenBits to length in
 *                   xdrwrd structures
 *
 * gbitlen - length in GenBits
 */
I32s GenBitsXdrWrdSize(gbitlen)
I32s gbitlen;
{   return(ByteXdrWrdSize(gbitlen*sizeof(GenBits)));
}

/*
 * AllocGenome - allocate genome array in GList structure
 * tgl - pointer to GList structure
 */
void AllocGenome(tgl,size)
Pgl tgl;
I32s size;
{   (tgl->genome)=(FpInst)tcalloc(
        InstrXdrWrdSize(size),sizeof(xdrwrd),1403);
}

/*
 * FreeGenome - free genome array in GList structure
 * tgl - pointer to GList structure
 */
void FreeGenome(tgl)
Pgl tgl;
{   tfree((void *)(tgl->genome),InstrXdrWrdSize(
        tgl->gen.size)*sizeof(xdrwrd),1403);
}

/*
 * AllocGenBits - allocate gbits array in GList structure
 * tgl - pointer to GList structure
 */
void AllocGenBits(tgl,size)
Pgl tgl;
{   (tgl->gbits)=(FpGenB)tcalloc(
        GenBitsXdrWrdSize(size),sizeof(xdrwrd),1404);
}


/*
 * FreeGenBits - allocate gbits array in GList structure
 * tgl - pointer to GList structure
 */
void FreeGenBits(tgl)
Pgl tgl;
{   tfree((void *)(tgl->gbits),GenBitsXdrWrdSize(
        tgl->gen.size)*sizeof(xdrwrd),1404);
}

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||defined(PROBE)

/*
 * WritAscFile - write ascii instruction mnemonic list ("source file")
 *               of genome ("disassembly" listing)
 *
 * g - pointer to GList structure for genome to be listed
 * file - pointer to "source" file name
 * tarpt -   0 - do not include thread analysis report in listing header
 *         <>0 - include thread analysis report in listing header
 * sucsiznslrat - size class migration destination node selection
 *                "success" ratio
 * sucsiznsl - size class migration destination node selection
 *             "success" count
 * siznsl    - size class migration destination node selection
 *             attempt count
 *
 * detailrpt - 0 - include thread analysis summary
 *           <>0 - include thread analysis detailed report
 *
 * clstrfmt  - 0 - no cluster analysis
 *             1 - cluster analysis format
 *
 * symclstranafmt - 0 - asymmetric cluster analysis
 *                  1 - symmetric cluster analysis
 *
 * exeptrn - 0 - non-execution pattern report
 *           1 - execution pattern report
 *
 * expttsarr - pointer to execution pattern report
 *             thread/"tissue" type array
 * expttsarrcnt - count of entries in expttsarr array
 * genelkup - gene lookup table
 */
void WritAscFile(g, file, tarpt, sucsiznslrat,
    sucsiznsl, siznsl, detailrpt, clstrfmt,
    symclstranafmt, exeptrn, expttsarr, expttsarrcnt,
    genelkup, gendef, spltisana)
Pgl g;
I8s *file;
I32s tarpt, detailrpt, clstrfmt, symclstranafmt,  *expttsarr, expttsarrcnt;
double sucsiznslrat;
I32s sucsiznsl, siznsl, exeptrn, spltisana;
I16s *genelkup;
GeneDefArr *gendef;
{   I8s bit[4], chm[4];
    I32u di, t;
#if defined(TIERRA)||defined(ARGTIE)
    I32s i;
#endif /* defined(TIERRA)||defined(ARGTIE) */
	I32s j;
    time_t tp;
    FILE *fp;

#ifdef IBM3090
    I8s lbl[4], plbl[4], *comnts;
#endif
    I8s gdt_gbits;

#if defined(TIERRA)||defined(ARGTIE)
    ThdTsTyArr codthdtstyarr[PLOIDY][NSTRTISTYP];
    I8s fstlinprt=0,svincld,incld;
    I32s lstmrk;
#endif /* defined(TIERRA)||defined(ARGTIE) */

    if (!strcmp((const char *)file, (const char *)"-"))
        fp = stdout;
    else if(!(fp=tfopen(file, (I8s *)"w")))
    {   tsprintf((char *)(&(Fbuf[0])),
           "Tierra WritAscFile() unable to open WritAscFile file %s", file);
        porterrmsg(617,(char *)(&(Fbuf[0])),1);
    }

#if defined(TIERRA)||defined(ARGTIE)
    if(tarpt||exeptrn)
        WrtThrdAnalysis(fp, g,
#ifdef NET
            sucsiznslrat, sucsiznsl, siznsl,
#else
            0.0, 0, 0,
#endif /* NET */
            detailrpt, clstrfmt, symclstranafmt,
            &codthdtstyarr, exeptrn, genelkup,
            gendef, spltisana);
#endif /* defined(TIERRA)||defined(ARGTIE) */

#ifdef ARGTIE
    if(!clstrfmt)
    {
#endif /* ARGTIE */
        WritEcoB(g->bits, Buff);
        tfprintf(fp, "\nbits: %u  %s\n", g->bits, Buff);
#ifdef IBM3090
        strcpy(lbl, g->gen.label);
        strcpy(plbl, g->parent.label);
        Ascii2Ebcdic(lbl);
        Ascii2Ebcdic(plbl);
        tfprintf(fp,
        "genotype: %.4d%s  genetic: %d,%d  parent genotype: %.4d%s\n",
            g->gen.size, lbl, g->mg.p, g->mg.s, g->parent.size, plbl);
#else
        tfprintf(fp,
        "genotype: %.4d%s  genetic: %d,%d  parent genotype: %.4d%s\n",
            g->gen.size, g->gen.label, g->mg.p, g->mg.s,
            g->parent.size, g->parent.label);
#endif
        tfprintf(fp, "1st_daughter_a:  inst: %d  instP: %d\n",
            g->d1.inst, g->d1.instP);
        tfprintf(fp,
            "1st_daughter_b:  flags: %d  mov_daught: \
            %d  breed_true: %d\n",
            g->d1.flags, g->d1.mov_daught, g->d1.BreedTrue);
        tfprintf(fp, "2nd_daughter_a:  inst: %d  instP: %d\n",
            g->d2.inst, g->d2.instP);
        tfprintf(fp,
            "2nd_daughter_b:  flags: %d  mov_daught: \
            %d  breed_true: %d\n",
            g->d2.flags, g->d2.mov_daught, g->d2.BreedTrue);
        tp = g->originC;
        tfprintf(fp, "Origin: InstExe: %d,%.6d",
                g->originI.m, g->originI.i);
        if (tp)
            tfprintf(fp, "  clock: %d  %s", g->originC, ctime(&tp));
        else
            tfprintf(fp, "\n");
        tfprintf(fp,
        "MaxPropPop: %g  MaxPropInst: %g  MaxPop: %d  mpp_time: %d,%d \n",
            g->MaxPropPop, g->MaxPropInst, g->MaxPop,
            g->mpp_time.m, g->mpp_time.i);
        tfprintf(fp, "MaxCpus: %d\n", g->max_cpus);
        tfprintf(fp, "ploidy: %d\n", g->ploidy);
        tfprintf(fp, "\nCODE\n\n");
        chm[3] = bit[3] = 0;
    
        for (j = 0; j < PLOIDY; j++)
        {   if (j)
                tfprintf(fp, "\n");
            tfprintf(fp, "track %d:\n\n", j);

            ExePtrnHead(fp, exeptrn, &(expttsarr[0]), expttsarrcnt);

#if defined(TIERRA)||defined(ARGTIE)
            incld=(I8s)(-1);
            lstmrk=(I8s)(-1);
#endif /* defined(TIERRA)||defined(ARGTIE) */

            for (t = 0; t < (I32u)(g->gen.size); t++)
            {
#if PLOIDY == 1
                di = g->genome[t];
#else /* PLOIDY > 1 */
                di = g->genome[t][j];
#endif /* PLOIDY > 1 */
                bit[0]='0';
                bit[1]='0';
                bit[2]='0';
                gdt_gbits=0;
                if(tarpt||exeptrn)
                {   if(g->glst_thrdanadat.mtad_codsegbti.mgda_segbti)
                        if(t<((I32u)(g->glst_thrdanadat.
                            mtad_codsegbti.mgda_ctrl.dync_elmaloc)))
                            if(g->glst_thrdanadat.mtad_codsegbti.
                                mgda_segbti[t])
                                gdt_gbits=g->glst_thrdanadat.
                                    mtad_codsegbti.mgda_segbti[t]
#if PLOIDY == 1
                                    ->
#else /* PLOIDY > 1 */
                                    [j].
#endif /* PLOIDY > 1 */
                                    mgdt_gbdt.gdt_gbits;
                }
                else
                {
#if PLOIDY == 1
                                gdt_gbits=g->gbits[t];
#else /* PLOIDY > 1 */
                                gdt_gbits=g->gbits[t][j];
#endif /* PLOIDY == 1 */
                }

                bit[0]=IsBit((gdt_gbits), 0) ?'1':'0';
                bit[1]=IsBit((gdt_gbits), 1) ?'1':'0';
                bit[2]=IsBit((gdt_gbits), 2) ?'1':'0';

#if defined(TIERRA)||defined(ARGTIE)
                svincld=incld;
                if((incld=IncLineChk(g,t,j,&lstmrk,
                    exeptrn,&(expttsarr[0]),
                    expttsarrcnt,&(codthdtstyarr[j][STRTISGENE]))))
                {
                    if(!svincld)
                        if(fstlinprt)
                           tfprintf(fp, "\n\n\n");
#endif /* defined(TIERRA)||defined(ARGTIE) */

                    tfprintf(fp, "%-8s; %s %.2x %3u",
                        (I8u *)((di>=((I32u)InstNum))?
                        ((I8s *)"data"):(id[di].mn)), bit, di, t);

#if defined(TIERRA)||defined(ARGTIE)
                    ExePtrnMrkLine(fp,g,t,j,exeptrn,
                        &(expttsarr[0]),expttsarrcnt,
                        &(codthdtstyarr[j][STRTISGENE]));
#endif /* defined(TIERRA)||defined(ARGTIE) */

                    tfprintf(fp, "\n");
#if defined(TIERRA)||defined(ARGTIE)
                    fstlinprt=1;
                }
#endif /* defined(TIERRA)||defined(ARGTIE) */
            }
        }
#ifdef ARGTIE
    }
#endif /* ARGTIE */

#if defined(TIERRA)||defined(ARGTIE)
    if(tarpt||exeptrn)
        if(detailrpt||exeptrn)
            for(i=0;i<PLOIDY;i++)
                for(j=0;j<((genelkup)?NSTRTISTYP:1);j++)
                    FreeDynArr((DynArr *)&(codthdtstyarr[i][j]),2212,0);
#endif /* defined(TIERRA)||defined(ARGTIE) */

    if(fp!=stdout)
    {
#ifndef AMIGA
#ifndef DECVAX
        tfflush(fp);
#endif /* DECVAX */
#endif /* AMIGA */
        tfclose(fp);
    }
}

/*
 * ExePtrnHead - execution pattern report heading
 *
 * fp - pointer to FILE structure for execution pattern report
 * exeptrn - 0 - no pattern execution run
 *           1 - pattern execution run
 * expttsarr - pointer to execution pattern report
 *             thread/"tissue" type array
 * expttsarrcnt - count of entries in expttsarr array
 */
static void ExePtrnHead(fp, exeptrn, expttsarr, expttsarrcnt)
FILE *fp;
I32s exeptrn, *expttsarr, expttsarrcnt;
{   I32s i, ncols=(expttsarrcnt<NEXEPTRNCOLS)?expttsarrcnt:NEXEPTRNCOLS;

    if(exeptrn)
    {   tfprintf(fp, "        ;            ");
        for(i=0; i<ncols; i++)
            tfprintf(fp, " %5d", expttsarr[i]);
        tfprintf(fp, "\n");
        tfprintf(fp, "        ;            ");
        for(i=0; i<ncols; i++)
            tfprintf(fp, " -----", i);
        tfprintf(fp, "\n");
        tfprintf(fp, "        ;\n");
    }
}

/*
 * IncLineChk - check for including line in disassembly listing
 *
 * g - pointer to GList structure for genome to be listed
 * t - genome offset for line to be marked
 * trkidx - track index
 * lstmrk - pointer to offset of last line marked
 * exeptrn - 0 - no pattern execution run
 *           1 - pattern execution run
 * expttsarr - pointer to execution pattern report
 *             thread/"tissue" type array
 * expttsarrcnt - count of entries in expttsarr array
 * codthdtstyarr - pointer to "tissue type" table (indexed by thread id)
 *
 * Returns:
 *
 * incld - 0 - omit line from listing
 *         1 - include line in listing
 */
static I8s IncLineChk(g, t, trkidx, lstmrk, exeptrn, expttsarr,
    expttsarrcnt, codthdtstyarr)
Pgl g;
I32s t, *expttsarr, expttsarrcnt, *lstmrk, exeptrn, trkidx;
ThdTsTyArr *codthdtstyarr;
{   I32s i,j;
    MemGenThrdDat *segbti_el;
    MemGenThrdDatEl *segbti_el2;
    I32s segbtithraloc;
    I8s incld, mrkflg=0;

    if(exeptrn)
    {   incld=0;
        if(g->glst_thrdanadat.mtad_codsegbti.mgda_segbti)
            if(t<(g->glst_thrdanadat.mtad_codsegbti.
                mgda_ctrl.dync_elmaloc))
            {   segbti_el=g->glst_thrdanadat.
                    mtad_codsegbti.mgda_segbti[t];
                if(segbti_el)
                {   segbti_el2=&(segbti_el[trkidx]);
                    segbtithraloc=segbti_el2->mgdt_thrd_ara.
                        mita_ctrl.dync_elmaloc;
                    for(i=0;i<expttsarrcnt;i++)
                    {   for(j=0;j<segbtithraloc;j++)
                            if(segbti_el2->mgdt_thrd_ara.mita_array[j])
                                if((expttsarr[i])==(
                                    codthdtstyarr->ttta_thdtsty[j]))
                                {   mrkflg=1;
                                    break;
                                }
                        if(j<segbtithraloc)
                            break;
                    }
                }
            }

        if(!mrkflg)
        {   if((*lstmrk)>=0)
                if((t-(*lstmrk))<=MAXEXEPTRNGAP)
                    incld=1;
        }
        else
        {   incld=1;
            (*lstmrk)=t;
        }
    }
    else
        incld=1;
    return incld;
}

/*
 * ExePtrnMrkLine - mark line for execution pattern report
 *
 * fp - pointer to FILE structure for execution pattern report
 * g - pointer to GList structure for genome to be listed
 * t - genome offset for line to be marked
 * trkidx - track
 * exeptrn - 0 - no pattern execution run
 *           1 - pattern execution run
 * expttsarr - pointer to execution pattern report
 *             thread/"tissue" type array
 * expttsarrcnt - count of entries in expttsarr array
 * codthdtstyarr - pointer to "tissue type" table (indexed by thread id)
 */
static void ExePtrnMrkLine(fp, g, t, trkidx, exeptrn, expttsarr,
    expttsarrcnt, codthdtstyarr )
FILE *fp;
Pgl g;
I32s t, *expttsarr, expttsarrcnt, exeptrn, trkidx;
ThdTsTyArr *codthdtstyarr;
{   I32s i,j;
    I32s segbtithraloc;
    MemGenThrdDat *segbti_el;
    MemGenThrdDatEl *segbti_el2;

    if(exeptrn)
    {   if(g->glst_thrdanadat.mtad_codsegbti.mgda_segbti)
            if(t<(g->glst_thrdanadat.mtad_codsegbti.
                mgda_ctrl.dync_elmaloc))
            {   segbti_el=g->glst_thrdanadat.
                    mtad_codsegbti.mgda_segbti[t];
                if(segbti_el)
                {   segbti_el2=&(segbti_el[trkidx]);
                    segbtithraloc=segbti_el2->mgdt_thrd_ara.
                        mita_ctrl.dync_elmaloc;
                    tfprintf(fp, " ");
                    for(i=0; i<expttsarrcnt; i++)
                    {   for(j=0; j<segbtithraloc; j++)
                            if(segbti_el2->mgdt_thrd_ara.mita_array[j])
                                if((expttsarr[i])==(codthdtstyarr->
                                    ttta_thdtsty[j]))
                                {   tfprintf(fp, " %5s", "*");
                                    break;
                                }
                        if(j==segbtithraloc)
                            tfprintf(fp, " %5s", " ");
                    }
                }
            }
    }
}

/*
 * WritEcoB - report GList structure "bits" element contents
 *            in string buffer
 *
 * bits - GList structure "bits" element contents
 *        see tierra.h for definition of bits
 * buf - scratch buffer for "bits" report
 */
void WritEcoB(bits, buf)
    I32u bits;
    I8s *buf;                   /* changed by DAN */
{   int i, j;

    if (!buf)
        return;
    tsprintf((char *)buf,
        "EX      TC      TP      MF      MT      MB      ");
    for (i = 0, j = 0; i < 6; i++, j = 0)
    {   if (IsBit(bits, 5 * i + 2))
            buf[2 + (i * 8) + j++] = 's';
        if (IsBit(bits, 5 * i + 3))
            buf[2 + (i * 8) + j++] = 'd';
        if (IsBit(bits, 5 * i + 4))
            buf[2 + (i * 8) + j++] = 'o';
        if (IsBit(bits, 5 * i + 5))
            buf[2 + (i * 8) + j++] = 'f';
        if (IsBit(bits, 5 * i + 6))
            buf[2 + (i * 8) + j++] = 'h';
    }
}
#endif /* defined(TIERRA)||defined(ARGTIE)||defined(PROBE) */
