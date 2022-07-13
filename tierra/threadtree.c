#define XDRBUFMAXSIZE 200000
#define WINDOWORIGINX      0
#define WINDOWORIGINY      0
#define BITSPERBYTE        8
#define MINRGBCF           0.2
#define MAXRGBCF           0.8
#define XMARGIN           15
#define YMARGIN           15
#define CLRKEYMRG          3
#define MAXGENNAMLEN     255
#define MAXSAVCMDLEN     255
#define MAXCORDBUFLEN     35
#define NUMARGS           10
#define MINGFXPW         300
#define MINGFXPH           0
#define BARGAPFCT          0.5
#define WINMARGIN          0.75
#define BARWIDTH           2
#define BARHEIGHT          2
#define WINDOWTITLE   "threadtree"
#define WNDWSAVECMD   "import -window threadtree -silent "
#define OPCODEMAPNAME "opcode.map"

#include "tierra.h"
#define EXTERN
#include "globals.h"
#undef EXTERN
#include "soup_in.h"
#include <math.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Viewport.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>

typedef struct
{   char   *arg_prgnam;
    char   *arg_file;
    char   *arg_genlbl;
    char   *arg_treetitle;
    char   *arg_imgfmt;
    int     arg_doall;
    int     arg_genetrn;
    int     arg_wwidth;
    int     arg_wheight;
    int     arg_fstdiv;
#ifdef NET
    int     arg_fstmig;
#endif
    int     arg_batch;
} argst;

typedef struct ThreadTree ThreadTree;
struct ThreadTree
{   ListNode    thtr_listnode;
    ListHead    thtr_subtree;
    ThreadTree *thtr_parent;
    int         thtr_thrdid;

    int         thtr_instP;          /* current */
    int         thtr_ipoffsettyp;
    int         thtr_ipoffset;

    int         thtr_fstinstP;       /* first   */
    int         thtr_fstipoffsettyp;
    int         thtr_fstipoffset;

    int         thtr_lstinstP;       /* last    */
    int         thtr_lstipoffsettyp;
    int         thtr_lstipoffset;

#ifndef HAVE_TDT_FST_LST_OFSTTYP
    int         thtr_lstsplitInstP;
    int         thtr_lstspltofsttyp;
    int         thtr_lstspltofst;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    int         thtr_position;
    int         thtr_lin2prnt;
};

typedef struct ClientData ClientData;
struct ClientData
{   GC                  cldt_gc;
    Pgl                 cldt_g;
    ThreadTree        **cldt_ThreadTreeIdx;
    argst               cldt_argbuf;
    I16s               *cldt_genelkup;
    XColor             *cldt_xcolor;
    int                 cldt_maxckeywdth;
    int                 cldt_barcnt;
    int                 cldt_instp;
    int                 cldt_dwidth;
    int                 cldt_dheight;
    GeneDefArr          cldt_gendef;
    XFontStruct        *cldt_pXFontStruct;
    int                 cldt_eventloop;
    Widget              cldt_draw;
    int                 cldt_zoomh;
    int                 cldt_zoomv;
    char               *cldt_gendir;
    int                 cldt_imgseq;
    int                 cldt_quitflag;
    unsigned long      *cldt_XPixelTable;
    Colormap            cldt_cmap;
    Widget              cldt_gene_dsp;
    Widget              cldt_thread_dsp;
    Widget              cldt_instp_dsp;
};

int main P_((int argc,char **argv));

static int getargs P_((int argc,char **argv,argst *argbuf));
static int opnsmpl P_((FILE **fp,argst *argbuf,
    head_t *head,indx_t **indx,char **gendir));
static void clssmpl P_((FILE *fp,indx_t *indx,char *gendir));
static void usage P_((argst *argbuf));
static void argdefaults P_((char **argv,argst *argbuf));
static void FreeThreadTree P_((ThreadTree *thrtroot,
    ThreadTree **ThreadTreeIdx));
static int BldThreadTree P_((ThreadTree **thrtroot,ClientData *client));
static void ThreadTreeXPsn P_((ThreadTree *thrtroot,
    int level,int *position,int endinstp));
static int alloccolors P_((ClientData *client));
static void freecolors P_((ClientData *client));
static int opngen P_((int *skipgen,head_t *head,
    int n,FILE *fp,indx_t *indx,ThreadTree **thrtroot,
    ClientData *client));
static void clsgen P_((ThreadTree *thrtroot,ClientData *client));
static void eventloop P_((ClientData *client));
static void ScanEventList P_((ClientData *client));
static void ColorKey P_((ClientData *client));
static void FindThrdLim P_((int *endinstp,ClientData *client));
static void redisplay_event(Widget w,XtPointer client,
    XEvent *ev,Boolean *continue_to_dispatch);
static void btnpress_event(Widget w,XtPointer client,
    XEvent *ev,Boolean *continue_to_dispatch);
static void next_fun(Widget w,XtPointer client_data,XtPointer call_data);
static void zoom_in_hv_fun(Widget w, XtPointer
    client_data,XtPointer call_data);
static void zoom_out_hv_fun(Widget w,XtPointer
    client_data,XtPointer call_data);
static void zoom_in_h_fun(Widget w, XtPointer
    client_data,XtPointer call_data);
static void zoom_out_h_fun(Widget w,XtPointer
    client_data,XtPointer call_data);
static void zoom_in_v_fun(Widget w, XtPointer
    client_data,XtPointer call_data);
static void zoom_out_v_fun(Widget w,XtPointer
    client_data,XtPointer call_data);
static void reset_fun(Widget w,XtPointer client_data,XtPointer call_data);
static void save_fun(Widget w,XtPointer client_data,XtPointer call_data);
static void palette_fun(Widget w,XtPointer client_data,XtPointer call_data);
static void quit_fun(Widget w,XtPointer client_data,XtPointer call_data);
static void resize_draw P_((ClientData *client));
static void save_win P_((ClientData *client));
static void GeneNameAtPoint P_((ClientData *client,int x,int y));
static int WidgetGeneName P_((
    ClientData *client,ThreadTree *pThreadTree,
    I32s thrdid,I32s instP,int x,int y,
    int xlolim,int xhilim,int ylolim,int yhilim));

int main(argc,argv)
int argc;
char **argv;
{
    FILE *fp;
    head_t head;
    indx_t *indx;
    int errflag=0,skipgen,i;
    ClientData client;
    Widget root,form,viewport,next,zoom_in,zoom_out,reset,save,palette,quit,
        horiz_title,zoom_in_h,zoom_out_h,gene_title,vert_title,
        zoom_in_v,zoom_out_v,thread_title,hv_title,zoom_in_hv,
        zoom_out_hv,instp_title;
    Arg args[NUMARGS];
    ThreadTree *thrtroot;

    client.cldt_quitflag=0;
    srand48(time(NULL));
    XDRBufMaxSize=XDRBUFMAXSIZE;
    enc_size_precomp();
    if(!getargs(argc,argv,&(client.cldt_argbuf)))
    {
        root=XtInitialize(client.cldt_argbuf.
            arg_prgnam,client.cldt_argbuf.arg_prgnam,
            NULL,0,&argc,argv);
        form=XtCreateManagedWidget("form",
            formWidgetClass,root,NULL,0);

        if(!(errflag=opnsmpl(&fp,&(client.
            cldt_argbuf),&head,&indx,&(client.cldt_gendir))))
        {   viewport=XtCreateManagedWidget("viewport",
                viewportWidgetClass,form,NULL,0);

            i=0;
            if(!(client.cldt_argbuf.arg_batch))
            {   XtSetArg(args[i],XtNallowVert,True);i++;
                XtSetArg(args[i],XtNallowHoriz,True);i++;
            }
            XtSetArg(args[i],XtNwidth,
                (client.cldt_argbuf.arg_wwidth)?
                (client.cldt_argbuf.arg_wwidth):
                ((int)(((double)DisplayWidth(
                XtDisplay(viewport),DefaultScreen(
                XtDisplay(viewport))))*WINMARGIN)));i++;
            XtSetArg(args[i],XtNheight,
                (client.cldt_argbuf.arg_wheight)?
                (client.cldt_argbuf.arg_wheight):
                ((int)(((double)DisplayHeight(
                XtDisplay(viewport),DefaultScreen(
                XtDisplay(viewport))))*WINMARGIN)));i++;
            XtSetValues(viewport,&(args[0]),i);

            client.cldt_draw=XtCreateManagedWidget("widget1",
                widgetClass,viewport,NULL,0);
            XtAddEventHandler(client.cldt_draw,
                ExposureMask,FALSE,&redisplay_event,&client);

            if(!(client.cldt_argbuf.arg_batch))
            {   
                next=XtCreateManagedWidget("next",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(next,&(args[0]),i);
                if(client.cldt_argbuf.arg_doall)
                    XtAddCallback(next,XtNcallback,
                    &next_fun,&client);

                palette=XtCreateManagedWidget("palette",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,next);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(palette,&(args[0]),i);
                XtAddCallback(palette,XtNcallback,
                    &palette_fun,&client);

                horiz_title=XtCreateManagedWidget("horiz",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,palette);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(horiz_title,&(args[0]),i);

                zoom_in_h=XtCreateManagedWidget("in",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,horiz_title);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_in_h,&(args[0]),i);
                XtAddCallback(zoom_in_h,XtNcallback,
                    &zoom_in_h_fun,&client);

                zoom_out_h=XtCreateManagedWidget("out",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_in_h);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_out_h,&(args[0]),i);
                XtAddCallback(zoom_out_h,XtNcallback,
                    &zoom_out_h_fun,&client);

                gene_title=XtCreateManagedWidget("gene",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_out_h);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(gene_title,&(args[0]),i);

                client.cldt_gene_dsp=XtCreateManagedWidget("",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,viewport);i++;
                XtSetArg(args[i],XtNfromHoriz,gene_title);i++;
                XtSetArg(args[i],XtNwidth,100);i++;
                XtSetValues(client.cldt_gene_dsp,&(args[0]),i);

                reset=XtCreateManagedWidget("reset",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,next);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(reset,&(args[0]),i);
                XtAddCallback(reset,XtNcallback,
                    &reset_fun,&client);

                quit=XtCreateManagedWidget("quit",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,palette);i++;
                XtSetArg(args[i],XtNfromHoriz,reset);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(quit,&(args[0]),i);
                XtAddCallback(quit,XtNcallback,
                    &quit_fun,&client);

                vert_title=XtCreateManagedWidget("vert",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,horiz_title);i++;
                XtSetArg(args[i],XtNfromHoriz,quit);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(vert_title,&(args[0]),i);

                zoom_in_v=XtCreateManagedWidget("in",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,zoom_in_h);i++;
                XtSetArg(args[i],XtNfromHoriz,vert_title);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_in_v,&(args[0]),i);
                XtAddCallback(zoom_in_v,XtNcallback,
                    &zoom_in_v_fun,&client);

                zoom_out_v=XtCreateManagedWidget("out",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,zoom_out_h);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_in_v);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_out_v,&(args[0]),i);
                XtAddCallback(zoom_out_v,XtNcallback,
                    &zoom_out_v_fun,&client);

                thread_title=XtCreateManagedWidget("thread",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,gene_title);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_out_v);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(thread_title,&(args[0]),i);

                client.cldt_thread_dsp=XtCreateManagedWidget("",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,
                    client.cldt_gene_dsp);i++;
                XtSetArg(args[i],XtNfromHoriz,thread_title);i++;
                XtSetArg(args[i],XtNwidth,100);i++;
                XtSetValues(client.cldt_thread_dsp,&(args[0]),i);

                save=XtCreateManagedWidget("save",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,reset);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(save,&(args[0]),i);
                XtAddCallback(save,XtNcallback,
                    &save_fun,&client);

                hv_title=XtCreateManagedWidget("h/v",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,vert_title);i++;
                XtSetArg(args[i],XtNfromHoriz,quit);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(hv_title,&(args[0]),i);

                zoom_in_hv=XtCreateManagedWidget("in",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,zoom_in_v);i++;
                XtSetArg(args[i],XtNfromHoriz,hv_title);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_in_hv,&(args[0]),i);
                XtAddCallback(zoom_in_hv,XtNcallback,
                    &zoom_in_hv_fun,&client);

                zoom_out_hv=XtCreateManagedWidget("out",
                    commandWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,zoom_out_v);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_in_hv);i++;
                XtSetArg(args[i],XtNwidth,30);i++;
                XtSetValues(zoom_out_hv,&(args[0]),i);
                XtAddCallback(zoom_out_hv,XtNcallback,
                    &zoom_out_hv_fun,&client);

                instp_title=XtCreateManagedWidget("instP",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,thread_title);i++;
                XtSetArg(args[i],XtNfromHoriz,zoom_out_hv);i++;
                XtSetArg(args[i],XtNwidth,80);i++;
                XtSetValues(instp_title,&(args[0]),i);

                client.cldt_instp_dsp=XtCreateManagedWidget("",
                    labelWidgetClass,form,NULL,0);
                i=0;
                XtSetArg(args[i],XtNfromVert,
                    client.cldt_thread_dsp);i++;
                XtSetArg(args[i],XtNfromHoriz,instp_title);i++;
                XtSetArg(args[i],XtNwidth,100);i++;
                XtSetValues(client.cldt_instp_dsp,&(args[0]),i);

                XtAddEventHandler(client.cldt_draw,
                    ButtonPressMask,FALSE,&btnpress_event,&client);
            }

            XtRealizeWidget(root);
            if(client.cldt_argbuf.arg_batch)
            {   for(i=0;i<head.n;i++)
                {   client.cldt_zoomh=1;
                    client.cldt_zoomv=1;
                    client.cldt_imgseq=0;
                    if(!(errflag=opngen(&skipgen,&head,
                        i,fp,indx,&thrtroot,&client)))
                    {   if(!skipgen)
                        {   ScanEventList(&client);
                            XFlush(XtDisplay(client.cldt_draw));
                            save_win(&client);
                            clsgen(thrtroot,&client);
                        }
                    }
                }
            }
            else if(client.cldt_argbuf.arg_doall)
            {   for(i=0;(i<head.n)&&(!(client.cldt_quitflag));i++)
                {   client.cldt_zoomh=1;
                    client.cldt_zoomv=1;
                    client.cldt_imgseq=0;
                    if(!(errflag=opngen(&skipgen,&head,i,
                        fp,indx,&thrtroot,&client)))
                    {   if(!skipgen)
                        {   eventloop(&client);
                            clsgen(thrtroot,&client);
                        }
                    }
                    else
                        break;
                }
            }
            else
            {   if((i=find_gen(indx,(I8s *)(client.cldt_argbuf.
                    arg_genlbl),head.n))!=(head.n))
                {   client.cldt_zoomh=1;
                    client.cldt_zoomv=1;
                    client.cldt_imgseq=0;
                    if(!(errflag=opngen(&skipgen,&head,i,
                        fp,indx,&thrtroot,&client)))
                    {   if(!skipgen)
                        {   eventloop(&client);
                            clsgen(thrtroot,&client);
                        }
                    }
                }
                else
                {   fprintf(stderr,"genotype not found");
                    errflag=1;
                }
            }
            clssmpl(fp,indx,client.cldt_gendir);
        }
    }
    return 0;
}

static void usage(argbuf)
argst *argbuf;
{   char *fmt1="%4s[-%-3s %s%-16s (%8s)%s]\n";
    char *fmt2="%4s[-%-3s %s%-16s (%8d)%s]\n";

    fprintf(stderr,"%s <genebank archive>\n",argbuf->arg_prgnam);
    fprintf(stderr,fmt1," ","gl","<","genome label",
        (!strlen((argbuf->arg_genlbl)))?
        ("do all"):(argbuf->arg_genlbl),">");
    fprintf(stderr,fmt1," ","gt"," "," ",
        (argbuf->arg_genetrn)?"yes":"no"," ");
    if(argbuf->arg_wwidth)
        fprintf(stderr,fmt2," ","ww"," "," ",argbuf->arg_wwidth," ");
    else
        fprintf(stderr,fmt1," ","ww"," "," ","screen"," ");
    if(argbuf->arg_wheight)
        fprintf(stderr,fmt2," ","wh"," "," ",argbuf->arg_wheight," ");
    else
        fprintf(stderr,fmt1," ","wh"," "," ","screen"," ");
    fprintf(stderr,fmt1," ","fd"," "," ",
        (argbuf->arg_fstdiv)?"yes":"no"," ");
#ifdef NET
    fprintf(stderr,fmt1," ","fs"," "," ",
        (argbuf->arg_fstmig)?"yes":"no"," ");
#endif
    fprintf(stderr,fmt1," ","tba"," "," ",
        (argbuf->arg_batch)?"yes":"no"," ");
    fprintf(stderr,fmt1," ","if","<",
        "image format",argbuf->arg_imgfmt,">");
    fprintf(stderr,fmt1," ","tti","<","image title",
        (!strlen((argbuf->arg_treetitle)))?
        "genotype":argbuf->arg_treetitle,">");
    fprintf(stderr,"\n");
    fprintf(stderr,"-gt  = gene transition thread analysis data available\n");
    fprintf(stderr,"-ww  = window width\n");
    fprintf(stderr,"-wh  = window height\n");
    fprintf(stderr,"-fd  = stop at first divide\n");
    fprintf(stderr,"-fs  = stop at first surf\n");
    fprintf(stderr,"-tba = batch mode\n");
    fprintf(stderr,"\n");
}

static void argdefaults(argv,argbuf)
char **argv;
argst *argbuf;
{   argbuf->arg_prgnam=argv[0];
    argbuf->arg_file="";
    argbuf->arg_genlbl="";
    argbuf->arg_treetitle="";
    (argbuf->arg_doall)=(strlen(argbuf->arg_genlbl))?0:1;
    argbuf->arg_imgfmt="gif";
    argbuf->arg_genetrn=0;
    argbuf->arg_wwidth=0;
    argbuf->arg_wheight=0;
    argbuf->arg_fstdiv=0;
#ifdef NET
    argbuf->arg_fstmig=0;
#endif
    argbuf->arg_batch=0;
}

static int getargs(argc,argv,argbuf)
int argc;
char **argv;
argst *argbuf;
{   int i,exitflag=0;
    argdefaults(argv,argbuf);
    if(argc>1)
    {   for(i=1;i<argc;i++)
        {   if(argv[i][0]=='-')
            {   if(!strcmp(&(argv[i][1]),"gl"))
                {   argbuf->arg_genlbl=argv[++i];
                    argbuf->arg_doall=strlen(argbuf->
                        arg_genlbl)?0:1;
                }
                else if(!strcmp(&(argv[i][1]),"gt"))
                    (argbuf->arg_genetrn)=1;
                else if(!strcmp(&(argv[i][1]),"ww"))
                    (argbuf->arg_wwidth)=atoi(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"wh"))
                    (argbuf->arg_wheight)=atoi(argv[++i]);
                else if(!strcmp(&(argv[i][1]),"fd"))
                {   (argbuf->arg_fstdiv)=1;
#ifdef NET
                    (argbuf->arg_fstmig)=0;
#endif
                }
#ifdef NET
                else if(!strcmp(&(argv[i][1]),"fs"))
                {   (argbuf->arg_fstmig)=1;
                    (argbuf->arg_fstdiv)=0;
                }
#endif
                else if(!strcmp(&(argv[i][1]),"tba"))
                    (argbuf->arg_batch)=1;
                else if(!strcmp(&(argv[i][1]),"tti"))
                    argbuf->arg_treetitle=argv[++i];
                else
                {   usage(argbuf);
                    exitflag=1;
                    break;
                }
            }
            else
                argbuf->arg_file=argv[i];
        }
    }
    else
    {   usage(argbuf);
        exitflag=1;
    }
    return exitflag;
}

static void FreeThreadTree(thrtroot,ThreadTreeIdx)
ThreadTree *thrtroot,**ThreadTreeIdx;
{   ThreadTree *listend,*crpThreadTree,*parent;
    if(thrtroot)
    {   listend=(ThreadTree *)
            (&(thrtroot->thtr_subtree.lsh_firstlast));
        for(crpThreadTree=(ThreadTree *)
            (thrtroot->thtr_subtree.lsh_first);
            crpThreadTree!=listend;crpThreadTree=
            (ThreadTree *)thrtroot->
            thtr_subtree.lsh_first)
            FreeThreadTree(crpThreadTree,ThreadTreeIdx);
        if((parent=thrtroot->thtr_parent))
            RemHeadNode((ListHead *)(&(parent->thtr_subtree)));
        if(!(thrtroot->thtr_parent))
            free(ThreadTreeIdx);
        free(thrtroot);
    }
}

static int BldThreadTree(thrtroot,client)
ThreadTree **thrtroot;
ClientData *client;
{   int errflag=0,thrdid;
    MemThreadDat *mtda_thdatara;
    ThreadTree *pThreadTree;

    (*thrtroot)=NULL;
    if(((client->cldt_ThreadTreeIdx)=(ThreadTree **)
        calloc(client->cldt_g->glst_thrdanadat.
        mtad_thdatara.mtda_ctrl.dync_elmaloc,
        sizeof(ThreadTree *))))
    {   for(thrdid=0;thrdid<(client->
            cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_ctrl.
            dync_elmaloc);thrdid++)
        {   if((mtda_thdatara=(client->cldt_g->
                glst_thrdanadat.mtad_thdatara.
                mtda_thdatara)[thrdid]))
            {   if((pThreadTree=(ThreadTree *)
                    calloc(1,sizeof(ThreadTree))))
                {   if(!(*thrtroot))
                        (*thrtroot)=pThreadTree;
                    else
                    {   AddTailNode(&(((client->
                            cldt_ThreadTreeIdx)[
                            mtda_thdatara->mtdt_basthrdat.
                            tdt_parent])->thtr_subtree),
                            &(pThreadTree->thtr_listnode));
                        pThreadTree->thtr_parent=
                            (client->cldt_ThreadTreeIdx)[
                            mtda_thdatara->mtdt_basthrdat.
                            tdt_parent];
                    }

                    pThreadTree->thtr_instP=-1;
                    pThreadTree->thtr_ipoffsettyp=-1;
                    pThreadTree->thtr_ipoffset=-1;

                    pThreadTree->thtr_fstinstP=-1;
                    pThreadTree->thtr_fstipoffsettyp=-1;
                    pThreadTree->thtr_fstipoffset=-1;

                    pThreadTree->thtr_lstinstP=-1;
                    pThreadTree->thtr_lstipoffsettyp=-1;
                    pThreadTree->thtr_lstipoffset=-1;

#ifndef HAVE_TDT_FST_LST_OFSTTYP
                    pThreadTree->thtr_lstsplitInstP=-1;
                    pThreadTree->thtr_lstspltofsttyp=-1;
                    pThreadTree->thtr_lstspltofst=-1;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */

                    pThreadTree->thtr_position=-1;
                    pThreadTree->thtr_lin2prnt=0;

                    InitListHead((ListHead *)(
                        &(pThreadTree->thtr_subtree)));
                    pThreadTree->thtr_thrdid=thrdid;
                    (client->cldt_ThreadTreeIdx)[
                        thrdid]=pThreadTree;
                }
                else
                {   perror("BldThreadTree");
                    errflag=1;
                    break;
                }
            }
        }
        if(errflag)
        {   FreeThreadTree(*thrtroot,
                client->cldt_ThreadTreeIdx);
            (*thrtroot)=NULL;
            free(client->cldt_ThreadTreeIdx);
            (client->cldt_ThreadTreeIdx)=NULL;
        }
    }
    else
    {   perror("BldThreadTree");
        errflag=1;
    }
    return errflag;
}

static void ThreadTreeXPsn(thrtroot,level,position,endinstp)
ThreadTree *thrtroot;
int level,*position,endinstp;
{   ThreadTree *listend,*crpThreadTree;
    listend=(ThreadTree *)
        (&(thrtroot->thtr_subtree.lsh_first));
    if((thrtroot->thtr_fstinstP)>=0)
        if((thrtroot->thtr_fstinstP)<=endinstp)
        {   thrtroot->thtr_position=(*position);
            (*position)++;
            level++;
            for(crpThreadTree=(ThreadTree *)
                (thrtroot->thtr_subtree.lsh_last);
                crpThreadTree!=listend;crpThreadTree=
                (ThreadTree *)(crpThreadTree->
                thtr_listnode.lnd_prev))
                ThreadTreeXPsn(crpThreadTree,
                level,position,endinstp);
        }
}

static void eventloop(client)
ClientData *client;
{   XEvent report;

    for(client->cldt_eventloop=1;
        client->cldt_eventloop;)
    {   XtNextEvent(&report);
        XtDispatchEvent(&report);
    }
}

static int alloccolors(client)
ClientData *client;
{   int errflag=0,maxrgbval,minrgb,maxrgb,rgbcinc,i;

    if(((client->cldt_xcolor)=(XColor *)
        malloc((client->cldt_gendef.gdf_ctrl.
        dync_elmused)*sizeof(XColor))))
    {   if(((client->cldt_XPixelTable)=(unsigned long *)
            malloc((client->cldt_gendef.gdf_ctrl.
            dync_elmused)*sizeof(*(client->cldt_XPixelTable)))))
        {   (client->cldt_cmap)=DefaultColormap(
                XtDisplay(client->cldt_draw),
                DefaultScreen(XtDisplay(client->cldt_draw)));
            maxrgbval=(1<<(sizeof((client->
                cldt_xcolor)[0].red)*BITSPERBYTE))-1;
            minrgb=(int)ceil(maxrgbval*MINRGBCF);
            maxrgb=(int)floor(maxrgbval*MAXRGBCF);
            rgbcinc=(maxrgb-minrgb)/(client->
                cldt_gendef.gdf_ctrl.dync_elmused);
            for(i=0;i<(client->cldt_gendef.
                gdf_ctrl.dync_elmused);i++)
            {   (client->cldt_xcolor)[i].red=minrgb+(rgbcinc*
                    (lrand48()%(maxrgb-minrgb)));
                (client->cldt_xcolor)[i].green=minrgb+(rgbcinc*
                    (lrand48()%(maxrgb-minrgb)));
                (client->cldt_xcolor)[i].blue=minrgb+(rgbcinc*
                    (lrand48()%(maxrgb-minrgb)));
                if(XAllocColor(XtDisplay(client->cldt_draw),
                    client->cldt_cmap,&((client->cldt_xcolor)[i])))
                    (client->cldt_XPixelTable)[i]=(client->
                        cldt_xcolor)[i].pixel;
                else
                {   fprintf(stderr,"XAllocColor failed\n");
                    XFreeColors(XtDisplay(client->cldt_draw),
                        client->cldt_cmap,client->
                        cldt_XPixelTable,i,0);
                    errflag=1;
                    break;
                }
            }
            if(errflag)
                free(client->cldt_XPixelTable);
        }
        else
        {   perror("alloccolors");
            errflag=1;
        }
        if(errflag)
            free(client->cldt_xcolor);
    }
    else
    {   perror("alloccolors");
        errflag=1;
    }
    return errflag;
}

static void freecolors(client)
ClientData *client;
{   XFreeColors(XtDisplay(client->cldt_draw),
        client->cldt_cmap,client->cldt_XPixelTable,
        client->cldt_gendef.gdf_ctrl.dync_elmused,0);
    free(client->cldt_XPixelTable);
    free(client->cldt_xcolor);
}

static void ScanEventList(client)
ClientData *client;
{   int i,genetrn;
    ThrdAnaEvent *pThrdAnaEvent;
    ThrdAnaEvent  **taea_event=client->cldt_g->
        glst_thrdanadat.mtad_event.taea_event;
    ThreadTree *pThreadTree,*parent;
    double xinc=((double)((client->cldt_barcnt)*
        BARWIDTH*(client->cldt_zoomh)))/
        ((double)(client->cldt_barcnt));
    double yinc=((double)((client->cldt_instp)*
        BARHEIGHT*(client->cldt_zoomv)))/
        ((double)(client->cldt_instp));

    XClearWindow(XtDisplay(client->cldt_draw),
        XtWindow(client->cldt_draw));
    for(i=0;i<(client->cldt_g->glst_thrdanadat.
        mtad_thdatara.mtda_ctrl.dync_elmaloc);i++)
        if((client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i])
        {   pThreadTree=(client->
                cldt_ThreadTreeIdx)[i];
            if((pThreadTree->thtr_fstinstP)>=0)
            {   pThreadTree->thtr_lin2prnt=0;
                pThreadTree->thtr_instP=
                    pThreadTree->thtr_fstinstP;
                pThreadTree->thtr_ipoffset=
                    pThreadTree->thtr_fstipoffset;
                pThreadTree->thtr_ipoffsettyp=
                    pThreadTree->thtr_fstipoffsettyp;
            }
        }
    XSetForeground(XtDisplay(client->cldt_draw),
        client->cldt_gc,BlackPixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    XSetBackground(XtDisplay(client->cldt_draw),
        client->cldt_gc,WhitePixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    for(i=0;i<(client->cldt_g->glst_thrdanadat.
        mtad_event.taea_ctrl.dync_elmused);i++)
    {   pThrdAnaEvent=taea_event[i];
        pThreadTree=client->cldt_ThreadTreeIdx[
            pThrdAnaEvent->taev_thrd];

        if(((client->cldt_argbuf.arg_fstdiv)&&
            ((pThrdAnaEvent->taev_instP)>
            (client->cldt_g->d1.instP)))
#ifdef NET
            ||((client->cldt_argbuf.arg_fstmig)&&
            ((pThrdAnaEvent->taev_instP)>
            (((client->cldt_g->glst_thrdanadat.mtad_base.
            btad_extrnterm)==REAP_SURF)?(client->
            cldt_g->glst_thrdanadat.mtad_base.
            btad_termInstP):(client->cldt_g->d1.instP))))
#endif
        )
            break;

        if((pThrdAnaEvent->taev_ipoffsettype!=0)||
            ((pThreadTree->thtr_position)<0)||
            (((client->cldt_genelkup)[pThreadTree->
            thtr_ipoffset])<0))
            continue;

        if((parent=pThreadTree->thtr_parent))
            if((!(pThreadTree->thtr_lin2prnt))&&
                ((parent->thtr_position)>=0))
            {
                XSetForeground(XtDisplay(client->cldt_draw),
                    client->cldt_gc,(client->
                    cldt_xcolor)[(client->cldt_genelkup)[
                    pThreadTree->thtr_ipoffset]].pixel);
                XFillRectangle(XtDisplay(client->cldt_draw),
                    XtWindow(client->cldt_draw),
                    client->cldt_gc,
                    XMARGIN+(client->cldt_maxckeywdth)+
                    ((int)((parent->thtr_position)*xinc)),
                    YMARGIN+((int)((pThreadTree->
                    thtr_instP)*yinc)),
                    (unsigned int)((((double)(((pThreadTree->
                    thtr_position)-(parent->thtr_position))+1))*
                    xinc)-(((double)(BARWIDTH*(client->
                    cldt_zoomh)))*(1.0-BARGAPFCT))),
                    (int)(((double)(BARHEIGHT*(client->
                    cldt_zoomv)))*BARGAPFCT));
                pThreadTree->thtr_lin2prnt=1;
            }
        if(((client->cldt_genelkup)[
            pThrdAnaEvent->taev_ipoffset]>=0))
        {   genetrn=0;
            if(!(client->cldt_argbuf.arg_genetrn))
            {   if(((client->cldt_genelkup)[
                  pThreadTree->thtr_ipoffset])!=
                  ((client->cldt_genelkup)[
                  pThrdAnaEvent->taev_ipoffset]))
                  genetrn=1;
            }
            else
                genetrn=pThrdAnaEvent->taev_genetrn;
            if(genetrn)
            {   XSetForeground(XtDisplay(client->cldt_draw),
                    client->cldt_gc,(client->
                    cldt_xcolor)[(client->
                    cldt_genelkup)[pThreadTree->
                    thtr_ipoffset]].pixel);
                XFillRectangle(XtDisplay(client->cldt_draw),
                    XtWindow(client->cldt_draw),
                    client->cldt_gc,
                    XMARGIN+(client->cldt_maxckeywdth)+
                    ((int)((pThreadTree->
                    thtr_position)*xinc)),
                    YMARGIN+((int)((pThreadTree->
                    thtr_instP)*yinc)),
                    (int)(((double)(BARWIDTH*(client->
                    cldt_zoomh)))*BARGAPFCT),
                    (unsigned int)((((pThrdAnaEvent->
                    taev_instP)-(pThreadTree->
                    thtr_instP))+1)*yinc));

                pThreadTree->thtr_instP=
                    pThrdAnaEvent->taev_instP;
                pThreadTree->thtr_ipoffsettyp=
                    pThrdAnaEvent->taev_ipoffsettype;
                pThreadTree->thtr_ipoffset=
                    pThrdAnaEvent->taev_ipoffset;
            }
        }
    }
    for(i=0;i<(client->cldt_g->glst_thrdanadat.
        mtad_thdatara.mtda_ctrl.dync_elmaloc);i++)
        if((client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i])
        {   pThreadTree=(client->cldt_ThreadTreeIdx)[i];
            if((pThreadTree->thtr_position)>=0)
            {   if(((client->cldt_genelkup)[
                    pThreadTree->thtr_ipoffset])>=0)
                {   XSetForeground(XtDisplay(client->cldt_draw),
                        client->cldt_gc,(client->
                        cldt_xcolor)[(client->
                        cldt_genelkup)[pThreadTree->
                        thtr_ipoffset]].pixel);
                    XFillRectangle(XtDisplay(client->cldt_draw),
                        XtWindow(client->cldt_draw),
                        client->cldt_gc,
                        XMARGIN+(client->cldt_maxckeywdth)+
                        ((int)((pThreadTree->
                        thtr_position)*xinc)),
                        YMARGIN+((int)((pThreadTree->
                        thtr_instP)*yinc)),
                        (int)(((double)(BARWIDTH*(client->
                        cldt_zoomh)))*BARGAPFCT),
                        (unsigned int)((((pThreadTree->
                        thtr_lstinstP)-(pThreadTree->
                        thtr_instP))+1)*yinc));
                }
            }
        }
    ColorKey(client);
    XSetForeground(XtDisplay(client->cldt_draw),
        client->cldt_gc,BlackPixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    XSetBackground(XtDisplay(client->cldt_draw),
        client->cldt_gc,WhitePixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    XFlush(XtDisplay(client->cldt_draw));
}

static int opnsmpl(fp,argbuf,head,indx,gendir)
FILE **fp;
argst *argbuf;
head_t *head;
indx_t **indx;
char **gendir;
{   int errflag=0;
    if(((*gendir)=(char *)getdirnam(
        (I8s *)(argbuf->arg_file))))
    {   GetAMap((I8s *)OPCODEMAPNAME);
        if(((*fp)=fopen(argbuf->arg_file,"rb")))
        {   (*head)=read_head(*fp);
            (*indx)=read_indx(*fp,head);
        }
        else
        {   perror("opnsmpl");
            errflag=1;
        }
        if(errflag)
        {   free(*gendir);
            (*gendir)=NULL;
        }
    }
    else
        errflag=1;
    return errflag;
}

static void clssmpl(fp,indx,gendir)
FILE *fp;
indx_t *indx;
char *gendir;
{   free(indx);
    fclose(fp);
    FreeAMap();
    free(gendir);
}

static int opngen(skipgen,head,n,fp,indx,thrtroot,client)
int *skipgen,n;
head_t *head;
FILE *fp;
indx_t *indx;
ThreadTree **thrtroot;
ClientData *client;
{   int errflag=0,strwidth,endinstp,i,ncolors;

    (*skipgen)=0;
    (client->cldt_g)=get_gen(fp,head,&(indx[n]),n);

    ReadGenDef(&(client->cldt_gendef),&(client->cldt_genelkup),
        head->size,(I8s *)(&((client->cldt_g)->
        gen.label[0])),(I8s *)(client->cldt_gendir));
    if(client->cldt_genelkup)
    {   ncolors=DisplayCells(XtDisplay(client->cldt_draw),
            DefaultScreen(XtDisplay(client->cldt_draw)));
        if((client->cldt_gendef.gdf_ctrl.dync_elmused)<=ncolors)
        {   if(!(errflag=alloccolors(client)))
            {   if(client->cldt_g->glst_thrdanadat.
                    mtad_thdatara.mtda_thdatara)
                {   if(!(errflag=BldThreadTree(thrtroot,client)))
                    {   FindThrdLim(&endinstp,client);
                        (client->cldt_barcnt)=0;
                        ThreadTreeXPsn(*thrtroot,0,
                            &(client->cldt_barcnt),endinstp);

                        client->cldt_instp=((client->cldt_argbuf.
                            arg_fstdiv)?(client->cldt_g->d1.
                            instP):(
#ifdef NET
                            (client->cldt_argbuf.arg_fstmig)?
                            (((client->cldt_g->
                            glst_thrdanadat.mtad_base.
                            btad_extrnterm)==REAP_SURF)?
                            (client->cldt_g->glst_thrdanadat.mtad_base.
                            btad_termInstP):(client->cldt_g->d1.
                            instP)):(
#endif
                            client->cldt_g->glst_thrdanadat.
                            mtad_event.taea_event[(client->cldt_g->
                            glst_thrdanadat.mtad_event.taea_ctrl.
                            dync_elmused)-1]->taev_instP
#ifdef NET
                            )
#endif
                            ))+1;

                        (client->cldt_gc)=XCreateGC(
                            XtDisplay(client->cldt_draw),
                            XtWindow(client->cldt_draw),0,NULL);

                        if(((client->cldt_pXFontStruct)=
                            XQueryFont(XtDisplay(client->cldt_draw),
                            XGContextFromGC(client->cldt_gc))))
                        {   for((client->cldt_maxckeywdth)=i=0;
                                i<(client->cldt_gendef.
                                gdf_ctrl.dync_elmused);i++)
                            {   strwidth=XTextWidth(client->
                                    cldt_pXFontStruct,(char *)
                                    (client->cldt_gendef.
                                    gdf_array[i].gdf_name),
                                    strlen((const char *)(
                                    client->cldt_gendef.
                                    gdf_array[i].gdf_name)));
                                (client->cldt_maxckeywdth)=
                                    (strwidth>(client->
                                    cldt_maxckeywdth))?
                                    strwidth:(client->
                                    cldt_maxckeywdth);
                            }
                            (client->cldt_maxckeywdth)+=CLRKEYMRG;
                            resize_draw(client);
                        }
                        else
                        {   (*skipgen)=1;
                            errflag=1;
                            fprintf(stderr,"XQueryFont failed\n");
                        }
                    }
                    else
                        (*skipgen)=1;
                    if(*skipgen)
                        FreeGen(client->cldt_g);
                }
                else
                {   fprintf(stderr,"thread analysis data");
                    fprintf(stderr," not available for");
                    fprintf(stderr," %.4d%s in %s\n",
                        client->cldt_g->gen.size,
                        &(client->cldt_g->gen.label[0]),
                        client->cldt_argbuf.arg_file);
                    (*skipgen)=1;
                }
                if(*skipgen)
                    freecolors(client);
            }
            else
                (*skipgen)=1;
        }
        else
        {   fprintf(stderr,"insufficient colors");
            fprintf(stderr," available from display\n");
            (*skipgen)=1;
        }
        if(*skipgen)
            FreeGenDef(&(client->cldt_gendef),
                &(client->cldt_genelkup),head->size);
    }
    else
    {   fprintf(stderr,"gene definition");
        fprintf(stderr," not available for");
        fprintf(stderr," %.4d%s in %s\n",
            client->cldt_g->gen.size,
            &(client->cldt_g->gen.label[0]),
            client->cldt_argbuf.arg_file);
        (*skipgen)=1;
    }
    if(*skipgen)
        FreeGen(client->cldt_g);
    return errflag;
}

static void clsgen(thrtroot,client)
ThreadTree *thrtroot;
ClientData *client;
{   XFreeFontInfo(NULL,client->cldt_pXFontStruct,0);
    XFreeGC(XtDisplay(client->cldt_draw),client->cldt_gc);
    FreeThreadTree(thrtroot,client->cldt_ThreadTreeIdx);
    freecolors(client);
    FreeGenDef(&(client->cldt_gendef),
        &(client->cldt_genelkup),client->cldt_g->gen.size);
    FreeGen(client->cldt_g);
}

static void ColorKey(client)
ClientData *client;
{   int fontheight,i;
    char gennam[MAXGENNAMLEN+1],*msg;

    fontheight=(client->cldt_pXFontStruct->
        max_bounds.ascent)+(client->
        cldt_pXFontStruct->max_bounds.descent);
    XSetForeground(XtDisplay(client->cldt_draw),
        client->cldt_gc,WhitePixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    for(i=0;i<(client->cldt_gendef.gdf_ctrl.dync_elmused);i++)
    {   XSetBackground(XtDisplay(client->cldt_draw),
            client->cldt_gc,(client->cldt_xcolor)[i].pixel);
        XDrawImageString(XtDisplay(client->cldt_draw),
            XtWindow(client->cldt_draw),
            client->cldt_gc,XMARGIN,
            YMARGIN+(i*fontheight),
            (char *)(client->cldt_gendef.
            gdf_array[i].gdf_name),strlen((
            const char *)(client->cldt_gendef.
            gdf_array[i].gdf_name)));
    }
    XSetForeground(XtDisplay(client->cldt_draw),
        client->cldt_gc,BlackPixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));
    XSetBackground(XtDisplay(client->cldt_draw),
        client->cldt_gc,WhitePixel(
        XtDisplay(client->cldt_draw),
        DefaultScreen(XtDisplay(client->cldt_draw))));

    if(strlen((client->cldt_argbuf.arg_treetitle)))
        sprintf(&(gennam[0]),"Thread Tree for %s",
            client->cldt_argbuf.arg_treetitle);
    else
        sprintf(&(gennam[0]),"Thread Tree for %.4d%s",
            client->cldt_g->gen.size,&(client->
            cldt_g->gen.label[0]));

    XDrawString(XtDisplay(client->cldt_draw),
        XtWindow(client->cldt_draw),
        client->cldt_gc,XMARGIN+(
        (((client->cldt_dwidth)-(2*XMARGIN))-
        XTextWidth(client->cldt_pXFontStruct,&(gennam[0]),
        strlen(&(gennam[0]))))/2),YMARGIN,
        &(gennam[0]),strlen(&(gennam[0])));

    if(!(client->cldt_argbuf.arg_genetrn))
    {   msg="gene transition thread analysis data not available";
        XDrawString(XtDisplay(client->cldt_draw),
            XtWindow(client->cldt_draw),
            client->cldt_gc,XMARGIN+
            ((((client->cldt_dwidth)-
            (2*XMARGIN))-XTextWidth(client->
            cldt_pXFontStruct,msg,strlen(msg)))/2),
            YMARGIN+fontheight,msg,strlen(msg));
    }
    if(client->cldt_argbuf.arg_fstdiv)
    {   msg="stop at first divide";
        XDrawString(XtDisplay(client->cldt_draw),
            XtWindow(client->cldt_draw),
            client->cldt_gc,
            XMARGIN+((((client->cldt_dwidth)-
            (2*XMARGIN))-XTextWidth(client->
            cldt_pXFontStruct,msg,strlen(msg)))/2),
            YMARGIN+(2*fontheight),msg,strlen(msg));
    }
#ifdef NET
    if(client->cldt_argbuf.arg_fstmig)
    {   msg="stop at first migration if any";
        XDrawString(XtDisplay(client->cldt_draw),
            XtWindow(client->cldt_draw),
            client->cldt_gc,
            XMARGIN+((((client->cldt_dwidth)-
            (2*XMARGIN))-XTextWidth(client->
            cldt_pXFontStruct,msg,strlen(msg)))/2),
            YMARGIN+(2*fontheight),msg,strlen(msg));
    }
#endif
}

static void FindThrdLim(endinstp,client)
int *endinstp;
ClientData *client;
{
#ifndef HAVE_TDT_FST_LST_OFSTTYP
    ThrdAnaEvent **taea_event=client->cldt_g->
        glst_thrdanadat.mtad_event.taea_event;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    int i,thrdid,strtipoffset;
    ThrdAnaEvent *pThrdAnaEvent;
    ThreadTree *pThreadTree,*parent;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
    MemThreadDat *mtda_thdatara;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    I32s lstinstp;

    (*endinstp)=0;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
    for(i=0;i<(client->cldt_g->
        glst_thrdanadat.mtad_thdatara.
        mtda_ctrl.dync_elmaloc);i++)
        if((mtda_thdatara=(client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i]))
        {   pThreadTree=(client->cldt_ThreadTreeIdx)[i];
            pThreadTree->thtr_fstinstP=mtda_thdatara->
                mtdt_basthrdat.tdt_fstInstP;
            pThreadTree->thtr_fstipoffsettyp=mtda_thdatara->
                mtdt_basthrdat.tdt_fstofsttyp;
            pThreadTree->thtr_fstipoffset=mtda_thdatara->
                mtdt_basthrdat.tdt_fstinstex;

            pThreadTree->thtr_lstinstP=mtda_thdatara->
                mtdt_basthrdat.tdt_lstInstP;
            pThreadTree->thtr_lstipoffsettyp=mtda_thdatara->
                mtdt_basthrdat.tdt_lstofsttyp;
            pThreadTree->thtr_lstipoffset=mtda_thdatara->
                mtdt_basthrdat.tdt_lstinstex;
        }
#else /* HAVE_TDT_FST_LST_OFSTTYP */
    for(i=0;i<(client->cldt_g->
        glst_thrdanadat.mtad_event.
        taea_ctrl.dync_elmused);i++)
    {   pThrdAnaEvent=taea_event[i];

        if(((client->cldt_argbuf.arg_fstdiv)&&
            ((pThrdAnaEvent->taev_instP)>
            (client->cldt_g->d1.instP)))
#ifdef NET
            ||((client->cldt_argbuf.arg_fstmig)&&
            ((pThrdAnaEvent->taev_instP)>
            (((client->cldt_g->glst_thrdanadat.mtad_base.
            btad_extrnterm)==REAP_SURF)?(client->
            cldt_g->glst_thrdanadat.mtad_base.
            btad_termInstP):(client->cldt_g->d1.instP))))
#endif
        )
            break;

        thrdid=pThrdAnaEvent->taev_thrd;
        pThreadTree=(client->cldt_ThreadTreeIdx)[thrdid];
        parent=pThreadTree->thtr_parent;
        if((pThreadTree->thtr_fstinstP)<0)
        {   if(parent)
            {   if(!(parent->thtr_lstspltofsttyp))
                    if((strtipoffset=(parent->
                        thtr_lstspltofst)+1)<
                        (client->cldt_g->gen.size))
                    {   pThreadTree->thtr_fstinstP=
                            (parent->thtr_lstsplitInstP)+1;
                        pThreadTree->thtr_fstipoffsettyp=
                            parent->thtr_lstspltofsttyp;
                        pThreadTree->thtr_fstipoffset=
                            strtipoffset;
                    }
            }
            else
            {   pThreadTree->thtr_fstinstP=0;
                pThreadTree->thtr_fstipoffsettyp=0;
                pThreadTree->thtr_fstipoffset=0;
            }
            if((pThreadTree->thtr_fstinstP)<0)
            {   pThreadTree->thtr_fstinstP=
                    pThrdAnaEvent->taev_instP;
                pThreadTree->thtr_fstipoffsettyp=
                    pThrdAnaEvent->taev_ipoffsettype;
                pThreadTree->thtr_fstipoffset=
                    pThrdAnaEvent->taev_ipoffset;
            }
        }
        if((pThreadTree->thtr_fstinstP)>=0)
        {   if((pThreadTree->thtr_lstinstP)<0)
            {   if(!strcmp((const char *)(&((id[pThrdAnaEvent->
                    taev_eventcode].mn)[0])),(const char *)"halt"))
                {   pThreadTree->thtr_lstinstP=
                        pThrdAnaEvent->taev_instP;
                    pThreadTree->thtr_lstipoffsettyp=
                        pThrdAnaEvent->taev_ipoffsettype;
                    pThreadTree->thtr_lstipoffset=
                        pThrdAnaEvent->taev_ipoffset;
                }
            }
            if(!(pThrdAnaEvent->taev_ipoffsettype))
            {   if(!strcmp((const char *)(&((id[pThrdAnaEvent->
                    taev_eventcode].mn)[0])),(const char *)"split"))
                {   pThreadTree->thtr_lstsplitInstP=
                        pThrdAnaEvent->taev_instP;
                    pThreadTree->thtr_lstspltofsttyp=
                        pThrdAnaEvent->taev_ipoffsettype;
                    pThreadTree->thtr_lstspltofst=
                        pThrdAnaEvent->taev_ipoffset;
                }
            }
        }
    }
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    for(i=0;i<(client->cldt_g->
        glst_thrdanadat.mtad_thdatara.
        mtda_ctrl.dync_elmaloc);i++)
        if((client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i])
        {   pThreadTree=(client->cldt_ThreadTreeIdx)[i];
            if((pThreadTree->thtr_fstinstP)>=0)
            {   if((pThreadTree->thtr_lstinstP)<0)
                    pThreadTree->thtr_lstinstP=
                        client->cldt_g->
                        glst_thrdanadat.mtad_event.
                        taea_event[(client->cldt_g->
                        glst_thrdanadat.mtad_event.
                        taea_ctrl.dync_elmused)-1]->
                        taev_instP;
                if(client->cldt_argbuf.arg_fstdiv)
                    (pThreadTree->thtr_lstinstP)=
                    ((pThreadTree->thtr_lstinstP)>
                    (client->cldt_g->d1.instP))?
                    (client->cldt_g->d1.instP):
                    (pThreadTree->thtr_lstinstP);
#ifdef NET
                if(client->cldt_argbuf.arg_fstmig)
                {   lstinstp=((client->cldt_g->
                        glst_thrdanadat.mtad_base.
                        btad_extrnterm)==REAP_SURF)?
                        (client->cldt_g->glst_thrdanadat.
                        mtad_base.btad_termInstP):
                        (client->cldt_g->d1.instP);
                    (pThreadTree->thtr_lstinstP)=
                        ((pThreadTree->thtr_lstinstP)>
                        lstinstp)?lstinstp:
                        (pThreadTree->thtr_lstinstP);
                }
#endif
                if((pThreadTree->thtr_lstinstP)>(*endinstp))
                    (*endinstp)=pThreadTree->thtr_lstinstP;
            }
        }
}


static void redisplay_event(w,client,ev,continue_to_dispatch)
Widget w;
XtPointer client;
XEvent *ev;
Boolean *continue_to_dispatch;
{   ScanEventList(client);
}

static void btnpress_event(w,client,ev,continue_to_dispatch)
Widget w;
XtPointer client;
XEvent *ev;
Boolean *continue_to_dispatch;
{   GeneNameAtPoint(client,ev->xbutton.x,ev->xbutton.y);
}

static void next_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   ((ClientData *)client_data)->cldt_eventloop=0;
}

static void zoom_in_hv_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   (((ClientData *)client_data)->cldt_zoomh)++;
    (((ClientData *)client_data)->cldt_zoomv)++;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void zoom_out_hv_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   if((((ClientData *)client_data)->cldt_zoomh)>1)
        (((ClientData *)client_data)->cldt_zoomh)--;
    if((((ClientData *)client_data)->cldt_zoomv)>1)
        (((ClientData *)client_data)->cldt_zoomv)--;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void zoom_in_h_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   (((ClientData *)client_data)->cldt_zoomh)++;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void zoom_out_h_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   if((((ClientData *)client_data)->cldt_zoomh)>1)
        (((ClientData *)client_data)->cldt_zoomh)--;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void zoom_in_v_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   (((ClientData *)client_data)->cldt_zoomv)++;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void zoom_out_v_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   if((((ClientData *)client_data)->cldt_zoomv)>1)
        (((ClientData *)client_data)->cldt_zoomv)--;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void reset_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   (((ClientData *)client_data)->cldt_zoomh)=1;
    (((ClientData *)client_data)->cldt_zoomv)=1;
    resize_draw((ClientData *)client_data);
    ScanEventList((ClientData *)client_data);
}

static void save_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   save_win((ClientData *)client_data);
}

static void save_win(ClientData *client)
{   char savecmd[MAXSAVCMDLEN+1];
    if(client->cldt_imgseq)
        sprintf(&(savecmd[0]),"%s%s%.4d%s_%d.%s",WNDWSAVECMD,
            client->cldt_gendir,client->cldt_g->gen.size,
            &(client->cldt_g->gen.label[0]),client->cldt_imgseq,
            client->cldt_argbuf.arg_imgfmt);
    else
        sprintf(&(savecmd[0]),"%s%s%.4d%s.%s",WNDWSAVECMD,
            client->cldt_gendir,client->cldt_g->gen.size,
            &(client->cldt_g->gen.label[0]),
            client->cldt_argbuf.arg_imgfmt);
    system(&(savecmd[0]));
    (client->cldt_imgseq)++;
}

static void palette_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   freecolors((ClientData *)client_data);
    if(!(alloccolors((ClientData *)client_data)))
        ScanEventList((ClientData *)client_data);
    else
    {   fprintf(stderr,"unable to allocate colors\n");
        ((ClientData *)client_data)->cldt_eventloop=0;
        ((ClientData *)client_data)->cldt_quitflag=1;
    }
}

static void quit_fun(w,client_data,call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{   ((ClientData *)client_data)->cldt_eventloop=0;
    ((ClientData *)client_data)->cldt_quitflag=1;
}

static void resize_draw(client)
ClientData *client;
{   int pixw,pixh,i;
    Arg args[NUMARGS];

    pixw=(client->cldt_barcnt)*BARWIDTH*(client->cldt_zoomh);
    client->cldt_dwidth=((MINGFXPW>pixw)?MINGFXPW:pixw)+
        (2*XMARGIN)+(client->cldt_maxckeywdth);

    pixh=(client->cldt_instp)*BARHEIGHT*(client->cldt_zoomv);
    client->cldt_dheight=((MINGFXPH>pixh)?MINGFXPH:pixh)+(2*YMARGIN);

    i=0;
    XtSetArg(args[i],XtNheight,
        client->cldt_dheight);i++;
    XtSetArg(args[i],XtNwidth,
        client->cldt_dwidth);i++;
    XtSetValues(client->cldt_draw,&(args[0]),i);
}

static void GeneNameAtPoint(client,x,y)
ClientData *client;
int x,y;
{   int i,genetrn,genefnd=0;
    int xlolim,xhilim,ylolim,yhilim;
    ThrdAnaEvent *pThrdAnaEvent;
    ThrdAnaEvent  **taea_event=client->cldt_g->
        glst_thrdanadat.mtad_event.taea_event;
    ThreadTree *pThreadTree,*parent;
    double xinc=((double)((client->cldt_barcnt)*
        BARWIDTH*(client->cldt_zoomh)))/
        ((double)(client->cldt_barcnt));
    double yinc=((double)((client->cldt_instp)*
        BARHEIGHT*(client->cldt_zoomv)))/
        ((double)(client->cldt_instp));

    for(i=0;i<(client->cldt_g->glst_thrdanadat.
        mtad_thdatara.mtda_ctrl.dync_elmaloc);i++)
        if((client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i])
        {   pThreadTree=(client->
                cldt_ThreadTreeIdx)[i];
            if((pThreadTree->thtr_fstinstP)>=0)
            {   pThreadTree->thtr_lin2prnt=0;
                pThreadTree->thtr_instP=
                    pThreadTree->thtr_fstinstP;
                pThreadTree->thtr_ipoffset=
                    pThreadTree->thtr_fstipoffset;
                pThreadTree->thtr_ipoffsettyp=
                    pThreadTree->thtr_fstipoffsettyp;
            }
        }
    for(i=0;i<(client->cldt_g->glst_thrdanadat.
        mtad_event.taea_ctrl.dync_elmused);i++)
    {   pThrdAnaEvent=taea_event[i];
        pThreadTree=client->cldt_ThreadTreeIdx[
            pThrdAnaEvent->taev_thrd];

        if(((client->cldt_argbuf.arg_fstdiv)&&
            ((pThrdAnaEvent->taev_instP)>
            (client->cldt_g->d1.instP)))
#ifdef NET
            ||((client->cldt_argbuf.arg_fstmig)&&
            ((pThrdAnaEvent->taev_instP)>
            (((client->cldt_g->glst_thrdanadat.mtad_base.
            btad_extrnterm)==REAP_SURF)?(client->
            cldt_g->glst_thrdanadat.mtad_base.
            btad_termInstP):(client->cldt_g->d1.instP))))
#endif
        )
            break;

        if((pThrdAnaEvent->taev_ipoffsettype!=0)||
            ((pThreadTree->thtr_position)<0)||
            (((client->cldt_genelkup)[pThreadTree->
            thtr_ipoffset])<0))
            continue;

        if((parent=pThreadTree->thtr_parent))
            if((!(pThreadTree->thtr_lin2prnt))&&
                ((parent->thtr_position)>=0))
            {
                xlolim=XMARGIN+(client->cldt_maxckeywdth)+
                    ((int)((parent->thtr_position)*xinc));

                ylolim=YMARGIN+((int)((pThreadTree->
                    thtr_instP)*yinc));

                xhilim=xlolim+(((unsigned int)
                    ((((double)(((pThreadTree->
                    thtr_position)-(parent->
                    thtr_position))+1))*xinc)-
                    (((double)(BARWIDTH*(client->
                    cldt_zoomh)))*(1.0-BARGAPFCT))))-1);

                yhilim=ylolim+(((int)(((double)
                    (BARHEIGHT*(client->
                    cldt_zoomv)))*BARGAPFCT))-1);

                genefnd=WidgetGeneName(client,pThreadTree,
                    pThrdAnaEvent->taev_thrd,
                    (I32s)(((double)(y-YMARGIN))/yinc),
                    x,y,xlolim,xhilim,ylolim,yhilim);

                pThreadTree->thtr_lin2prnt=1;
            }
        if((((client->cldt_genelkup)[pThrdAnaEvent->
            taev_ipoffset])>=0)&&(!genefnd))
        {   genetrn=0;
            if(!(client->cldt_argbuf.arg_genetrn))
            {   if(((client->cldt_genelkup)[
                  pThreadTree->thtr_ipoffset])!=
                  ((client->cldt_genelkup)[
                  pThrdAnaEvent->taev_ipoffset]))
                  genetrn=1;
            }
            else
                genetrn=pThrdAnaEvent->taev_genetrn;
            if(genetrn)
            {
                client->cldt_gendef.gdf_array[
                    (client->cldt_genelkup)[
                    pThreadTree->thtr_ipoffset]].gdf_name;

                xlolim=XMARGIN+(client->cldt_maxckeywdth)+
                    ((int)((pThreadTree->thtr_position)*xinc));

                ylolim=YMARGIN+((int)((pThreadTree->
                    thtr_instP)*yinc));

                xhilim=xlolim+(((int)(((double)
                    (BARWIDTH*(client->cldt_zoomh)))*
                    BARGAPFCT))-1);

                yhilim=ylolim+(((unsigned int)
                    ((((pThrdAnaEvent->taev_instP)-
                    (pThreadTree->thtr_instP))+1)*yinc))-1);

                genefnd=WidgetGeneName(client,pThreadTree,
                    pThrdAnaEvent->taev_thrd,
                    (I32s)(((double)(y-YMARGIN))/yinc),
                    x,y,xlolim,xhilim,ylolim,yhilim);

                pThreadTree->thtr_instP=
                    pThrdAnaEvent->taev_instP;
                pThreadTree->thtr_ipoffsettyp=
                    pThrdAnaEvent->taev_ipoffsettype;
                pThreadTree->thtr_ipoffset=
                    pThrdAnaEvent->taev_ipoffset;
            }
        }
    }
    for(i=0;(i<(client->cldt_g->glst_thrdanadat.
        mtad_thdatara.mtda_ctrl.dync_elmaloc))&&(!genefnd);i++)
        if((client->cldt_g->glst_thrdanadat.
            mtad_thdatara.mtda_thdatara)[i])
        {   pThreadTree=(client->cldt_ThreadTreeIdx)[i];
            if((pThreadTree->thtr_position)>=0)
            {   if(((client->cldt_genelkup)[
                    pThreadTree->thtr_ipoffset])>=0)
                {
                    xlolim=XMARGIN+(client->cldt_maxckeywdth)+
                        ((int)((pThreadTree->thtr_position)*xinc));

                    ylolim=YMARGIN+((int)((pThreadTree->
                        thtr_instP)*yinc));

                    xhilim=xlolim+(((int)(((double)(
                        BARWIDTH*(client->cldt_zoomh)))
                        *BARGAPFCT))-1);

                    yhilim=ylolim+(((unsigned int)(
                        (((pThreadTree->thtr_lstinstP)-
                        (pThreadTree->thtr_instP))+1)*yinc))-1);

                    genefnd=WidgetGeneName(client,
                        pThreadTree,i,
                        (I32s)(((double)(y-YMARGIN))/yinc),
                        x,y,xlolim,xhilim,ylolim,yhilim);
                }
            }
        }
}

static int WidgetGeneName(client,pThreadTree,thrdid,
    instP,x,y,xlolim,xhilim,ylolim,yhilim)
ClientData *client;
ThreadTree *pThreadTree;
I32s thrdid,instP;
int x,y,xlolim,xhilim,ylolim,yhilim;
{   int i,genefnd=0;
    Arg args[NUMARGS];
    char cordbuf[MAXCORDBUFLEN];

    if((genefnd=((x>=xlolim)&&(x<=xhilim)&&
        (y>=ylolim)&&(y<=yhilim))?1:0))
    {
        sprintf(&(cordbuf[0]),"%s",
            client->cldt_gendef.gdf_array[
            (client->cldt_genelkup)[pThreadTree->
            thtr_ipoffset]].gdf_name);
        i=0;
        XtSetArg(args[i],XtNlabel,&(cordbuf[0]));i++;
        XtSetValues(((ClientData *)client)->
            cldt_gene_dsp,&(args[0]),i);

        sprintf(&(cordbuf[0]),"%d",thrdid);
        i=0;
        XtSetArg(args[i],XtNlabel,&(cordbuf[0]));i++;
        XtSetValues(((ClientData *)client)->
            cldt_thread_dsp,&(args[0]),i);

        sprintf(&(cordbuf[0]),"%d",instP);
        i=0;
        XtSetArg(args[i],XtNlabel,&(cordbuf[0]));i++;
        XtSetValues(((ClientData *)client)->
            cldt_instp_dsp,&(args[0]),i);
    }
    return genefnd;
}
