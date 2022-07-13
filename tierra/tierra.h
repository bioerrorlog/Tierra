/* tierra.h 30-3-98 */
/* type, structure and function definitions for the Tierra Simulator */
/* Tierra Simulator V5.0: Copyright (c) 1991 - 1998 */
/* Tom Ray & Virtual Life */

/*
 * tierra_h_sccsid: @(#)tierra.h        1.5    7/21/92
 */

#ifndef LTIERRA_H
#define LTIERRA_H

#include "license.h"
#include "configur.h"
#include "portable.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define uint32_t unsigned int
#endif /* _WIN32 */

#include <stdio.h>
#ifndef SEEK_SET
#define SEEK_SET 0 /* Seek from beginning of file.  */
#define SEEK_CUR 1 /* Seek from current position.  */
#define SEEK_END 2 /* Seek from end of file.  */
#endif /* SEEK_SET */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else /* TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* HAVE_SYS_TIME_H */
#include <time.h>
#endif /* HAVE_SYS_TIME_H */
#endif /* TIME_WITH_SYS_TIME */

#define MAX_MONS 8
#define BITSPERBYTE 8
#define NAMBUFSIZ 80
#define TIERRALOGNAME "tierra.log"
#ifdef NET
#define TPINGLOGNAME "tping.dat"
#define MIGR_BUF_MAX 50
#endif /* NET */
#define NUMSPECREG 3
#define EXPTARRSIZ   80
#define NEXEPTRNCOLS  9
#define MAXEXEPTRNGAP 10
#define TISSUEDEF_INCR 10
#define TAEV_INCR 10
#define CODE_SEG_INCR 100
#define COPY_SEG_INCR 100
#define THREADID_INCR 10
#define CALVLINT_INCR 10
#define EXETAGGEN_INCR 10
#define MAXPRINTFMTSIZ 256
#define MAXGENDEFLINLEN 255
#define MAXGENDEFNAMLEN 255
#define GENDEFARRINC 10
#define MAXHOSTNAME 80

/* cell termination codes for setting tad_extrnterm */
#define REAP_LAZY               1 /* non-reproducing - LazyTol           */
#define REAP_DISTURB            2 /* disturbance                         */
#define REAP_HALT               3 /* halt of last cpu                    */
#define REAP_NON_NET_EJECT      4 /* non-net ejection                    */
#define REAP_SOUP_FULL          5 /* soup full                           */
#ifdef NET
#define REAP_APOCALYPSE       101 /* Apocalypse                          */
#define REAP_SUBNET           102 /* attempt to surf to different subnet */
#define REAP_SURF             103 /* normal surf                         */
#define REAP_DIVIDE           104 /* remote divide                       */
#endif /* NET */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(SOUPUPDTRC)||defined(CLSTRSRVR)
#include <errno.h>
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(SOUPUPDTRC)||defined(CLSTRSRVR) */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif /* HAVE_SYS_FILE_H */

#ifndef BGL_CLNT
#ifndef __MWERKS__
#ifdef _WIN32
#include <io.h>
#endif /* _WIN32 */
#endif /* __MWERKS__ */
#endif /* BGL_CLNT */

#ifdef unix
#include <signal.h>
#endif

#ifdef TSysV
#ifndef sigmask
#define sigmask(n) ((unsigned int)1 << (((n) - 1) & (32 - 1)))
#endif /* sigmask */

#ifdef DECVAX
#define sigmask(m)      (1 << ((m)-1))
#endif /* DECVAX */
#endif /* BGL_CLNT */

#ifdef __MWERKS__
#include <types.h>
#include <SIOUX.h>
#include <unistd.h>
#ifndef NET
#include <sys/types.h>
#include <sys/stat.h>
#endif /* NET */
#else /* __MWERKS__ */
#include <sys/types.h>
#include <sys/stat.h>

#ifdef unix
#ifndef HAVE_CRYPT_H
#ifdef __linux__
#define _XOPEN_SOURCE
#endif /* __linux__ */
#endif /* HAVE_CRYPT_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#else /* HAVE_CRYPT_H */
#if defined(NEED_CRYPT_PROTO)
extern char *crypt P_((const char *key, const char *salt));
#endif /* defined(NEED_CRYPT_PROTO) */
#endif /* HAVE_CRYPT_H */

#ifdef __linux__
extern int getdomainname __P((char *__name, size_t __len));
extern int nice __P((int __val));
#endif /* __linux__ */
#endif /* unix */

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif /* HAVE_SYS_WAIT_H */

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif /* HAVE_DIRENT_H */

#endif /* __MWERKS__ */

#ifdef DJGPP
#undef unix
#endif /* DJGPP */

#ifdef __TURBOC__
#include <conio.h>
#include <dir.h>
#include <dos.h>
#endif /* __TURBOC__ */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else /* HAVE_STDARG_H */
#ifdef _WIN32
#include <stdarg.h>
#else /* _WIN32 */
#ifdef AMIGA
#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */
#else /* AMIGA */
#ifndef __VARARGS_H__
#include <varargs.h>
#endif /* __VARARGS_H__ */
#endif /* AMIGA */
#endif /* _WIN32 */
#endif /* HAVE_STDARG_H */

#if defined(NET)||defined(BGL)||defined(TIEAUDIO)
#ifdef DECVAX
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include.sys]socket.h"
#else /* if not DECVAX */
#ifdef _WIN32   /* use WINSOCK */
#include <winsock.h>
typedef int socklen_t;
#else /* if not _WIN32 */
#include <sys/socket.h>
#endif /* not _WIN32 */
#endif /* not DECVAX */
#endif /* defined(NET)||defined(BGL)||defined(TIEAUDIO) */

#if defined(NET)||defined(BGL)
#ifdef __MWERKS__
#include <GUSI.h>
#endif /* __MWERKS__ */
#ifdef DECVAX
#include "multinet_root:[multinet.include.netinet]in.h"
#include "multinet_root:[multinet.include.arpa]inet.h"
#else /* if not DECVAX */
#ifndef _WIN32   /* use WINSOCK */
#include <netinet/in.h>
#ifndef __MWERKS__
#include <arpa/inet.h>
#endif /* __MWERKS__ */
#endif /* not _WIN32 */
#endif /* not DECVAX */
#endif /* defined(NET)||defined(BGL) */

#if defined(TIERRA)||defined(BGL)||defined(CLSTRSRVR)
#ifndef _WIN32
#if defined(NET)||defined(BGL)||defined(TIEAUDIO)
#include <netdb.h>
#endif /* defined(NET)||defined(BGL)||defined(TIEAUDIO) */
#endif /* not _WIN32 */
#endif /* defined(TIERRA)||defined(BGL)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(CLSTRSRVR)||defined(LOG2IPMAP)
#ifdef NET
#include "mesg.h"
#endif /* NET */
#endif /* defined(TIERRA)||defined(CLSTRSRVR)||defined(LOG2IPMAP) */

#ifndef BGL_CLNT
#ifdef unix
#if FRONTEND == BASIC
#include <curses.h>
#endif /* FRONTEND == BASIC */
#define KEYHIT() ( TC_Menu )
#endif /* unix */

#ifdef AMIGA
#include <exec/types.h>
#if FRONTEND==BASIC
#include <curses.h>
#define KEYHIT() (kbhit())
#else /* FRONTEND == BASIC */
#define KEYHIT() (TC_Menu)
#endif /* FRONTEND == BASIC */
#endif /* AMIGA */

#ifdef DECVAX
#if FRONTEND == BASIC
#include <curses.h>
#endif /* FRONTEND == BASIC */
#define KEYHIT() ( TC_Menu )
#endif /* DECVAX */

#if (OPSYS == BCDOS) || (OPSYS == DJGPPDOS) || (OPSYS == WIN32TIE)
#define KEYHIT() ( KeyHit() )
#endif /* (OPSYS == BCDOS) || (OPSYS == DJGPPDOS) || (OPSYS == WIN32TIE) */

#ifdef __MWERKS__
#define KEYHIT() ( FEProcessEvent() )
#endif /* __MWERKS__ */
#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(PROBE)||defined(ARGTIE)

#ifndef I32UONE
#define I32UONE (I32u) 1
#endif /* I32UONE */

#define IsBit(seed, bit) ((((I32u) seed) & (I32UONE << ((I32u) bit))) ? 1 : 0)
#define ad(A) ((A) >=0 ? ((A)%SoupSize) \
               : ((SoupSize-(-(A)%SoupSize))%SoupSize))
#define TNULL(pData) (((I32u)((unsigned long)(pData)))<((I32u)5))

#endif /* defined(TIERRA)||defined(PROBE)||defined(ARGTIE) */


#if defined(TIERRA)||defined(PROBE)||defined(ARGTIE)||defined(CLSTRSRVR)

#define tuintrand() ((unsigned int)(tdrand()*(((double)INTU_MAX)+1.0)))
#define tintrand()  ((int)(tdrand()*(((double)INTS_MAX)+1.0)))
#ifdef RAND64BIT
#define tlrand()    ((I32s)((((I32s)tintrand())<<\
                    ((I32s)((sizeof(I32s)*CHAR_BIT)/2)))|\
                    ((I32u)tuintrand())))
#define tulrand()   ((I32u)((((I32u)tuintrand())<<\
                    ((I32u)((sizeof(I32u)*CHAR_BIT)/2)))|\
                    ((I32u)tuintrand())))
#else /* RAND64BIT */
#define tulrand()   ((I32u)(tdrand()*(((double)I32U_MAX)+1.0)))
#define tlrand()    ((I32s)(tdrand()*(((double)I32S_MAX)+1.0)))
#endif /* RAND64BIT */

/* returns random 16 bit positive signed integer */
#define tirand()  ((I16s)(tdrand()*(((double)I16S_MAX)+1.0)))

/* returns random 16 bit unsigned integer */
#define tuirand() ((I16u)(tdrand()*(((double)I16U_MAX)+1.0)))

/* returns random 8 bit positive signed integer */
#define tcrand()  ((I8s)(tdrand()*(((double)I8S_MAX)+1.0)))

/* returns random 8 bit unsigned integer */
#define tucrand() ((I8u)(tdrand()*(((double)I8U_MAX)+1.0)))

#define mo(A,B) ((A) >=0 ? ((A)%(B)) : (((B)-(-(A)%(B)))%(B)))

#endif /* defined(TIERRA)||defined(PROBE)||defined(ARGTIE)||defined(CLSTRSRVR) */

#ifdef TIERRA
#define PLN_STATS 1
#define SIZ_HIST 2
#define SIZM_HIST 3
#define GEN_HIST 4
#define SIZ_QUERY 5
#define SIZ_EFF 6
#define GEN_EFF 7
#define SCRN_DEBUG 8

/* histogram sort order */
#define FREQ_SIZE 0  /* frequency/size */
#define SIZE_FREQ 1  /* size/frequency */
#define FREQ_GEN  2  /* frequency/genotype */
#define GEN_FREQ  3  /* genotype/frequency */

#endif /* TIERRA */


#if defined(TIERRA)||defined(CLSTRSRVR)
#ifndef _WIN32
#ifdef NET
#define OPEN_MAX_GUESS 256  /* if OPEN_MAX is indeterminate */
                            /* we're not guaranteed this is adequate */
#endif /* NET */
#endif /* _WIN32 */
#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR)
#define WRITE 1
#define NOWRITE 0
#define EXIT 1
#define NOEXIT 0
#endif /* defined(TIERRA)||defined(BGL_CLNT)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(CLSTRSRVR)||defined(SOUPUPDTRC)
#if defined(unix)||defined(AMIGA)
#include <rpc/xdr.h>
#else /* defined(unix)||defined(AMIGA) */
#include "xdr.h"
#endif /* defined(unix)||defined(AMIGA) */
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(CLSTRSRVR)||defined(SOUPUPDTRC) */

#ifdef TIERRA
#ifdef __GUI__
typedef struct
{   short szMarg;
    char szDesc[80];
}
wScreen;
#endif /* __GUI__ */
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef unsigned char Inst;
#if PLOIDY == 1
typedef Inst Instruction;
typedef I8s GenBits;
#else /* PLOIDY == 1 */
typedef Inst Instruction[PLOIDY];
typedef I8s GenBits[PLOIDY];
#endif /* PLOIDY == 1 */

/*
 * contiguous genome offsets with identical sets of associated threads
 * are grouped in the same interval
 * non-contiguous genome offsets or contiguous genome offsets with different
 * sets of associated threads are grouped in different intervals
 */
typedef struct
{   I32s oftd_start;   /* interval start  */
    I32s oftd_end;     /* interval end    */
    I32s oftd_thrd;    /* thread id       */
    I32s oftd_devstat; /* 0 - nonpromoted */
                       /* 1 - promoted    */
} ofst_thd;

typedef struct {
    I32s tsdf_tsdfstrt; /* ofst_thd array index of tissue */
                        /*    definition start            */
    I32s tsdf_tsdfcnt[2];  /* number of ofst_thd elements in */
                           /*    tissue definition           */
                           /* 0 - nonpromoted */
                           /* 1 - promoted    */
} TissueDef;

typedef struct {
    I32s thdi_thrd;
    I32s thdi_thdtistrt;
    I32s thdi_thdticnt[2];
} ThdTis;

typedef Instruction Fp FpInst;
typedef GenBits Fp FpGenB;

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef Instruction Hp HpInst;
typedef Instruction Np NpInst;
typedef Inst Fp FpIns;
typedef struct cell TCell;
typedef struct cell Fp Pcells;
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)
typedef struct g_list GList;
typedef struct g_list Fp Pgl;
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef I32s Reg;   /* type for use in CPU registers */
typedef Reg *Preg;  /* pointer to register */
typedef struct
{   I32s n;     /* allocated size of file list */
    I32s *l;    /* pointer to list of .tmp file names */
} TmpList;      /* list of names of .tmp files in genebank */
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)
typedef struct
{   I32s size;     /* size class (~species) */
    I8s label[4];  /* genotype label */
} Genotype;
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef struct
{   I32s si;    /* size index */
    I32s gi;    /* genotype index */
} GlIndex;
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(SOUPUPDTRC)

#include "tiexdrcom.h"

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

#include "genebank.h"

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(BGL_CLNT)||\
    defined(CLSTRSRVR)||defined(SOUPUPDTRC)

typedef struct
{   void          *dyna_array;    /* structure array   */
    DynArrCtrl     dyna_ctrl;     /* control structure */
} DynArr;

/* list structures */
typedef struct ListNode ListNode;
struct ListNode
{   ListNode *lnd_next;
    ListNode *lnd_prev;
};

typedef struct
{   ListNode *lsh_first;
    ListNode *lsh_firstlast;
    ListNode *lsh_last;
} ListHead;

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(BGL_CLNT)||
          defined(CLSTRSRVR)||defined(SOUPUPDTRC) */

#ifndef BGL_CLNT
#if defined(TIERRA)||defined(PROBE)||defined(ARGTIE)
typedef struct
{   ofst_thd      *ofta_ofstthd;  /* structure array   */
    DynArrCtrl     ofta_ctrl;     /* control structure */
} OfstThdArr;

typedef struct
{   TissueDef     *tsda_tisdef;   /* structure array   */
    DynArrCtrl     tsda_ctrl;     /* control structure */
} TissueDefArr;

typedef struct
{   TissueDefArr bdy_tisdefar;   /* tissue definitions */
    I32s         bdy_splttiscnt; /* split tissue count */
} BodyDef;

typedef struct
{   ThdTis        *thda_thdtis;   /* structure array   */
    DynArrCtrl     thda_ctrl;     /* control structure */
} ThdTisArr;

typedef struct /* "tissue type" lookup table - indexed by thread id */
{   I32s          *ttta_thdtsty;  /* structure array   */
    DynArrCtrl     ttta_ctrl;     /* control structure */
} ThdTsTyArr;
#endif /* defined(TIERRA)||defined(PROBE)||defined(ARGTIE) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef struct /* segment data indexed by offset */
{   ThrdAnaEvent  **taea_event;  /* structure array   */
    DynArrCtrl      taea_ctrl;   /* control structure */
} ThrdAnaEventArr;

typedef struct /* segment data indexed by offset */
{   SoupUpdEvent  **suea_event;  /* structure array   */
    DynArrCtrl      suea_ctrl;   /* control structure */
} SoupUpdEventArr;

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef struct /* structure for indexing cells in the cells arrays */
{   I32u a; /* which array */
    I32u i; /* which element of the array */
} CellInd;

#ifdef EXEINSTSIZTIMSLC
#define INSTEXECD 0 /* instruction executed */
typedef I8u CellGenBits;
#endif /* EXEINSTSIZTIMSLC */

typedef struct /* structure for demographic data */
{   Genotype gen; /* size and genotype name of cell */
    I32s RanID;   /* random ID # of for this cell */
    I32s gi;      /* index to this genotype in the gl array */
    I32s hash;    /* hash value for this genotype */
    Mem  mg;        /* main cell genetic memory */
    I32s fecundity; /* count of number of daughter cells produced */
    I32s sensattempt; /* count of number of sensory system attempts */
    I32s flags;     /* count of flags (error conditions) set */
    I32s mov_daught; /* count of number of instructions moved to daughter */
    I32s MovOffMin; /* minimun offset into daughter movii-ed to */
    I32s MovOffMax; /* maximum offset into daughter movii-ed to */
    I32s inst;   /* count of instructions executed */
    I32s repinst;   /* count of instructions executed in each reproduction */
    I32s instP;   /* count of parallel instructions executed */
    I32s mut;    /* 0 = no somatic mutations, >= 1 = somatic mutations */
    I32s flaw;   /* 0 = no flaws, >= 1 = flaws */
    I32s max_cpus; /* max number CPUs simultaneously used during lifetime */
    Metabolism d1; /* metabolic data for first daughter */
    Genotype parent; /* size and genotype name of parent */
    CellInd ne;      /* address of daughter cell */
    I8s is;      /* 1 = this cpu is active, in the slicer; 0 = not active */
    I8s dm;          /* 0 = mother, 1 = daughter */
    I8s ploidy;      /* how many tracks */
    FpInst genome;   /* pointer to genome itself, in soup */
    I32s   origgi;   /* original cell genotype */
    I32s nonslfmut;  /* "non-self" mutated */
    I32s srctrksw;  /* source track has switched */
    I32s thrdanacolldone; /* thread analysis data collection done */
    I32s birthtime; /* time of birth */
#ifdef NET
    BglTrackFlag BglTrack; /* flag for migration tracking by Beagle */
#endif /* NET */
#ifdef EXEINSTSIZTIMSLC
    CellGenBits *cellgenbits;
    I32s ExeSegSiz; /* total executed segments */
    I32s PExeSegSiz; /* total executed segments in parent */
#endif /* EXEINSTSIZTIMSLC */
} Dem;

typedef struct /* pointers to this, previous and next cells in queues */
{   CellInd htis;   /* index of this cell */
    CellInd n_time; /* index to next cell in slicer queue */
    CellInd p_time; /* index to previous cell in slicer queue */
    CellInd n_reap; /* index to next cell in reaper queue */
    CellInd p_reap; /* index to previous cell in reaper queue */
    CellInd h_fecu; /* index to next higher cell in fecundity queue */
    CellInd l_fecu; /* index to next  lower cell in fecundity queue */
} Que;

#ifdef IO

typedef struct
{   Preg pgbf_buf; /* put-get buffer pointer */
    I32s pgbf_siz; /* currently allocated size of buffer */
} PGbf;

typedef struct /* structure for put-get communications */
{   PGbf pgb_put; /* "put" buffer */
    PGbf pgb_get; /* "get" buffer */
} PGb;

typedef struct /* structure for global communications */
{   I32s gb; /* global buffer for inter-cpu communications */
    I8s ty;  /* type of communication */
    I8s np;  /* number participating so far */
} GloCom;

#endif /* IO */

typedef struct /* structure for communications data */
{   I32s len; /* length of message */
    I8s *d;   /* array for data communications */
} ComData;

typedef struct /* structure for IO for communication */
{   I32s t;    /* tag for type of message 0 reserved for free struct */
#ifdef NET
    ComAddr s; /* source address */
    ComAddr e; /* destination address */
#endif /* NET */
    ComData d; /* data */
} IOS;

typedef struct /* structure for IO for communications buffer */
{
#ifdef NET
    I32s ipic; /* index into IPMapC list, of NumNodesC */
    I32s ipic1; /* first index into IPMapC list, of NumNodesC */
    I32s ipis; /* index into IPMapS list, of NumNodesS */
    I32s ipis1; /* first index into IPMapS list, of NumNodesS */
#endif /* NET */
    I32s siz; /* currently allocated size of IOS buffer */
    I32s nio; /* index of next free IOS for incoming messages */
    IOS *io;  /* pointer to IOS buffer; 0 struct for outgoing */
} IOb;        /* 1 to (siz - 1) for incoming */
#endif /* TIERRA */

#if defined(TIERRA)||defined(CLSTRSRVR)
#ifdef NET

typedef struct /* structure for IPMap array */
{   NetAddr address;
    TPingData data;
    I32s PendReq;   /* number of pending TPing requests */
    I32s ipi1;
    TPrivPingData PrivPingData;
} MapNode;

typedef struct
{   I32u  SNumMess;
    I32u  SNumPack;
    I32u  SNumByte;
    I32u  RNumMess;
    I32u  RNumPack;
    I32u  RNumByte;
} ComCount;

#endif /* NET */
#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef struct
{   I8s n;  /* number of register to toggle */
    I8s *r; /* list of n registers to toggle */
} Rtog;     /* for keeping track of toggled registers */

typedef struct /* there will be only one global copy of this structure */
{   Rtog Se; /* list of segment registers */
    Rtog De; /* list of destination registers */
    Rtog So; /* list of source registers */
} IDRegs;

typedef struct
{   unsigned Se:1;  /* Offset Segment F -> O */
    unsigned B:1;   /* Bits */
    unsigned De:1;  /* Destination T -> D */
    unsigned So:1;  /* Source      G -> S */
    unsigned D:1;   /* Direction   D -> I */
    unsigned H:1;   /* sHadow registers */
    unsigned P:1;   /* reverse Polish */
    unsigned C:1;   /* speCial */
} IDflags;

typedef struct /* structure for instruction set definitions */
{   I8s op;                     /* op code */
    I32s cyc;                   /* the cost in cpu cycles of this instruct */
    I8s mn[9];                  /* assembler mnemonic */
    void (*execute) P_((void)); /* pointer to execute function */
    void (*decode) P_((void));  /* pointer to decode function */
    I8s re[8];                  /* register assignments */
    IDflags idf;                /* flags usage */
} InstDef;
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef struct
{   unsigned E:1;    /* Error : 1 = error condition has occurred
                   0 = no error condition occurred */
    unsigned S:1;    /* Sign : resulting sign of arithmetic operation,
                   0 = +, 1 = - */
    unsigned Z:1;    /* Zero : result of arithmetic or compare operation,
                   0 = nonzero, 1 = zero */
    unsigned B:2;    /* Bits: 00 = 32, 01 = 16, 10 = 8 */
    unsigned D:1;    /* Direction: for shift, rotate, move, search */
} Flags;

typedef struct
{   I8s i; /* index into Cpu.re array of registers */
    I8s t; /* index into Rtog.r list of registers */
} TRind;                          /* register toggle indexes */
    /* note that TRind.t will be toggled by incrementing it with the
       modulus Rtog.n, after each toggle, TRind.i will be updated to
       provide a more direct reference to the actual Cpu.re register */

typedef struct
{   TRind Se;   /* current segment register */
    TRind De;   /* current destination register */
    TRind So;   /* current  source register */
} CRflags;      /* CPU register flags */

typedef struct
{   I8s siz;    /* current allocated size of this buffer */
    I8s *sig;   /* signal buffer */
} TSignal;      /* Signal buffer for cell */

typedef struct
{   I32s ncpu; /* number of CPUs in this sync group */
    I32s sync; /* 0 = sync not started, !0 = count remaining unsynced CPUs */
} Sync;        /* synchronization group structure */

typedef struct
{   I32s n;   /* allocated size of sync group array */
    Sync *sy; /* sync group array */
} SyncA;      /* sync group array structure */

typedef struct
{   I32s G;    /* sync group #, -1 = no group */
    I8s  sync; /* wait for sync */
} CSync;       /* CPU sync structure */
/* Structures for new soup allocator. CJS, July 1992. */

typedef struct /* Describes a free area of the soup */
{  I32s l,     /* Index of left son in cartesian tree; 
                   or index of 1st recycled node (>0);
                   or -(number of untouched nodes)   */
        r,      /* Index of rt son in cartesian tree */
        p,      /* Soup addr of this unoccupied area */
        s;      /* Size of the area (in instr slots) */
} MemFr;

typedef MemFr Fp Pmf;
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef struct      /* gene definition  */
{   I8s *gdf_name;  /* name             */
    I32s gdf_start; /* start            */
    I32s gdf_end;   /* end              */
    I32s gdf_typ;   /* type             */
                    /*    0 - ordinary  */
                    /*    1 - dev       */
    I8s *gdf_cmnt;  /* comment          */
#ifdef GENALIGN
    I32s gdf_cnt;   /* name usage count */
#endif /* GENALIGN */
} GenDef;

typedef struct
{   GenDef        *gdf_array;    /* indexed by order of occurrence */
    DynArrCtrl     gdf_ctrl;     /* control structure */
} GeneDefArr;
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
/* definitions of bits in struct g_list:
   bit  0  permanent genotype name, saved in .gen file
   bit  1  gen is saved to disk, but went extinct, then reappeared,
   and has not yet re-crossed save threshold
   bit  2  EXs = executes own instructions (self)
   bit  3  EXd = executes daughter's instructions
   bit  4  EXo = executes other cell's instructions
   bit  5  EXf = executes instructions in free memory
   bit  6  EXh = own instructions are executed by other creature (host)
   bit  7  TCs = matches template complement of self
   bit  8  TCd = matches template complement of daughter
   bit  9  TCo = matches template complement of other
   bit 10  TCf = matches template complement of free memory
   bit 11  TCh = own template complement is matched by other creature (host)
   bit 12  TPs = uses template pattern of self
   bit 13  TPd = uses template pattern of daughter
   bit 14  TPo = uses template pattern of other
   bit 15  TPf = uses template pattern of free memory
   bit 16  TPh = own template pattern is used by other creature (host)
   bit 17  MFs = moves instruction from self
   bit 18  MFd = moves instruction from daughter
   bit 19  MFo = moves instruction from other cell
   bit 20  MFf = moves instruction from free memory
   bit 21  MFh = own instructions are moved by other creature (host)
   bit 22  MTs = moves instruction to self
   bit 23  MTd = moves instruction to daughter
   bit 24  MTo = moves instruction to other cell
   bit 25  MTf = moves instruction to free memory
   bit 26  MTh = is written on by another creature (host)
   bit 27  MBs = executing other creatures code, moves inst from self
   bit 28  MBd = executing other creatures code, moves inst from daughter
   bit 29  MBo = executing other creatures code, moves inst from other cell
   bit 30  MBf = executing other creatures code, moves inst from free memory
   bit 31  MBh = other creature uses another cpu to move your instructions
 */

typedef struct /* structure for size list */
{   I32s num_c; /* # adult creatures of this size in soup */
    I32s num_g; /* # genotypes of this size extant in soup */
    I32s a_num; /* allocated size of *g array */
    GList **g;  /* array of GList structures */
    I32s *hash; /* hash array */
    double AvgRpdEff[2]; /* size class average reproduction efficiency */
                         /* (instructions executed / bytes copied */
    I32s AvgEffCnt[2];   /* count of genotypes included in average */
                         /* reproduction efficiency */
    double sucsiznslrat; /* succ divide frac, prev million */

#ifdef NET
    I32s siznsl; /* # divides, curr million */
    I32s sucsiznsl; /* # successful divides, curr million */
    I32s svsiznsl; /* # divides, prev million */
    I32s svsucsiznsl; /* # successful divides, prev million */
#endif /* NET */

    I16s *genelkup;  /* gene lookup table */
    GeneDefArr slst_gendef;
} SList;

typedef struct
{   I32u size;
    I32s lbl;
    I32s count;
    double dblcount;
    I8u nstar;
} HistType;

typedef struct /* record of last data output to disk */
{   I32s time;  /* elapsed time */
    I32s mtime; /* millions of instructions */
    I8s bd;     /* b = birth, d = death */
    I32s size;  /* size of creature */
    I8s label[4]; /* genotype name of creature, e.g., aaa */
} LastOut;

#ifdef MEM_PROF

typedef struct
{   I32s label; /* arbitrary label for this kind of allocation */
    I32s num;   /* current number of allocations of this type */
    I32s siz;   /* amount of memory held in allocations of this type, bytes */
} MemProf;

typedef struct
{   I32s DynMemUse;  /* total dynamic memory allocated */
    I32s TotNumAloc; /* total number of dynamic memory blocks allocated */
    I32s AlocSiz;    /* current allocated size of MemProf array */
    MemProf *mpa;    /* pointer to MemProf array */
} MemProfStat;
#endif /* MEM_PROF */

#if defined(AMIGA)||defined(DECVAX)
struct timeval
{   long tv_sec;
    long tv_usec;
};
#endif /* defined(AMIGA)||defined(DECVAX) */

#ifndef MON

typedef struct /* Multiple Monitor Socket */
{   I32s fd;   /* File descriptor */
    I8s flg;   /* Flag for if this socket is used(=T) or unused(=F) */
} MonSock;

#endif /* MON */

#ifdef DECVAX
#define bzero(a,b) memset(a,0,b)
#endif /* DECVAX */


#ifdef _WIN32
#define bcopy(a,b,c) memmove(b,a,c)
#define bzero(a,b) memset(a,0,b)
#endif /* _WIN32 */

typedef struct
{   Pcells MicroSlice;   /* cell that is selected */
    I32s MicroCpuI;      /* selected thread */
#ifdef MICRO
    I32s TrapType;       /* trap type */
    I32s BrkSet;         /* "break point set" flag */
    I32s BrkFnd;         /* "break point found" flag */
    I8s szclsgn;         /* size class or genotype */
    GlIndex prim_genome; /* primary genome */
    I32s sizecls;        /* size class */
    GlIndex host_genome; /* host genome */
    I32s genoffset;      /* genome offset */
    Event AbsTime;       /* absolute stop time. */
    Event IntervalTime;  /* interval stop time. */
    double pctchg;       /* percent change */
    I32s lowlim;         /* lower boundary of limit */
    I32s hilim;          /* upper boundary of limit */
    I8s permtemp[2];     /* p-permanent, t-temporary, b-both */
    double avgpoprng;    /* average population over range. */
    I32s trkcurthrd;     /* 1 = track current thread */
#endif /* MICRO */
} BrkTrapDat;

typedef struct
{   I32s NewCpuI;
    BrkTrapDat curbrkdat;
    BrkTrapDat savbrkdat;
} BrkTrapCond;
#ifdef MICRO

#define VIEWSOUPHDRLINES                 3

#define BRKTYP_GIVENGEN_OFFSET           1
#define BRKTYP_CURCELL_OFFSET            2
#define BRKTYP_ABSTIMEINST               3
#define BRKTYP_INTRVLTIMEINST            4
#define BRKTYP_GIVENPARAGIVENHOST        5
#define BRKTYP_PCTCHGPOP                 6
#define BRKTYP_ANYPARAANYHOSTTIME        7
#define BRKTYP_ANYGENGIVENTIME           8
#define BRKTYP_GIVENPARAANYHOST          9
#define BRKTYP_ANYPARAGIVENHOST         10
#define BRKTYP_ANYGENSIZERANGE          11
#define BRKTYP_ANYPARA_HOSTSIZERANGE    12
#define BRKTYP_ANYHOST_PARASIZERANGE    13
#define BRKTYP_THRDANADATRDY            14

#endif /* MICRO */

typedef struct
{   I32s count;
    I32s lbl;
    I32s moves;
    I32s flags;
    I32u bits;
} size_buf;
#endif /* TIERRA */

#if defined(TIERRA)||defined(CLSTRSRVR)
typedef struct
{   I8s type;
    I8s *s;
    I32u h;
    I32s i;
    float f;
    double d;
    I8s name[40];
    I8s value[40];
} VarQuer;
#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef struct CallLvlIntrv CallLvlIntrv;
struct CallLvlIntrv /* call level interval structure */
{   ListNode      clv_listnode;       /* link to list at this level */
    ListHead      clv_subintrv;       /* subordinate intervals */
    CallLvlIntrv *clv_parent;         /* parent call level interval */
    BaseCallLvlIntrv clv_base;        /* base call level interval struct */
};
#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifndef BGL_CLNT
#ifdef TIERRA
typedef struct  /* struct for passing arguments from decode to execute */
{   Preg sreg;  /* pointer to source register */
    FpIns sins; /* pointer to source instruction */
    I32s sval;  /* source value */
    I8s stra;   /* source track */

    Preg dreg;  /* pointer to destination register */
    I32s dval;  /* original destination value */
    FpIns dins; /* pointer to destination instruction */
    I8s dtra;   /* destination track */
    I32s dmod;  /* destination modulused positive this size */
    I32s dran;  /* destination kept in signed range of this value */
    Pcells dcel; /* destination cell */

    Preg sreg2; /* pointer to 2nd source register */
    FpIns sins2;/* pointer to 2nd source instruction */
    I32s sval2; /* 2nd source value */
    I8s stra2;  /* 2nd source track */

    Preg dreg2; /* pointer to 2nd destination register */
    I32s dval2; /* original destination value */
    FpIns dins2;/* pointer to 2nd destination instruction */
    I8s dtra2;  /* 2nd destination track */
    I32s dmod2; /* 2nd dest modulused positive this size */
    I32s dran2; /* 2nd dest kept in signed range of this value */

    Preg sreg3; /* pointer to 3rd source register */
    FpIns sins3;/* pointer to source instruction */
    I32s sval3; /* 3rd source value */
    I8s stra3;  /* 3rd source track */

    Preg dreg3; /* pointer to 3rd destination register */
    I32s dval3; /* original destination value */
    FpIns dins3;/* pointer to 3rd destination instruction */
    I8s dtra3;  /* 3rd destination track */
    I32s dmod3; /* 3rd dest modulused positive this size */
    I32s dran3; /* 3rd dest kept in signed range of this value */

    Preg sreg4; /* pointer to 4th source register */
    FpIns sins4;/* pointer to 4th instruction */
    I32s sval4; /* 4th source value */
    I8s stra4;  /* 3rd source track */

    Preg dreg4; /* pointer to 4th destination register */
    I32s dval4; /* original destination value */
    FpIns dins4;/* pointer to 4th destination instruction */
    I8s dtra4;  /* 4th destination track */
    I32s dmod4; /* 4th dest modulused positive this size */
    I32s dran4; /* 4th dest kept in signed range of this value */

    I8s mode;   /* mode of instruction */
    I8s mode2;  /* 2nd mode of instruction */
    I8s mode3;  /* 3rd mode of instruction */

    I8s expr;   /* execute protection 0 = no protection, 1 = protected */
    I32s oip;   /* address of instruction being executed: ce->c.ip */
    I32s cipofst; /* ip offset relative to currently executing cell */

    I8u hipofsttype; /* type of host */
                     /* 0 - own memory */
                     /* 1 - own daughter */
                     /* 2 - host memory */
                     /* 3 - host daughter */
                     /* 4 - free */
    I32s hipofst; /* ip offset relative to host */
    I32s oncpu; /* number of allocated cpus */
    I32s oac;   /* number of this active cpu */
    I32s othreadid; /* currently executing thread id */
    Dem odem; /* cell demographic data */
    Mem ocellmem;
    I32s oextrk; /* execution track */

    I8u odstoffsettype; /* type of host */
                        /* 0 - own memory */
                        /* 1 - own daughter */
                        /* 2 - host memory */
                        /* 3 - host daughter */
                        /* 4 - free */
    I32s odstoffset; /* destination offset */

    N32u oi32a;
    N16u oport;
    I8u  onodetype;
    I32s divide; /* 1 - last instruction was divide */
    FpIns eins; /* pointer to instruction being executed */
    I32s  di;   /* decoded instruction (current)  for debugging */
    I32s pdi;   /* decoded instruction (previous) for debugging */
    I8s iip;    /* amount to increment instruction pointer */
    I8s dib;    /* amount to decrement instruction bank */
    I32s ts;    /* size of time slice, used to control central loop */
    I32s oipic1; /* first index into IPMapC list, of NumNodesC */
    I32s oipis1; /* first index into IPMapS list, of NumNodesS */
    CallLvlIntrv *crcalvlint; /* current cpu current call interval */
} PInst;
#endif /* TIERRA */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(SOUPUPDTRC)

typedef struct
{   CallLvlIntrv   **clid_array; /* structure array   */
                                 /* indexed by call level interval id */
    DynArrCtrl       clid_ctrl;  /* control structure */
} CalLvlIntByIDArr;

typedef struct
{   BasInstrThrd       mith_base;
    CalLvlIntByIDArr   mith_calvlint;    /* call level interval array */
} MemInstThrd;

typedef struct
{   MemInstThrd  **mita_array;    /* structure array   */
    DynArrCtrl      mita_ctrl;     /* control structure */
} MemInstThrdArr;

typedef struct
{   MemInstThrdArr mgdt_thrd_ara; /* thread array pointer */
    BasGenThrdDat mgdt_gbdt;     /* genome byte thread data */
} MemGenThrdDatEl;

#if PLOIDY == 1
typedef MemGenThrdDatEl MemGenThrdDat;
#else /* PLOIDY == 1 */
typedef MemGenThrdDatEl MemGenThrdDat[PLOIDY];
#endif /* PLOIDY == 1 */

typedef struct /* segment data indexed by offset */
{   MemGenThrdDat **mgda_segbti; /* structure array   */
    DynArrCtrl      mgda_ctrl;   /* control structure */
} MemGenThrdDatArr;

typedef struct
{   BasThreadDat  mtdt_basthrdat;
} MemThreadDat;

typedef struct                    /* indexed by thread id */
{   MemThreadDat **mtda_thdatara; /* structure array   */
    DynArrCtrl     mtda_ctrl;     /* control structure */
} MemThreadDatArr;

/*
 * Thread analysis data is accessed via the ThrdAnaDat structure.  An instance
 * of this structure exists for each adult cell.  An instance also exists for
 * each genotype listed in the ram bank.
 */
typedef struct                         /* thread analysis data           */
{   MemThreadDatArr   mtad_thdatara;   /* index by thread id             */
    MemGenThrdDatArr  mtad_cpysegbti;  /* copied seg-indexed by offset   */
    MemGenThrdDatArr  mtad_codsegbti;  /* executed seg-indexed by offset */
    ThrdAnaEventArr   mtad_event;      /* event list                     */
    SoupUpdEventArr   mtad_soupupdevt; /* soup memory update event       */
    CallLvlIntrv     *mtad_rtcalvlint; /* call level interval tree root  */
    BaseThrdAnaDat    mtad_base;       /* base thrd ana data struct      */
} MemThrdAnaDat;

struct g_list /* structure for genotype list */
{   I32s pop;           /* current number of adults of this genotype in soup */
    I32s origpop;       /* curr num of adults originally of this genotype */
    Genotype gen;       /* genotype of creature */
    Genotype parent;    /* genotype of parent genotype (ancestor) */
    Mem  mg;            /* cell genetic memory */
    I32s hash;          /* hash number for genome identification */
    I32u bits;          /* see below */
    Metabolism d1;      /* metabolic data for first daughter */
    Metabolism d2;      /* metabolic data for second daughter */
    Event originI;      /* time of origin, in instruction time */
    I32s originC;       /* time of first origin of genotype, in clock time */
    double MaxPropPop;   /* max. propor. of NumCells reached by this gen. */
    double MaxPropInst;  /* max. propor. of SoupSize reached by this gen. */
    I32s  MaxPop;     /* max population reached by this gen */
    Event mpp_time;     /* most recent time of MaxPropPop update */
    I8s ploidy;         /* how many tracks */
    FpInst genome;      /* pointer to genome itself */
    FpGenB gbits;       /* pointer to genome bit field */
    I32s max_cpus;      /* max number CPUs simult used during lifetime */

#ifdef NET
    double sucgennslrat; /* succ divide frac, prev million */
    I32s gennsl;        /* # divides, orig gen. curr million */
    I32s sucgennsl;     /* # successful divides, orig gen. prev million */
    I32s svgennsl;      /* # divides, orig gen. curr million */
    I32s svsucgennsl;   /* # successful divides, orig gen. prev million */
#endif /* NET */

    MemThrdAnaDat glst_thrdanadat; /* thread analysis data */    
};

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(SOUPUPDTRC) */

#ifdef TIERRA
typedef struct /* structure for registers of virtual cpu */
{   Reg re[ALOC_REG];         /* array of registers */
    Reg ip;                   /* instruction pointer */
    Reg sp;                   /* stack pointer */
    Reg st[STACK_SIZE];       /* stack */
    Flags fl;                 /* flags */
    CRflags cf;               /* CPU Register flags */
    CSync sy;                 /* CPU sync structure */
    I8s slicexit;             /* this cpu exits the current slice */
    I32s threadid;            /* thread id */
    I32s parthrdid;           /* parent thread id */
    I16s prvgene;             /* previous gene */
    I16s curgene;             /* current gene */

    I32s prvcallins;          /* 1 = previous instruction was a "call" */
    /* if previous instr was a "call" and templ compl existed */
    I32s prvcalltcmpstr;      /* template complement start, else -1 */
    I32s prvcalltcmplen;      /* template complement length, else 0 */

    I32s callins;             /* 1 = current instruction is a "call" */
    /* if current instr is a "call" and templ compl exists */
    I32s calltcmpstr;         /* template complement start, else -1 */
    I32s calltcmplen;         /* template complement length, else 0 */

    I32s prvretins;          /* 1 = previous instruction was a "ret" */
    I32s retins;             /* 1 = current instruction is a "ret" */
    I32s curcallvlcell;       /*    cell                      */
    I32s curcallvlthrd;       /*    thread                    */
#if PLOIDY > 1
    I8s ex;                   /* track of execution */
    I8s so;                   /* source trace for reads */
    I8s de;                   /* destination track for writes */
    I8s wc;                   /* wait count for error-track switching */
#endif /* PLOIDY > 1 */
    I32s          cpu_crcalvlid;  /* current call level interval id */
    CallLvlIntrv *cpu_crcalvlint; /* current call level interval */
} Cpu;

typedef struct /* structure for cpu array */
{   TSignal sig;    /* signal buffer for this cell */
    I32s ib;        /* instruction bank */
    I32s ac;        /* number of this active cpu */
    Cpu *c;         /* pointer to currently active cpu */
    I32s n;         /* number of allocated cpus */
    Cpu *ar;        /* pointer to array of cpus */
    SyncA sy;       /* sync group array */
    InstDef *d;     /* pointer to current InstDef structure for parsing */
    IOb io;         /* IO buffer for network communications */
#ifdef IO
    PGb pg;         /* IO buffer for put - get communications */
    GloCom gc;
#endif /* IO */
    I32s threadct; /* cell thread count */
} CpuA;

struct cell /* structure for cell of organisms */
{   Dem d;  /* fecundity and times and dates of birth and death */
    Que q;  /* pointers to previous and next cells in queues */
    Mem mm; /* main cell memory */
    Mem md; /* daughter cell memory */
    CpuA c; /* virtual cpus */
    I8s ld; /* 0 = dead, 1 = alive */
    MemThrdAnaDat cell_thrdanadat; /* cell thread analysis data */
};          /* sizeof(struct cell) = XX */

/* type for tracking communication */
typedef enum {M_normal, M_apocalypse, M_sleep, M_mutated} MigrStatus;

#ifdef DMALLOC
   #include <dmalloc.h>
#endif /* DMALLOC */

typedef I32s HAMMINGDIST;

#endif /* TIERRA */

#if defined(TIERRA)||defined(CLSTRSRVR)
#ifdef NET
#include "tiecomm.h"
#if NETTYPE == UDPASM
#include "datpkt.h"
#endif /* NETTYPE == UDPASM */
#endif /* NET */
#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#ifdef MEM_CHECKER
#include "checker_api.h"
#endif /* MEM_CHECKER */

#if defined(TIERRA)||defined(ARGTIE)||\
    defined(GENALIGN)||defined(THREADTREE)||\
    defined(PROBE)||defined(CLSTRSRVR)||\
    defined(SOUPUPDTRC)

typedef struct
{   I32s pec_head_t;
    I32s pec_indx_t;
    I32s pec_FilThrdAnaDat;
    I32s pec_BaseCallLvlIntrv;
    I32s pec_IdxThreadDat;
    I32s pec_ThrdAnaEvent;
    I32s pec_SoupUpdEvent;
    I32s pec_FilGenThrdDat;
    I32s pec_FilInstrThrd;
    I32s pec_pkthdr;
} prec_enc_size;

#endif /* defined(TIERRA)||defined(ARGTIE)||
          defined(GENALIGN)||defined(THREADTREE)||
          defined(PROBE)||defined(CLSTRSRVR)||
          defined(SOUPUPDTRC) */

#endif /* BGL_CLNT */

#if defined(TIERRA)||defined(CLSTRSRVR)
typedef I16u chksum;
typedef I16s schksum;
typedef I32u cksacc;
#define ntohchksum ntohs
#define htonchksum htons
#define ARBLRGSPEED 30000
typedef struct
{   union
    {   chksum cksh_chksum;
        I32u   cksh_pad;
    } cksmhdr_u;
} cksmhdr;

#endif /* defined(TIERRA)||defined(CLSTRSRVR) */

#if defined(TIERRA)||defined(ARGTIE)

#define CLSIMTXDIMS  2
#define CLSIMTXROW   0
#define CLSIMTXCOL   1
#define CLSTR_INC   10

#define nmax(x,y) (((x)>(y))?(x):(y))
#define nmin(x,y) (((x)<(y))?(x):(y))

#define NSTRTISTYP 2
#define STRTISINST 0
#define STRTISGENE 1

typedef struct Clstr Clstr;
struct Clstr
{   ListNode  clstr_node;
    I32s      clstr_leafid;
    double    clstr_mrgval;
    Clstr    *clstr_parent;
#ifdef CLSTRAVGLNK
    double       clstr_wgt;
#endif /* CLSTRAVGLNK */
    ListHead  clstr_list;
};

typedef struct
{   Clstr      **clsta_array; /* indexed by order of occurrence */
    DynArrCtrl   clsta_ctrl;  /* control structure */
} ClstrArr;

#endif /* defined(TIERRA)||defined(ARGTIE) */

#if defined(NEED_STRERROR)
   #define strerror(errnum) (sys_errlist[(errnum)])
   extern char *sys_errlist[];
#endif

#include "prototyp.h"

#endif /* LTIERRA_H */
