/* tieudptnl */

#define REQARGCNT 4

#include "opnsock.h"

typedef struct
{   char           *arg_prgnam;
    int             arg_updcntmsg;
    unsigned short  arg_loctcprt;
    unsigned short  arg_locudprt;
    char           *arg_remudphst;
    unsigned short  arg_remudprt;
} args;

enum ArgsType
{   LOCTCPRT  = 1,
    LOCUDPRT  = 2,
    REMUDPHST = 3,
    REMUDPRT  = 4
};

static int getargs P_((int argc,char **argv,args *argbuf));
static void argdefaults P_((char **argv,args *argbuf));
static void usage P_((args *argbuf));

int main(argc,argv)
int argc;
char **argv;
{   int tcpskt,lstnskt,tcpadrlen,udpskt,sleepflg;
    int incudpcnt=0,prvincudpcnt=0,inctcpcnt=0,prvinctcpcnt=0;
    struct sockaddr_in acptaddr,remudpadr;
    char *buf;
    fd_set readfds;
    struct hostent *phostent;
    time_t prvupdtme;
    args argbuf;

    prvupdtme=time(NULL);
    if(!getargs(argc,argv,&argbuf))
    {   for(sleepflg=0;;sleepflg=1)
        {   if(sleepflg)
                sleep(RETRYSLEEP);
            if((lstnskt=opnsock(SOCK_STREAM,1,
                argbuf.arg_loctcprt))>=0)
            {   if(!listen(lstnskt,1))
                {   if((udpskt=opnsock(SOCK_DGRAM,1,
                        argbuf.arg_locudprt))>=0)
                    {   memset(&remudpadr,0,sizeof(struct sockaddr_in));
                        remudpadr.sin_family=AF_INET;
                        remudpadr.sin_port=htons(argbuf.arg_remudprt);
                        if((phostent=gethostbyname(argbuf.arg_remudphst)))
                        {   remudpadr.sin_addr.s_addr=*((uint32_t *)
                                (phostent->h_addr_list[0]));
                            if((buf=malloc(UDPMTU)))
                            {   tcpadrlen=sizeof(struct sockaddr_in);
                                if((tcpskt=accept(lstnskt,(struct
                                    sockaddr *)(&acptaddr),&tcpadrlen))>=0)
                                {   for(;;)
                                    {   FD_ZERO(&readfds);
                                        FD_SET(udpskt,&readfds);
                                        FD_SET(tcpskt,&readfds);
                                        if(select(OPEN_MAX,&readfds,
                                            NULL,NULL,NULL)>=0)
                                        {   if(!udp2tcp(udpskt,&readfds,
                                                &incudpcnt,tcpskt,buf))
                                            {   if(!tcp2udp(tcpskt,&readfds,
                                                    &inctcpcnt,udpskt,
                                                    &remudpadr,buf))
                                                    statusdisplay(
                                                        &prvupdtme,incudpcnt,
                                                        &prvincudpcnt,
                                                        inctcpcnt,
                                                        &prvinctcpcnt,
                                                        argbuf.arg_updcntmsg);
                                                else
                                                    break;
                                            }
                                            else
                                                break;
                                        }
                                        else
                                            break;
                                    }
                                    close(tcpskt);
                                }
                                free(buf);
                            }
                        }
                        close(udpskt);
                    }
                }
                close(lstnskt);
            }
        }
    }
    return 0;
}

static void usage(argbuf)
args *argbuf;
{   printf("\n%s [-u (%3s)]",argbuf->arg_prgnam,
        (argbuf->arg_updcntmsg)?"yes":"no");
    printf(" <tieudptnl tcp port> <tieudptnl udp port>\n");
    printf("                      ");
    printf(" <tierra udp host   > <tierra udp port   >\n");
    printf("-u  = incoming traffic status messages\n\n");
}

static void argdefaults(argv,argbuf)
char **argv;
args *argbuf;
{   argbuf->arg_prgnam=argv[0];
    argbuf->arg_updcntmsg=0;
    argbuf->arg_loctcprt=0;
    argbuf->arg_locudprt=0;
    argbuf->arg_remudphst="";
    argbuf->arg_remudprt=0;
}

static int getargs(argc,argv,argbuf)
int argc;
char **argv;
args *argbuf;
{   int i,exitflag=0,argcnt=0;
    argdefaults(argv,argbuf);
    for(i=1;i<argc;i++)
        if(argv[i][0]=='-')
        {   if(!strcmp(&(argv[i][1]),"u"))
                argbuf->arg_updcntmsg=1;
            else
                break;
        }
        else
        {   argcnt++;
            switch(argcnt)
            {   case LOCTCPRT:
                    argbuf->arg_loctcprt=
                        (unsigned short)atoi(argv[i]);
                    break;
                case LOCUDPRT:
                    argbuf->arg_locudprt=
                        (unsigned short)atoi(argv[i]);
                    break;
                case REMUDPHST:
                    argbuf->arg_remudphst=argv[i];
                    break;
                case REMUDPRT:
                    argbuf->arg_remudprt=
                        (unsigned short)atoi(argv[i]);
                    break;
                default:
                    break;
            }
        }
    if((i!=argc)||((argcnt)!=REQARGCNT))
    {   usage(argbuf);
        exitflag=1;
    }
    return exitflag;
}
