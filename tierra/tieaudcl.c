#ifdef TIEAUDIO

#define SIGBLOCK

#define HOSTARG          1
#define PORTARG          2
#define REQSMPLRATEARG   3
#define RSPTMEARG        4
#define AUTOSCALEFRQARG  5
#define LOFREQARG        6
#define HIFREQARG        7
#define LOSIZEARG        8
#define HISIZEARG        9
#define LOGFSCLARG      10
#define INIRPTWRTCNTARG 11

#define MAXRDBUFLEN 255
#define BITSPERSAMPLE 8
#define MINFRGTME 0.01 /* minimum fragment play time (sec) */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#ifdef SIGBLOCK
#include <signal.h>
#endif /* SIGBLOCK */
#include <string.h>
#include <errno.h>

typedef struct
{   int            asl_size;
    int            asl_pop;
    struct timeval asl_rcvtme;
} AudSList;

int main(int argc,char **argv);
static int tcprdmsg(FILE *fptr,char *msgbuf,int buflen);
static int tcprdbuf(FILE *fptr,char *rdbuf,int buflen);
static double smplfun(double smpltime,
    AudSList *audsl,int siz_audsl,
    int totpop,int lofreq,int hifreq,
    int losize,int hisize,int logfscl);
static FILE *opntieaudcl(char *host,int port,
    int reqsmplrate,double rsptme,
    int *dspdes,int *nsmpl,unsigned char **wavetable,
    int *samplerate);
static void clstieaudcl(FILE *fptr,int dspdes,unsigned char *wavetable);
static int procmsgbklog(FILE *fptr,AudSList **audsl,int *siz_audsl,int *totpop);
static int opntieaudev(int *dspdes,int *samplerate,
    int reqsmplrate,int *nsmpl,double rsptme,
    unsigned char **wavetable);
static FILE *opntieaudcn(char *host,int port);
static int SetSigHndlr(int signum,void (*sighndlr)(int hndlrarg));
static void siginthndlr(int hndlrarg);
static sigset_t TSigBlock(sigset_t value);
static void TSigRelease(sigset_t value);

static int sigintflg=0;
#ifdef SIGBLOCK
sigset_t SigSet;
#endif /* SIGBLOCK */

int main(argc,argv)
int argc;
char **argv;
{   int dspdes,bitamp,i,j,nsmpl,samplerate,
        siz_audsl=0,totpop=0,lofreq,hifreq,losize,
        hisize,autofrqscale,cnsmpl,logfscl,
        rptwrtcnt,inirptwrtcnt;
    unsigned char *wavetable;
    AudSList *audsl=NULL;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */
    FILE *fptr;
    double tme,tmeinc;

    autofrqscale=atoi(argv[AUTOSCALEFRQARG]);
    hifreq=atoi(argv[HIFREQARG]);
    lofreq=atoi(argv[LOFREQARG]);
    losize=atoi(argv[LOSIZEARG]);
    hisize=atoi(argv[HISIZEARG]);
    logfscl=atoi(argv[LOGFSCLARG]);
    inirptwrtcnt=atoi(argv[INIRPTWRTCNTARG]);
    rptwrtcnt=1;
    if((fptr=opntieaudcl(argv[HOSTARG],atoi(
        argv[PORTARG]),atoi(argv[REQSMPLRATEARG]),
        atof(argv[RSPTMEARG]),&dspdes,
        &nsmpl,&wavetable,&samplerate)))
    {   bitamp=((int)pow((double)2,
            (double)(BITSPERSAMPLE-1)))-1;
        for(;!sigintflg;)
        {   if(!procmsgbklog(fptr,&audsl,&siz_audsl,&totpop))
            {   if(siz_audsl)
                {   if(autofrqscale)
                    {   hisize=losize=audsl[0].asl_size;
                        for(i=1;(i<siz_audsl)&&(!sigintflg);i++)
                        {   hisize=(audsl[i].asl_size>hisize)?
                                audsl[i].asl_size:hisize;
                            losize=(audsl[i].asl_size<losize)?
                                audsl[i].asl_size:losize;
                        }
                    }
                    /* build wave table */
                    if(!sigintflg)
                    {   tmeinc=1.0/((double)samplerate);
                        for(i=0,tme=0.0;(i<nsmpl)
                            &&(!sigintflg);i++,tme+=tmeinc)
                            wavetable[i]=(unsigned char)
                                ((((double)bitamp)*smplfun(
                                tme,audsl,siz_audsl,totpop,
                                lofreq,hifreq,losize,
                                hisize,logfscl))+bitamp);

                        /* trim wave table */
                        cnsmpl=nsmpl;
#ifdef DECLICK
                        if(nsmpl>1)
                        {   if(wavetable[nsmpl-1]<bitamp)
                            {   for(i=nsmpl-2,j=0;(i>=0)&&
                                    (!sigintflg);i--,j++)
                                    if(wavetable[i]>=bitamp)
                                        break;
                            }
                            else if(wavetable[nsmpl-1]!=bitamp)
                            {   for(i=nsmpl-2,j=0;(i>=0)&&
                                    (!sigintflg);i--,j++)
                                    if(wavetable[i]<=bitamp)
                                        break;
                            }
                            cnsmpl-=j;
                        }
#endif /* DECLICK */
                        if(!sigintflg)
                        {
#ifdef SIGBLOCK
                            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
                            /* write new wave table */
                            if(write(dspdes,wavetable,
                                cnsmpl*(sizeof(*wavetable)))<0)
                            {   perror("argv[0]");
#ifdef SIGBLOCK
                                TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                                break;
                            }
                            else if(inirptwrtcnt)
                            {   if(!(--rptwrtcnt))
                                {   if(write(dspdes,wavetable,
                                        cnsmpl*(sizeof(*wavetable)))<0)
                                    {   perror("argv[0]");
#ifdef SIGBLOCK
                                        TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                                        break;
                                    }
                                    else
                                        rptwrtcnt=inirptwrtcnt;
                                }
                            }
#ifdef SIGBLOCK
                            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
                        }
                    }
                }
            }
            else
                break;
        }
        if(audsl)
        {
#ifdef SIGBLOCK
            SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
            free(audsl);
#ifdef SIGBLOCK
            TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
        }
        clstieaudcl(fptr,dspdes,wavetable);
    }
    return 0;
}

/* process message backlog */
static int procmsgbklog(fptr,audsl,siz_audsl,totpop)
FILE *fptr;
int *siz_audsl,*totpop;
AudSList **audsl;
{   int errflag=0,tcprdmsgrtn,sizcls,sizpop,i,j;
    char rdbuf[MAXRDBUFLEN+1];
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    SigSaveSet=TSigBlock(SigSet);
#endif /* SIGBLOCK */
    while(((tcprdmsgrtn=tcprdmsg(fptr,
        &(rdbuf[0]),sizeof(rdbuf)))==1)&&(!sigintflg))
    {   sscanf(&(rdbuf[0]),"size %d;pop %d",
            &sizcls,&sizpop);
        for(i=0;i<(*siz_audsl)&&(!sigintflg);i++)
            if((*audsl)[i].asl_size==sizcls)
                break;
        if(!sigintflg)
        {   if(i<(*siz_audsl))
            {   if(sizpop)
                {   (*totpop)=(*totpop)-(*audsl)[i].asl_pop+sizpop;
                    (*audsl)[i].asl_pop=sizpop;
                    if(gettimeofday(&((*audsl)[i].
                        asl_rcvtme),NULL)<0)
                    {   perror("argv[0]");
                        errflag=1;
                        break;
                    }
                }
                else
                {   (*totpop)-=(*audsl)[i].asl_pop;
                    for(j=i;(j<((*siz_audsl)-1))&&(!sigintflg);j++)
                        (*audsl)[j]=(*audsl)[j+1];
                    if(!sigintflg)
                    {   if(((*audsl)=realloc(*audsl,
                            (--(*siz_audsl))*sizeof(AudSList))))
                        {   if(!(*siz_audsl))
                                (*audsl)=NULL;
                        }
                        else if(*siz_audsl)
                        {   perror("argv[0]");
                            errflag=1;
                            break;
                        }
                    }
                }
            }
            else
            {   if(sizpop)
                {   (*totpop)+=sizpop;
                    if(((*audsl)=realloc(*audsl,
                        (++(*siz_audsl))*sizeof(AudSList))))
                    {   (*audsl)[i].asl_size=sizcls;
                        (*audsl)[i].asl_pop=sizpop;
                        if(gettimeofday(&((*audsl)[i].
                            asl_rcvtme),NULL)<0)
                        {   perror("argv[0]");
                            errflag=1;
                            break;
                        }
                    }
                    else
                    {   perror("argv[0]");
                        errflag=1;
                        break;
                    }
                }
            }
            if(!sigintflg)
                if(!errflag)
                    if((*totpop)<0)
                    {   printf("negative total population\n");
                        break;
                    }
        }
    }
    if(!sigintflg)
        if(!errflag)
            if(tcprdmsgrtn==2)
                errflag=1;
#ifdef SIGBLOCK
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return errflag;
}

static FILE *opntieaudcl(host,port,reqsmplrate,
   rsptme,dspdes,nsmpl,wavetable,samplerate)
char *host;
unsigned char **wavetable;
int port,reqsmplrate,*dspdes,*nsmpl,*samplerate;
double rsptme;
{   int errflag=1;
    FILE *fptr=NULL;
#ifdef SIGBLOCK
    sigset_t SigSaveSet;
#endif /* SIGBLOCK */

#ifdef SIGBLOCK
    sigemptyset(&SigSet);
    sigaddset(&SigSet,SIGINT);
    SigSaveSet=TSigBlock(SigSet);
    if(!SetSigHndlr(SIGINT,siginthndlr))
    {
#endif /* SIGBLOCK */
        if((fptr=opntieaudcn(host,port)))
        {   if(!opntieaudev(dspdes,samplerate,
                reqsmplrate,nsmpl,rsptme,wavetable))
                errflag=0;
            if(errflag)
            {   fclose(fptr);
                fptr=NULL;
            }
        }
#ifdef SIGBLOCK
    }
    TSigRelease(SigSaveSet);
#endif /* SIGBLOCK */
    return fptr;
}

static FILE *opntieaudcn(host,port)
char *host;
int port;
{   FILE *rtnval=NULL,*fptr;
    int s;
    struct protoent *protoent;
    struct sockaddr_in cnctaddr;
    struct hostent *phostent;

    if((protoent=getprotobyname("tcp")))
    {   if((s=socket(PF_INET,SOCK_STREAM,
            protoent->p_proto))>=0)
        {   cnctaddr.sin_family=AF_INET;
            cnctaddr.sin_port=htons(port);
            if((phostent=gethostbyname(host)))
            {   cnctaddr.sin_addr.s_addr=
                    *((u_int32_t *)(phostent->h_addr));
                if(!connect(s,(struct sockaddr *)(&cnctaddr),
                    sizeof(struct sockaddr_in)))
                {   if((fptr=fdopen(s,"r")))
                        rtnval=fptr;
                    else
                        perror("opntieaudcn");
                }
                else
                    perror("opntieaudcn");
            }
            else
                fprintf(stderr,"gethostbyname error\n");

            if(!(rtnval||fptr))
                close(s);
        }
        else
            perror("opntieaudcn");
    }
    else
        fprintf(stderr,"getprotobyname error\n");
    return rtnval;
}

static int opntieaudev(dspdes,samplerate,
    reqsmplrate,nsmpl,rsptme,wavetable)
int *dspdes,*samplerate,reqsmplrate,*nsmpl;
double rsptme;
unsigned char **wavetable;
{   int smplfmt,errflag=1,arg,frgsizpwr,nsmplfrg,nfrags;

    if(((*dspdes)=open("/dev/dsp",O_WRONLY))>=0)
    {   (*samplerate)=reqsmplrate;
        if(ioctl(*dspdes,SNDCTL_DSP_SPEED,samplerate)>=0)
        {   if((*samplerate)!=reqsmplrate)
                printf("sample rate used: %d\n",*samplerate);
            frgsizpwr=(int)(log(((double)
                (*samplerate))*MINFRGTME)/log(2.0));
            nsmplfrg=1<<frgsizpwr;
            nfrags=(int)((((double)(*samplerate))*
                rsptme)/((double)nsmplfrg));
            arg=(nfrags<<16)|frgsizpwr;
            if(ioctl(*dspdes,SNDCTL_DSP_SETFRAGMENT,&arg)>=0)
            {
                smplfmt=AFMT_U8;
                if(ioctl(*dspdes,SNDCTL_DSP_SETFMT,&smplfmt)>=0)
                {   if(smplfmt==AFMT_U8)
                    {   (*nsmpl)=nsmplfrg*nfrags;
                        if(((*wavetable)=calloc(*nsmpl,
                            sizeof(**wavetable))))
                            errflag=0;
                    }
                    else
                        printf("sample format not supported\n");
                }
                else
                    perror("opntieaudev");
            }
            else
                perror("opntieaudev");
        }
        else
            perror("opntieaudev");
        if(errflag)
            close(*dspdes);
    }
    else
        perror("optienaudev");
    return errflag;
}

static void clstieaudcl(fptr,dspdes,wavetable)
int dspdes;
FILE *fptr;
unsigned char *wavetable;
{   free(wavetable);
    close(dspdes);
    fclose(fptr);
}

static int tcprdmsg(fptr,msgbuf,buflen)
FILE *fptr;
char *msgbuf;
int buflen;
{   int rtnval=0,selrtn,s;
    fd_set readfds;
    struct timeval tmeout;

    s=fileno(fptr);
    FD_ZERO(&readfds);
    FD_SET(s,&readfds);
    tmeout.tv_sec=0;
    tmeout.tv_usec=0;
    if((selrtn=select(s+1,&readfds,NULL,NULL,&tmeout))>=0)
    {   if(FD_ISSET(s,&readfds))
        {   if(!tcprdbuf(fptr,msgbuf,buflen))
                rtnval=1;
            else
                rtnval=2;
        }
    }
    else
    {   perror("tcprdmsg");
        rtnval=2;
    }
    return rtnval;
}

static int tcprdbuf(fptr,rdbuf,buflen)
FILE *fptr;
char *rdbuf;
int buflen;
{   int rtnval=0,len;
    for(len=0;len<(buflen-1);)
    {   if(fgets(&(rdbuf[len]),buflen-len,fptr))
        {   if((len=strlen(rdbuf))>0)
            {   if(rdbuf[len-1]=='\n')
                    break;
            }
            else
                break;
        }
        else
            break;
    }
    if(len<(buflen-1))
    {   if(len>0)
        {   if(rdbuf[len-1]!='\n')
            {   perror("tcprdbuf");
                rtnval=1;
            }
        }
        else
        {   printf("server disconnected\n");
            rtnval=1;
        }
    }
    else
    {   printf("read buffer overflow\n");
        rtnval=1;
    }
    return rtnval;
}

static double smplfun(smpltime,audsl,siz_audsl,
    totpop,lofreq,hifreq,losize,hisize,logfscl)
double smpltime;
int siz_audsl,totpop,lofreq,hifreq,losize,hisize,logfscl;
AudSList *audsl;
{   double rtnval=0.0,tpi=((double)2)*M_PI,frq2siz,
           frqbas=((double)hifreq)/((double)lofreq);
    int i,cursiz,freq;

    for(i=0;(i<siz_audsl)&&(!sigintflg);i++)
    {
        cursiz=audsl[i].asl_size;
        if(hisize==losize)
            freq=lofreq;
        else
        {   frq2siz=((double)(hifreq-lofreq))/
                ((double)(hisize-losize));
            freq=(logfscl)?((int)(((double)lofreq)*
                pow(frqbas,1.0-(((double)(((cursiz<
                losize)?losize:((cursiz>hisize)?
                hisize:cursiz))-losize))/((double)
                (hisize-losize)))))):(hifreq-((int)
                (frq2siz*((double)(((cursiz>hisize)?
                hisize:cursiz)-losize)))));
        }
        rtnval+=(((double)(audsl[i].asl_pop))/
            ((double)totpop))*sin(tpi*((double)freq)*smpltime);
    }
    return rtnval;
}

static int SetSigHndlr(signum,sighndlr)
int signum;
void (*sighndlr)(int hndlrarg);
{   struct sigaction act;
    int rtnval=0;
    memset(&act,0,sizeof(act));
    act.sa_handler=sighndlr;
    if(sigaction(signum,&act,NULL)<0)
    {   rtnval=1;
        perror("SetSigHndlr");
    }
    return rtnval;
}

static void siginthndlr(hndlrarg)
int hndlrarg;
{   fprintf(stdout,"client exiting\n");
    sigintflg=1;
}

#ifdef SIGBLOCK

static sigset_t TSigBlock(value)
sigset_t value;
{   sigset_t oset;
    while(1)
        if(!sigprocmask(SIG_BLOCK,&value,&oset))
            break;
    return oset;
}

static void TSigRelease(value)
sigset_t value;
{   while(1)
        if(!sigprocmask(SIG_SETMASK,&value,NULL))
            break;
}

#endif /* SIGBLOCK */

#endif /* TIEAUDIO */
