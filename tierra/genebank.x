#ifdef RPC_HDR
%#ifndef GENEBANK_H
%#define GENEBANK_H
%#include "portable.h"
%#include "tiexdrcom.h"
#endif /* RPC_HDR */

#ifdef RPC_XDR
%#include "port_xdr.h"
#endif /* RPC_XDR */

struct head_t
{   I32s g_off;            /* highest offset used + 1 */
    I32s size;             /* size of genome */
    I32s n;                /* number of genomes in bank */
    I32s n_alloc;          /* allocated size of bank */
    I32s n_thread;         /* number of threads listed */
    double hdsucsiznslrat; /* succ divide frac, prev million */
    I32s hdsvsiznsl;       /* # divides, prev million */
    I32s hdsvsucsiznsl;    /* # successful divides, prev million */
};

struct GBGenome
{   xdrwrd array <>;
};

struct GBGenBits
{   xdrwrd array <>;
};

struct Metabolism    /* structure for metabolic data */
{   I32s inst;       /* count of instructions executed */
    I32s instP;      /* count of parallel instructions executed */
    I32s flags;      /* count of flags (error conditions) set */
    I32s mov_daught; /* count of number of instructions moved to daughter */
    I32s BreedTrue;  /* 0=not, 1 = this daughter genetically same as parent */
};

struct indx_t
{   I8s  gen[4];
    I8s  pgen[4];
    I32s psize;
    Mem  mg;       /* cell genetic memory */
    I32s hash;
    I32u bits;
    I32s originC;
    Event originI;
    Event mppT;     /* last time of mpp update */
    I32s mpp;
    I32s mpi;
    I32s MaxPop;        /* MaxPop */
    Metabolism d1;
    Metabolism d2;
    I32s pt;       /* ploidy and track */
    I32s max_cpus; /* max num of CPUs simultaneously used during lifetime */
    I32s g_off;   /* archive offset of this genome */

#ifdef NET
    double ixsucgennslrat; /* succ divide frac, prev million */
    I32s ixsvgennsl;      /* # divides, orig gen. curr million */
    I32s ixsvsucgennsl;   /* # successful divides, orig gen. prev million */
#endif /* NET */
};

struct GBindx_t
{   indx_t array <>;
};

struct BaseThrdAnaDat           /* thread analysis data        */
{   I32s btad_extrnterm;        /* cause of termination        */
                                /*    if not self-terminated   */
    I32s btad_termcause;        /* termination cause for cell  */
                                /*    -1 = not self-terminated */
    I32s btad_termthrdid;       /* terminating thread          */
                                /*    if self-terminated       */
    I32s btad_termInstP;        /* termination InstP           */
    I32s btad_FilInstrThrd_ct;  /* FilInstrThrd                */
    I32s btad_CalLvlIntByID_ct; /* CalLvlIntByID entry count   */
    I32s btad_callvlintcnt;     /* call level interval count   */
};

struct FilThrdAnaDat                 /* thread analysis data */
{                                    /************************/
                                     /* dyn arr ctrl structs */
    DynArrCtrl ftad_thdatara_ctrl;   /* tad_thdatara         */
    DynArrCtrl ftad_cpysegbti_ctrl;  /* tad_cpysegbti        */
    DynArrCtrl ftad_codsegbti_ctrl;  /* tad_codsegbti        */
    DynArrCtrl ftad_event_ctrl;      /* tad_event            */
    DynArrCtrl ftad_soupupdevt_ctrl; /* tad_event            */
                                     /************************/
    I32s             ftad_rtcalvlint;     /* root call level intrvl id */
    BaseThrdAnaDat   ftad_base;           /* base thrd ana data struct */
};

struct BaseCallLvlIntrv
{   I32s          clb_id;             /* call level interval id */
    I32s          clb_parentid;       /* parent call level interval id */
                                      /* call level at intrvl strt */
    I32s          clb_celcallvl;      /*    cell                   */
    I32s          clb_thdcallvl;      /*    thrd                   */
    I32s          clb_entinp;         /* entry InstP */
    I32s          clb_enthipofsttype; /* entry offset type */
    I32s          clb_enthipofst;     /* entry offset */
    I32s          clb_extinp;         /* exit InstP */
    I32s          clb_exthipofsttype; /* exit offset type */
    I32s          clb_exthipofst;     /* exit offset */
    I32s          clb_prom;           /* 1 = dev gene indirectly called
                                             from parent level */
    I32s          clb_subintrvcnt;    /* count of nodes in clv_subintrv */
};

struct BasThreadDat
{   I32s tdt_dght1bytcpd;  /* number of bytes copied to 1st daughter */
    I32s tdt_dght1instex;  /* number of instr exec for 1st daughter */
    I32s tdt_dght2bytcpd;  /* number of bytes copied to 2nd daughter */
    I32s tdt_dght2instex;  /* number of instr exec for 2nd daughter */
    I32s tdt_instex;       /* number of instr exec */
    I32s tdt_fstinstex;    /* first instruction executed */
#ifdef HAVE_TDT_FST_LST_OFSTTYP
    I32s tdt_fstofsttyp;   /* first instruction executed - offset type */
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    I32s tdt_lstinstex;    /* last instruction executed */
#ifdef HAVE_TDT_FST_LST_OFSTTYP
    I32s tdt_lstofsttyp;   /* last instruction executed - offset type */
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
    I32s tdt_locrepl;      /* number of local replications */
    I32s tdt_nlocrepl;     /* number of nonlocal replications */
    I32s tdt_parent;       /* parent thread */
    I32u tdt_bits;         /* thread "GList" type bits */
    I32s tdt_cpu;          /* cpu for this thread */
    I32s tdt_termcause;    /* termination cause for this thread */
                           /* -1 = not caused by this thread */
    I32s tdt_fstInstP;     /* InstP at thread start */
    I32s tdt_lstInstP;     /* InstP of last instr exec */
};

struct IdxThreadDat
{   I32s         xtdt_threadid; /* thread id */
    BasThreadDat xtdt_thrdat;   /* thread data */
};

struct ThrdAnaEvent
{   I8u  taev_eventcode;     /* instruction opcode */

    I8u  taev_ipoffsettype; /* type of host */
                            /* 0 - own memory */
                            /* 1 - own daughter */
                            /* 2 - host memory */
                            /* 3 - host daughter */
                            /* 4 - free */
    I32u taev_ipoffset;

    I32u taev_thrd;

    I8u  taev_dstoffsettype; /* type of host */
                             /* 0 - own memory */
                             /* 1 - own daughter */
                             /* 2 - host memory */
                             /* 3 - host daughter */
                             /* 4 - free */
    I32s taev_dstoffset;

    N32u taev_i32a;
#ifdef USE_PORT
    N16u taev_port;
#endif /* USE_PORT */
    I8u  taev_nodetype;      /* (0-local, 1-remote) */
    I8u  taev_genetrn;       /* 1=gene transition */
    I32s taev_instP;
};

struct SoupUpdEvent
{   I8u  supd_eventcode;     /* instruction opcode */
    I8u  supd_ipoffsettype;  /* type of host */
                             /* 0 - own memory */
                             /* 1 - own daughter */
                             /* 2 - host memory */
                             /* 3 - host daughter */
                             /* 4 - free */
    I8u  supd_extrk;         /* execution track */
    I32u supd_ipoffset;
    I32u supd_thrd;
    I8u  supd_dstoffsettype; /* type of host */
                             /* 0 - own memory */
                             /* 1 - own daughter */
                             /* 2 - host memory */
                             /* 3 - host daughter */
                             /* 4 - free */
    I8u  supd_dtra;          /* destination track */
    I32s supd_dstoffset;     /* destination offset */
    I8u  supd_srcoffsettype; /* type of host */
                             /* 0 - own memory */
                             /* 1 - own daughter */
                             /* 2 - host memory */
                             /* 3 - host daughter */
                             /* 4 - free */
    I8u  supd_stra;          /* source track */
    I32s supd_srcoffset;     /* source offset */
    I32s supd_instP;
};

struct BasGenThrdDat
{   I32s gdt_opcnt;      /* op count */
    I32s gdt_dght1opcnt; /* op count to 1st daught - exe, cpy */
    I32s gdt_dght2opcnt; /* op count to 2nd daught - exe, cpy */
    I8s  gdt_gbits;      /* genome "GList gbits" type flags */
};

struct FilGenThrdDat
{   I32s          xgdt_bytoffst;      /* genome byte offset */
    DynArrCtrl    xgdt_thrd_ara_ctrl; /* mgdt_thrd_ara.mita_ctrl */
    BasGenThrdDat xgdt_gbdt;          /* genome byte thread data */
};

struct BasInstrThrd
{   I32s ith_nonprom;   /* 1 - nonpromoted */
    I32s ith_prom;      /* 1 - promoted */
};

struct FilInstrThrd
{   I32s fith_threadid;
    BasInstrThrd fith_base;
    DynArrCtrl fith_calvlint_ctrl;
};

#ifdef RPC_HDR
%#endif /* GENEBANK_H */
#endif /* RPC_HDR */
