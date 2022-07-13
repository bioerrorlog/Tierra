#include "stralign.h"

static void InitMatchScoreMatrix P_((match *F,int *dims));
static void CalcMatchScores P_((match *F,int *dims,
    void **bufs,double threshold,double mtchwgt,
    double mismtchwgt,double gapopn,double gapext,int trk,
    int (*mtchcompar)(const void *a1,const void *a2),int bufosiz));
static void BackTrace P_((match *F,int *dims,void *userdat,
    void (*bk_trc_init)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_enter_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_leave_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_iteration)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    int trk));

static void UpdateElScore P_((match *F,
    int dstix1,int dstix2,int srcix1,
    int srcix2,double newscore,int *dims));

match *stralign(bufs,dims,mtchcompar,
    threshold,mtchwgt,mismtchwgt,gapopn,gapext,
    trk,bufosiz,userdat,
    bk_trc_init,bk_trc_enter_align,
    bk_trc_leave_align,bk_trc_iteration)
void **bufs,*userdat,
    (*bk_trc_init)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    (*bk_trc_enter_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    (*bk_trc_leave_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    (*bk_trc_iteration)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk);
int (*mtchcompar)(const void *a1,const void *a2),
    trk,bufosiz,*dims;
double threshold,mtchwgt,mismtchwgt,gapopn,gapext;
{   match *F;

    if((F=(match *)malloc((dims[UNKNOWNSIDE]+2)*
        (dims[KNOWNSIDE]+1)*sizeof(match))))
    {   InitMatchScoreMatrix(F,&(dims[0]));
        
        /* calculate match scores */
        CalcMatchScores(F,&(dims[0]),&(bufs[0]),
            (dims[KNOWNSIDE])*threshold,
            mtchwgt,mismtchwgt,gapopn,gapext,
            trk,mtchcompar,bufosiz);
    
        /* back trace */
        BackTrace(F,&(dims[0]),userdat,
            bk_trc_init,bk_trc_enter_align,
            bk_trc_leave_align,bk_trc_iteration,trk);
    }
    else
        perror("stralign");
    return F;
}

/* initialize match score matrix */
static void InitMatchScoreMatrix(F,dims)
match *F;
int *dims;
{   int i,j,k;

    for(i=0;i<(dims[UNKNOWNSIDE]+2);i++)
        for(j=0;j<(dims[KNOWNSIDE]+1);j++)
        {   F[sb(i,j)].mtch_score=0.0;
            F[sb(i,j)].mtch_execnt=0;
            F[sb(i,j)].mtch_alignscore=0.0;
            F[sb(i,j)].mtch_endix=-1;
            for(k=0;k<NUMSTRINGS;k++)
                F[sb(i,j)].mtch_fwd_ix[k]=-1;
        }

    F[sb(0,0)].mtch_type=INITYP;
    F[sb(0,0)].mtch_rev_ix[UNKNOWNSIDE]=-1;
    F[sb(0,0)].mtch_rev_ix[KNOWNSIDE]=-1;

    for(i=1;i<(dims[UNKNOWNSIDE]+2);i++)
    {   F[sb(i,0)].mtch_type=GAP1;
        F[sb(i,0)].mtch_rev_ix[UNKNOWNSIDE]=i-1;
        F[sb(i,0)].mtch_rev_ix[KNOWNSIDE]=0;
    }

    for(j=1;j<(dims[KNOWNSIDE]+1);j++)
    {   F[sb(0,j)].mtch_type=GAP0;
        F[sb(0,j)].mtch_rev_ix[UNKNOWNSIDE]=0;
        F[sb(0,j)].mtch_rev_ix[KNOWNSIDE]=j-1;
    }

    for(i=1;i<(dims[UNKNOWNSIDE]+2);i++)
        for(j=1;j<(dims[KNOWNSIDE]+1);j++)
        {   F[sb(i,j)].mtch_type=INITYP;
            for(k=0;k<NUMSTRINGS;k++)
                F[sb(i,j)].mtch_rev_ix[k]=-1;
        }
}

/* calculate match scores */
static void CalcMatchScores(F,dims,bufs,threshold,
    mtchwgt,mismtchwgt,gapopn,gapext,trk,mtchcompar,bufosiz)
match *F;
int (*mtchcompar)(const void *a1,const void *a2),*dims,trk,bufosiz;
double threshold,mtchwgt,mismtchwgt,gapopn,gapext;
void **bufs;
{   int ix[NUMSTRINGS],j;
    double score_align,score_0_gap,score_1_gap,diff,*maxscore;

    for(ix[UNKNOWNSIDE]=1;ix[UNKNOWNSIDE]<
        (dims[UNKNOWNSIDE]+1);ix[UNKNOWNSIDE]++)
    {
        UpdateElScore(F,ix[UNKNOWNSIDE],0,ix[UNKNOWNSIDE]-1,0,
            F[sb(ix[UNKNOWNSIDE]-1,0)].mtch_score,&(dims[0]));
        for(j=1;j<(dims[KNOWNSIDE]+1);j++)
            if((diff=(F[sb(ix[UNKNOWNSIDE]-1,j)].mtch_score)-threshold)>
                (F[sb(ix[UNKNOWNSIDE],0)].mtch_score))
                UpdateElScore(F,ix[UNKNOWNSIDE],0,
                    ix[UNKNOWNSIDE]-1,j,diff,&(dims[0]));

        for(ix[KNOWNSIDE]=1;ix[KNOWNSIDE]<
            (dims[KNOWNSIDE]+1);ix[KNOWNSIDE]++)
        {
            score_align=F[sb(ix[UNKNOWNSIDE]-1,
                ix[KNOWNSIDE]-1)].mtch_score+
                ((*mtchcompar)((void *)(((char *)
                (bufs[UNKNOWNSIDE]))+((((ix[
                UNKNOWNSIDE]-1)*PLOIDY)+trk)*
                bufosiz)),(void *)(((char *)(
                bufs[KNOWNSIDE]))+((((ix[
                KNOWNSIDE]-1)*PLOIDY)+trk)*
                bufosiz)))?mtchwgt:mismtchwgt);

            score_1_gap=F[sb(ix[UNKNOWNSIDE]-1,
                ix[KNOWNSIDE])].mtch_score+
                (((F[sb(ix[UNKNOWNSIDE]-1,
                ix[KNOWNSIDE])].mtch_type)!=GAP1)?
                gapopn:gapext);

            score_0_gap=F[sb(ix[UNKNOWNSIDE],
                ix[KNOWNSIDE]-1)].mtch_score+
                (((F[sb(ix[UNKNOWNSIDE],
                ix[KNOWNSIDE]-1)].mtch_type)!=GAP0)?
                gapopn:gapext);

            maxscore=amax(&score_align,amax(&(
                F[sb(ix[UNKNOWNSIDE],0)].mtch_score),amax(
                &score_0_gap,&score_1_gap)));

            if(maxscore==&score_align)
            {   F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].mtch_type=ALIGN;
                UpdateElScore(F,ix[UNKNOWNSIDE],
                    ix[KNOWNSIDE],ix[UNKNOWNSIDE]-1,
                    ix[KNOWNSIDE]-1,score_align,&(dims[0]));
            }
            else if(maxscore==&(F[sb(ix[UNKNOWNSIDE],0)].mtch_score))
            {   UpdateElScore(F,ix[UNKNOWNSIDE],
                    ix[KNOWNSIDE],ix[UNKNOWNSIDE],0,
                    (F[sb(ix[UNKNOWNSIDE],0)].mtch_score),&(dims[0]));
            }
            else if(maxscore==&score_0_gap)
            {   F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].mtch_type=GAP0;
                UpdateElScore(F,ix[UNKNOWNSIDE],
                    ix[KNOWNSIDE],ix[UNKNOWNSIDE],
                    ix[KNOWNSIDE]-1,score_0_gap,&(dims[0]));
            }
            else if(maxscore==&score_1_gap)
            {   F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].mtch_type=GAP1;
                UpdateElScore(F,ix[UNKNOWNSIDE],
                    ix[KNOWNSIDE],ix[UNKNOWNSIDE]-1,
                    ix[KNOWNSIDE],score_1_gap,&(dims[0]));
            }
        }
    }
    UpdateElScore(F,dims[UNKNOWNSIDE]+1,0,dims[UNKNOWNSIDE],0,
        F[sb(dims[UNKNOWNSIDE],0)].mtch_score,&(dims[0]));
    for(j=1;j<(dims[KNOWNSIDE]+1);j++)
        if((diff=(F[sb(dims[UNKNOWNSIDE],j)].mtch_score)-threshold)>
            (F[sb(dims[UNKNOWNSIDE]+1,0)].mtch_score))
            UpdateElScore(F,dims[UNKNOWNSIDE]+1,0,
                dims[UNKNOWNSIDE],j,diff,&(dims[0]));
}

/* back trace */
static void BackTrace(F,dims,userdat,
    bk_trc_init,bk_trc_enter_align,
    bk_trc_leave_align,bk_trc_iteration,trk)
match *F;
int *dims,trk;
void *userdat,
    (*bk_trc_init)(void *userdat,match *F,
        int *ix,int *dims,int inalign,int trk),
    (*bk_trc_enter_align)(void *userdat,match *F,
        int *ix,int *dims,int inalign,int trk),
    (*bk_trc_leave_align)(void *userdat,match *F,
        int *ix,int *dims,int inalign,int trk),
    (*bk_trc_iteration)(void *userdat,match *F,
        int *ix,int *dims,int inalign,int trk);
{   int ix[NUMSTRINGS],prvix[NUMSTRINGS],svix[NUMSTRINGS],
        inalign=0,mtch_type;
    int endix=0;
    double endscore=0.0;
    (*bk_trc_init)(userdat,F,NULL,dims,inalign,trk),

    memcpy(&(ix[0]),&((F[sb(dims[UNKNOWNSIDE]+1,0)].
        mtch_rev_ix)[0]),NUMSTRINGS*sizeof(int));
    for(;(ix[UNKNOWNSIDE]>=0)||(ix[KNOWNSIDE]>=0);)
    {   if(ix[KNOWNSIDE]>0)
        {   mtch_type=F[sb(ix[UNKNOWNSIDE],
                ix[KNOWNSIDE])].mtch_type;
            if((mtch_type==ALIGN)||(mtch_type==GAP0)||
                (mtch_type==GAP1))
            {   if(!inalign)
                {   endscore=F[sb(ix[UNKNOWNSIDE],
                        ix[KNOWNSIDE])].mtch_score;
                    endix=ix[UNKNOWNSIDE];
                    (*bk_trc_enter_align)(userdat,
                        F,NULL,dims,inalign,trk),
                    inalign=1;
                }
                memcpy(&(svix[0]),&(ix[0]),
                    NUMSTRINGS*sizeof(int));
            }
        }
        else
        {   if(inalign)
            {   F[sb(svix[UNKNOWNSIDE],svix[KNOWNSIDE])].
                    mtch_alignscore=endscore-
                    (F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].mtch_score);
                F[sb(svix[UNKNOWNSIDE],svix[
                    KNOWNSIDE])].mtch_endix=endix;
                (*bk_trc_leave_align)(userdat,
                    F,&(svix[0]),dims,inalign,trk),
                inalign=0;
            }
        }
        (*bk_trc_iteration)(userdat,F,&(ix[0]),dims,inalign,trk);
        memcpy(&(prvix[0]),&((F[sb(ix[UNKNOWNSIDE],ix[KNOWNSIDE])].
            mtch_rev_ix)[0]),NUMSTRINGS*sizeof(int));
        if((prvix[UNKNOWNSIDE]>=0)&&(prvix[KNOWNSIDE]>=0))
            memcpy(&((F[sb(prvix[UNKNOWNSIDE],
                prvix[KNOWNSIDE])].mtch_fwd_ix)[0]),
                &(ix[0]),NUMSTRINGS*sizeof(int));
        memcpy(&(ix[0]),&(prvix[0]),NUMSTRINGS*sizeof(int));
    }
    if(inalign)
        F[sb(svix[UNKNOWNSIDE],svix[KNOWNSIDE])].
            mtch_alignscore=endscore;
}

static void UpdateElScore(F,dstix1,dstix2,srcix1,srcix2,newscore,dims)
match *F;
int dstix1,dstix2,srcix1,srcix2,*dims;
double newscore;
{   F[sb(dstix1,dstix2)].mtch_score=newscore;
    F[sb(dstix1,dstix2)].mtch_rev_ix[UNKNOWNSIDE]=srcix1;
    F[sb(dstix1,dstix2)].mtch_rev_ix[KNOWNSIDE]=srcix2;
}

/* PrtStrAlign - print alignments for single "standard" string
 * input
 *    F         - "match" array
 *    dims      - string sizes
 */
void PrtStrAlign(F,dims,userdat,enteralign,aligndtl,exitalign)
match *F;
int *dims;
void *userdat,
     (*enteralign)(void *userdat,match *F,int *ix,int *dims,int mtch_type),
     (*aligndtl)(void *userdat,match *F,int *ix,int *dims,int mtch_type),
     (*exitalign)(void *userdat,match *F,int *ix,int *dims,int mtch_type);
{   int ix[NUMSTRINGS],mtch_type=0,inalign=0;

    ix[UNKNOWNSIDE]=ix[KNOWNSIDE]=0;
    for(;(ix[UNKNOWNSIDE]>=0)&&(ix[KNOWNSIDE]>=0);
        memcpy(&(ix[0]),&(F[sb(ix[UNKNOWNSIDE],
        ix[KNOWNSIDE])].mtch_fwd_ix[0]),
        NUMSTRINGS*sizeof(int)))
    {
        if(ix[KNOWNSIDE]>0)
        {   mtch_type=F[sb(ix[UNKNOWNSIDE],
                ix[KNOWNSIDE])].mtch_type;
            if((mtch_type==ALIGN)||(mtch_type==GAP0)||
                (mtch_type==GAP1))
                if(!inalign)
                {   (*enteralign)(userdat,F,&(ix[0]),dims,mtch_type);
                    inalign=1;
                }
            (*aligndtl)(userdat,F,&(ix[0]),dims,mtch_type);
        }
        else if(inalign)
        {   inalign=0;
            (*exitalign)(userdat,F,&(ix[0]),dims,mtch_type);
        }
    }
}
