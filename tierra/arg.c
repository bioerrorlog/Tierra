/* @(#)arg.c 1.11 12/26/95 */
/*-------------------------------------------------------------------------*/
/* arg.c 30-3-98 genebank archive utility, by T Uffner, D Pirone, & T Ray  */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998                        */
/* Tom Ray & Virtual Life                                                  */
/*-------------------------------------------------------------------------*/

#include "license.h"
#include "tierra.h"
#define  EXTERN
#include "globals.h"
#undef EXTERN
#include "soup_in.h"

#include <errno.h>

#ifdef __TRS6000__
void *malloc();
#endif

InstDef *id;

static I32s expttsarrarg P_((char *tisarg, I32s *expttsarr,
    I32s *expttsarrcnt, I32s maxtiscnt, I32s *bufpsn));
static I32s tisstrend P_((char *buf, I32s *bufpsn, char *fmt, void *fldval));
static I32s expttsarradd P_((I32s *expttsarr, I32s *expttsarrcnt,
    I32s tistyp, I32s maxtiscnt));
static int tiscmp P_((const void *obj1, const void *obj2));

/*
 * n a m e
 *
 * description
 *
 * Returns:
 */
int main(argc, argv)
    int argc;
    char *argv[];
{   I8s com, *mod;
    I16s *genelkup;
    GeneDefArr gendef;

   char *u = "usage:\n\
   arg c|r[v] <archive> <source> [<source>...]\n\
   arg t[v] <archive>\n\
   arg x[v]p tiss[-tiss][,tiss[-tiss]...] archive [genotype [genotype...]]\n\
   arg x[a[d[l][u[s]]]][v][i=<fract>] archive [genotype [genotype...]]\n";

    I8s *file = 0, m[20];
    I32s i, tarpt, v = 0, dtlrpt, clstrfmt, symclstranafmt, exeptrn, bufpsn;
    I32s spltisana;
    FILE *afp = 0;
#if PLOIDY == 1
    I32s j, splits;
    I8s rep[64];                /* added by kurt for e (entropy) option */
    I32s hist[64];              /* added by kurt for e (entropy) option */
    I32u di;                    /* added by kurt for e (entropy) option */
    I32s t;                     /* added by kurt for e (entropy) option */
    double h, p;                /* added by kurt for e (entropy) option */
#endif
#ifdef __TURBOC__
    float z = sin(0);           /* fix silly Turbo bug */
#endif /* __TURBOC__ */

    GList *g = 0;
    head_t head;
    indx_t *indx, *tindx;

    I32s expttsarr[EXPTARRSIZ], expttsarrcnt=0;
    I32s argvidx=0;

#ifdef __MWERKS__  /* get command line and parse it if on Mac */
    /* Choose to use my command line or the MWERKS dialog box */
    I8s *p1 = "\nPlease input arguments for arg separated by spaces:\n\narg ";
    fprintf(stdout, u);
    fprintf(stdout, p1);
    argc = cmdline("arg ", &argv);
#endif /* __MWERKS__ */

    tarpt=0;
    dtlrpt=0;
    clstrfmt=0;
    spltisana=0;
    symclstranafmt=0;
    exeptrn=0;
    hangup = 0;
    GeneBnker = 1;  /* turn genebanker on and off */
    NumCells = 1;  /* # of creatures and gaps used to inoculate new soup */
    XDRBufMaxSize=200000;
    enc_size_precomp();
    
    if (argc < 3)
    {   fprintf(stderr, u);
        exit(1);
    }
    GetAMap((I8s *)"opcode.map");
    argvidx++;
    switch (com = *argv[argvidx])
    {   case 'c':
        case 'r':
            if (argc < 4)
            {   fprintf(stderr, u);
                exit(2);
            }
        case 'k':
        case 't':
        case 'x':
        case 'e':
            break;
        default:
            fprintf(stderr, u);
            exit(4);
    }
    for (mod=(I8s *)(++(argv[argvidx])); *mod; mod++)
        switch (*mod)
        {   case 'v':
                ++v;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                /* f = *mod - '0'; */
                break;
            case 'a':
                tarpt=1;
                break;
            case 'd':
                dtlrpt=1;
                break;
            case 'l':
                spltisana=1;
                break;
            case 'p':
                exeptrn=1;
                break;
            case 's':
                symclstranafmt=1;
                break;
            case 'u':
                clstrfmt=1;
                break;
            default:
                fprintf(stderr, u);
                exit(5);
                break;
        }
    if(exeptrn)
    {   argvidx++;
        if(expttsarrarg(argv[argvidx], &(expttsarr[0]),
            &expttsarrcnt, NEXEPTRNCOLS, &bufpsn))
        {   fprintf(stderr, u);
            exit(5);
        }
    }
    argvidx++;
    file = (I8s *)argv[argvidx];
    switch (com)
    {
#if PLOIDY == 1
    case 'k':
        for (j = argvidx; j < argc; j++)
        {   file = (I8s *)argv[j];
            if (!(afp = fopen((const char *)file, (const char *)"rb")))
            {   perror(argv[0]);
                exit(7);
            }
            head = read_head(afp);
            indx = read_indx(afp, &head);
            for (i = 0; i < head.n; i++)
            {   tindx = &indx[i];
                g = get_gen(afp, &head, tindx, i);
                if (tindx->d1.BreedTrue)
                {   sprintf((char *)(&(m[0])), "%3s", tindx->gen);
                    fprintf(stdout, "%.4d %.3s %6d ",
                        head.size, m, tindx->mppT.m);
                    if (tindx->d1.mov_daught == head.size)
                        fprintf(stdout, " %6d %6d %6.2f .",
                            tindx->d1.mov_daught, tindx->d1.instP,
                            (float) tindx->d1.instP / (float) head.size);
                    else if (tindx->d1.mov_daught != 0)
                        fprintf(stdout, " %6d %6d %6.2f *",
                            tindx->d1.mov_daught, tindx->d1.instP,
                            (float) tindx->d1.instP / (float) head.size);
                    else
                        fprintf(stdout, " %6d %6d %6.2f *",
                            tindx->d1.mov_daught, tindx->d1.instP, 0.0);
                    /* only works if PLOIDY == 1 */
                    splits = 0;
                    for (t = 0; t < (I32s)(g->gen.size); t++)
                    {   di = g->genome[t];
                        if (strcmp((const char *)
                            (&(id[di%InstNum].mn[0])),
                            (const char *)"split") == 0)
                            splits++;
                    }
                    fprintf(stdout, " %d %d\n", splits, g->max_cpus);
                }
            }
            fclose(afp);
        }
        break;
#endif
    case 'c':
    case 'r':
        g = (GList *) calloc(1, sizeof(GList));
        InitThrdAnaDat(&(g->glst_thrdanadat));
        argvidx++;
        GetAscGen(g, (I8s *)argv[argvidx]);
        if(com == 'c')
            fprintf(stdout, "creating archive \"%s\"\n", file);
        else
            fprintf(stdout, "updating archive \"%s\"\n", file);
        if (!(afp = open_ar(file, g->gen.size, com == 'c' ? 1 : 0)))
        {   perror(argv[0]);
            exit(6);
        }
        head = read_head(afp);
        indx = read_indx(afp, &head);
        if (g)
        {   free((void *) g);
            g = NULL;
        }
        g = (GList *) calloc(1, sizeof(GList));
        InitThrdAnaDat(&(g->glst_thrdanadat));
        for (i = argvidx; i < argc; i++)
        {   int j;
            GetAscGen(g, (I8s *)argv[i]);
            if (head.size != g->gen.size)
            {   fprintf(stderr, "%s is wrong size\n", g->gen.label);
                continue;
            }
            j = add_gen(afp, &head, &indx, g);
            if (v)
            {   fprintf(stdout, "%c - %.4d%3s %.3f %.3f", j ? 'r' : 'a',
                    g->gen.size, g->gen.label, g->MaxPropPop,
                    g->MaxPropInst);
                if (IsBit(g->bits, 0))
                {   WritEcoB(g->bits, Buff);
                    fprintf(stdout, " 1 %s\n", Buff);
                }
                else
                    fprintf(stdout, " 0\n");
            }
            else
                fprintf(stdout, "%c - %3s\n", j ? 'r' : 'a', g->gen.label);
        }
        if (g)
        {   free((void *) g);
            g = NULL;
        }
        fclose(afp);
        break;
    case 't':
        if (!(afp = fopen((const char *)file, (const char *)"rb")))
        {   perror(argv[0]);
            exit(7);
        }
        head = read_head(afp);
        indx = read_indx(afp, &head);
        fprintf(stdout, "Size: %d, # of entries: %d\n\n",
            head.size, head.n);
        for (i = 0; i < head.n; i++)
        {   tindx = &indx[i];
            if (v)
            {   g = get_gen(afp, &head, tindx, i);
                sprintf((char *)(&(m[0])), "%3s", tindx->gen);
                fprintf(stdout, "%.4d%.3s %.3f %.3f", head.size, m,
                    tindx->mpp / 10000., tindx->mpi / 10000.);
                if (IsBit(tindx->bits, 0))
                {   WritEcoB(tindx->bits, Buff);
                    fprintf(stdout, " 1 %s\n", Buff);
                }
                else
                    fprintf(stdout, " 0\n");
            }
            else
                fprintf(stdout, "%.3s\n", tindx->gen);
        }
        break;
    case 'x':
        if(!strncmp(argv[argvidx],"i=",2))
        {   LifeCycFrct=atof(&(argv[argvidx][2]));
            argvidx++;
            file = (I8s *)argv[argvidx];
        }
        if (!(afp = fopen((const char *)file, (const char *)"rb")))
        {   perror(argv[0]);
            exit(9);
        }
        head = read_head(afp);
        indx = read_indx(afp, &head);
        argvidx++;
        if(argc>argvidx)
            for(i = argvidx; i < argc; i++)
            {   int j;
                if ((j = find_gen(indx, (I8s *)argv[i], head.n)) == head.n)
                {   fprintf(stderr, "%s not in archive\n", argv[i]);
                    continue;
                }
                tindx = &indx[j];
                g = get_gen(afp, &head, tindx, j);

                ReadGenDef(&gendef, &genelkup,
                    head.size, (I8s *)"", (I8s *)"");

                file = (I8s *) malloc(8);
                sprintf((char *)file, "%.4d%3s", head.size, g->gen.label);
    
#ifdef NET
                WritAscFile(g, file, tarpt,                 
                    head.hdsucsiznslrat, head.hdsvsucsiznsl,
                    head.hdsvsiznsl, dtlrpt, clstrfmt,
                    symclstranafmt, exeptrn, &(expttsarr[0]), expttsarrcnt,
                    genelkup, &gendef, spltisana);
#else
                WritAscFile(g, file, tarpt, 0.0, 0, 0,
                    dtlrpt, clstrfmt, symclstranafmt,
                    exeptrn, &(expttsarr[0]), expttsarrcnt,
                    genelkup, &gendef, spltisana);
#endif /* NET */
                if (v)
                {   fprintf(stdout, "x - %.4d%3s %.3f %.3f", g->gen.size,
                        g->gen.label, g->MaxPropPop, g->MaxPropInst);
                    if(IsBit(g->bits, 0))
                    {   WritEcoB(g->bits, Buff);
                        fprintf(stdout, " 1 %s\n", Buff);
                    }
                    else
                        fprintf(stdout, " 0\n");
                }
                else
                    fprintf(stdout, "x - %3s\n", g->gen.label);

                FreeGenDef(&gendef, &genelkup, head.size);

                if (file)
                {   free((void *) file);
                    file = NULL;
                }
                if (g)
                {   if (g->genome)
                    {   free((void *) g->genome);
                        g->genome = NULL;
                    }
                    if (g->gbits)
                    {   free((void *) g->gbits);
                        g->gbits = NULL;
                    }
                    free((void *) g);
                    g = NULL;
                }
            }
        else
            for(i = 0; i < head.n; i++)
            {   tindx = &indx[i];
                g = get_gen(afp, &head, tindx, i);

                ReadGenDef(&gendef, &genelkup,
                    head.size, (I8s *)"", (I8s *)"");

                file = (I8s *) malloc(12);
                sprintf((char *)file, "%.4d%3s", head.size, g->gen.label);
#ifdef NET
                WritAscFile(g, file, tarpt,
                    head.hdsucsiznslrat, head.hdsvsucsiznsl,
                    head.hdsvsiznsl, dtlrpt, clstrfmt,
                    symclstranafmt, exeptrn,
                    &(expttsarr[0]), expttsarrcnt, genelkup,
                    &gendef, spltisana);
#else
                WritAscFile(g, file, tarpt, 0.0, 0, 0,
                    dtlrpt, clstrfmt, symclstranafmt, exeptrn,
                    &(expttsarr[0]), expttsarrcnt, genelkup,
                    &gendef, spltisana);
#endif /* NET */
    
    
    
                if (v)
                {   fprintf(stdout, "x - %.4d%3s %.3f %.3f",
                        g->gen.size, g->gen.label,
                        g->MaxPropPop, g->MaxPropInst);
                    if (IsBit(g->bits, 0))
                    {   WritEcoB(g->bits, Buff);
                        fprintf(stdout, " 1 %s\n", Buff);
                    }
                    else
                        fprintf(stdout, " 0\n");
                }
                else
                    fprintf(stdout, "x - %3s\n", g->gen.label);
                if (file)
                {   free((void *) file);
                    file = NULL;
                }
                if (g)
                {   if (g->genome)
                    {   free((void *) g->genome);
                        g->genome = NULL;
                    }
                    if (g->gbits)
                    {   free((void *) g->gbits);
                        g->gbits = NULL;
                    }
                    free((void *) g);
                    g = NULL;
                }
            }
        break;
#if PLOIDY == 1
    case 'e':
        rep[0] = 0;
        for (i = 1; i < InstNum; i++)
            for (j = 0; j < i; j++)
            {   if (strncmp((const char *)(&(id[i].mn[0])),
                    (const char *)(&(id[j].mn[0])), 9) == 0)
                {   rep[i] = (I8s)j;
                    break;
                }
                else
                    rep[i] = (I8s)i;
            }

        for (j = argvidx; j < argc; j++)
        {   file = (I8s *)argv[j];
            if (!(afp = fopen((const char *)file, (const char *)"rb")))
            {   perror(argv[0]);
                exit(7);
            }
            head = read_head(afp);
            indx = read_indx(afp, &head);
            for (i = 0; i < head.n; i++)
            {   tindx = &indx[i];
                g = get_gen(afp, &head, tindx, i);
                if (tindx->d1.BreedTrue)
                {   sprintf((char *)m, "%3s", tindx->gen);
                    fprintf(stdout, "%.4d %.3s %6d ",
                        head.size, m, tindx->mppT.m);
                    if (tindx->d1.mov_daught == head.size)
                        fprintf(stdout, " %6d %6d %6.2f .",
                            tindx->d1.mov_daught,
                            tindx->d1.instP, (float) tindx->d1.instP /
                            (float) head.size);
                    else if (tindx->d1.mov_daught != 0)
                        fprintf(stdout, " %6d %6d %6.2f *",
                            tindx->d1.mov_daught, tindx->d1.instP,
                            (float) tindx->d1.instP / (float) head.size);
                    else
                        fprintf(stdout, " %6d %6d %6.2f *",
                            tindx->d1.mov_daught, tindx->d1.instP, 0.0);
                    /* only works if PLOIDY == 1 */
                    for (t = 0; t < InstNum; t++)
                        hist[t] = 0;
                    for (t = 0; t < g->gen.size; t++)
                    {   di = g->genome[t];
                        hist[(int)rep[(int)id[di].op]]++;
                    }
                    /* only works if PLOIDY == 1 */
                    for (t = 0; t < InstNum; t++)
                        hist[t] = 0;
                    for (t = 0; t < g->gen.size; t++)
                    {   di = g->genome[t];
                        hist[(int)rep[(int)id[di].op]]++;
                    }
                    h = 0.0;
                    for (t = 0; t < InstNum; t++)
                        if (hist[t])
                        {   p = ((double) hist[t]) / g->gen.size;
                            h += p * log(p);
                        }
                    h /= -log(2.0);
                    fprintf(stdout, "  %7.3f\n", h);
                }
            }
            fclose(afp);
        }
        break;
#endif
    }
    return 0;
}


/*
 * expttsarrarg - generate execution pattern tissue array
 *
 * tisarg - tissue type specification string
 * expttsarr - execution pattern tissue type array
 * expttsarrcnt - number of elements in expttsarr array
 * maxtiscnt - maximum tissue count
 *
 * returns: errflag = 0 - no error
 *                    1 - error
 */
static I32s expttsarrarg(tisarg, expttsarr, expttsarrcnt, maxtiscnt, bufpsn)
char *tisarg;
I32s *expttsarr, *expttsarrcnt, maxtiscnt, *bufpsn;
{   I32s brkflg=0, intrvstr=0, i;
    I32s intrvend=0, rtnval, errflag=1;
    char delim='\0';

    (*bufpsn)=0;
    for(;!brkflg;)
    {   brkflg=1;
        if(!tisstrend(tisarg, bufpsn, "%d%n", &intrvstr))
        {   if(!(rtnval=tisstrend(tisarg, bufpsn, "%c%n", &delim)))
            {   if(delim=='-')
                {   if(!tisstrend(tisarg, bufpsn, "%d%n", &intrvend))
                    {   for(i=intrvstr;i<=intrvend;i++)
                            if(expttsarradd(expttsarr, expttsarrcnt,
                                i, maxtiscnt))
                                break;
                        if(i>intrvend)
                        {   if(!(rtnval=tisstrend(tisarg, bufpsn,
                                "%c%n", &delim)))
                            {   if(delim==',')
                                    brkflg=0;
                            }
                            else
                                if(rtnval!=1)
                                    errflag=0;
                        }
                    }
                }
                else
                    if(delim==',')
                        if(!expttsarradd(expttsarr, expttsarrcnt,
                            intrvstr, maxtiscnt))
                            brkflg=0;
            }
            else
                if(rtnval!=1)
                    if(!expttsarradd(expttsarr, expttsarrcnt,
                        intrvstr, maxtiscnt))
                        errflag=0;
        }
    }
    if(!errflag)
        qsort(expttsarr, *expttsarrcnt, sizeof(int), tiscmp);
    return errflag;
}

/*
 * tisstrend - get thread range start/end from string
 *
 * buf - string
 * bufpsn - pointer to current buffer position
 * fmt - format for sscanf()
 * fldval - pointer to start/end value read
 *
 * Returns:
 *    *bufpsn - updated buffer position
 *    *fldval - start/end value read 
 *    rtnval - 0 - no error
 *             1 - error
 *             2 - end of string
 */
static I32s tisstrend(buf,bufpsn,fmt,fldval)
char *buf, *fmt;
I32s *bufpsn;
void *fldval;
{   I32s rtnval=0, bufpsninc;
    if(buf[*bufpsn])
        if(sscanf((const char *)(&(buf[*bufpsn])),
            fmt, fldval, &bufpsninc))
        {   (*bufpsn)+=bufpsninc;
            rtnval=0;
        }
        else
            rtnval=1;
    else
        rtnval=2;
    return rtnval;
}

/*
 * expttsarradd - add thread to execution pattern thread array
 *
 * expttsarr - thread array
 * expttsarrcnt - number of elements in thread array
 * tistyp - thread id to be added to thread array
 * maxtiscnt - maximum tissue count
 *
 * returns: rtnval = 0 - no error
 *                   1 - error
 */
static I32s expttsarradd(expttsarr, expttsarrcnt, tistyp, maxtiscnt)
I32s *expttsarr, *expttsarrcnt, tistyp, maxtiscnt;
{   I32s i, rtnval=1;
    if(tistyp>=0)
        if((*expttsarrcnt)<maxtiscnt)
        {   for(i=0;i<(*expttsarrcnt);i++)
                if(expttsarr[i]==tistyp)
                    break;
            if(i==(*expttsarrcnt))
                expttsarr[(*expttsarrcnt)++]=tistyp;
            rtnval=0;
        }
    return rtnval;
}

/*
 * tiscmp - compare function for execution pattern thread array qsort
 */
static int tiscmp(obj1, obj2)
const void *obj1, *obj2;
{   return (((*((int *)obj1))<(*((int *)obj2)))?(-1):1);
}
