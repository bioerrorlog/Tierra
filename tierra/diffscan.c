#define MAXLINLEN 255
#define EXECPTRN " 100 "

#include <stdio.h>
#include <string.h>

#ifdef ANSI     /* ANSI prototyping */
#define P_(A) A
#else /* non-ANSI prototyping */
#define P_(A) ()
#endif

static int patrnfnd P_((char *inlin, char *patrnstr));
static int rdlintrm P_((FILE *infilptr, char *inlin));
static int usage P_((void));
static int setinfilnam P_((char **infilnam, char *infilnamstr));
int main P_((int argc, char **argv));

int main(argc, argv)
int argc;
char *argv[];
{   FILE *infilptr;
    char inlinstrt[MAXLINLEN+1], inlinend[MAXLINLEN+1];
    char *infilnam;
    int rdlintrmrtn, diffcnt=0, i;
    int exitflg, dtlflg;

    exitflg=0;
    dtlflg=0;
    infilnam=(char *)0;
    for(i=1; i<argc; i++)
    {   if(strlen(argv[i])>1)
        {   if(argv[i][0]=='-')
                if(!strcmp(&(argv[i][1]), "d"))
                    dtlflg=1;
                else
                {   exitflg=usage();
                    break;
                }
            else
                if((exitflg=setinfilnam(&infilnam, argv[i])))
                    break;
        }
        else
            if((exitflg=setinfilnam(&infilnam, argv[i])))
                break;
    }
    if(!exitflg)
        if(!infilnam)
            exitflg=usage();

    if(!exitflg)
    {   if((infilptr=fopen(infilnam, "r")))
        {   rdlintrmrtn=rdlintrm(infilptr, &(inlinend[0]));
            while(rdlintrmrtn!=((int)NULL))
            {   strcpy(&(inlinstrt[0]), &(inlinend[0]));
                if((rdlintrmrtn=rdlintrm(infilptr,
                    &(inlinend[0])))!=(int)NULL)
                {   if(patrnfnd(&(inlinstrt[0]), "if")==0)
                        if((patrnfnd(&(inlinend[0]), "jmp")==0)||
                           (patrnfnd(&(inlinend[0]), "call")==0)||
                           (patrnfnd(&(inlinend[0]), "rts")==0)||
                           (patrnfnd(&(inlinend[0]), "halt")==0))
                        {   if(dtlflg)
                            {   printf("\n%s\n", &(inlinstrt[0]));
                                printf("%s\n", &(inlinend[0]));
                            }
                            diffcnt++;
                        }
                }
                else
                    break;
            }
            fclose(infilptr);
            if(dtlflg)
                printf("\n");
            printf("%s - %d potential differentiation sites found\n",
                infilnam, diffcnt);
        }
        else
            printf("file open failed\n");
    }
    return 0;
}

static int patrnfnd(inlin, patrnstr)
char *patrnstr, *inlin;
{   int patrnfndflg, patrnlen, i, linlen;
    patrnlen=strlen(patrnstr);
    linlen=strlen(inlin);
    for(i=0,patrnfndflg=-1;i<(linlen-patrnlen); i++)
        if(!strncmp(&(inlin[i]), patrnstr, patrnlen))
        {   patrnfndflg=i;
            break;
        }
    return patrnfndflg;
}

static int rdlintrm(infilptr, inlin)
FILE *infilptr;
char *inlin;
{   int fgetsrtn, linlen;
    while((fgetsrtn=fgets(&(inlin[0]), MAXLINLEN, infilptr)!=NULL))
    {   linlen=strlen(&(inlin[0]));
        if(linlen>1)
            if(inlin[linlen-2]=='\r')
                inlin[linlen-2]='\0';
        if(linlen>0)
            if(inlin[linlen-1]=='\n')
                inlin[linlen-1]='\0';
        if((linlen=strlen(&(inlin[0]))))
            if(((inlin[0]>='a')&&(inlin[0]<='z'))||
                ((inlin[0]>='A')&&(inlin[0]<='Z')))
                if(patrnfnd(&(inlin[0]), EXECPTRN)>=0)
                    break;
    }
    return fgetsrtn;
}

static int usage()
{   printf("diffscan -h\n");
    printf("diffscan [-d] filename\n");
    return 1;
}

static int setinfilnam(infilnam, infilnamstr)
char **infilnam;
char *infilnamstr;
{   int exitflg=0;
    if(!(*infilnam))
        (*infilnam)=infilnamstr;
    else
        exitflg=usage();
    return exitflg;
}
