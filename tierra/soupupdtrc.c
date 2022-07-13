#define XDRBUFMAXSIZE 200000
#define OPCODEMAPNAME "opcode.map"
#define SUEIDXALOCINC 10

#include "tierra.h"
#include "tierra.h"
#define EXTERN
#include "globals.h"
#undef EXTERN
#include "soup_in.h"

typedef struct
{   char   *arg_prgnam;
    char   *arg_file;
    char   *arg_genlbl;
    I32s   arg_dstofsttyp;
    I32s   arg_dstofst;
    I32s   arg_dstofst_spec;
    I32s   arg_dsttrk;
    I32s   arg_dstlen;
    I32s   arg_instp;
    I32s   arg_instp_spec;
    I32s   arg_thrdid;
    I32s   arg_thrdid_spec;
} argst;

typedef struct
{   I32s       *suei_idx;
    DynArrCtrl  suei_ctrl;
} SouUpdEvtIdxArr;

int main P_((int argc,char **argv));
static int getargs P_((int argc,char **argv,argst *argbuf));
static void usage P_((argst *argbuf));
static void argdefaults P_((char **argv,argst *argbuf));
static void ScanSoupUpdEvt P_((FILE *fp,Pgl g,
    argst *argbuf,SouUpdEvtIdxArr *sueidx));
static int sueidxcmp P_((const void *i,const void *j));
static void RptSoupUpdEvt P_((Pgl g,SouUpdEvtIdxArr *sueidx));

int main(argc,argv)
int argc;
char **argv;
{   FILE *fp;
    argst argbuf;
    head_t head;
    indx_t *indx;
    Pgl g;
    I32s i;
    SouUpdEvtIdxArr sueidx;

    XDRBufMaxSize=XDRBUFMAXSIZE;
    enc_size_precomp();
    InitDynArr((DynArr *)(&sueidx),sizeof(SouUpdEvtIdxArr),0);
    if(!getargs(argc,argv,&argbuf))
    {   GetAMap((I8s *)"opcode.map");
        if((fp=fopen(argbuf.arg_file,"rb")))
        {   head=read_head(fp);
            indx=read_indx(fp,&head);
            if((i=(strlen(argbuf.arg_genlbl))?
                find_gen(indx,(I8s *)argbuf.
                arg_genlbl,head.n):0)!=(head.n))
            {   g=get_gen(fp,&head,&(indx[i]),i);
                ScanSoupUpdEvt(fp,g,&argbuf,&sueidx);
                RptSoupUpdEvt(g,&sueidx);
                FreeGen(g);
            }
            else
                fprintf(stderr,"genotype not found\n");
            free(indx);
            fclose(fp);
        }
        else
            perror(argbuf.arg_prgnam);
    }
    FreeDynArr((DynArr *)(&sueidx),0,0);
    return 0;
}


static void argdefaults(argv,argbuf)
char **argv;
argst *argbuf;
{   argbuf->arg_prgnam=argv[0];
    argbuf->arg_file="";
    argbuf->arg_genlbl="";
    argbuf->arg_dstofsttyp=0;
    argbuf->arg_dstofst_spec=0;
    argbuf->arg_dsttrk=0;
    argbuf->arg_dstlen=1;
    argbuf->arg_instp_spec=0;
    argbuf->arg_thrdid_spec=0;
}

static int getargs(argc,argv,argbuf)
int argc;
char **argv;
argst *argbuf;
{   int i,exitflag=0,errflag=0;
    argdefaults(argv,argbuf);
    if(argc>1)
    {   for(i=1;i<argc;i++)
        {   if(argv[i][0]=='-')
            {   if(!strcmp(&(argv[i][1]),"gl"))
                    argbuf->arg_genlbl=argv[++i];
                else if(!strcmp(&(argv[i][1]),"ot"))
                    argbuf->arg_dstofsttyp=atoi(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"of"))
                {   argbuf->arg_dstofst=atoi(argv[++i]);
                    argbuf->arg_dstofst_spec=1;
                }
                else if(!strcmp(&(argv[i][1]),"dt"))
                    argbuf->arg_dsttrk=atoi(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"ds"))
                    argbuf->arg_dstlen=atoi(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"ip"))
                {   argbuf->arg_instp=atoi(argv[++i]);
                    argbuf->arg_instp_spec=1;
                }
                else if(!strcmp(&(argv[i][1]),"th"))
                {   argbuf->arg_thrdid=atoi(argv[++i]);
                    argbuf->arg_thrdid_spec=1;
                }
                else
                {   usage(argbuf);
                    exitflag=1;
                    break;
                }
            }
            else
                argbuf->arg_file=argv[i];
        }
        if ((!(argbuf->arg_dstofst_spec))||
            (!(argbuf->arg_instp_spec ))||
            (!(argbuf->arg_thrdid_spec)))
        {   usage(argbuf);
            exitflag=1;
        }
    }
    else
    {   usage(argbuf);
        exitflag=1;
    }
    return exitflag;
}

static void usage(argbuf)
argst *argbuf;
{   fprintf(stderr,"%s <genebank archive>\n",argbuf->arg_prgnam);
    printf("    [-gl <%-20s (%-8s)>]\n","genome label",
        (!strlen(argbuf->arg_genlbl))?("use 1st"):argbuf->arg_genlbl);
    printf("    [-ot <%-20s (%8d)>]\n","offset type",
        argbuf->arg_dstofsttyp);
    printf("       0 = self\n");
    printf("       1 = own daughter\n");
    printf("       2 = host\n");
    printf("       3 = host daughter\n");
    printf("       4 = self (free memory)\n");
    printf("    <-of <%-20s %-10s>>\n","destination offset"," ");
    printf("    [-dt <%-20s (%8d)>]\n","destination track",
        argbuf->arg_dsttrk);
    printf("    [-ds <%-20s (%8d)>]\n","destination size",
        argbuf->arg_dstlen);
    printf("    <-ip <%-20s %-10s>>\n","InstP"," ");
    fprintf(stderr,"\n");
}

static void ScanSoupUpdEvt(fp,g,argbuf,sueidx)
FILE *fp;
Pgl g;
argst *argbuf;
SouUpdEvtIdxArr *sueidx;
{   I32s i,j,prvfnd,trg_dstofsttyp,trg_dstofst,
         trg_dsttrk,trg_instp,trg_thrdid,nwidx;
    SoupUpdEvent **soupupdevt=g->glst_thrdanadat.
        mtad_soupupdevt.suea_event,*soupupdevtbuf;
    if(soupupdevt)
    {   for(i=0;i<argbuf->arg_dstlen;i++)
        {   prvfnd=0;
            trg_dstofsttyp=argbuf->arg_dstofsttyp;
            trg_dstofst=(argbuf->arg_dstofst)+i;
            trg_dsttrk=argbuf->arg_dsttrk;
            trg_instp=(argbuf->arg_instp);
            trg_thrdid=argbuf->arg_thrdid;
            for(j=(g->glst_thrdanadat.mtad_soupupdevt.
                suea_ctrl.dync_elmaloc)-1;j>=0;j--)
            {   soupupdevtbuf=soupupdevt[j];
                if(soupupdevtbuf)
                {   if(!prvfnd)
                    {   if((soupupdevtbuf->
                            supd_instP)<(trg_instp))
                            prvfnd=1;
                        else
                            if((soupupdevtbuf->
                                supd_instP)==(trg_instp))
                                if((soupupdevtbuf->
                                   supd_thrd)==(trg_thrdid))
                                   prvfnd=1;
                    }
                    if(prvfnd)
                        if((soupupdevtbuf->supd_dstoffsettype)==
                            (trg_dstofsttyp))
                            if((soupupdevtbuf->supd_dstoffset)==
                                (trg_dstofst))
                                if((soupupdevtbuf->supd_dtra)==
                                    (trg_dsttrk))
                                {   nwidx=AddDynArrEl((
                                        DynArr *)sueidx,-1,
                                        SUEIDXALOCINC,0,0);
                                    (sueidx->suei_idx)[nwidx]=j;
                                    sueidx->suei_ctrl.dync_elmused++;
                                    if((soupupdevtbuf->
                                        supd_srcoffset)>=0)
                                    {   trg_dstofsttyp=
                                            soupupdevtbuf->
                                            supd_srcoffsettype;
                                        trg_dstofst=soupupdevtbuf->
                                            supd_srcoffset;
                                        trg_dsttrk=soupupdevtbuf->
                                            supd_stra;
                                    }
                                    else
                                        break;
                                }
                }
            }
        }
    }
    qsort(sueidx->suei_idx,sueidx->suei_ctrl.dync_elmused,
        sizeof(*(sueidx->suei_idx)),&sueidxcmp);
}

static int sueidxcmp(i,j)
const void *i,*j;
{   return ((*((I32s *)i))<(*((I32s *)j)))?
        (-1):(((*((I32s *)i))==(*((I32s *)j)))?0:1);
}

static void RptSoupUpdEvt(g,sueidx)
Pgl g;
SouUpdEvtIdxArr *sueidx;
{   int i;
    Rpt_soup_update_event_head(stdout);
    for(i=0;i<(sueidx->suei_ctrl.dync_elmused);i++)
        Rpt_soup_update_event(stdout,g,(sueidx->suei_idx)[i]);
}
