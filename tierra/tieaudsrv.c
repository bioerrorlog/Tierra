#ifdef TIEAUDIO

/* MemProf labels: #26XX, in use: none */
/* FEError labels: #26XX, in use: 2600-2601,2605-2607*/

#include "tierra.h"
#include "globals.h"
#include "tieaudsrv.h"

static I32s tmevaldifms P_((struct timeval *tm1,struct timeval *tm2));
static void SendToClient P_((void *buf,int buflen));

void SndTieAudDat(sizcls)
I32s sizcls;
{   if(GeneBnker&&(audcnctskt>=0))
    {   tsprintf(&(Fbuf[0]),"size %d;pop %d;\n",
            sizcls,((sl[sizcls]->num_c)>
            AudioPopThresh)?(sl[sizcls]->num_c):0);
#ifdef TIEAUDMP
        tfprintf(audmptr,"%s",&(Fbuf[0]));
#endif /* TIEAUDMP */

        SendToClient(&(Fbuf[0]),strlen(&(Fbuf[0])));
    }
}

static void SendToClient(buf,buflen)
void *buf;
int buflen;
{
#ifdef _WIN32
   int wsaerrtn;
#endif /* _WIN32 */

    if(tsend(audcnctskt,buf,buflen,0)<0)
    {
#ifdef _WIN32
        wsaerrtn=WSAGetLastError();
	    if((wsaerrtn==WSAECONNRESET)||(wsaerrtn==WSAECONNABORTED))
#else /* _WIN32 */
	    if((errno==ECONNRESET)||(errno==EPIPE))
#endif /* _WIN32 */
        {   tclosesocket(audcnctskt);
            audcnctskt=-1;
        }
        else
            porterrmsg(2606,NULL,1);
    }
}

static I32s tmevaldifms(tm1,tm2)
struct timeval *tm1,*tm2;
{   return (1000*((tm1->tv_sec)-(tm2->tv_sec)))+
           (((tm1->tv_usec)-(tm2->tv_usec))/1000);
}

void OpnTieAud()
{   struct protoent *protoent;
    struct sockaddr_in lstnaddr;
    int errflag=1,optval=1,sverrno;
    char hostname[MAXHOSTNAME+1];
    struct hostent *hostentbuf;

    audlstnskt=audcnctskt=-1;
#ifdef TIEAUDMP
    OpnTieAudDmp();
#endif /* TIEAUDMP */
    if((protoent=tgetprotobyname("tcp")))
        if((audlstnskt=tsocket(PF_INET,SOCK_STREAM,
            protoent->p_proto))>=0)
        {   if(!tsetsockopt(audlstnskt,
                SOL_SOCKET,SO_REUSEADDR,
                &optval,sizeof(optval)))
            {   lstnaddr.sin_family=AF_INET;
                lstnaddr.sin_port=htons((I16s)AudioPort);
                if(!tgethostname((I8s *)(&(hostname[0])),sizeof(hostname)))
                    if((hostentbuf=tgethostbyname(&(hostname[0]))))
                    {   lstnaddr.sin_addr.s_addr=((struct in_addr *)
                            &(hostentbuf->h_addr_list[0][0]))->s_addr;
                        if(!tbind(audlstnskt,(struct sockaddr *)(&lstnaddr),
                            sizeof(struct sockaddr_in)))
                            if(!tlisten(audlstnskt,SOMAXCONN))
                                errflag=0;
                    }
            }
            if(errflag)
            {   sverrno=errno;
                tclosesocket(audlstnskt);
                errno=sverrno;
            }
        }
    if(errflag)
        porterrmsg(2601,NULL,1);
}

void ClsTieAud()
{   if(audcnctskt>=0)
        tclosesocket(audcnctskt);
    if(audlstnskt>=0)
        tclosesocket(audlstnskt);
#ifdef TIEAUDMP
    ClsTieAudDmp();
#endif /* TIEAUDMP */
}

void AcceptClient()
{   struct sockaddr_in cnctaddr;
    socklen_t addrlen; 
    fd_set readfds;
    struct timeval tmeout;

    if(audcnctskt<0)
    {   FD_ZERO(&readfds);
        FD_SET(audlstnskt,&readfds);
        tmeout.tv_sec=0;
        tmeout.tv_usec=0;
        if(tselect(audlstnskt+1,&readfds,NULL,NULL,&tmeout)>=0)
        {   if(FD_ISSET(audlstnskt,&readfds))
            {   addrlen=sizeof(struct sockaddr_in);
                if((audcnctskt=taccept(audlstnskt,
                    (struct sockaddr *)(&cnctaddr),&addrlen))<0)
                    porterrmsg(2605,NULL,1);
            }
        }
        else
            porterrmsg(2608,NULL,1);
    }
}

#ifdef TIEAUDMP

void OpnTieAudDmp()
{   I8s audmpfiln[MAXAUDMPFILNAMLEN];
    tsprintf((I8s *)(&(audmpfiln[0])),
        "%s%s", GenebankPath, AUDMPFILNAM);
    if(!(audmptr=tfopen(&(audmpfiln[0]),"w")))
        porterrmsg(2600,NULL,1);
}

void ClsTieAudDmp()
{   if(audmptr)
    {   tfclose(audmptr);
        audmptr=NULL;
    }
}
#endif /* TIEAUDMP */
#endif /* TIEAUDIO */
