#define MAXFLDLEN  80
#define MAXFMTLEN 256
#define MAXPASS     2
#define DISPLPASS   1
#define MAXLINCNT  10

#include "tierra.h"

typedef struct
{   char *fldg_tag;
    void *fldg_val;
    char *fldg_name;
    char *fldg_type;
    int   fldg_key;
} fldgrp;

typedef union
{   int    svk_int;
    double svk_double;
} svkey;

int main(int argc,char **argv);
static int ProcFld(char *fld,fldgrp *fldgrpbf,
    int nelm,int *havekey,svkey *svkeyb,int *entrycnt);
static void ProcEntry(int pass,int *skipcnt,int skipmax,
    int InstExec,TPingData *IPMap_elb,
    int AvgSize,int FecundityAvg);

int main(argc,argv)
int argc;
char **argv;
{   FILE *fptr;
    char fld[MAXFLDLEN+1];
    int havekey,pass,skipcnt,skipmax,AvgSize,InstExec,entrycnt;
    TPingData IPMap_elb;
    double FecundityAvg;
    fldgrp fldgrpbf[]=
    {   {"fa",&FecundityAvg,        "FecundityAvg","f",0},
        {"sp",&(IPMap_elb.Speed),   "Speed",       "i",0},
        {"nc",&(IPMap_elb.NumCells),"NumCells",    "i",0},
        {"aa",&(IPMap_elb.AgeAvg),  "AgeAvg",      "i",0},
        {"ie",&InstExec,            "InstExec",    "i",1},
        {"as",&AvgSize,             "AvgSize",     "i",0}
    };
    svkey svkeyb;

    for(pass=0;pass<MAXPASS;pass++)
    {   havekey=0;
        if(pass==DISPLPASS)
        {   skipmax=(entrycnt/MAXLINCNT)-1;
            skipcnt=0;
        }
        entrycnt=0;
        if((fptr=fopen(argv[1],"r")))
        {   while(fscanf(fptr,"%s",&(fld[0]))!=EOF)
                if(ProcFld(&(fld[0]),&(fldgrpbf[0]),
                    sizeof(fldgrpbf)/sizeof(fldgrp),
                    &havekey,&svkeyb,&entrycnt))
                    ProcEntry(pass,&skipcnt,skipmax,
                        svkeyb.svk_int,&IPMap_elb,
                        AvgSize,FecundityAvg);
            ProcEntry(pass,&skipcnt,skipmax,
                InstExec,&IPMap_elb,AvgSize,FecundityAvg);
            fclose(fptr);
        }
        else
            break;
    }
    return 0;
}

static int ProcFld(fld,fldgrpbf,nelm,havekey,svkeyb,entrycnt)
char *fld;
fldgrp *fldgrpbf;
int *havekey;
svkey *svkeyb;
int *entrycnt;
{   char scnfmt[MAXFMTLEN+1];
    int i,rtnval=0;

    for(i=0;i<nelm;i++)
    {   if(fldgrpbf[i].fldg_key)
        {   if(!strcmp((fldgrpbf[i].fldg_type),"i"))
                svkeyb->svk_int=(*((int *)
                    (fldgrpbf[i].fldg_val)));
            else
                svkeyb->svk_double=(*((double *)
                    (fldgrpbf[i].fldg_val)));
        }
        sprintf(&(scnfmt[0]),"%s%%%s",fldgrpbf[i].
            fldg_tag,(!strcmp(fldgrpbf[i].
            fldg_type,"i"))?"d":"lf");
        if(sscanf(fld,&(scnfmt[0]),fldgrpbf[i].fldg_val)==1)
        {   if(fldgrpbf[i].fldg_key)
            {   if(*havekey)
                {   rtnval=1;
                    (*entrycnt)++;
                }
                (*havekey)=1;
            }
            break;
        }
    }
    return rtnval;
}

static void ProcEntry(pass,skipcnt,skipmax,
    InstExec,IPMap_elb,AvgSize,FecundityAvg)
int pass,*skipcnt,AvgSize,FecundityAvg;
TPingData *IPMap_elb;
{   IPMap_elb->SoupSize=(AvgSize*(IPMap_elb->NumCells))*2;
    IPMap_elb->FecundityAvg=(int)(FecundityAvg*100.0);
    IPMap_elb->InstExec=InstExec;
    if(pass==DISPLPASS)
    {   if((*skipcnt)<skipmax)
           (*skipcnt)++;
        else
        {   printf("%5d %6d %4d %8d %7d %6d\n",
                IPMap_elb->FecundityAvg,
                IPMap_elb->Speed,
                IPMap_elb->NumCells,
                IPMap_elb->AgeAvg,
                IPMap_elb->SoupSize,
                IPMap_elb->InstExec);
            (*skipcnt)=0;
        }
    }
}
