/*-----------------------------------------------------------------------*/
/* 92-3-23 a new tool to slice trough the genebank files on disk ...     */
/* Tierra Simulator V3.1: Copyright (c) 1992 Dan Pirone & Virtual Life   */
/*-----------------------------------------------------------------------*/

/* #define _DBG_  */
/* #define X  */ /* use xterms for unix shells */
#ifndef PROBE
#define PROBE
#endif /* PROBE */
#ifdef _WIN32
#define SLASH '\\'
#else
#define SLASH '/'
#endif /* else of _WIN32 */

#include <stdio.h>
#include <sys/types.h>

#ifdef _WIN32
//#include <dir.h>
#include <dos.h>
#endif /* else of _WIN32 */

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#include "tierra.h"
#define  EXTERN
#include "globals.h"
#undef   EXTERN
#include "soup_in.h"
#ifndef _DIRENT_HAVE_D_NAMLEN
#define _D_EXACT_NAMLEN(d) (strlen ((d)->d_name))
#endif


InstDef *id;

I32s usize = 80L , uLbl = 1L , umpp = 50 , umpi = 50, umpo = 2;
Event uOeventB,uOeventE,uMeventB,uMeventE;
I8s file_type[4]="gen",

#ifdef _WIN32
    gb_in[255] = "gb0",
    gb_out[255] = "td",
    viewer[255] = "more < ",
#else
    gb_in[255] = "gb0",
    gb_out[255] = "td",
    viewer[255] = "vi",
#endif

    imap[255] = "opcode.map",
    data[600],
    ubits[33] =  "????????????????????????????????";

FILE *afp;
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
char * bit_names[] = {
"permanent genotype name, saved in .gen file ",
"swapped out to disk from the rambank, saved in .mem file ",
"EXs = executes own instructions (self) ",
"EXd = executes daughter's instructions ",
"EXo = executes other cell's instructions ",
"EXf = executes instructions in free memory ",
"EXh = own instructions are executed by other creature (host) ",
"TCs = matches template complement of self ",
"TCd = matches template complement of daughter ",
"TCo = matches template complement of other ",
"TCf = matches template complement of free memory ",
"TCh = own template complement is matched by other creature (host) ",
"TPs = uses template pattern of self ",
"TPd = uses template pattern of daughter ",
"TPo = uses template pattern of other ",
"TPf = uses template pattern of free memory ",
"TPh = own template pattern is used by other creature (host) ",
"MFs = moves instruction from self ",
"MFd = moves instruction from daughter ",
"MFo = moves instruction from other cell ",
"MFf = moves instruction from free memory ",
"MFh = own instructions are moved by other creature (host) ",
"MTs = moves instruction to self ",
"MTd = moves instruction to daughter ",
"MTo = moves instruction to other cell ",
"MTf = moves instruction to free memory ",
"MTh = is written on by another creature (host) ",
"MBs = executing other creatures code, moves inst from self ",
"MBd = executing other creatures code, moves inst from daughter ",
"MBo = executing other creatures code, moves inst from other cell ",
"MBf = executing other creatures code, moves inst from free memory ",
"MBh = other creature uses another cpu to move your instructions "};

static void FEClrscr P_((void));
static void init_uparams  P_((I32s mode));
static void set_imap P_((void));
static I8s IsInBitSet P_((I8s bstr[32],I32s bits));
static void WritEcoFuzzyB P_((I8s bits[33], I8s *buf));
static I8s MatchesProfile P_((I32s usize,I32s uLbl,
    I32s umpp,I32s umpi,I32s umpo, Event uOeventB,
    Event uOeventE,Event uMeventB,Event uMeventE,
    I8s ubits[32],Pgl Gen));
static void Probe P_((void));
static void GetBits P_((void));
static void Probe_ascii_io P_((void));
static I32s FEGetch P_((I32s x, I32s y));

/*-----------------------------------------------------------------------*/
static void FEClrscr()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/*-----------------------------------------------------------------------*/
static I32s FEGetch(x, y)
I32s x,y;
{
#ifdef _WIN32
    return printf("\n"),getch();
#else
    char data[84],c;
    fgets(data,84,stdin);
    sscanf(data,"%c",&c);
    return c;
#endif
}

/*-----------------------------------------------------------------------*/
static void init_uparams(mode)
I32s mode;
{   if(mode == 0)
    {   usize = -1L;
        uLbl = -1L;
        umpp = -1L;
        umpi = -1L;
        umpo = -1L;
        uOeventB.m = -1L;
        uOeventB.i = -1L;
        uOeventE.m = -1L;
        uOeventE.i = -1L;
        uMeventB.m = -1L;
        uMeventB.i = -1L;
        uMeventE.m = -1L;
        uMeventE.i = -1L;
        strcpy((char *)(&(ubits[0])), 
            (const char *)"????????????????????????????????");
        strcpy((char *)(&(file_type[0])),(const char *)"gen");
#if 0
        strcpy(gb_in,"gb/");
        strcpy(gb_out,"td/");
        strcpy(viewer,"vi");
#endif
    }
}

/*-----------------------------------------------------------------------*/

static void set_imap()
{   
#ifdef _DBG_
    I32s i;
#endif

    sprintf((char *)(&(data[0])),"%s/%s",gb_in,imap);
#ifdef _DBG_
    printf("SET_IMAP: file is %s\n",data);
#endif
    GetAMap(data);
#ifdef _DBG_
    for (i=0; i < INSTNUM; i++)
        printf("SET_IMAP: Aid [%d] = %hx, %s\n",i,aid[i].op,aid[i].mn);
    FEGetch(0,0);
#endif
}
/*-----------------------------------------------------------------------*/

static I8s IsInBitSet(bstr,bits)
I8s bstr[32];
I32s bits;
{   I32s b,p;
    /* WARNING first 2 bits are not watch bits! */
    for(b=1,p=0; p < 32;p++, b = b << 1L)
    {   if ((bstr[p] == '?') || (bstr[p] == '1' && (b&bits)) ||
            (bstr[p] == '0' && !(b&bits)) )
            continue;
        return 0;
    }
    return 1;
}

/*-----------------------------------------------------------------------*/

static void WritEcoFuzzyB(bits, buf)
    I8s bits[33];
    I8s *buf;   
{
    I32s b,i, j;

    if(!buf) return;
    sprintf((char *)buf,"EX      TC      TP      MF      MT      MB      ");
    for (b=0,i = 0, j = 0; i < 6; i++, j = 0)
    {   if (bits[b++]!='0')
            buf[2+(i*8)+j++] = 's';
        if (bits[b++]!='0')
            buf[2+(i*8)+j++] = 'd';
        if (bits[b++]!='0')
            buf[2+(i*8)+j++] = 'o';
        if (bits[b++]!='0')
            buf[2+(i*8)+j++] = 'f';
        if (bits[b++]!='0')
            buf[2+(i*8)+j++] = 'h';
    }
}

/*-----------------------------------------------------------------------*/

static I8s MatchesProfile(usize,uLbl,umpp,umpi,umpo,
    uOeventB,uOeventE,uMeventB,uMeventE,ubits,Gen)
I32s usize,uLbl,umpp,umpi,umpo;
Event uOeventB,uOeventE,uMeventB,uMeventE;
I8s ubits[32];
Pgl Gen;
{   if ((Gen == NULL) ||
        ((usize > -1L) && (Gen->gen.size != usize)) ||
        ((uLbl > -1L) && (Lbl2Int(Gen->gen.label) != uLbl)) ||  
        ((uOeventB.m > -1L) && (Gen->originI.m < uOeventB.m)) ||
        ((uOeventB.i > -1L) && (Gen->originI.i < uOeventB.i)) ||
        ((uOeventE.m > -1L) && (Gen->originI.m > uOeventE.m)) ||
        ((uOeventE.i > -1L) && (Gen->originI.i > uOeventE.i)) ||
        ((uMeventB.m > -1L) && (Gen->mpp_time.m < uMeventB.m)) ||
        ((uMeventB.i > -1L) && (Gen->mpp_time.i < uMeventB.i)) ||
        ((uMeventE.m > -1L) && (Gen->mpp_time.m > uMeventE.m)) ||
        ((uMeventE.i > -1L) && (Gen->mpp_time.i > uMeventE.i)) ||
        ((umpp > -1L) && (100.0*Gen->MaxPropPop < umpp)) ||
        ((umpi > -1L) && (100.0*Gen->MaxPropInst < umpi)) ||
        ((umpo > -1L) && (Gen->MaxPop < umpo)) ||
        !IsInBitSet(ubits,Gen->bits))
        return 0;
    else
        return 1;
}

/*-----------------------------------------------------------------------*/

static void Probe()
{   I32s i;
#ifdef _WIN32
    I32s done=0L, handle;
#endif /* _WIN32 */
    I8s c, file[255];
    head_t head;
    indx_t *indx, *tindx;
    GList *g = NULL;
#ifdef _WIN32
    struct /* ffblk */ _finddata_t ffblk;
#else /* unix ... */
    DIR *dirp;
    struct dirent *dep;
#endif /* unix ... */

    printf("\nBEGIN Probing Genebank\n\n");

#ifdef _WIN32

    sprintf(file,"%s%c*.%3s",gb_in,SLASH,file_type);

    if (!(handle = _findfirst(file,&ffblk)))
    {   perror("PROBE :");
        exit(-666);
    }

    while(!done)
    {   if (kbhit() && ( FEGetch(0,0) == 0x1b))
            return;
        sprintf(file,"%s%c%s",gb_in,SLASH, ffblk.name);

#else /* unix ... */

        if ((dirp = opendir((const char *)(&(gb_in[0])))) == NULL)
        {   perror("PROBE :");
            exit(-666);
        }
        while ( (dep = readdir(dirp)) != NULL)
        {   if ((_D_EXACT_NAMLEN(dep) <= 3) ||
                (strcmp((const char *)(&(dep->
                d_name)[_D_EXACT_NAMLEN(dep)-3]),
                (const char *)(&(file_type[0])))))
                continue;
            sprintf((char *)file,"%s%c%s",gb_in,SLASH,dep->d_name);
#endif /* unix ... */

            printf(" scanning archive  |%s|\r",file);
            fflush(stdout);
            if (!(afp = fopen((const char *)file,
                (const char *)"rb")))
            {   perror("PROBE open archive ");
                exit(-666);
            }
            head = read_head(afp);
            indx = read_indx(afp, &head);
            for (i=0; i<head.n; i++) 
            {   tindx = &indx[i];
                g = get_gen(afp, &head, tindx, i);
                if(!MatchesProfile(usize,uLbl,umpp,umpi,umpo,
                    uOeventB,uOeventE,uMeventB,uMeventE, ubits,g))
                    continue;
                printf("\n\t%.4d%3s\n",g->gen.size,g->gen.label);
                printf("Enter y to show, s to save, ESC to break out, any other key to continue ->");
                c = (I8s)FEGetch(0,0);
                if (c == 0x1b)
                {   if (g)
                    {   if (g->genome)
                        {   free(g->genome);
                            g->genome = NULL;
                        }
                        if (g->gbits)
                        {   free(g->gbits);
                            g->gbits = NULL;
                        }
                        free(g);
                        g = NULL;
                    }
                    fclose(afp);
                    return;
                }
                if (c == 's')
                {   sprintf((char *)file, "%s%c%.4d%3s",
                        gb_out, SLASH, head.size, g->gen.label);
                    WritAscFile(g, file, 0, 0, 0,
                        0, 0, 0, 0, 0, NULL, 0, NULL,NULL,0);
                }
                if (c == 'y')
                {   sprintf((char *)file,"%s%c%.4d%3s",
                        gb_out, SLASH, head.size, g->gen.label);
                        WritAscFile(g, file, 0, 0, 0,
                        0, 0, 0, 0, 0, NULL, 0, NULL,NULL,0);
#ifdef _WIN32
                    sprintf(file, "%s  %s%c%.4d%s", 
                        viewer,gb_out,SLASH,head.size, g->gen.label);
#else
#ifdef X
                    sprintf(file, "xterm -title \"%.4d%s\" -e %s  %s%c%.4d%s &",
                        head.size, g->gen.label, viewer,
                        gb_out,SLASH,head.size, g->gen.label);
#else
                    sprintf((char *)file, "%s  %s%c%.4d%s ",
                        viewer,gb_out,SLASH,head.size, g->gen.label);
#endif /* X */
#endif
                    system((char *)file);
                }
                if (g)
                {   if (g->genome)
                    {   free(g->genome);
                        g->genome = NULL;
                    }
                    if (g->gbits)
                    {   free(g->gbits);
                        g->gbits = NULL;
                    }
                    free(g);
                    g = NULL;
                }
            }
            fclose(afp);
#ifdef _WIN32
            done = _findnext(handle, &ffblk);
#endif
        }
        printf("\n\nEND   Probing Genebank\n\n");
        printf("Enter to continue \n");
        c = (I8s)FEGetch(0,0);
}
/*-----------------------------------------------------------------------*/
static void GetBits()
{   I32s b;
    I8s c;

    for(b = 0; b < 32; b++)
    {   printf("Bit %2d %65.65s (%c)>",b,bit_names[b],ubits[b]);
        c= (I8s)FEGetch(0,0);
        if( c == 0x1b)
            return; 
        if( c == '1')
            ubits[b] = '1'; 
        else if( c == '0')
            ubits[b] = '0'; 
        else if( c == '?')
            ubits[b] = '?'; 
    }
}

/*-----------------------------------------------------------------------*/

static void Probe_ascii_io()
{   I8s  answer = ' ', bbuf[4], data[120];
    int ll;

    while(1)
    {   FEClrscr();
        printf("\nTIERRA Genebank Probe\t\t\tby Daniel Pirone\n\n");
        printf("1 - Size                                     (%d)\n",usize);
        printf("2 - Label                                    (%3s)\n",
            (uLbl > -1)? (I8s *)Int2Lbl(uLbl) :(I8s *)"-1");
        printf("3 - MaxPropPop  threshold [0-100]%%           (%d) \n",umpp);
        printf("4 - MaxPropInst threshold [0-100]%%           (%d) \n",umpi);
        printf("P - MaxPop                                   (%d) \n",umpo);
        printf("5 - Begin Time of MaxPropPop threshold cross (%d,%d)\n",
            uMeventB.m,uMeventB.i);
        printf("6 - End   Time of MaxPropPop threshold cross (%d,%d)\n",
            uMeventE.m,uMeventE.i);
        printf("7 - Begin Time of Origin                     (%d,%d) \n",
            uOeventB.m,uOeventB.i);
        printf("8 - End   Time of Origin                     (%d,%d) \n",
            uOeventE.m,uOeventE.i);
        printf("9 - Watch Bit String [0,1,?] (%32s) \n",ubits);
        WritEcoFuzzyB(ubits,Buff); 
        printf("    Watch Bit String (%s) \n",Buff);
        printf("f - Toggle file type                         (%3s)\n",file_type);
        printf("i - Genebank Input directory                 (%s)\n",gb_in);
        printf("o - Match    Output directory                (%s)\n",gb_out);
        printf("m - Opcode - Instruction Map file            (%s)\n",imap);
        printf("v - Set Gene Output viewer                   (%s)\n",viewer);
        printf("G - Begin Probe\n\n");
        printf("PROBE | (1-9) = edit param, 0 = init params, G=go <ESC> = quit      -> ");
        answer = (I8s) FEGetch(0,0);
        switch (answer)
        {   case 0x1b :
                exit(-1);
            case 'G' : 
            {   set_imap();
                Probe();
                break;
            }
            case '5' :
            case '6' :
            case '7' :
            case '8' :
                printf("Enter millions , insts (-1 = don't care)  -> ");
                    fgets((char *)(&(data[0])),84,stdin);
        }
        switch (answer)
        {   case 'f' :
            {   (file_type[0] == 'g') ?strcpy(
                    (char *)(&(file_type[0])),"tmp"):
                    strcpy((char *)(&(file_type[0])),"gen");
                break;
            }
            case 'i' :
            {   printf("Enter Input Direcotry -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%s",gb_in);
                set_imap();
                break;
            }
            case 'o' :
            {   printf("Enter Output Direcotry -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%s",gb_out);
                break;
            }
            case 'm' :
            {   printf("Enter Imap filename (assumed to be in Genebank dir)-> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%s",imap);
                set_imap();
                break;
            }
            case 'v' :
            {   printf("Enter text viewer -> ");
                fgets((char *)(&(data[0])),84,stdin);
                /* sscanf(data,"%s",viewer); */
                for(ll=0;ll < 80;viewer[ll++] = '\0');
                strncpy((char *)(&(viewer[0])),
                    (const char *)(&(data[0])),
                    (strlen((const char *)(&(data[0])))-1));
                break;
            }
            case '0' :
            {   init_uparams(0L);
                break;
            }
            case '1' :
            {   printf("Enter Size [-1 = don't care] -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%d",&usize);
                break;
            }
            case '2' :
            {   printf("Enter Label [-1 = don't care,aaa-zzz] -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%s",bbuf);
                if (atoi((const char *)(&(bbuf[0]))) < 0)
                {   uLbl = -1;
                    break;
                }
                uLbl = Lbl2Int(bbuf);
                break;
            }
            case '3' :
            {   printf( "Enter threshold [-1 = don't care, 0-100]%% -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%d",&umpp);
                break;
            }
            case '4' :
            {   printf("Enter threshold [-1 = don't care, 0-100]%% -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%d",&umpi);
                break;
            }
            case 'P' :
            {   printf("Enter threshold [-1 = don't care] -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%d",&umpo);
                break;
            }
            case '5' :
            {   sscanf((char *)(&(data[0])),"%d,%d",&uMeventB.m,&uMeventB.i);
                break;
            }
            case '6' :
            {   sscanf((char *)(&(data[0])),"%d,%d",
                    &uMeventE.m,&uMeventE.i);
                break;
            }
            case '7' :
            {   sscanf((char *)(&(data[0])),"%d,%d",&uOeventB.m,&uOeventB.i);
                break;
            }
            case '8' :
            {   sscanf((char *)(&(data[0])),"%d,%d",&uOeventE.m,&uOeventE.i);
                break;
            }
            case '9' :
            {
#if 0
                printf("Enter Watch Bits [? = don't care,0-1] -> ");
                fgets((char *)(&(data[0])),84,stdin);
                sscanf((char *)(&(data[0])),"%32s",ubits);
#endif
                FEClrscr();
                GetBits();
                break;
            }
            default :
                answer = ' ';
        }
    }
}
/*-----------------------------------------------------------------------*/
int main (argc,argv)
int argc;
char *argv[];
{       GeneBnker = 1;
        NumCells = 1;
        enc_size_precomp();
        FEClrscr();
        init_uparams(0L);
        Probe_ascii_io();
        FEClrscr();
        return 0;
}
/*-----------------------------------------------------------------------*/
