#ifndef _GENEBANK_H_RPCGEN
#define _GENEBANK_H_RPCGEN

#ifndef _WIN32
#include <rpc/rpc.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GENEBANK_H
#define GENEBANK_H
#include "portable.h"
#include "tiexdrcom.h"

struct head_t {
	I32s g_off;
	I32s size;
	I32s n;
	I32s n_alloc;
	I32s n_thread;
	double hdsucsiznslrat;
	I32s hdsvsiznsl;
	I32s hdsvsucsiznsl;
};
typedef struct head_t head_t;

struct GBGenome {
	struct {
		u_int array_len;
		xdrwrd *array_val;
	} array;
};
typedef struct GBGenome GBGenome;

struct GBGenBits {
	struct {
		u_int array_len;
		xdrwrd *array_val;
	} array;
};
typedef struct GBGenBits GBGenBits;

struct Metabolism {
	I32s inst;
	I32s instP;
	I32s flags;
	I32s mov_daught;
	I32s BreedTrue;
};
typedef struct Metabolism Metabolism;

struct indx_t {
	I8s gen[4];
	I8s pgen[4];
	I32s psize;
	Mem mg;
	I32s hash;
	I32u bits;
	I32s originC;
	Event originI;
	Event mppT;
	I32s mpp;
	I32s mpi;
	I32s MaxPop;
	Metabolism d1;
	Metabolism d2;
	I32s pt;
	I32s max_cpus;
	I32s g_off;
#ifdef NET
	double ixsucgennslrat;
	I32s ixsvgennsl;
	I32s ixsvsucgennsl;
#endif /* NET */
};
typedef struct indx_t indx_t;

struct GBindx_t {
	struct {
		u_int array_len;
		indx_t *array_val;
	} array;
};
typedef struct GBindx_t GBindx_t;

struct BaseThrdAnaDat {
	I32s btad_extrnterm;
	I32s btad_termcause;
	I32s btad_termthrdid;
	I32s btad_termInstP;
	I32s btad_FilInstrThrd_ct;
	I32s btad_CalLvlIntByID_ct;
	I32s btad_callvlintcnt;
};
typedef struct BaseThrdAnaDat BaseThrdAnaDat;

struct FilThrdAnaDat {
	DynArrCtrl ftad_thdatara_ctrl;
	DynArrCtrl ftad_cpysegbti_ctrl;
	DynArrCtrl ftad_codsegbti_ctrl;
	DynArrCtrl ftad_event_ctrl;
	DynArrCtrl ftad_soupupdevt_ctrl;
	I32s ftad_rtcalvlint;
	BaseThrdAnaDat ftad_base;
};
typedef struct FilThrdAnaDat FilThrdAnaDat;

struct BaseCallLvlIntrv {
	I32s clb_id;
	I32s clb_parentid;
	I32s clb_celcallvl;
	I32s clb_thdcallvl;
	I32s clb_entinp;
	I32s clb_enthipofsttype;
	I32s clb_enthipofst;
	I32s clb_extinp;
	I32s clb_exthipofsttype;
	I32s clb_exthipofst;
	I32s clb_prom;
	I32s clb_subintrvcnt;
};
typedef struct BaseCallLvlIntrv BaseCallLvlIntrv;

struct BasThreadDat {
	I32s tdt_dght1bytcpd;
	I32s tdt_dght1instex;
	I32s tdt_dght2bytcpd;
	I32s tdt_dght2instex;
	I32s tdt_instex;
	I32s tdt_fstinstex;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
	I32s tdt_fstofsttyp;
#endif /*HAVE_TDT_FST_LST_OFSTTYP */
	I32s tdt_lstinstex;
#ifdef HAVE_TDT_FST_LST_OFSTTYP
	I32s tdt_lstofsttyp;
#endif /* HAVE_TDT_FST_LST_OFSTTYP */
	I32s tdt_locrepl;
	I32s tdt_nlocrepl;
	I32s tdt_parent;
	I32u tdt_bits;
	I32s tdt_cpu;
	I32s tdt_termcause;
	I32s tdt_fstInstP;
	I32s tdt_lstInstP;
};
typedef struct BasThreadDat BasThreadDat;

struct IdxThreadDat {
	I32s xtdt_threadid;
	BasThreadDat xtdt_thrdat;
};
typedef struct IdxThreadDat IdxThreadDat;

struct ThrdAnaEvent {
	I8u taev_eventcode;
	I8u taev_ipoffsettype;
	I32u taev_ipoffset;
	I32u taev_thrd;
	I8u taev_dstoffsettype;
	I32s taev_dstoffset;
	N32u taev_i32a;
#ifdef USE_PORT
	N16u taev_port;
#endif /* USE_PORT */
	I8u taev_nodetype;
	I8u taev_genetrn;
	I32s taev_instP;
};
typedef struct ThrdAnaEvent ThrdAnaEvent;

struct SoupUpdEvent {
	I8u supd_eventcode;
	I8u supd_ipoffsettype;
	I8u supd_extrk;
	I32u supd_ipoffset;
	I32u supd_thrd;
	I8u supd_dstoffsettype;
	I8u supd_dtra;
	I32s supd_dstoffset;
	I8u supd_srcoffsettype;
	I8u supd_stra;
	I32s supd_srcoffset;
	I32s supd_instP;
};
typedef struct SoupUpdEvent SoupUpdEvent;

struct BasGenThrdDat {
	I32s gdt_opcnt;
	I32s gdt_dght1opcnt;
	I32s gdt_dght2opcnt;
	I8s gdt_gbits;
};
typedef struct BasGenThrdDat BasGenThrdDat;

struct FilGenThrdDat {
	I32s xgdt_bytoffst;
	DynArrCtrl xgdt_thrd_ara_ctrl;
	BasGenThrdDat xgdt_gbdt;
};
typedef struct FilGenThrdDat FilGenThrdDat;

struct BasInstrThrd {
	I32s ith_nonprom;
	I32s ith_prom;
};
typedef struct BasInstrThrd BasInstrThrd;

struct FilInstrThrd {
	I32s fith_threadid;
	BasInstrThrd fith_base;
	DynArrCtrl fith_calvlint_ctrl;
};
typedef struct FilInstrThrd FilInstrThrd;
#endif /* GENEBANK_H */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_head_t (XDR *, head_t*);
extern  bool_t xdr_GBGenome (XDR *, GBGenome*);
extern  bool_t xdr_GBGenBits (XDR *, GBGenBits*);
extern  bool_t xdr_Metabolism (XDR *, Metabolism*);
extern  bool_t xdr_indx_t (XDR *, indx_t*);
extern  bool_t xdr_GBindx_t (XDR *, GBindx_t*);
extern  bool_t xdr_BaseThrdAnaDat (XDR *, BaseThrdAnaDat*);
extern  bool_t xdr_FilThrdAnaDat (XDR *, FilThrdAnaDat*);
extern  bool_t xdr_BaseCallLvlIntrv (XDR *, BaseCallLvlIntrv*);
extern  bool_t xdr_BasThreadDat (XDR *, BasThreadDat*);
extern  bool_t xdr_IdxThreadDat (XDR *, IdxThreadDat*);
extern  bool_t xdr_ThrdAnaEvent (XDR *, ThrdAnaEvent*);
extern  bool_t xdr_SoupUpdEvent (XDR *, SoupUpdEvent*);
extern  bool_t xdr_BasGenThrdDat (XDR *, BasGenThrdDat*);
extern  bool_t xdr_FilGenThrdDat (XDR *, FilGenThrdDat*);
extern  bool_t xdr_BasInstrThrd (XDR *, BasInstrThrd*);
extern  bool_t xdr_FilInstrThrd (XDR *, FilInstrThrd*);

#else /* K&R C */
extern bool_t xdr_head_t ();
extern bool_t xdr_GBGenome ();
extern bool_t xdr_GBGenBits ();
extern bool_t xdr_Metabolism ();
extern bool_t xdr_indx_t ();
extern bool_t xdr_GBindx_t ();
extern bool_t xdr_BaseThrdAnaDat ();
extern bool_t xdr_FilThrdAnaDat ();
extern bool_t xdr_BaseCallLvlIntrv ();
extern bool_t xdr_BasThreadDat ();
extern bool_t xdr_IdxThreadDat ();
extern bool_t xdr_ThrdAnaEvent ();
extern bool_t xdr_SoupUpdEvent ();
extern bool_t xdr_BasGenThrdDat ();
extern bool_t xdr_FilGenThrdDat ();
extern bool_t xdr_BasInstrThrd ();
extern bool_t xdr_FilInstrThrd ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_GENEBANK_H_RPCGEN */
