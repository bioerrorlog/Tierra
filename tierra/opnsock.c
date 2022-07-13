#define PEXTERN
#include "opnsock.h"

int opnsock(socktyp,bindflg,port)
int socktyp;
int port;
{   int sock=-1,errflag=0;
    struct protoent *p_protoent;
    struct sockaddr_in sockaddr;

    if((p_protoent=getprotobyname((
        socktyp==SOCK_STREAM)?"tcp":"udp")))
    {   if((sock=socket(PF_INET,socktyp,p_protoent->p_proto)))
        {   if(bindflg)
            {   memset(&sockaddr,0,sizeof(struct sockaddr_in));
                sockaddr.sin_family=AF_INET;
                sockaddr.sin_port=htons((unsigned short)port);
                sockaddr.sin_addr.s_addr=INADDR_ANY;
                if(bind(sock,(struct sockaddr *)
                    (&sockaddr),sizeof(struct sockaddr_in)))
                    errflag=1;
            }
            if(errflag)
            {   close(sock);
                sock=-1;
            }
        }
        else
            errflag=1;
    }
    else
    {   fprintf(stderr,"opnsock: getprotobyname failed\n");
        errflag=1;
    }
    return sock;
}

int udp2tcp(udpskt,readfds,incudpcnt,tcpskt,buf)
int udpskt,*incudpcnt,tcpskt;
fd_set *readfds;
char *buf;
{   int sndlen,encsndlen,errflag=0;

    if(FD_ISSET(udpskt,readfds))
    {   ++(*incudpcnt);
        if((sndlen=recv(udpskt,buf,UDPMTU,0))>0)
        {   encsndlen=htonl(sndlen);
            if(send(tcpskt,&encsndlen,
                sizeof(encsndlen),0)==sizeof(encsndlen))
            {   if(send(tcpskt,buf,sndlen,0)!=sndlen)
                    errflag=1;
            }
            else
                errflag=1;
        }
        else
            errflag=1;
    }
    return errflag;
}

int tcp2udp(tcpskt,readfds,inctcpcnt,udpskt,to,buf)
int tcpskt,*inctcpcnt,udpskt;
fd_set *readfds;
struct sockaddr_in *to;
char *buf;
{   int rtnval,encsndlen,sndlen,i,errflag=0;

    if(FD_ISSET(tcpskt,readfds))
    {   ++(*inctcpcnt);
        if((rtnval=recv(tcpskt,&encsndlen,
            sizeof(encsndlen),0))==sizeof(encsndlen))
        {   sndlen=ntohl(encsndlen);
            for(i=0,rtnval=0;i<sndlen;i+=rtnval)
                if((rtnval=recv(tcpskt,&(buf[i]),sndlen-i,0))<0)
                    break;
            if(i==sndlen)
            {   if(sendto(udpskt,buf,sndlen,0,
                    (struct sockaddr *)to,
                    sizeof(struct sockaddr_in))!=sndlen)
                    errflag=1;
            }
            else
                errflag=1;
        }
        else
            errflag=1;
    }
    return errflag;
}

void statusdisplay(prvupdtme,incudpcnt,prvincudpcnt,
    inctcpcnt,prvinctcpcnt,updcntmsg)
time_t *prvupdtme;
int incudpcnt,*prvincudpcnt,inctcpcnt,*prvinctcpcnt,updcntmsg;
{   time_t curupdtme;
    if(((curupdtme=time(NULL))-
        (*prvupdtme))>=STATUSINTERVAL)
    {   if((incudpcnt!=(*prvincudpcnt))||
           (inctcpcnt!=(*prvinctcpcnt)))
        {   if(updcntmsg)
            {   printf("incoming:");
                printf(" udp count=%d",incudpcnt);
                printf(" tcp count=%d",inctcpcnt);
                printf("\n");
                (*prvupdtme)=curupdtme;
                (*prvincudpcnt)=incudpcnt;
                (*prvinctcpcnt)=inctcpcnt;
            }
        }
    }
}
