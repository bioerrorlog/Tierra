#define XDRBUFMAXSIZE 200000
#define MAXOFNLEN 255

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "tierra.h"
#define EXTERN
#include "globals.h"
#undef   EXTERN
#include "soup_in.h"
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif /* _WIN32 */
#include "stralign.h"

typedef struct
{   FILE   *gasd_fp;
    head_t  gasd_head;
    indx_t *gasd_indx;
    Pgl     gasd_g;
    int     n;
} GenAlignSide;

typedef struct
{   char   *arg_prgnam;
    char   *(arg_files[NUMSTRINGS]);
    char   *(arg_genlbls[NUMSTRINGS]);
    int     arg_doall;
    double  arg_mtchwgt;
    double  arg_mismtchwgt;
    double  arg_gapopn;
    double  arg_gapext;
    double  arg_threshfact;
    int     arg_prtalign;
    int     arg_prtunalign;
    int     arg_useglist;
} args;

typedef struct
{   int            bktrc_execnt;
    GenAlignSide  *bktrc_UnknownSide;
    int            bktrc_useglist;
} BkTrcDat;

typedef struct
{   int            prald_dupgen;
    int            prald_execnt;
    GeneDefArr    *prald_unkgendef;
    int           *prald_fstlinprtd;
    int            prald_nancgenmtch;
    FILE          *prald_ofp;
    int            prald_prtalign;
    int            prald_prtunalign;
    GenDef        *prald_gdfelm;
    double         prald_mtchwgt;
    Instruction  **prald_bufs;
} PrtAlignDat;

int main P_((int argc,char **argv));
static GenAlignSide *OpenGenAlignSide P_((char *file,char *genlbl));
static void CloseGenAlignSide P_((GenAlignSide *GenAlignSideBuf));
static I8s GetGbits P_((Pgl g,int idx,int trk,int useglist));
static void usage P_((args *argbuf));
static int getargs P_((int argc,char **argv,args *argbuf));
static void argdefaults P_((char **argv,args *argbuf));
static int PrintNewGenes P_((int trk,I16s *genelkup,
    args *argbuf,GenAlignSide *UnknownSide,
    GeneDefArr *unkgendef,FILE *ofp));
static int FindGenDef P_((GeneDefArr *gendef,int genstrt,int genend));
static int newgensizcatcmp P_((const void *el1,const void *el2));
static void AddNewGeneSizeCat P_((GeneDefArr
    *newgensizcat,int genstrt,int genend));
static int NewGeneSizeCat P_((GeneDefArr *newgensizcat,
    int size,int trk,int useglist,I16s *genelkup,
    GenAlignSide *UnknownSide));
static int GeneSetAlign P_((GeneDefArr *knwgendef,
    args *argbuf,int trk,int *fstlinprtd,
    GenAlignSide *UnknownSide,GenAlignSide *KnownSide,
    GeneDefArr *unkgendef,FILE *ofp));
static int FndIntUnkGenCorrLoc P_((
    match *F,int *ix,int *dims,
    int *genstrt,int *genend,GeneDefArr *unkgendef));
static int ChkPrtAlign P_((int nancgenmtch,
    match *F,int *ix,int *dims,GeneDefArr *gendef));
static int NextGenAlignSide P_((GenAlignSide *GenAlignSideBuf));
static void gdftitle P_((FILE *ofp,
    GenAlignSide *UnknownSide,GenAlignSide *KnownSide,
    args *argbuf));
static FILE *cretmpfil P_((char *filnam,char *prgnam));
static int mtchcompar P_((const void *a1,const void *a2));
static void bk_trc_init        P_((
    void *userdat,match *F,int *ix,
    int *dims,int inalign,int trk));
static void bk_trc_enter_align P_((
    void *userdat,match *F,int *ix,
    int *dims,int inalign,int trk));
static void bk_trc_leave_align P_((
    void *userdat,match *F,int *ix,
    int *dims,int inalign,int trk));
static void bk_trc_iteration   P_((
    void *userdat,match *F,int *ix,
    int *dims,int inalign,int trk));
static void prt_align_enteralign P_((
    void *userdat,match *F,int *ix,
    int *dims,int mtch_type));
static void prt_align_aligndtl P_((
    void *userdat,match *F,int *ix,
    int *dims, int mtch_type));
static void prt_align_exitalign P_((
    void *userdat,match *F,int *ix,
    int *dims,int mtch_type));

int main(argc,argv)
int argc;
char **argv;
{
    args argbuf;
    GenAlignSide *UnknownSide,*KnownSide;
    int trk,fstlinprtd=0,errflag=0;
    GeneDefArr gendef[NUMSTRINGS];
    I16s *(genelkup[NUMSTRINGS]);
    FILE *ofp;
    char ofn[MAXOFNLEN],tmpn[MAXOFNLEN],*gendir;

#ifdef _WIN32
    srand(time(NULL));
#endif /* _WIN32 */

    XDRBufMaxSize=XDRBUFMAXSIZE;

    enc_size_precomp();
    if(!getargs(argc,argv,&argbuf))
    {   GetAMap((I8s *)"opcode.map");

        if((KnownSide=OpenGenAlignSide(
            (argbuf.arg_files)[KNOWNSIDE],
            (argbuf.arg_genlbls)[KNOWNSIDE])))
        {
            ReadGenDef(&(gendef[KNOWNSIDE]),
                &(genelkup[KNOWNSIDE]),
                KnownSide->gasd_head.size,&(KnownSide->
                gasd_g->gen.label[0]),(I8s *)"");

            if((UnknownSide=OpenGenAlignSide(
                (argbuf.arg_files)[UNKNOWNSIDE],
                (argbuf.arg_genlbls)[UNKNOWNSIDE])))
            {   for(;;)
                {   if((ofp=cretmpfil(&(tmpn[0]),argbuf.arg_prgnam)))
                    {   gdftitle(ofp,UnknownSide,KnownSide,&argbuf);
                        for(trk=0;trk<PLOIDY;trk++)
                        {   InitDynArr((DynArr *)(&gendef[UNKNOWNSIDE]),
                                sizeof(GenDef),0);
            
                            if(!(errflag=GeneSetAlign(
                                &(gendef[KNOWNSIDE]),
                                &argbuf,trk,&fstlinprtd,
                                UnknownSide,KnownSide,
                                &(gendef[UNKNOWNSIDE]),ofp)))
                            {   BldGenLkup(&(gendef[UNKNOWNSIDE]),
                                    &(genelkup[UNKNOWNSIDE]),
                                    UnknownSide->gasd_head.size);
                
                                /* print new genes */
                                errflag=PrintNewGenes(trk,genelkup[
                                    UNKNOWNSIDE],&argbuf,UnknownSide,
                                    &(gendef[UNKNOWNSIDE]),ofp);
                
                                FreeGenDef(&(gendef[UNKNOWNSIDE]),
                                    &(genelkup[UNKNOWNSIDE]),
                                    UnknownSide->gasd_head.size);
                            }
                            if(errflag)
                                break;
                        }
                        fclose(ofp);
                        if(!errflag)
                        {   if((gendir=(char *)getdirnam((I8s *)(argbuf.
                                arg_files)[UNKNOWNSIDE])))
                            {   sprintf(&(ofn[0]),"%s%.4d%s.gdf",gendir,
                                    UnknownSide->gasd_g->gen.size,
                                    UnknownSide->gasd_g->gen.label);
                                rename(&(tmpn[0]),&(ofn[0]));
                                free(gendir);
                            }
                            else
                            {   unlink(&(tmpn[0]));
                                errflag=1;
                            }
                        }
                        else
                            unlink(&(tmpn[0]));
                    }
                    else
                    {   perror(argv[0]);
                        errflag=1;
                    }
                    if(errflag)
                        break;
                    else if(!(argbuf.arg_doall))
                        break;
                    else if((errflag=NextGenAlignSide(UnknownSide)))
                        break;
                }
                CloseGenAlignSide(UnknownSide);
            }
            else
                errflag=1;

            FreeGenDef(&(gendef[KNOWNSIDE]),
                &(genelkup[KNOWNSIDE]),
                KnownSide->gasd_head.size);
            CloseGenAlignSide(KnownSide);
        }
        else
            errflag=1;
    }
    return 0;
}

static GenAlignSide *OpenGenAlignSide(file,genlbl)
char *file,*genlbl;
{   int errflag=0;
    GenAlignSide *GenAlignSideBuf;
    if((GenAlignSideBuf=(GenAlignSide *)
        malloc(sizeof(GenAlignSide))))
    {   if((GenAlignSideBuf->gasd_fp=fopen(file,"rb")))
        {   GenAlignSideBuf->gasd_head=
                read_head(GenAlignSideBuf->gasd_fp);
            GenAlignSideBuf->gasd_indx=
                read_indx(GenAlignSideBuf->gasd_fp,
                &(GenAlignSideBuf->gasd_head));
            if(((GenAlignSideBuf->n)=(
                strlen(genlbl))?find_gen(
                GenAlignSideBuf->gasd_indx,
                (I8s *)genlbl,GenAlignSideBuf->
                gasd_head.n):0)<(GenAlignSideBuf->
                gasd_head.n))
            {   if(!(GenAlignSideBuf->gasd_g=
                    get_gen(GenAlignSideBuf->gasd_fp,
                    &(GenAlignSideBuf->gasd_head),
                    &((GenAlignSideBuf->gasd_indx)[
                    GenAlignSideBuf->n]),GenAlignSideBuf->n)))
                    errflag=1;
            }
            else
                errflag=1;
            if(errflag)
            {   free(GenAlignSideBuf->gasd_indx);
                fclose(GenAlignSideBuf->gasd_fp);
            }
        }
        else
        {   perror("OpenGenAlignSide");
            errflag=1;
        }
        if(errflag)
        {   free(GenAlignSideBuf);
            GenAlignSideBuf=NULL;
        }
    }
    else
    {   perror("OpenGenAlignSide");
        errflag=1;
    }
    return GenAlignSideBuf;
}

static void CloseGenAlignSide(GenAlignSideBuf)
GenAlignSide *GenAlignSideBuf;
{   FreeGen(GenAlignSideBuf->gasd_g);
    free(GenAlignSideBuf->gasd_indx);
    fclose(GenAlignSideBuf->gasd_fp);
    free(GenAlignSideBuf);
}

static I8s GetGbits(g,idx,trk,useglist)
Pgl g;
int idx,trk,useglist;
{   I8s gbits=0;

    if(idx>=0)
    {   if(g->glst_thrdanadat.mtad_codsegbti.mgda_segbti)
        {   if(idx<((int)(g->glst_thrdanadat.
                mtad_codsegbti.mgda_ctrl.dync_elmaloc)))
                if(g->glst_thrdanadat.
                    mtad_codsegbti.mgda_segbti[idx])
                {   gbits=g->glst_thrdanadat.
                        mtad_codsegbti.mgda_segbti[idx][trk].
                        mgdt_gbdt.gdt_gbits;
                }
        }
        else if(useglist)
        {
#if PLOIDY == 1
            gbits=g->gbits[idx];
#else /* PLOIDY > 1 */
            gbits=g->gbits[idx][trk];
#endif /* PLOIDY == 1 */
        }
    }
    return gbits;
}

static void usage(argbuf)
args *argbuf;
{   printf("%s <unknown genebank archive>\n",argbuf->arg_prgnam);
    printf("         [-ul  <unknown genome label   (%8s) ]\n",
        (!strlen(&((argbuf->arg_genlbls)
        [UNKNOWNSIDE][0])))?("do all"):
        &((argbuf->arg_genlbls)[UNKNOWNSIDE][0]));
    printf("         [-kg  <known genebank archive (%8s)>]\n",
        &((argbuf->arg_files)[KNOWNSIDE][0]));
    printf("         [-kl  <known genome label     (%8s) ]\n",
        (!strlen(&((argbuf->arg_genlbls)
        [KNOWNSIDE][0])))?("use 1st"):
        &((argbuf->arg_genlbls)[KNOWNSIDE][0]));
    printf("         [-tf  <threshold factor       (%8.3f)>]\n",
        argbuf->arg_threshfact);
    printf("         [-maw <match weight           (%8.3f)>]\n",
        argbuf->arg_mtchwgt);
    printf("         [-miw <mismatch weight        (%8.3f)>]\n",
        argbuf->arg_mismtchwgt);
    printf("         [-go  <gap open               (%8.3f)>]\n",
        argbuf->arg_gapopn);
    printf("         [-ge  <gap ext                (%8.3f)>]\n",
        argbuf->arg_gapext);
    printf("         [-pa                          (%8s)>]\n",
        (argbuf->arg_prtalign)?"yes":"no");
    printf("         [-ug                          (%8s)>]\n",
        (argbuf->arg_useglist)?"yes":"no");
    printf("\n");
    printf("-pa = print alignment\n");
    printf("-ug = use Glist gbits if no thread analysis data\n");
    printf("\n");
}

static void argdefaults(argv,argbuf)
char **argv;
args *argbuf;
{   argbuf->arg_prgnam=argv[0];
    argbuf->arg_files[UNKNOWNSIDE]="";
    argbuf->arg_files[KNOWNSIDE]="0960.gen";
    argbuf->arg_genlbls[UNKNOWNSIDE]="";
    (argbuf->arg_doall)=(strlen((argbuf->
        arg_genlbls)[UNKNOWNSIDE]))?0:1;
    argbuf->arg_genlbls[KNOWNSIDE]="";
    argbuf->arg_threshfact=0.5;
    argbuf->arg_mtchwgt=1.0;
    argbuf->arg_mismtchwgt=-1.0;
    argbuf->arg_gapopn=-1.0;
    argbuf->arg_gapext=-0.2;
    argbuf->arg_prtalign=0;
    argbuf->arg_prtunalign=0;
    argbuf->arg_useglist=0;
}

static int getargs(argc,argv,argbuf)
int argc;
char **argv;
args *argbuf;
{   int i,exitflag=0;
    argdefaults(argv,argbuf);
    if(argc>1)
    {   for(i=1;i<argc;i++)
        {   if(argv[i][0]=='-')
            {   if(!strcmp(&(argv[i][1]),"ul"))
                {   (argbuf->arg_genlbls)[UNKNOWNSIDE]=argv[++i];
                    (argbuf->arg_doall)=(strlen((argbuf->
                        arg_genlbls)[UNKNOWNSIDE]))?0:1;
                }
                else if(!strcmp(&(argv[i][1]),"kg"))
                    (argbuf->arg_files)[KNOWNSIDE]=argv[++i];
                else if(!strcmp(&(argv[i][1]),"kl"))
                    (argbuf->arg_genlbls)[KNOWNSIDE]=argv[++i];
                else if(!strcmp(&(argv[i][1]),"tf"))
                    (argbuf->arg_threshfact)=atof(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"maw"))
                    (argbuf->arg_mtchwgt)=atof(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"miw"))
                    (argbuf->arg_mismtchwgt)=atof(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"go"))
                    (argbuf->arg_gapopn)=atof(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"ge"))
                    (argbuf->arg_gapext)=atof(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"pa"))
                    (argbuf->arg_prtalign)=1;
                else if(!strcmp(&(argv[i][1]),"ug"))
                    (argbuf->arg_useglist)=1;
                else
                {   usage(argbuf);
                    exitflag=1;
                    break;
                }
            }
            else
                (argbuf->arg_files)[UNKNOWNSIDE]=argv[i];
        }
    }
    else
    {   usage(argbuf);
        exitflag=1;
    }
    return exitflag;
}

static int PrintNewGenes(trk,genelkup,argbuf,UnknownSide,unkgendef,ofp)
int trk;
I16s *genelkup;
args *argbuf;
GenAlignSide *UnknownSide;
GeneDefArr *unkgendef;
FILE *ofp;
{   GeneDefArr newgensizcat;
    int fstlinprtd=0,errflag=0;

    if(!(errflag=NewGeneSizeCat(&newgensizcat,UnknownSide->gasd_head.size,
        trk,argbuf->arg_useglist,genelkup,UnknownSide)))
    {   errflag=GeneSetAlign(&newgensizcat,
            argbuf,trk,&fstlinprtd,UnknownSide,
            UnknownSide,unkgendef,ofp);

        FreeGenDef(&newgensizcat,NULL,0);
    }
    return errflag;
}

static int FindGenDef(gendef,genstrt,genend)
GeneDefArr *gendef;
int genstrt,genend;
{   int i;
    GenDef *gdfelm;
    for(i=0;i<(gendef->gdf_ctrl.dync_elmused);i++)
    {   gdfelm=&(gendef->gdf_array[i]);
        if((genend>=(gdfelm->gdf_start))&&
            (genstrt<=(gdfelm->gdf_end)))
            break;
    }
    return((i<(gendef->gdf_ctrl.dync_elmused))?i:-1);
}

static int NewGeneSizeCat(newgensizcat,size,
    trk,useglist,genelkup,UnknownSide)
GeneDefArr *newgensizcat;
int size,trk,useglist;
GenAlignSide *UnknownSide;
I16s *genelkup;
{   int i,fstgen=0,genstrt=0,genend=0,gencnt=0,errflag=0;
    char gdfnam[MAXGENDEFLINLEN];

    InitDynArr((DynArr *)newgensizcat,sizeof(GenDef),0);
    for(i=0;i<size;i++)
        if(GetGbits(UnknownSide->gasd_g,i,trk,useglist)&0x1)
            if(genelkup[i]<0)
            {   if(!fstgen)
                {  genend=genstrt=i;
                   fstgen=1;
                }
                else if(i==(genend+1))
                    genend=i;
                else
                {   AddNewGeneSizeCat(newgensizcat,genstrt,genend);
                    genend=genstrt=i;
                }
            }
    if(fstgen)
        AddNewGeneSizeCat(newgensizcat,genstrt,genend);
    qsort(newgensizcat->gdf_array,newgensizcat->
        gdf_ctrl.dync_elmused,sizeof(GenDef),newgensizcatcmp);
    for(i=0;i<(newgensizcat->gdf_ctrl.dync_elmused);i++)
    {   sprintf(&(gdfnam[0]),"%s%d","gene",++gencnt);
        if(!(newgensizcat->gdf_array[i].gdf_name=
            calloc(strlen(&(gdfnam[0]))+1,sizeof(I8s))))
        {   perror("NewGeneSizeCat");
            errflag=1;
            break;
        }
    }
    if(errflag)
        FreeGenDef(newgensizcat,NULL,0);
    return errflag;
}

static void AddNewGeneSizeCat(newgensizcat,genstrt,genend)
GeneDefArr *newgensizcat;
int genstrt,genend;
{   int genlen;
    char cmnt[MAXGENDEFLINLEN];
    genlen=(genend-genstrt)+1;
    sprintf(&(cmnt[0]),"; primary is %d instr",genlen);
    AddGenDef(newgensizcat,NULL,(I8s *)(&(cmnt[0])),genstrt,genend,0);
}

static int newgensizcatcmp(el1,el2)
const void *el1,*el2;
{   int gdfstart1=((GenDef *)el1)->gdf_start;
    int gdfstart2=((GenDef *)el2)->gdf_start;

    int gensize1=((((GenDef *)el1)->gdf_end)-gdfstart1)+1;
    int gensize2=((((GenDef *)el2)->gdf_end)-gdfstart2)+1;
    return((gensize1<gensize2)?1:
        ((gensize1==gensize2)?
        ((gdfstart1<gdfstart2)?-1:
        ((gdfstart1==gdfstart2)?0:1)):-1));
}

static int GeneSetAlign(knwgendef,argbuf,trk,
    fstlinprtd,UnknownSide,KnownSide,unkgendef,ofp)
GeneDefArr *knwgendef,*unkgendef;
int trk,*fstlinprtd;
args *argbuf;
GenAlignSide *KnownSide,*UnknownSide;
FILE *ofp;
{   int geneidx,dims[NUMSTRINGS],errflag=0,
        domatch,nancgenmtch,gencnt=0;
    GenDef *gdfelm;
    Instruction *(bufs[NUMSTRINGS]);
    match *F;
    char gdfnam[MAXGENDEFLINLEN];
    BkTrcDat stralnusrdat;
    PrtAlignDat prtalnusrdat;

    bufs[UNKNOWNSIDE]=UnknownSide->gasd_g->genome;
    dims[UNKNOWNSIDE]=UnknownSide->gasd_head.size;
    nancgenmtch=(UnknownSide==KnownSide)?1:0;

    for(geneidx=0;geneidx<(knwgendef->
        gdf_ctrl.dync_elmused);geneidx++)
    {   gdfelm=&(knwgendef->gdf_array[geneidx]);
        gdfelm->gdf_cnt=0;
        bufs[KNOWNSIDE]=&((KnownSide->gasd_g->
            genome)[gdfelm->gdf_start]);
        dims[KNOWNSIDE]=((gdfelm->gdf_end)-
            (gdfelm->gdf_start))+1;

        domatch=1;
        if(nancgenmtch)
        {   if(FindGenDef(unkgendef,gdfelm->
                gdf_start,gdfelm->gdf_end)>=0)
                domatch=0;
            else
            {   sprintf(&(gdfnam[0]),"%s%d","gene",++gencnt);
                if((gdfelm->gdf_name=calloc(
                    strlen(&(gdfnam[0]))+1,1)))
                    strcpy((char *)(gdfelm->gdf_name),
                        (const char *)(&(gdfnam[0])));
                else
                {   perror("GeneSetAlign");
                    errflag=1;
                    break;
                }
            }
        }

        if(domatch)
        {   stralnusrdat.bktrc_UnknownSide=UnknownSide;
            stralnusrdat.bktrc_useglist=argbuf->arg_useglist;
            if((F=(match *)stralign((void **)(&(bufs[0])),
                &(dims[0]),&mtchcompar,
                argbuf->arg_threshfact,argbuf->arg_mtchwgt,
                argbuf->arg_mismtchwgt,argbuf->arg_gapopn,
                argbuf->arg_gapext,trk,sizeof(Inst),&stralnusrdat,
                &bk_trc_init,&bk_trc_enter_align,
                &bk_trc_leave_align,&bk_trc_iteration)))
            {
                /* print alignment */
                prtalnusrdat.prald_unkgendef=unkgendef;
                prtalnusrdat.prald_nancgenmtch=nancgenmtch;
                prtalnusrdat.prald_ofp=ofp;
                prtalnusrdat.prald_prtalign=argbuf->arg_prtalign;
                prtalnusrdat.prald_prtunalign=argbuf->arg_prtunalign;
                prtalnusrdat.prald_gdfelm=gdfelm;
                prtalnusrdat.prald_mtchwgt=argbuf->arg_mtchwgt;
                prtalnusrdat.prald_bufs=&(bufs[0]);
                prtalnusrdat.prald_fstlinprtd=fstlinprtd;
                PrtStrAlign(F,&(dims[0]),&prtalnusrdat,
                    &prt_align_enteralign,&prt_align_aligndtl,
                    &prt_align_exitalign);
                free(F);
            }
            else
            {   perror("GeneSetAlign");
                errflag=1;
                break;
            }
        }
    }
    return errflag;
}

static int FndIntUnkGenCorrLoc(
    F,ix,dims,genstrt,genend,unkgendef)
int *ix,*dims,*genstrt,*genend;
match *F;
GeneDefArr *unkgendef;
{   int locfnd=0;
    (*genstrt)=(ix[UNKNOWNSIDE]>0)?ix[UNKNOWNSIDE]-1:0;
    (*genend)=((F[sb(ix[UNKNOWNSIDE],
        ix[KNOWNSIDE])].mtch_endix)>0)?
        (F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].
        mtch_endix-1):0;
    if(FindGenDef(unkgendef,*genstrt,*genend)>=0)
       locfnd=1;
    return locfnd;
}

static int ChkPrtAlign(nancgenmtch,F,ix,dims,gendef)
int nancgenmtch,*ix,*dims;
match *F;
GeneDefArr *gendef;
{   int prtalign,genstrt,genend;
    prtalign=1;
    if(nancgenmtch)
        if(FndIntUnkGenCorrLoc(
            F,&(ix[0]),dims,
            &genstrt,&genend,gendef))
            if(((ix[UNKNOWNSIDE]-1)<genstrt)||
                ((ix[UNKNOWNSIDE]-1)>genend))
                if(FindGenDef(gendef,ix[UNKNOWNSIDE]-1,
                    ix[UNKNOWNSIDE]-1)>=0)
                    prtalign=0;
    return prtalign;
}

static int NextGenAlignSide(GenAlignSideBuf)
GenAlignSide *GenAlignSideBuf;
{   int errflag=0;
    if(((GenAlignSideBuf->n))<
        ((GenAlignSideBuf->gasd_head.n)-1))
    {   (GenAlignSideBuf->n)++;
        FreeGen(GenAlignSideBuf->gasd_g);
        if(!((GenAlignSideBuf->gasd_g)=
            get_gen(GenAlignSideBuf->gasd_fp,
            &(GenAlignSideBuf->gasd_head),
            &((GenAlignSideBuf->gasd_indx)[
            GenAlignSideBuf->n]),
            GenAlignSideBuf->n)))
            errflag=1;
    }
    else
        errflag=1;
    return errflag;
}

static void gdftitle(ofp,UnknownSide,KnownSide,argbuf)
FILE *ofp;
GenAlignSide *UnknownSide,*KnownSide;
args *argbuf;
{
    printf("%.4d%s\n",
        UnknownSide->gasd_g->gen.size,
        UnknownSide->gasd_g->gen.label);

    fprintf(ofp,"; gene definitions for %.4d%s\n",
        UnknownSide->gasd_g->gen.size,
        UnknownSide->gasd_g->gen.label);

    fprintf(ofp,"; file name : %s\n",
        (argbuf->arg_files)[UNKNOWNSIDE]);

    fprintf(ofp,
        "; standard genotype used : %.4d%s\n",
        KnownSide->gasd_g->gen.size,
        KnownSide->gasd_g->gen.label);
    fprintf(ofp,
        "; threshold factor       = %8.3f\n",
        argbuf->arg_threshfact);
    fprintf(ofp,
        "; match weight           = %8.3f\n",
        argbuf->arg_mtchwgt);
    fprintf(ofp,
        "; mismatch weight        = %8.3f\n",
        argbuf->arg_mismtchwgt);
    fprintf(ofp,
        "; gap open               = %8.3f\n",
        argbuf->arg_gapopn);
    fprintf(ofp,
        "; gap ext                = %8.3f\n",
        argbuf->arg_gapext);
    if(UnknownSide->gasd_g->glst_thrdanadat.
        mtad_codsegbti.mgda_segbti)
        fprintf(ofp,"; note: thread analysis data used\n");
    else
    {   fprintf(ofp,"; note: thread analysis data not used,");
        fprintf(ofp," table may be incomplete\n");
    }
}

static FILE *cretmpfil(filnam,prgnam)
char *filnam,*prgnam;
{   FILE *filptr=NULL;
    int fd;

#ifdef _WIN32
    for(;;)
    {   sprintf(filnam,"%s.%d\n",rand());
        if((fd=open(filnam,_O_WRONLY|_O_CREAT|_O_EXCL))>=0)
#else /* _WIN32 */
        sprintf(filnam,"%s.XXXXXX",prgnam);
        if((fd=mkstemp(filnam))>=0)
#endif /* _WIN32 */
        {   if(!(filptr=fdopen(fd,"w")))
            {   close(fd);
                unlink(filnam);
            }
#ifdef _WIN32
            break;
#endif /* _WIN32 */
        }
#ifdef _WIN32
        else if(errno!=EEXIST)
            break;
    }
#endif /* _WIN32 */
    return filptr;
}

static int mtchcompar(const void *a1,const void *a2)
{   return(((*((Inst *)a1))==(*((Inst *)a2)))?1:0);
}

static void bk_trc_init(userdat,F,ix,dims,inalign,trk)
void *userdat;
match *F;
int *ix,*dims,inalign,trk;
{   ((BkTrcDat *)userdat)->bktrc_execnt=0;
}

static void bk_trc_iteration(userdat,F,ix,dims,inalign,trk)
void *userdat;
match *F;
int *ix,*dims,inalign,trk;
{   if(inalign)
        if(GetGbits(((BkTrcDat *)userdat)->
            bktrc_UnknownSide->gasd_g,ix[UNKNOWNSIDE]-1,trk,
            ((BkTrcDat *)userdat)->bktrc_useglist)&0x1)
            (((BkTrcDat *)userdat)->bktrc_execnt)++;
}

static void bk_trc_enter_align(userdat,F,ix,dims,inalign,trk)
void *userdat;
match *F;
int *ix,*dims,inalign,trk;
{   ((BkTrcDat *)userdat)->bktrc_execnt=0;
}

static void bk_trc_leave_align(userdat,F,ix,dims,inalign,trk)
void *userdat;
match *F;
int *ix,*dims,inalign,trk;
{   F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].
        mtch_execnt=((BkTrcDat *)userdat)->bktrc_execnt;
}

static void prt_align_enteralign(userdat,F,ix,dims,mtch_type)
void *userdat;
match *F;
int *ix,*dims,mtch_type;
{   int genstrt,genend;
    char gdfnam[MAXGENDEFLINLEN],cmnt[MAXGENDEFLINLEN];
    double matchratio;

    ((PrtAlignDat *)userdat)->prald_dupgen=0;
    if(((((PrtAlignDat *)userdat)->
        prald_execnt)=F[sb(ix[UNKNOWNSIDE],
        ix[KNOWNSIDE])].mtch_execnt))
    {   if(!((((PrtAlignDat *)userdat)->
            prald_dupgen)=FndIntUnkGenCorrLoc(
            F,ix,dims,&genstrt,&genend,
            ((PrtAlignDat *)userdat)->
            prald_unkgendef)))
        {
            if(!(*(((PrtAlignDat *)userdat)->
                prald_fstlinprtd)))
            {   (*(((PrtAlignDat *)userdat)->
                    prald_fstlinprtd))=1;
                if(((PrtAlignDat *)userdat)->
                    prald_nancgenmtch)
                    fprintf(((PrtAlignDat *)userdat)->prald_ofp,
                        "\n; unmatched executed fragments\n");
            }
            else
            {   if(((PrtAlignDat *)userdat)->
                    prald_prtalign)
                    fprintf(((PrtAlignDat *)userdat)->
                        prald_ofp,"\n\n");
            }
            if(!(((PrtAlignDat *)userdat)->
                prald_gdfelm->gdf_cnt))
                sprintf(&(gdfnam[0]),"%s",
                    ((PrtAlignDat *)userdat)->
                    prald_gdfelm->gdf_name);
            else
                sprintf(&(gdfnam[0]),"%s_%d",
                    ((PrtAlignDat *)userdat)->
                    prald_gdfelm->gdf_name,
                    ((PrtAlignDat *)userdat)->
                    prald_gdfelm->gdf_cnt);
            (((PrtAlignDat *)userdat)->
                prald_gdfelm->gdf_cnt)++;
            fprintf(((PrtAlignDat *)userdat)->
                prald_ofp,"%-15s %6d %6d %1d",
                &(gdfnam[0]),genstrt,genend,
                ((PrtAlignDat *)userdat)->
                prald_gdfelm->gdf_typ);
        
            matchratio=(F[sb(ix[UNKNOWNSIDE],
                ix[KNOWNSIDE])].mtch_alignscore)/
                (((double)(dims[KNOWNSIDE]))*
                (((PrtAlignDat *)userdat)->
                prald_mtchwgt));
        
            if(((PrtAlignDat *)userdat)->
                prald_prtalign&&(!(((
                PrtAlignDat *)userdat)->
                prald_nancgenmtch)))
                fprintf(((PrtAlignDat *)userdat)->
                    prald_ofp," %6.2f",matchratio);
        
            if(((PrtAlignDat *)userdat)->prald_nancgenmtch)
            {   sprintf(&(cmnt[0]),
                    "; match=%6.2f  len=%d",
                    matchratio,(genend-genstrt)+1);
                fprintf(((PrtAlignDat *)userdat)->
                    prald_ofp," %s\n",&(cmnt[0]));
            }
            else
                fprintf(((PrtAlignDat *)userdat)->
                    prald_ofp," %s\n",
                    ((PrtAlignDat *)userdat)->
                    prald_gdfelm->gdf_cmnt);
        
            if(((PrtAlignDat *)userdat)->prald_prtalign)
                fprintf(((PrtAlignDat *)userdat)->prald_ofp,
                    "-------------------------------------\n");
            AddGenDef(((PrtAlignDat *)userdat)->
                prald_unkgendef,(I8s *)(&(gdfnam[0])),
                (((PrtAlignDat *)userdat)->
                prald_nancgenmtch)?((I8s *)&(cmnt[0])):
                ((I8s *)(((PrtAlignDat *)userdat)->
                prald_gdfelm->gdf_cmnt)),
                genstrt,genend,((PrtAlignDat *)userdat)->
                prald_gdfelm->gdf_typ);
        }
    }
}

static void prt_align_aligndtl(userdat,F,ix,dims,mtch_type)
void *userdat;
match *F;
int *ix,*dims,mtch_type;
{   char *(mne[NUMSTRINGS]);
    if(mtch_type==ALIGN)
    {   if((((PrtAlignDat *)userdat)->
            prald_execnt)&&(((PrtAlignDat *)userdat)->
            prald_prtalign)&&
            (!(((PrtAlignDat *)userdat)->
            prald_dupgen))&&(ChkPrtAlign(
            (((PrtAlignDat *)userdat)->prald_nancgenmtch),F,
            &(ix[0]),dims,((PrtAlignDat *)userdat)->
            prald_unkgendef)))
        {   mne[UNKNOWNSIDE]=(char *)&(id[(unsigned int)
                (((((PrtAlignDat *)userdat)->
                prald_bufs)[UNKNOWNSIDE][ix[UNKNOWNSIDE]-1]
#if PLOIDY>1
                [trk]
#endif /* PLOIDY>1 */
                )%InstNum)].mn[0]);
    
            mne[KNOWNSIDE]=(char *)&(id[(unsigned int)
                (((((PrtAlignDat *)userdat)->
                prald_bufs)[KNOWNSIDE][ix[KNOWNSIDE]-1]
#if PLOIDY>1
                [trk]
#endif /* PLOIDY>1 */
                )%InstNum)].mn[0]);
            fprintf(((PrtAlignDat *)userdat)->
                prald_ofp,"%6d:%-10s %c %6d:%-10s\n",
                ix[UNKNOWNSIDE]-1,mne[UNKNOWNSIDE],
                (strcmp(mne[0],mne[1]))?'*':' ',
                ix[KNOWNSIDE]-1,mne[KNOWNSIDE]);
        }
    }
    else if(mtch_type==GAP1)
    {   if((((PrtAlignDat *)userdat)->
            prald_execnt)&&(((PrtAlignDat *)
            userdat)->prald_prtalign)&&
            (!(((PrtAlignDat *)userdat)->
            prald_dupgen))&&(ChkPrtAlign(
            ((PrtAlignDat *)userdat)->
            prald_nancgenmtch,F,&(ix[0]),dims,
            ((PrtAlignDat *)userdat)->prald_unkgendef)))
            fprintf(((PrtAlignDat *)userdat)->
                prald_ofp,"%6d:%-10s   %6s:%-10c\n",
                ix[UNKNOWNSIDE]-1,
                &(id[(unsigned int)(((((
                PrtAlignDat *)userdat)->
                prald_bufs)[UNKNOWNSIDE]
                [ix[UNKNOWNSIDE]-1]
#if PLOIDY>1
                [trk]
#endif /* PLOIDY>1 */
                )%InstNum)].mn[0])," ",'-');

    }
    else if((mtch_type==GAP0)&&(ix[KNOWNSIDE]!=0))
        if((((PrtAlignDat *)userdat)->
            prald_execnt)&&(((PrtAlignDat *)
            userdat)->prald_prtalign)&&
            (!(((PrtAlignDat *)userdat)->
            prald_dupgen))&&(ChkPrtAlign(
            ((PrtAlignDat *)userdat)->
            prald_nancgenmtch,F,&(ix[0]),dims,
            ((PrtAlignDat *)userdat)->prald_unkgendef)))
            fprintf(((PrtAlignDat *)userdat)->
                prald_ofp,"%6s:%-10c   %6d:%-10s\n",
                " ",'-',ix[KNOWNSIDE]-1 ,
                &(id[(unsigned int)(((((
                PrtAlignDat *)userdat)->
                prald_bufs)[KNOWNSIDE][ix[KNOWNSIDE]-1]
#if PLOIDY>1
                [trk]
#endif /* PLOIDY>1 */
                )%InstNum)].mn[0]));
}

static void prt_align_exitalign(userdat,F,ix,dims,mtch_type)
void *userdat;
match *F;
int *ix,*dims,mtch_type;
{   (((PrtAlignDat *)userdat)->prald_execnt)=0;
    if((((PrtAlignDat *)userdat)->
        prald_prtalign)&&(((PrtAlignDat *)userdat)->
        prald_prtunalign))
    {   fprintf(((PrtAlignDat *)userdat)->prald_ofp,
            "\nunaligned\n");
        fprintf(((PrtAlignDat *)userdat)->prald_ofp,
            "-------------------------------------\n");
    }
    if((((PrtAlignDat *)userdat)->prald_prtalign)&&
        (((PrtAlignDat *)userdat)->prald_prtunalign))
        fprintf(((PrtAlignDat *)userdat)->prald_ofp,
            "%6d:%-10s   %6s:%-10c\n",
            ix[UNKNOWNSIDE]-1,
            &(id[(unsigned int)(((((PrtAlignDat *)
            userdat)->prald_bufs)[UNKNOWNSIDE]
            [ix[UNKNOWNSIDE]-1]
#if PLOIDY>1
            [trk]
#endif /* PLOIDY>1 */
            )%InstNum)].mn[0])," ",'.');
}
