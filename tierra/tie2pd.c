#define TIEHOSTARG     0
#define TIEPORTARG     1
#define PDHOSTARG      2
#define PDPORTARG      3
#define MAXMSGBUFLEN 255

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv);
static FILE *cnct2port(struct protoent *protoent,
    char *host,int port,char *mode);
static int tcprdbuf(FILE *fptr,char *rdbuf,int buflen);

int main(argc,argv)
int argc;
char **argv;
{   struct protoent *protoent;
    FILE *tiefptr,*pdfptr;
    char msgbuf[MAXMSGBUFLEN+1];

    if((protoent=getprotobyname("tcp")))
    {   if((tiefptr=cnct2port(protoent,argv[TIEHOSTARG],
            atoi(argv[TIEPORTARG]),"r"))>=0)
        {   if((pdfptr=cnct2port(protoent,argv[PDHOSTARG],
                atoi(argv[PDPORTARG]),"w"))>=0)
            {   while(1)
                    if(!tcprdbuf(tiefptr,&(msgbuf[0]),sizeof(msgbuf)))
                    {   if(send(fileno(pdfptr),&(msgbuf[0]),
                            strlen(&(msgbuf[0]))+1,0)<0)
                        {   perror(argv[0]);
                            break;
                        }
                    }
                    else
                        break;
                fclose(pdfptr);
            }
            fclose(tiefptr);
        }
    }
    else
        fprintf(stderr,"getprotobyname error\n");
    return 0;
}

static FILE *cnct2port(protoent,host,port,mode)
struct protoent *protoent;
int port;
char *host,*mode;
{   int sockfd,errflag=1;
    struct sockaddr_in cnctaddr;
    struct hostent *phostent;
    FILE *fptr=NULL;

    if((sockfd=socket(PF_INET,SOCK_STREAM,
        protoent->p_proto))>=0)
    {   cnctaddr.sin_family=AF_INET;
        cnctaddr.sin_port=htons(port);
        if((phostent=gethostbyname(host)))
        {   cnctaddr.sin_addr.s_addr=
                *((u_int32_t *)(phostent->h_addr));
            if(!(connect(sockfd,(struct sockaddr *)
                (&cnctaddr),sizeof(struct sockaddr_in))))
                if((fptr=fdopen(sockfd,mode)))
                    errflag=0;
                else
                    perror("cnct2port");
            else
                perror("cnct2port");
        }
        else
            fprintf(stderr,"gethostbyname error\n");
        if(errflag)
        {   close(sockfd);
            sockfd=-1;
        }
    }
    else
        perror("cnct2port");
    return fptr;
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
