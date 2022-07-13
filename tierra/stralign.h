#ifndef STRALIGN_H
#define STRALIGN_H

#define NUMSTRINGS 2
#define UNKNOWNSIDE 0
#define KNOWNSIDE   1

/* match types */
#define INITYP -1
#define ALIGN   0
#define GAP0    1
#define GAP1    2

#ifndef P_
#if defined(DJGPP)||defined(AMIGA)|| \
    defined(DECVAX)||defined(__MWERKS__)|| \
    defined(__TURBOC__)||defined(_WIN32)
#define ANSI
#else
#ifdef MSDOS
#ifndef DJGPP
#define ANSI
#endif /* DJGPP */
#else /* MSDOS */
#ifdef unix
#ifdef __GNUC__
#define ANSI
#else
#ifdef __sgi
#define ANSI
#endif /* __sgi*/
#endif /* __GNUC__ */
#endif /* unix */
#endif /* MSDOS */
#endif
#ifdef ANSI     /* ANSI prototyping */
#define P_(A) A
#else /* non-ANSI prototyping */
#define P_(A) ()
#endif
#endif /* P_ */

#define sb(ix1,ix2) (((ix2)*(dims[UNKNOWNSIDE]+2))+(ix1))
#define amax(x,y) (((*(x))>(*(y)))?(x):(y))

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{   double mtch_score;
    int mtch_type;
    int mtch_fwd_ix[NUMSTRINGS];
    int mtch_rev_ix[NUMSTRINGS];
    int mtch_execnt;
    int mtch_endix;
    double mtch_alignscore;
} match;

match *stralign P_((void **bufs,int *dims,
    int (*mtchcompar)(const void *a1,const void *a2),
    double threshold,double mtchwgt,double mismtchwgt,
    double gapopn,double gapext,int trk,
    int bufosiz,void *userdat,
    void (*bk_trc_init)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_enter_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_leave_align)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk),
    void (*bk_trc_iteration)(void *userdat,
        match *F,int *ix,int *dims,int inalign,int trk)));

void PrtStrAlign P_((match *F,int *dims,void *userdat,
    void (*enteralign)(void *userdat,match *F,
        int *ix,int *dims,int mtch_type),
    void (*aligndtl)(void *userdat,match *F,
        int *ix,int *dims,int mtch_type),
    void (*exitalign)(void *userdat,match *F,
        int *ix,int *dims,int mtch_type)));

#endif /* STRALIGN_H */
