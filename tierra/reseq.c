/* <file name> <offset width> */

#define BUFLEN 256
#define CODEDIR "CODE"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef ANSI     /* ANSI prototyping */
#define P_(A) A
#else /* non-ANSI prototyping */
#define P_(A) ()
#endif

static int trimline P_((char *buf));
static int chkdirective P_((char *buf));
static int chkcomnt P_((char *buf));
static int chkcodedir P_((char *buf));
static int chkoffset P_((char *buf, int *offsetstrt,
    int *offsetend, int *stpflg));

static int chkspace P_((int c));
static int chkdigit P_((int c));
int main P_((int argc, char **argv));

int main(argc, argv)
int argc;
char **argv;
{   char buf[BUFLEN];
    FILE *stream;
    int offsetstrt, offsetend, stpflg, linlen, ofst=0, ofstwidth, ofstfnd;

    ofstwidth=atoi(argv[2]);
    if((stream=fopen(argv[1], "r")))
    {   for(;fgets(&(buf[0]), BUFLEN, stream);)
        {   if((linlen=trimline(&(buf[0])))&&
                (!chkdirective(&(buf[0])))&&
                (!chkcomnt(&(buf[0])))&&
                (!chkcodedir(&(buf[0]))))
            {   ofstfnd=chkoffset(&(buf[0]),
                    &offsetstrt, &offsetend, &stpflg);
                if(!stpflg)
                    printf("%s;",&(buf[0]));
                else
                    printf("%*.*s", offsetstrt-1,
                        offsetstrt-1, &(buf[0]));
                printf(" %*.*d",ofstwidth,
                    ofstwidth, ofst);
                if(ofstfnd)
                {   if((offsetend+1)<linlen)
                        printf("%s",&(buf[offsetend+1]));
                }
                else if(stpflg)
                    if((offsetstrt-1)<linlen)
                    {   if(stpflg==1)
                            printf(" ");
                        printf("%s",&(buf[offsetstrt-1]));
                    }
                ofst++;
            }
            else
                printf("%s",&(buf[0]));
            printf("\n");
        }
        fclose(stream);
    }
    else
        perror("main:");
    return 0;
}

static int trimline(buf)
char *buf;
{   int linlen;
    char lstchar;
    for(linlen=strlen(&(buf[0]));linlen>0;)
    {   lstchar=buf[linlen-1];
        if((lstchar=='\n')||(lstchar=='\r'))
            buf[--linlen]='\0';
        else
            break;
    }
    return linlen;
}

static int chkdirective(buf)
char *buf;
{   int dirctvflg,i;
    for(i=dirctvflg=0;(buf[i])&&(buf[i]!=';');i++)
        if(buf[i]==':')
        {   dirctvflg=1;
            break;
        }
    return dirctvflg;
}

static int chkcomnt(buf)
char *buf;
{   int cmntflg,i;
    for(i=cmntflg=0;buf[i];i++)
        if(buf[i]==';')
        {   cmntflg=1;
            break;
        }
        else if(!chkspace(buf[i]))
            break;
    return cmntflg;
}

static int chkspace(c)
int c;
{   return ((c==' ')||(c=='\t'))?1:0;
}

static int chkcodedir(buf)
char *buf;
{   int codedirflg,i;
    for(i=codedirflg=0;(buf[i])&&(buf[i]!=';');i++)
        if(!strcmp(&(buf[i]),CODEDIR))
        {   codedirflg=1;
            break;
        }
        else if(!chkspace(buf[i]))
            break;
    return codedirflg;
}

static int chkoffset(buf, offsetstrt, offsetend, stpflg)
char *buf;
int *offsetstrt, *offsetend, *stpflg;
{   int i,offsetfnd, loopflg;
    for(i=offsetfnd=(*stpflg)=0,loopflg=1;buf[i]&&loopflg;i++)
    {   switch(*stpflg)
        {   case 0:
                if(buf[i]==';')
                {   (*stpflg)=1;
                    (*offsetstrt)=i+2;
                }
                break;
            case 1:
                if(chkspace(buf[i]))
                    (*stpflg)=2;
                else
                    loopflg=0;
                break;
            case 2:
                if(!chkspace(buf[i]))
                {   if(chkdigit(buf[i]))
                        (*stpflg)=3;
                    else
                        loopflg=0;
                }
                break;
            case 3:
                if(!chkdigit(buf[i]))
                {   if(chkspace(buf[i]))
                    {   offsetfnd=1;
                        (*offsetend)=i-1;
                    }
                    loopflg=0;
                }
                break;
            default :
                break;
        }
    }
    if(((*stpflg)==3)&&(loopflg))
    {   offsetfnd=1;
        (*offsetend)=i-1;
    }
    return offsetfnd;
}

static int chkdigit(c)
int c;
{   return ((c>='0')&&(c<='9'))?1:0;
}
